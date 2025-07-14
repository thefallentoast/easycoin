# Compiler commands
CC = g++
MINGW_CC = x86_64-w64-mingw32-g++
SHARED = -shared

# Source files
SRC = mine_test.cpp easyhash.cpp
SRC_HASH = easyhash.cpp

# Output names
OUT_LINUX = mining_test
OUT_WIN = mining_test.exe
OUT_DLL = easyhash.dll

# Compiler flags
CFLAGS = -Ofast -march=native

.PHONY: all linux windows windowsdll clean

all: linux windows windowsdll

linux:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT_LINUX)

windows:
	$(MINGW_CC) $(CFLAGS) $(SRC) -o $(OUT_WIN) -static-libgcc -static-libstdc++

windowsdll:
	$(MINGW_CC) $(CFLAGS) $(SHARED) $(SRC_HASH) -o $(OUT_DLL)

clean:
	rm -f $(OUT_LINUX) $(OUT_WIN)
