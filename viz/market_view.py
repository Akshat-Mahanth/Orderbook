from PySide6 import QtWidgets
from viz.asset_panel import AssetPanel
from viz.colors import ASSET_COLORS


class MarketView(QtWidgets.QWidget):
    def __init__(self, num_assets=3, view_count=2):
        super().__init__()

        # ---------- state ----------
        self.num_assets = num_assets
        self.view_count = view_count
        self.asset_ids = list(range(num_assets))

        # ---------- UI ----------
        self.setWindowTitle("Multi-Asset Market View")
        self.resize(1200, 800)

        self.grid = QtWidgets.QGridLayout(self)
        self.panels = []

        self._build_grid()

    # -------------------------------------------------
    def _build_grid(self):
        while self.grid.count():
            item = self.grid.takeAt(0)
            widget = item.widget()
            if widget:
                widget.setParent(None)

        self.panels.clear()

        cols = int(self.view_count ** 0.5)
        if cols * cols < self.view_count:
            cols += 1

        for i in range(self.view_count):
            asset_id = self.asset_ids[i % len(self.asset_ids)]
            color = ASSET_COLORS[asset_id % len(ASSET_COLORS)]

            panel = AssetPanel(asset_id, color)
            self.panels.append(panel)

            row = i // cols
            col = i % cols
            self.grid.addWidget(panel, row, col)

    # -------------------------------------------------
    def reconfigure(self, asset_ids, view_count):
        self.asset_ids = asset_ids
        self.view_count = view_count
        self._build_grid()

    # -------------------------------------------------
    def update_market(self, snapshots_by_asset):
        for panel in self.panels:
            snap = snapshots_by_asset.get(panel.asset_id)
            if not snap:
                continue

            panel.update_l2(snap["bids"], snap["asks"])
            panel.add_trades(snap["trades"])

            if "l3" in snap:
                panel.update_l3(
                    snap["l3"]["bids"],
                    snap["l3"]["asks"]
                )

    # -------------------------------------------------
    # SHM routing: L2 + L3
    # -------------------------------------------------
    def update_from_shm(self, l2_snaps, l3_snaps):
        snapshots = {}

        for asset_id in range(len(l2_snaps)):
            l2 = l2_snaps[asset_id]
            l3 = l3_snaps[asset_id]

            bids = [(lvl.price, lvl.qty)
                    for lvl in l2.bids[:l2.bid_levels]]

            asks = [(lvl.price, lvl.qty)
                    for lvl in l2.asks[:l2.ask_levels]]

            # ✅ FIXED: use l2, not undefined s
            trades = []
            for t in l2.trades[:l2.trade_count]:
                ts, price, qty = t.timestamp, t.price, t.qty
                trades.append((ts, price, qty))

            snapshots[asset_id] = {
                "bids": bids,
                "asks": asks,
                "trades": trades,
                "l3": {
                    "bids": [
                        {"order_id": o.order_id, "price": o.price, "qty": o.qty}
                        for o in l3.bids[:l3.bid_count]
                    ],
                    "asks": [
                        {"order_id": o.order_id, "price": o.price, "qty": o.qty}
                        for o in l3.asks[:l3.ask_count]
                    ],
                }
            }

        self.update_market(snapshots)

