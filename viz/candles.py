class CandleSeries:
    def __init__(self, interval_sec=1):
        self.interval = interval_sec
        self.current = None
        self.candles = []  # list of (t, o, h, l, c)

    def _bucket(self, ts_ms):
        return (ts_ms // 1000) // self.interval

    def add_trade(self, ts_ms, price, qty):
        bucket = self._bucket(ts_ms)

        if self.current is None:
            self.current = [bucket, price, price, price, price]
            return

        if bucket == self.current[0]:
            self.current[2] = max(self.current[2], price)  # high
            self.current[3] = min(self.current[3], price)  # low
            self.current[4] = price                         # close
        else:
            self.candles.append(tuple(self.current))
            self.current = [bucket, price, price, price, price]
    def get_ohlc(self):
            data = list(self.candles)
            if self.current:
                data.append(tuple(self.current))
            return data
