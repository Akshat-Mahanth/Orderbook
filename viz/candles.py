# viz/candles.py

class CandleSeries:
    def __init__(self, interval_sec=1, max_candles=300):
        self.interval_ms = interval_sec * 1000
        self.max = max_candles

        self.candles = []      # list of (o, h, l, c)
        self.current = None    # [bucket, o, h, l, c]

    def _bucket(self, ts_ms):
        return ts_ms // self.interval_ms

    def on_trade(self, ts_ms, price, qty):
        bucket = self._bucket(ts_ms)

        # ---------- first candle ----------
        if self.current is None:
            self.current = [bucket, price, price, price, price]
            return

        # ---------- same time bucket ----------
        if bucket == self.current[0]:
            self.current[2] = max(self.current[2], price)  # high
            self.current[3] = min(self.current[3], price)  # low
            self.current[4] = price                         # close
            return

        # ---------- bucket rollover ----------
        self._close_current()
        self.current = [bucket, price, price, price, price]

    def _close_current(self):
        if self.current is None:
            return

        _, o, h, l, c = self.current
        self.candles.append((o, h, l, c))

        if len(self.candles) > self.max:
            self.candles.pop(0)

        self.current = None

    def all_candles(self):
        out = list(self.candles)
        if self.current:
            _, o, h, l, c = self.current
            out.append((o, h, l, c))
        return out

