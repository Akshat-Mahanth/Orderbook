import os, json, time, struct

class RunLogger:
    def __init__(self, base="runs"):
        ts = int(time.time())
        self.dir = os.path.join(base, f"run_{ts}")
        os.makedirs(self.dir, exist_ok=True)

        self.snap_fp = open(os.path.join(self.dir, "snapshots.bin"), "wb")
        self.trade_fp = open(os.path.join(self.dir, "trades.csv"), "w")
        self.trade_fp.write("ts,asset,price,qty\n")

        meta = {"start_ts": ts}
        with open(os.path.join(self.dir, "meta.json"), "w") as f:
            json.dump(meta, f, indent=2)

    def log_snapshot(self, asset_id, snap):
        # store minimal binary (example)
        self.snap_fp.write(struct.pack(
            "I I I Q Q",
            asset_id,
            snap["best_bid"],
            snap["best_ask"],
            snap["best_bid_qty"],
            snap["best_ask_qty"],
        ))

    def log_trades(self, asset_id, trades):
        for t in trades:
            self.trade_fp.write(
                f'{t["timestamp"]},{asset_id},{t["price"]},{t["qty"]}\n'
            )

    def close(self):
        self.snap_fp.close()
        self.trade_fp.close()

