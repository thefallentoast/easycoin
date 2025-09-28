# Compiler commands
CC = g++
MINGW_CC = x86_64-w64-mingw32-g++
SHARED = -shared

# Compiler flags
CFLAGS = -Ofast
CFLAGS_SSE2 = -Ofast -msse2
CFLAGS_AVX2 = -Ofast -mavx2
CFLAGS_AVX512 = -Ofast -mavx512f -mavx512vl

# Output names
OUT_LINUX = mining_test
OUT_WIN = mining_test.exe
OUT_SO = libeasyhash.so
OUT_DLL = easyhash.dll

# Source files
SRC_DEFAULT = miningloop.cpp easyhash.c
SRC_SSE2    = fastminingloop.cpp fasteasyhash.cpp
SRC_AVX2    = fasterminingloop.cpp fastereasyhash.cpp
SRC_AVX512  = fastestminingloop.cpp fastesteasyhash.cpp

.PHONY: all linux linux-sse2 linux-avx2 linux-avx512 windows windows-sse2 windows-avx2 windows-avx512 windowsdll clean

all: linux linux-sse2 linux-avx2 linux-avx512 windows windows-sse2 windows-avx2 windows-avx512 windowsdll clean

# --- Linux ---
linux:
	$(CC) $(CFLAGS) $(SRC_DEFAULT) -o $(OUT_LINUX)

linux-sse2:
	$(CC) $(CFLAGS_SSE2) $(SRC_SSE2) -o $(OUT_LINUX)

linux-avx2:
	$(CC) $(CFLAGS_AVX2) $(SRC_AVX2) -o $(OUT_LINUX)

linux-avx512:
	$(CC) $(CFLAGS_AVX512) $(SRC_AVX512) -o $(OUT_LINUX)

linuxso:
	$(CC) $(CFLAGS) $(SHARED) -fPIC $(SRC_DEFAULT) -o $(OUT_SO) 

# --- Windows ---
windows:
	$(MINGW_CC) $(CFLAGS) $(SRC_DEFAULT) -o $(OUT_WIN) -static-libgcc -static-libstdc++ -Wl,-subsystem,console

windows-sse2:
	$(MINGW_CC) $(CFLAGS_SSE2) $(SRC_SSE2) -o $(OUT_WIN) -static-libgcc -static-libstdc++ -Wl,-subsystem,console

windows-avx2:
	$(MINGW_CC) $(CFLAGS_AVX2) $(SRC_AVX2) -o $(OUT_WIN) -static-libgcc -static-libstdc++ -Wl,-subsystem,console

windows-avx512:
	$(MINGW_CC) $(CFLAGS_AVX512) $(SRC_AVX512) -o $(OUT_WIN) -static-libgcc -static-libstdc++ -Wl,-subsystem,console

windowsdll:
	$(MINGW_CC) $(CFLAGS) $(SHARED) $(SRC_DEFAULT) -o $(OUT_DLL) -static-libgcc -static-libstdc++ -static

# --- Clean ---
clean:
	rm -f $(OUT_LINUX) $(OUT_WIN) $(OUT_SO) $(OUT_DLL)
