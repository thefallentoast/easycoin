# Compiler commands
CC = g++
MINGW_CC = x86_64-w64-mingw32-g++

# Source files
SRC = mine_test.cpp easyhash.cpp

# Output names
OUT_LINUX = mining_test
OUT_WIN = mining_test.exe

# Compiler flags
CFLAGS = -Ofast -march=native

.PHONY: all linux windows clean

all: linux windows

linux:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT_LINUX)

windows:
	$(MINGW_CC) $(CFLAGS) $(SRC) -o $(OUT_WIN) -static-libgcc -static-libstdc++

clean:
	rm -f $(OUT_LINUX) $(OUT_WIN)
