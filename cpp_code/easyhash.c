#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t eh_rol16(uint16_t x, uint32_t n) {
    n &= 15;
    return (x << n) | (x >> (16 - n));
}

static inline uint32_t eh_rol32(uint32_t x, uint32_t n) {
    n &= 31;
    return (x << n) | (x >> (32 - n));
}

static inline uint64_t eh_rol64(uint64_t x, uint32_t n) {
    n &= 63;
    return (x << n) | (x >> (64 - n));
}

static inline uint64_t eh_b64(uint32_t x)  { return x & 0xFFFFFFFFFFFFFFFF; }
static inline uint32_t eh_b32(uint32_t x)  { return x & 0xFFFFFFFF; }
static inline uint16_t eh_b16(uint32_t x)  { return x & 0xFFFF; }

EXPORT uint16_t easyhash_u16(uint16_t last_hash, uint16_t nonce) {
    uint16_t state[4] = {0xDEAD, 0xABCD, 0xAA55, 0xC0DE};

    const uint16_t EH_U16_CONST1 = 0x26;
    const uint16_t EH_U16_CONST2 = 54435;
    const uint16_t EH_U16_CONST3 = 26544;
    const uint16_t EH_U16_CONST4 = 0x8AA8;

    for (int i = 0; i < 4; i++) {
        uint8_t v = (i < 2) ? ((last_hash >> (8 * i)) & 0xFF)
                            : ((nonce     >> (8 * (i - 2))) & 0xFF);

        state[0] = (v & state[0]) ^ eh_rol16(v, i & 0x3);
        state[1] = eh_b16(v * EH_U16_CONST1) ^ eh_rol16(state[1], v & i);
        state[2] = state[2] ^ eh_b16(v * EH_U16_CONST2);
        state[3] = eh_b16(v * EH_U16_CONST3) ^ eh_rol32(eh_b32(v ^ EH_U16_CONST4), i);

        if ((i % 2) == 0) {
            state[0] = eh_b16(EH_U16_CONST3 ^ state[1] * (state[2] & state[3]));
            state[1] = eh_b16(state[0] * state[2] ^ eh_rol16(state[3], state[1] & 0xF));
            state[2] = eh_b16(EH_U16_CONST3 ^ state[3] & (eh_rol16(state[1], state[2] & 0xF)));
            state[3] = eh_b16(state[3] ^ eh_rol16(EH_U16_CONST3, state[2] & 0xF) ^ state[0]);
        }
    }

    return eh_b16(state[0] ^ state[1] ^ state[2] ^ state[3]);
}

EXPORT uint32_t easyhash_u32(uint32_t last_hash, uint32_t nonce) {
    uint32_t state[4] = {0xDEADBEEF, 0xABCDEF01, 0xAA5555AA, 0xC0DE9999};

    for (int i = 0; i < 8; i++) {
        uint8_t v = (i < 4) ? ((last_hash >> (8 * i)) & 0xFF)
                            : ((nonce     >> (8 * (i - 4))) & 0xFF);

        state[0] = (v & state[0]) ^ eh_rol32(v, i & 0x3);
        state[1] = eh_b32(v * 0x2654) ^ eh_rol32(state[1], v & i);
        state[2] = state[2] ^ eh_b32(v * 54435761);
        state[3] = eh_b32(v * 2654435761UL) ^ eh_rol32(eh_b32(v ^ 0x8AA8A88A), i);

        if ((i % 2) == 0) {
            state[0] = eh_b32(2654435761UL ^ state[1] * (state[2] & state[3]));
            state[1] = eh_b32(state[0] * state[2] ^ eh_rol32(state[3], state[1] & 0xF));
            state[2] = eh_b32(2654435761UL ^ state[3] & (eh_rol32(state[1], state[2] & 0xF)));
            state[3] = eh_b32(state[3] ^ eh_rol32(2654435761UL, state[2] & 0xF) ^ state[0]);
        }
    }

    return eh_b32(state[0] ^ state[1] ^ state[2] ^ state[3]);
}

#ifdef __cplusplus
}
#endif
