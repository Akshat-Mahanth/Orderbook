from PySide6 import QtWidgets
import pyqtgraph as pg
from viz.candles import CandleSeries

L2_DEPTH = 5

class AssetPanel(QtWidgets.QWidget):
    def __init__(self, asset_id, color):
        super().__init__()

        self.asset_id = asset_id
        self.color = color
        self.candles = CandleSeries(interval_sec=1)

        layout = QtWidgets.QHBoxLayout(self)

        # -------- L2 TABLE --------
        self.table = QtWidgets.QTableWidget(2 * L2_DEPTH, 2)
        self.table.setHorizontalHeaderLabels(["Price", "Qty"])
        self.table.verticalHeader().setVisible(False)
        self.table.setEditTriggers(QtWidgets.QAbstractItemView.NoEditTriggers)

        # -------- PRICE PLOT --------
        self.plot = pg.PlotWidget()
        self.plot.setBackground("k")
        self.plot.showGrid(x=True, y=True, alpha=0.2)

        layout.addWidget(self.table, 1)
        layout.addWidget(self.plot, 2)

    # -------- L2 --------
    def update_l2(self, bids, asks):
        self.table.clearContents()

        for i, (p, q) in enumerate(bids[:L2_DEPTH]):
            self.table.setItem(i, 0, QtWidgets.QTableWidgetItem(str(p)))
            self.table.setItem(i, 1, QtWidgets.QTableWidgetItem(str(q)))

        for i, (p, q) in enumerate(asks[:L2_DEPTH]):
            r = L2_DEPTH + i
            self.table.setItem(r, 0, QtWidgets.QTableWidgetItem(str(p)))
            self.table.setItem(r, 1, QtWidgets.QTableWidgetItem(str(q)))

    # -------- TRADES --------
    def add_trades(self, trades):
        if not trades:
            return

        for ts, price, qty in trades:
            self.candles.add_trade(ts, price, qty)

        self._redraw_candles()

    # -------- CANDLE RENDER --------
    def _redraw_candles(self):
        self.plot.clear()

        data = self.candles.get_ohlc()
        if not data:
            return

        start = max(0, len(data) - 50)
        view = data[start:]

        for i, (_, o, h, l, c) in enumerate(view):
            x = i
            up = c >= o
            color = (0, 255, 0) if up else (255, 0, 0)

            # wick
            self.plot.plot(
                [x, x], [l, h],
                pen=pg.mkPen(color, width=1)
            )

            # body
            self.plot.plot(
                [x, x], [o, c],
                pen=pg.mkPen(color, width=6)
            )

        self.plot.enableAutoRange(axis="y")

