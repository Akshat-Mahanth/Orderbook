import mmap
import ctypes
import os
from viz.snapshot_struct import ShmBuffer

SHM_PATH = "/dev/shm/market_snapshot"

class ShmReader:
    def __init__(self):
        size = ctypes.sizeof(ShmBuffer)

        # open POSIX shared memory
        self.fd = os.open(SHM_PATH, os.O_RDWR)

        # mmap must be writable for ctypes.from_buffer
        self.map = mmap.mmap(
            self.fd,
            size,
            mmap.MAP_SHARED,
            mmap.PROT_READ | mmap.PROT_WRITE
        )

        self.buf = ShmBuffer.from_buffer(self.map)

    def read_snapshot(self):
        while True:
            seq1 = self.buf.seq
            snap = self.buf.snapshot
            seq2 = self.buf.seq
            if seq1 == seq2:
                return snap

