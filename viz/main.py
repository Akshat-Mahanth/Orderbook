# viz/main.py

import sys
from PySide6.QtWidgets import QApplication, QVBoxLayout, QWidget
from PySide6.QtCore import QTimer

from viz.market_view import MarketView
from viz.sim_controller import SimController
from viz.sim_controls import SimControls
from viz.shm_reader import ShmReader


def main():
    print("[UI] starting app")

    app = QApplication(sys.argv)

    root = QWidget()
    layout = QVBoxLayout(root)

    NUM_ASSETS = 3

    # -------------------------------------------------
    # controller (NOT a QWidget)
    # -------------------------------------------------
    controller = SimController(sim_path="./sim")

    # -------------------------------------------------
    # controls (QWidget, uses controller)
    # -------------------------------------------------
    controls = SimControls(controller)

    # -------------------------------------------------
    # market view
    # -------------------------------------------------
    view = MarketView(num_assets=NUM_ASSETS, view_count=2)

    layout.addWidget(controls)
    layout.addWidget(view)

    root.resize(1400, 900)
    root.show()

    # -------------------------------------------------
    # SHM handling
    # -------------------------------------------------
    shm = None
    last_running = False

    def tick():
        nonlocal shm, last_running

        running = controller.is_running()

        # sim just stopped → drop SHM
        if last_running and not running:
            print("[UI] sim stopped → resetting SHM")
            shm = None

        last_running = running

        if not running:
            return

        try:
            if shm is None:
                shm = ShmReader()
                print("[UI] SHM connected")

            l2, l3 = shm.read_all()
            view.update_from_shm(l2, l3)

        except FileNotFoundError:
            shm = None

    timer = QTimer()
    timer.timeout.connect(tick)
    timer.start(50)

    sys.exit(app.exec())


if __name__ == "__main__":
    main()

