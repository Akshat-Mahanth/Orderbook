import ctypes

L2_DEPTH = 10
MAX_TRADES = 64
class PriceLevel(ctypes.Structure):
    _fields_ = [
        ("price", ctypes.c_uint32),
        ("qty",   ctypes.c_uint64),
    ]
class TradeEvent(ctypes.Structure):
    _fields_ = [
        ("price", ctypes.c_uint32),
        ("qty", ctypes.c_uint32),
        ("timestamp", ctypes.c_uint64),
    ]

class MarketSnapshot(ctypes.Structure):
    _fields_ = [
        ("timestamp", ctypes.c_uint64),

        ("best_bid", ctypes.c_uint32),
        ("best_ask", ctypes.c_uint32),
        ("best_bid_qty", ctypes.c_uint64),
        ("best_ask_qty", ctypes.c_uint64),

        ("bids", PriceLevel * L2_DEPTH),
        ("asks", PriceLevel * L2_DEPTH),

        ("bid_levels", ctypes.c_uint32),
        ("ask_levels", ctypes.c_uint32),
        ("trades", TradeEvent * MAX_TRADES),
        ("trade_count", ctypes.c_uint32),
    ]

class ShmBuffer(ctypes.Structure):
    _fields_ = [
        ("seq", ctypes.c_uint32),
        ("snapshot", MarketSnapshot),
    ]
