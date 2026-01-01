class CandleSeries:
    def __init__(self, max_candles=200):
        self.candles = []          # list of (o, h, l, c)
        self.current = None
        self.max = max_candles

    def on_trade(self, ts_ms, price, qty):
        # start new candle
        if self.current is None:
            self.current = [price, price, price, price]
        else:
            # update current
            self.current[1] = max(self.current[1], price)  # high
            self.current[2] = min(self.current[2], price)  # low
            self.current[3] = price                         # close

        # close immediately → tick candle
        self.candles.append(tuple(self.current))
        if len(self.candles) > self.max:
            self.candles.pop(0)

        self.current = None

    def all_candles(self):
        return self.candles

