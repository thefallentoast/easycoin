import ctypes, time

lib = ctypes.CDLL('./libeasyhash.so')  # or 'mylib.dll' on Windows

lib.easyhash_u32.argtypes = [
    ctypes.c_uint32,  # last_hash
    ctypes.c_uint32   # nonce
]
lib.easyhash_u32.restype = ctypes.c_uint32

lib.mine_u32.argtypes = [
    ctypes.c_uint32,  # difficulty
    ctypes.c_uint32,  # previous_hash
    ctypes.c_uint32,  # num_threads
    ctypes.POINTER(ctypes.c_uint32),  # out_nonce
    ctypes.POINTER(ctypes.c_uint32),  # out_hash
    ctypes.POINTER(ctypes.c_uint32),   # out_hashcount
]
lib.mine_u32.restype = None  # void function

difficulty = 0x000000FF
start_nonce = 0
previous_hash = 0xEB
miner_id = 0xFA0D
block_height = 0

out_nonce = ctypes.c_uint32()
out_hash = ctypes.c_uint32()
out_hashcount = ctypes.c_uint32()

def gen_miner_hash(previous_hash, block_height, miner_id):
    return lib.easyhash_u32(
                previous_hash + block_height,
                miner_id)

miner_hash = gen_miner_hash(previous_hash, block_height, miner_id)

while (True):
    start_time = time.time()
    lib.mine_u32(
        difficulty,
        miner_hash,
        2, # Num threads
        ctypes.byref(out_nonce),
        ctypes.byref(out_hash),
        ctypes.byref(out_hashcount)
    )

    print(f"Found nonce: {out_nonce.value} for hash {hex(out_hash.value)}")
    print(f"Hashrate: {out_hashcount.value / (time.time() - start_time) / 1000000} MH/s")
    block_height += 1
    miner_hash = gen_miner_hash(previous_hash, block_height, miner_id)