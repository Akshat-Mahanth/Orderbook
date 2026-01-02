from PySide6 import QtWidgets
import pyqtgraph as pg

from viz.candles import CandleSeries
from viz.l3_panel import L3Panel

L2_DEPTH = 5
MAX_DRAW = 120


class AssetPanel(QtWidgets.QWidget):
    def __init__(self, asset_id, color=(0, 255, 0)):
        super().__init__()

        self.asset_id = asset_id
        self.color = color

        self.candles = CandleSeries(max_candles=500)

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
        self.plot.setMouseEnabled(x=False, y=True)

        # persistent graphics
        self.wicks = []
        self.bodies = []

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

    # ---------------- TRADES ----------------
    def add_trades(self, trades):
        if not trades:
            return

        for ts, price, qty in trades:
            self.candles.on_trade(ts, price, qty)

        self._draw_candles()

    # ---------------- CANDLES ----------------
    def _draw_candles(self):
        # remove old graphics
        for i in self.wicks + self.bodies:
            self.plot.removeItem(i)

        self.wicks.clear()
        self.bodies.clear()

        candles = self.candles.all_candles()
        if not candles:
            return

        view = candles[-MAX_DRAW:]
        base_x = max(0, len(candles) - MAX_DRAW)

        MIN_BODY = 0.4  # visual minimum

        for i, candle in enumerate(view):
            # support (o,h,l,c) OR (ts,o,h,l,c)
            if len(candle) == 4:
                o, h, l, c = candle
            else:
                _, o, h, l, c = candle
   
            x = base_x + i

            up = c >= o
            color = (0, 220, 0) if up else (220, 0, 0)

            # ----- wick -----
            wick = pg.PlotDataItem(
                [x, x],
                [l, h],
                pen=pg.mkPen(color, width=1)
            )
            self.plot.addItem(wick)
            self.wicks.append(wick)

            # ----- body -----
            low = min(o, c)
            high = max(o, c)

            if high - low < MIN_BODY:
                mid = (high + low) / 2
                low = mid - MIN_BODY / 2
                high = mid + MIN_BODY / 2

            body = pg.PlotDataItem(
                [x, x],
                [low, high],
                pen=pg.mkPen(color, width=6)
            )
            self.plot.addItem(body)
            self.bodies.append(body)

        self.plot.enableAutoRange(axis="y")

