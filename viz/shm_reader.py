import os
import mmap
import ctypes
from viz.config import NUM_ASSETS

SHM_PATH = "/dev/shm/market_snapshot"


class TradeEvent(ctypes.Structure):
    _fields_ = [
        ("price", ctypes.c_uint32),
        ("qty", ctypes.c_uint32),
        ("timestamp", ctypes.c_uint64),
    ]


class PriceLevel(ctypes.Structure):
    _fields_ = [
        ("price", ctypes.c_uint32),
        ("qty", ctypes.c_uint64),
    ]


L2_DEPTH = 10
MAX_TRADES = 64


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
        ("snaps", MarketSnapshot * NUM_ASSETS)
    ]


class ShmReader:
    def __init__(self):
        if not os.path.exists(SHM_PATH):
            raise FileNotFoundError(SHM_PATH)

        self.fd = open(SHM_PATH, "r+b")
        self.map = mmap.mmap(
            self.fd.fileno(),
            ctypes.sizeof(ShmBuffer),
            access=mmap.ACCESS_READ,
        )

        self.buf = ShmBuffer.from_buffer_copy(self.map)

    def read_all(self):
        self.map.seek(0)
        self.buf = ShmBuffer.from_buffer_copy(self.map)
        return self.buf.snaps

