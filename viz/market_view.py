from PySide6 import QtWidgets
from viz.asset_panel import AssetPanel
from viz.colors import ASSET_COLORS


class MarketView(QtWidgets.QWidget):
    def __init__(self, num_assets=3, view_count=2):
        super().__init__()

        # ---------- state (MUST come first) ----------
        self.num_assets = num_assets
        self.view_count = view_count
        self.asset_ids = list(range(num_assets))   # <-- THIS WAS MISSING AT RUNTIME

        # ---------- UI ----------
        self.setWindowTitle("Multi-Asset Market View")
        self.resize(1200, 800)

        self.grid = QtWidgets.QGridLayout(self)
        self.panels = []

        # ---------- build ----------
        self._build_grid()

    # -------------------------------------------------
    # grid construction
    # -------------------------------------------------
    def _build_grid(self):
        # clear existing widgets
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
    # external reconfiguration
    # -------------------------------------------------
    def reconfigure(self, asset_ids, view_count):
        self.asset_ids = asset_ids
        self.view_count = view_count
        self._build_grid()

    # -------------------------------------------------
    # core update (expects per-asset snapshots)
    # -------------------------------------------------
    def update_market(self, snapshots_by_asset):
        for panel in self.panels:
            snap = snapshots_by_asset.get(panel.asset_id)
            if not snap:
                continue

            panel.update_l2(snap["bids"], snap["asks"])
            panel.add_trades(snap["trades"])

    # -------------------------------------------------
    # SHM routing shim (single-asset → multi-asset)
    # -------------------------------------------------
    def update_from_shm(self, snaps):
        snapshots = {}
    
        for asset_id in range(len(snaps)):
            s = snaps[asset_id]
    
            bids = [(lvl.price, lvl.qty) for lvl in s.bids[:s.bid_levels]]
            asks = [(lvl.price, lvl.qty) for lvl in s.asks[:s.ask_levels]]
            trades = [(t.timestamp, t.price, t.qty)
                      for t in s.trades[:s.trade_count]]
            
            snapshots[asset_id] = {
                "bids": bids,
                "asks": asks,
                "trades": trades,
            }
    
        self.update_market(snapshots)
        
