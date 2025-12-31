class Candle:
    def __init__(self, ts, price, qty):
        self.ts = ts            # bucket timestamp
        self.open = price
        self.high = price
        self.low = price
        self.close = price
        self.volume = qty

    def update(self, price, qty):
        if price > self.high:
            self.high = price
        if price < self.low:
            self.low = price
        self.close = price
        self.volume += qty


class CandleSeries:
    def __init__(self, interval_sec=1):
        self.interval = interval_sec
        self.candles = []
        self.current = None 

    def on_trade(self, trade):
        """
        trade: TradeEvent from snapshot
        """
        bucket = trade.timestamp // (self.interval * 1000)
        if self.current is None or bucket != self.current.ts:
            self.current = Candle(bucket, trade.price, trade.qty)
            self.candles.append(self.current)
        else:
            self.current.update(trade.price, trade.qty)

