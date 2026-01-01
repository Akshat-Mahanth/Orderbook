from PySide6 import QtWidgets
import pyqtgraph as pg

from viz.candles import CandleSeries
from viz.l3_panel import L3Panel

L2_DEPTH = 5


class AssetPanel(QtWidgets.QWidget):
    def __init__(self, asset_id, color=(0, 255, 0)):
        super().__init__()

        self.asset_id = asset_id
        self.color = color
        self.candles = CandleSeries(max_candles=300)

        layout = QtWidgets.QVBoxLayout(self)

        # ================= L2 =================
        self.l2 = QtWidgets.QTableWidget(2 * L2_DEPTH, 2)
        self.l2.setHorizontalHeaderLabels(["Price", "Qty"])
        self.l2.verticalHeader().setVisible(False)
        self.l2.setEditTriggers(QtWidgets.QAbstractItemView.NoEditTriggers)

        # ================= PRICE CHART =================
        self.plot = pg.PlotWidget()
        self.plot.setBackground("k")
        self.plot.showGrid(x=True, y=True, alpha=0.2)

        self.price_plot = self.plot.plot(
            pen=pg.mkPen(self.color, width=2)
        )

        # ================= L3 =================
        self.l3 = L3Panel(asset_id)

        layout.addWidget(self.l2, 1)
        layout.addWidget(self.plot, 2)
        layout.addWidget(self.l3, 2)

    # ---------------- L2 ----------------
    def update_l2(self, bids, asks):
        self.l2.clearContents()

        for i, (p, q) in enumerate(bids[:L2_DEPTH]):
            self.l2.setItem(i, 0, QtWidgets.QTableWidgetItem(str(p)))
            self.l2.setItem(i, 1, QtWidgets.QTableWidgetItem(str(q)))

        for i, (p, q) in enumerate(asks[:L2_DEPTH]):
            r = L2_DEPTH + i
            self.l2.setItem(r, 0, QtWidgets.QTableWidgetItem(str(p)))
            self.l2.setItem(r, 1, QtWidgets.QTableWidgetItem(str(q)))

    # ---------------- L3 ----------------
    def update_l3(self, bids, asks):
        self.l3.update_l3(bids, asks)

    # ---------------- TRADES → CANDLES ----------------
    def add_trades(self, trades):
        if not trades:
            return

        for ts, price, qty in trades:
            self.candles.on_trade(ts, price, qty)

        self._update_price_plot()

    # ---------------- PLOT ----------------
    def _update_price_plot(self):
        candles = self.candles.all_candles()
        if not candles:
            return

        xs = list(range(len(candles)))
        closes = [c[3] for c in candles]

        self.price_plot.setData(xs, closes)
        self.plot.enableAutoRange(axis="y")

