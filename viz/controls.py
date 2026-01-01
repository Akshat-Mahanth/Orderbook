from PySide6 import QtWidgets


class MarketControls(QtWidgets.QWidget):
    def __init__(self, num_assets):
        super().__init__()

        self.num_assets = num_assets

        layout = QtWidgets.QHBoxLayout(self)

        # ---- asset selector ----
        self.asset_boxes = []
        asset_group = QtWidgets.QGroupBox("Assets")
        asset_layout = QtWidgets.QHBoxLayout(asset_group)

        for i in range(num_assets):
            cb = QtWidgets.QCheckBox(f"A{i}")
            cb.setChecked(True)
            self.asset_boxes.append(cb)
            asset_layout.addWidget(cb)

        # ---- view count ----
        self.view_spin = QtWidgets.QSpinBox()
        self.view_spin.setRange(1, num_assets)
        self.view_spin.setValue(min(2, num_assets))

        # ---- apply ----
        self.apply_btn = QtWidgets.QPushButton("Apply")

        layout.addWidget(asset_group)
        layout.addWidget(QtWidgets.QLabel("Views"))
        layout.addWidget(self.view_spin)
        layout.addWidget(self.apply_btn)
        layout.addStretch()
        self.start_btn = QtWidgets.QPushButton("Start")
        self.stop_btn  = QtWidgets.QPushButton("Stop")
        self.replay_btn= QtWidgets.QPushButton("Replay")
        
        layout.addWidget(self.start_btn)
        layout.addWidget(self.stop_btn)
        layout.addWidget(self.replay_btn)

    def selected_assets(self):
        return [i for i, cb in enumerate(self.asset_boxes) if cb.isChecked()]

