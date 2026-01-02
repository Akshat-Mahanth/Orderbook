class CandleSeries:
    def __init__(self, interval_ms=1000, max_candles=200):
        self.interval = interval_ms
        self.max = max_candles
        self.candles = []      # (t, o, h, l, c)
        self.current = None    # [t, o, h, l, c]

    def _bucket(self, ts):
        return (ts // self.interval) * self.interval

    def on_trade(self, ts, price, qty):
        bucket = self._bucket(ts)

        if self.current is None:
            self.current = [bucket, price, price, price, price]
            return

        if bucket == self.current[0]:
            self.current[2] = max(self.current[2], price)  # high
            self.current[3] = min(self.current[3], price)  # low
            self.current[4] = price                         # close
        else:
            self.candles.append(tuple(self.current))
            if len(self.candles) > self.max:
                self.candles.pop(0)

            self.current = [bucket, price, price, price, price]

    def all_candles(self):
        data = list(self.candles)
        if self.current:
            data.append(tuple(self.current))
        return data

