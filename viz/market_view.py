from PySide6 import QtWidgets, QtCore
from viz.shm_reader import ShmReader
import pyqtgraph as pg
from viz.candles import CandleSeries

class MarketView(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()

        self.reader = ShmReader()

        self.setWindowTitle("Orderbook Market View")
        self.resize(600, 500)

        layout = QtWidgets.QVBoxLayout(self)

        # L1 display
        self.l1_label = QtWidgets.QLabel("L1")
        self.l1_label.setStyleSheet("font-size: 16px; font-weight: bold;")
        layout.addWidget(self.l1_label)

        # L2 table
        self.table = QtWidgets.QTableWidget(20, 3)
        self.table.setHorizontalHeaderLabels(["Side", "Price", "Qty"])
        self.table.verticalHeader().setVisible(False)
        self.table.horizontalHeader().setStretchLastSection(True)
        layout.addWidget(self.table)
        self.candles = CandleSeries(interval_sec=1)
        # timer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_view)
        self.timer.start(50)  # ~20 FPS
        self.price_plot = pg.PlotWidget(title="Price (last trades)")
        self.price_plot.showGrid(x=True, y=True)
        layout.addWidget(self.price_plot)
        self.price_curve = self.price_plot.plot([], [], pen=pg.mkPen('y', width=2))        

    def update_view(self):
        snap = self.reader.read_snapshot()

## debug print        print(
##            "PY SNAP:",
##            snap.best_bid,
##            snap.best_ask,
##            snap.bid_levels,
##            snap.ask_levels
##        )

        self.l1_label.setText(
            f"BID {snap.best_bid} ({snap.best_bid_qty}) | "
            f"ASK {snap.best_ask} ({snap.best_ask_qty})"
        )

        rows = snap.bid_levels + snap.ask_levels
        self.table.setRowCount(rows)
        self.table.clearContents()
        for i in range(snap.trade_count):
            self.candles.on_trade(snap.trades[i])
        row = 0
        for i in range(snap.bid_levels):
            self.table.setItem(row, 0, QtWidgets.QTableWidgetItem("BID"))
            self.table.setItem(row, 1, QtWidgets.QTableWidgetItem(str(snap.bids[i].price)))
            self.table.setItem(row, 2, QtWidgets.QTableWidgetItem(str(snap.bids[i].qty)))
            row += 1

        for i in range(snap.ask_levels):
            self.table.setItem(row, 0, QtWidgets.QTableWidgetItem("ASK"))
            self.table.setItem(row, 1, QtWidgets.QTableWidgetItem(str(snap.asks[i].price)))
            self.table.setItem(row, 2, QtWidgets.QTableWidgetItem(str(snap.asks[i].qty)))
            row += 1

        xs = []
        ys = []
        for c in self.candles.candles[-100:]:
            xs.append(len(xs))
            ys.append(c.close)
        self.price_curve.setData(xs, ys)
        print("trade_count:", snap.trade_count,
            "candles:", len(self.candles.candles))
        
