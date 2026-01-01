from PySide6 import QtWidgets

MAX_ROWS = 50

class L3Panel(QtWidgets.QWidget):
    def __init__(self, asset_id):
        super().__init__()
        self.asset_id = asset_id

        layout = QtWidgets.QHBoxLayout(self)

        self.bid_box, self.bids = self._make_table("BIDS")
        self.ask_box, self.asks = self._make_table("ASKS")

        layout.addWidget(self.bid_box)
        layout.addWidget(self.ask_box)

    def _make_table(self, title):
        box = QtWidgets.QGroupBox(title)
        v = QtWidgets.QVBoxLayout(box)

        table = QtWidgets.QTableWidget(MAX_ROWS, 3)
        table.setHorizontalHeaderLabels(["OrderID", "Price", "Qty"])
        table.verticalHeader().setVisible(False)
        table.setEditTriggers(QtWidgets.QAbstractItemView.NoEditTriggers)

        v.addWidget(table)
        return box, table   # IMPORTANT: return BOTH

    def update_l3(self, bids, asks):
        self._fill(self.bids, bids)
        self._fill(self.asks, asks)

    def _fill(self, table, rows):
        table.clearContents()
        for i, o in enumerate(rows[:MAX_ROWS]):
            table.setItem(i, 0, QtWidgets.QTableWidgetItem(str(o["order_id"])))
            table.setItem(i, 1, QtWidgets.QTableWidgetItem(str(o["price"])))
            table.setItem(i, 2, QtWidgets.QTableWidgetItem(str(o["qty"])))

