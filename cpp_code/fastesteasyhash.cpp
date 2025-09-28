#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "avx512macros.h"

#define W .words
#define C(x) AVX512FIL(K[r]W[x])

#ifdef _WIN32
#define EXPORT __declspec(DLLEXPORT)
#else
#define EXPORT
#endif

#ifdef _MSC_VER
#define ALIGN64 __declspec(align(64))
#else
#define ALIGN64 __attribute__((aligned(64)))
#endif

typedef uint64_t u64;
typedef __m512i u512;

static inline u512 eh_ror512(u512 x, int n) {
    int m = n & 63;
    return AVX512OR(AVX512RSH(x, m), AVX512LSH(x, (64 - m) & 63));
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

EXPORT void eh_permute512(u512* state) {
    for (int r = 0; r < 4; r++) {
        // First half mixing
        state[0] = AVX512XOR(AVX512ADD(state[1], state[3]), AVX512ADD(C(0), C(3)));
        state[1] = AVX512ADD(eh_ror512(state[2], 15), state[3]);
        state[2] = AVX512XOR(AVX512ADD(eh_ror512(AVX512XOR(state[0], state[1]), 61), C(1)), C(2));
        state[3] = AVX512XOR(state[1], AVX512ADD(state[2], eh_ror512(state[0], 19)));

        // Second half mixing
        state[4] = AVX512XOR(AVX512ADD(state[5], state[7]), AVX512ADD(C(0), C(3)));
        state[5] = eh_ror512(state[6], 33);
        state[6] = AVX512XOR(AVX512ADD(eh_ror512(AVX512ADD(state[4], state[5]), 17), C(1)), C(2));
        state[7] = AVX512XOR(state[5], AVX512ADD(state[6], eh_ror512(state[0], 19)));

        // Pairwise mixing
        state[0] = AVX512XOR(state[0], AVX512ADD(eh_ror512(state[4], 19), C(0)));
        state[1] = eh_ror512(AVX512ADD(state[1], state[5]), 55);
        state[2] = AVX512XOR(AVX512ADD(state[2], state[6]), eh_ror512(AVX512XOR(C(2), state[6]), 7));
        state[3] = AVX512XOR(eh_ror512(AVX512XOR(state[3], state[7]), 15), C(3));
        state[4] = AVX512XOR(state[4], AVX512ADD(eh_ror512(state[0], 19), C(0)));
        state[5] = eh_ror512(AVX512ADD(state[5], state[1]), 55);
        state[6] = AVX512XOR(AVX512ADD(state[6], state[2]), eh_ror512(AVX512XOR(C(2), state[6]), 7));
        state[7] = AVX512XOR(eh_ror512(AVX512XOR(state[7], state[3]), 15), C(3));
    }
}

EXPORT u512 eh_hashu512(u512 input) {
    ALIGN64 u512 state[8] = {
        AVX512ZERO,
        AVX512ZERO,
        AVX512ZERO,
        AVX512ZERO,
        AVX512ZERO,
        AVX512ZERO,
        AVX512ZERO,
        AVX512ZERO
    };
    // Unroll the 4 absorb rounds
    state[0] = AVX512XOR(state[0], eh_ror512(input, 37));
    state[1] = AVX512XOR(state[1], eh_ror512(input, 11));
    state[2] = AVX512XOR(state[2], eh_ror512(input, 53));
    state[3] = AVX512XOR(state[3], eh_ror512(input, 29));
    eh_permute512(state);
    state[0] = AVX512XOR(state[0], eh_ror512(input, 37));
    state[1] = AVX512XOR(state[1], eh_ror512(input, 11));
    state[2] = AVX512XOR(state[2], eh_ror512(input, 53));
    state[3] = AVX512XOR(state[3], eh_ror512(input, 29));
    eh_permute512(state);
    state[0] = AVX512XOR(state[0], eh_ror512(input, 37));
    state[1] = AVX512XOR(state[1], eh_ror512(input, 11));
    state[2] = AVX512XOR(state[2], eh_ror512(input, 53));
    state[3] = AVX512XOR(state[3], eh_ror512(input, 29));
    eh_permute512(state);
    state[0] = AVX512XOR(state[0], eh_ror512(input, 37));
    state[1] = AVX512XOR(state[1], eh_ror512(input, 11));
    state[2] = AVX512XOR(state[2], eh_ror512(input, 53));
    state[3] = AVX512XOR(state[3], eh_ror512(input, 29));
    eh_permute512(state);
    // Unroll the 12 permute rounds
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    eh_permute512(state);
    return AVX512XOR(AVX512XOR(state[0], state[2]), AVX512XOR(state[4], state[6]));
}

#ifdef __cplusplus
}
#endif