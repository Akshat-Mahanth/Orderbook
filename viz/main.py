import sys
from PySide6 import QtWidgets
from viz.market_view import MarketView

def main():
    app = QtWidgets.QApplication(sys.argv)
    win = MarketView()
    win.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
