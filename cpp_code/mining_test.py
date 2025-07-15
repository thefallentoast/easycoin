import ctypes, time

lib = ctypes.CDLL('./libeasyhash.so')  # or 'mylib.dll' on Windows

lib.mine_u32.argtypes = [
    ctypes.c_uint32,  # difficulty
    ctypes.c_uint32,  # previous_hash
    ctypes.c_uint32,  # start_nonce
    ctypes.POINTER(ctypes.c_uint32),  # out_nonce
    ctypes.POINTER(ctypes.c_uint32),  # out_hash
    ctypes.POINTER(ctypes.c_uint32),   # out_hashcount
]
lib.mine_u32.restype = None  # void function

difficulty = 0x000000FF
start_nonce = 0
previous_hash = 0xCC

out_nonce = ctypes.c_uint32()
out_hash = ctypes.c_uint32()
out_hashcount = ctypes.c_uint32()

start_time = time.time()
lib.mine_u32(
    difficulty,
    start_nonce,
    previous_hash,
    ctypes.byref(out_nonce),
    ctypes.byref(out_hash),
    ctypes.byref(out_hashcount)
)

print(f"Found nonce: {out_nonce.value}")
print(f"Hashrate: {out_hashcount.value / (time.time() - start_time) / 1000000} MH/s")
