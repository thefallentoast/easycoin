import ctypes as ct
import time

class Miner:
    def __init__(self, lib_name_or_path: str) -> None:
        self.lib = ct.CDLL(lib_name_or_path)
        self.lib.mine_u32.argtypes = [
            ct.c_uint32,  # difficulty
            ct.c_uint32,  # previous_hash
            ct.c_uint32,  # num_threads
            ct.POINTER(ct.c_uint32),  # out_nonce
            ct.POINTER(ct.c_uint32),  # out_hash
            ct.POINTER(ct.c_uint32),   # out_hashcount
        ]
        self.lib.mine_u32.restype = None  # void function

    def mine(self, difficulty, previous_hash, num_threads, ) -> tuple[int, int, float]:
        difficulty = 0x000000FF
        start_nonce = 0
        previous_hash = 0xCC

        out_nonce = ct.c_uint32()
        out_hash = ct.c_uint32()
        out_hashcount = ct.c_uint32()

        start_time = time.time()
        self.lib.mine_u32(
            difficulty,
            previous_hash,
            num_threads,
            ct.byref(out_nonce),
            ct.byref(out_hash),
            ct.byref(out_hashcount)
        )

        print(f"Found nonce: {out_nonce.value}")
        print(f"Hashrate: {out_hashcount.value / (time.time() - start_time) / 1000000} MH/s")
        return (out_nonce.value, out_hash.value, out_hashcount.value)
    def mine_parallel(self, difficulty, previous_hash) -> tuple[int, int, int]:
        # TODO