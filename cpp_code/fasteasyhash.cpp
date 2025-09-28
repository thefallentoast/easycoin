#include <emmintrin.h>
#include <stdint.h>
#include <stdio.h>

#include "sse2macros.h"

#define W .words
#define C(x) SSE2FIL(K[r]W[x])

#ifdef _WIN32 
#define EXPORT __declspec(DLLEXPORT)
#else
#define EXPORT
#endif

#if defined(_MSC_VER)
  #define ALIGN16 __declspec(align(16))
#else
  #define ALIGN16 __attribute__((aligned(16)))
#endif

typedef uint64_t u64;
typedef __m128i u128;

static inline u128 eh_ror128(u128 x, int n) { // Can be int because the rots are fixed
    int m = n & 63;
    return SSE2OR(SSE2RSH(x, m), SSE2LSH(x, (64 - m) & 63));
}

typedef struct {
    u64 words[4];
} u256;

static const u256 K[4] = {
    { .words = {0xfb37a30bfc6db00c, 0x20aaa580ab0a543f, 0xb70a723a66323117, 0xed04db3275575ad5}},
    { .words = {0x3991c4cbc30d5123, 0xa9a299f2f2dff247, 0x93a843501e1a53ca, 0x06f9e304c9f9a41b}},
    { .words = {0x20b718f96dd87e14, 0x14e55ef766fc546d, 0x627742d047ae3954, 0x3b4b414740da6435}},
    { .words = {0xd94681a60fdb761f, 0x9174fc91c8cb7be3, 0xa6cbe0eb14ee8ee5, 0xfe62ab0a0cdfb64f}}
};

EXPORT void eh_permute128(u128* state) {
    for (int r = 0; r < 4; r++) {
        // First half mixing
        state[0] = SSE2XOR(SSE2ADD(state[1], state[3]), SSE2ADD(C(0), C(3)));
        state[1] = SSE2ADD(eh_ror128(state[2], 15), state[3]);
        state[2] = SSE2XOR(SSE2ADD(eh_ror128(SSE2XOR(state[0], state[1]), 61), C(1)), C(2));
        state[3] = SSE2XOR(state[1], SSE2ADD(state[2], eh_ror128(state[0], 19)));

        // Second half mixing
        state[4] = SSE2XOR(SSE2ADD(state[5], state[7]), SSE2ADD(C(0), C(3)));
        state[5] = eh_ror128(state[6], 33);
        state[6] = SSE2XOR(SSE2ADD(eh_ror128(SSE2ADD(state[4], state[5]), 17), C(1)), C(2));
        state[7] = SSE2XOR(state[5], SSE2ADD(state[6], eh_ror128(state[0], 19)));

        // Pairwise mixing
        state[0] = SSE2XOR(state[0], SSE2ADD(eh_ror128(state[4], 19), C(0)));
        state[1] = eh_ror128(SSE2ADD(state[1], state[5]), 55);
        state[2] = SSE2XOR(SSE2ADD(state[2], state[6]), eh_ror128(SSE2XOR(C(2), state[6]), 7));
        state[3] = SSE2XOR(eh_ror128(SSE2XOR(state[3], state[7]), 15), C(3));
        state[4] = SSE2XOR(state[4], SSE2ADD(eh_ror128(state[0], 19), C(0)));
        state[5] = eh_ror128(SSE2ADD(state[5], state[1]), 55);
        state[6] = SSE2XOR(SSE2ADD(state[6], state[2]), eh_ror128(SSE2XOR(C(2), state[6]), 7));
        state[7] = SSE2XOR(eh_ror128(SSE2XOR(state[7], state[3]), 15), C(3));
    }
}

EXPORT u128 eh_hashu128(u128 input) {
    ALIGN16 u128 state[8] = {
        SSE2FIL(0),
        SSE2FIL(0),
        SSE2FIL(0),
        SSE2FIL(0),
        SSE2FIL(0),
        SSE2FIL(0),
        SSE2FIL(0),
        SSE2FIL(0)
    };
    // Unroll the 4 absorb rounds
    state[0] = SSE2XOR(state[0], eh_ror128(input, 37));
    state[1] = SSE2XOR(state[1], eh_ror128(input, 11));
    state[2] = SSE2XOR(state[2], eh_ror128(input, 53));
    state[3] = SSE2XOR(state[3], eh_ror128(input, 29));
    eh_permute128(state);
    state[0] = SSE2XOR(state[0], eh_ror128(input, 37));
    state[1] = SSE2XOR(state[1], eh_ror128(input, 11));
    state[2] = SSE2XOR(state[2], eh_ror128(input, 53));
    state[3] = SSE2XOR(state[3], eh_ror128(input, 29));
    eh_permute128(state);
    state[0] = SSE2XOR(state[0], eh_ror128(input, 37));
    state[1] = SSE2XOR(state[1], eh_ror128(input, 11));
    state[2] = SSE2XOR(state[2], eh_ror128(input, 53));
    state[3] = SSE2XOR(state[3], eh_ror128(input, 29));
    eh_permute128(state);
    state[0] = SSE2XOR(state[0], eh_ror128(input, 37));
    state[1] = SSE2XOR(state[1], eh_ror128(input, 11));
    state[2] = SSE2XOR(state[2], eh_ror128(input, 53));
    state[3] = SSE2XOR(state[3], eh_ror128(input, 29));
    eh_permute128(state);
    // Unroll the 12 permute rounds
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    eh_permute128(state);
    return SSE2XOR(SSE2XOR(state[0], state[2]), SSE2XOR(state[4], state[6]));
}