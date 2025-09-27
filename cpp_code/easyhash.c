#include <stdint.h>
#include <stdio.h>

#define W .words
#define C K[r]W

#ifdef _WIN32 
#define EXPORT __declspec(DLLEXPORT)
#else
#define EXPORT
#endif

typedef uint64_t u64;

typedef struct {
    u64 words[4];  // little-endian: words[0] = least significant 64 bits
} u256;

static const u256 K[4] = {
    { .words = {0xfb37a30bfc6db00c, 0x20aaa580ab0a543f, 0xb70a723a66323117, 0xed04db3275575ad5}},
    { .words = {0x3991c4cbc30d5123, 0xa9a299f2f2dff247, 0x93a843501e1a53ca, 0x06f9e304c9f9a41b}},
    { .words = {0x20b718f96dd87e14, 0x14e55ef766fc546d, 0x627742d047ae3954, 0x3b4b414740da6435}},
    { .words = {0xd94681a60fdb761f, 0x9174fc91c8cb7be3, 0xa6cbe0eb14ee8ee5, 0xfe62ab0a0cdfb64f}}
};

static inline u64 eh_ror64(u64 x, u64 n) {
    u64 a = n & 0x3F;
    return (x >> a) | (x << (64 - a));
}

EXPORT void eh_permute(u64* state) {
    for (int r = 0; r < 4; r++) {
        // First half mixing
        state[0] = state[1] + state[3] ^ C[0] + C[3];
        state[1] = eh_ror64(state[2], 15) + state[3];
        state[2] = eh_ror64(state[0] ^ state[1], 61) + C[1] ^ C[2];
        state[3] = state[1] ^ state[2] + eh_ror64(state[0], 19);

        // Second half mixing
        state[4] = state[5] + state[7] ^ C[0] + C[3];
        state[5] = eh_ror64(state[6], 33);
        state[6] = eh_ror64(state[4] + state[5], 17) + C[1] ^ C[2];
        state[7] = state[5] ^ state[6] + eh_ror64(state[0], 19);

        // Pairwise mixing
        state[0] = state[0] ^ eh_ror64(state[4], 19) + C[0];
        state[1] = eh_ror64(state[1] + state[5], 55);
        state[2] = state[2] + state[6] ^ eh_ror64(C[2] ^ state[6], 7);
        state[3] = eh_ror64(state[3] ^ state[7], 15) ^ C[3];
        state[4] = state[4] ^ eh_ror64(state[0], 19) + C[0];
        state[5] = eh_ror64(state[5] + state[1], 55);
        state[6] = state[6] + state[2] ^ eh_ror64(C[2] ^ state[6], 7);
        state[7] = eh_ror64(state[7] ^ state[3], 15) ^ C[3];
    }
}

EXPORT u64 eh_hashu64(u64 input) {
    u64 state[8] = {
        0,0,0,0,0,0,0,0
    };
    for (int round = 0; round < 4; round++) { // Absorb rounds
        state[0] ^= eh_ror64(input, 37);
        state[1] ^= eh_ror64(input, 11);
        state[2] ^= eh_ror64(input, 53);
        state[3] ^= eh_ror64(input, 29);
        eh_permute(state); // eh_permute modifies state in place
    }
    for (int round = 0; round < 12; round++) { // Permute rounds
        eh_permute(state);
    }
    return state[0] ^ state[2] ^ state[4] ^ state[6];
}

