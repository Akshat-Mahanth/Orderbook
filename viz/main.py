import sys
from PySide6.QtWidgets import QApplication, QVBoxLayout, QWidget
from PySide6.QtCore import QTimer

from viz.market_view import MarketView
from viz.controls import MarketControls
from viz.shm_reader import ShmReader


def main():
    print("STARTING APP")

    app = QApplication(sys.argv)
    print("QAPP OK")

    root = QWidget()
    layout = QVBoxLayout(root)

    NUM_ASSETS = 3

    controls = MarketControls(NUM_ASSETS)
    view = MarketView(num_assets=NUM_ASSETS, view_count=2)

    layout.addWidget(controls)
    layout.addWidget(view)

    root.resize(1400, 900)
    root.show()

    shm = None

    def try_connect_shm():
        nonlocal shm
        if shm is not None:
            return
        try:
            shm = ShmReader()
            print("SHM CONNECTED")
            print("sizeof snapshot =", shm.map.size())
        except FileNotFoundError:
            return

    def tick():
        try_connect_shm()
        if shm is None:
            return
        snaps = shm.read_all()
        for i, snap in enumerate(snaps):
            print("PY asset", i, "ts", snap.timestamp, "trades", snap.trade_count)
        view.update_from_shm(snaps)


    timer = QTimer()
    timer.timeout.connect(tick)
    timer.start(50)

    sys.exit(app.exec())


if __name__ == "__main__":
    main()

