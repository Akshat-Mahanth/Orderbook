import csv, time

class Replay:
    def __init__(self, trades_csv):
        self.trades = []
        with open(trades_csv) as f:
            for r in csv.DictReader(f):
                self.trades.append({
                    "timestamp": int(r["ts"]),
                    "asset": int(r["asset"]),
                    "price": int(r["price"]),
                    "qty": int(r["qty"]),
                })
        self.idx = 0

    def step(self, dt_ms):
        out = {}
        if self.idx >= len(self.trades):
            return out

        start = self.trades[self.idx]["timestamp"]
        while self.idx < len(self.trades) and \
              self.trades[self.idx]["timestamp"] <= start + dt_ms:
            t = self.trades[self.idx]
            out.setdefault(t["asset"], []).append(t)
            self.idx += 1
        return out

