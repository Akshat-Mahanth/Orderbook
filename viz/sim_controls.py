# viz/sim_controls.py

from PySide6 import QtWidgets


class SimControls(QtWidgets.QWidget):
    def __init__(self, sim_controller):
        super().__init__()

        self.sim = sim_controller

        layout = QtWidgets.QHBoxLayout(self)

        self.start_btn = QtWidgets.QPushButton("▶ Start")
        self.stop_btn  = QtWidgets.QPushButton("⏹ Stop")

        layout.addWidget(self.start_btn)
        layout.addWidget(self.stop_btn)
        layout.addStretch(1)

        self.start_btn.clicked.connect(self._start)
        self.stop_btn.clicked.connect(self._stop)

        self._sync_buttons()

    # -----------------------------------------
    def _sync_buttons(self):
        running = self.sim.is_running()
        self.start_btn.setEnabled(not running)
        self.stop_btn.setEnabled(running)

    # -----------------------------------------
    def _start(self):
        self.sim.start()
        self._sync_buttons()

    # -----------------------------------------
    def _stop(self):
        self.sim.stop()
        self._sync_buttons()

