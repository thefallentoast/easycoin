#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t rol32(uint32_t x, uint32_t n) {
    n &= 31;
    return (x << n) | (x >> (32 - n));
}

EXPORT uint32_t easyhash_u32pair(uint32_t last_hash, uint32_t nonce) {
    uint32_t state[4] = {0xDEADBEEF, 0xABCDEF01, 0xAA5555AA, 0xC0DE9999};

    #define b32(x) ((x) & 0xFFFFFFFF)
    #define b4(x)  ((x) & 0xF)

    for (int i = 0; i < 8; i++) {
        uint8_t v = (i < 4) ? ((last_hash >> (8 * i)) & 0xFF)
                            : ((nonce     >> (8 * (i - 4))) & 0xFF);

        state[0] = (v & state[0]) ^ rol32(v, i & 0x3);
        state[1] = b32(v * 0x2654) ^ rol32(state[1], v & i);
        state[2] = state[2] ^ b32(v * 54435761);
        state[3] = b32(v * 2654435761UL) ^ rol32(b32(v ^ 0x8AA8A88A), i);

        if ((i % 2) == 0) {
            state[0] = b32(2654435761UL ^ state[1] * (state[2] & state[3]));
            state[1] = b32(state[0] * state[2] ^ rol32(state[3], b4(state[1])));
            state[2] = b32(2654435761UL ^ state[3] & (rol32(state[1], b4(state[2]))));
            state[3] = b32(state[3] ^ rol32(2654435761UL, b4(state[2])) ^ state[0]);
        }
    }

    return b32(state[0] ^ state[1] ^ state[2] ^ state[3]);
}

#ifdef __cplusplus
}
#endif
