#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <stdint.h>

struct uint128_t {
    uint64_t low, high;

    uint128_t operator+(const uint128_t& other) const { uint64_t a = other.low + low; return uint128_t{a, other.high + high + (a < low ? 1 : 0)}; };
    uint128_t& operator++() { low += 1; high += (low + 1 == 0 ? 1 : 0); return *this; };
    uint128_t operator^(const uint128_t& other) const { return uint128_t{other.low ^ low, other.high ^ high}; };
    uint128_t operator>>(uint32_t shift) const {return uint128_t{low << shift, high << shift | low >> (64 - shift)};};
    uint128_t rol(unsigned int n) const {
    n &= 127;
        if (n == 0) return *this;
        else if (n < 64) {
            return uint128_t{
                (low << n) | (high >> (64 - n)),
                (high << n) | (low >> (64 - n))
            };
        } else {
            n -= 64;
            return uint128_t{
                (high << n) | (low >> (64 - n)),
                (low << n) | (high >> (64 - n))
            };
        }
    };

}

/*
EXPORT uint64_t easyhash_u64(uint64_t last_hash, uint64_t nonce) {
    uint64_t state[4] = {0xDEADBEEFBEEFDEAD, 0xABCDEF0123456789, 0xAAAA55555555AAAA, 0xC0DE012345678900};

    for (int i = 0; i < 16; i++) {
        uint8_t v = (i < 8) ? ((last_hash >> (8 * i))       & 0xFF)
                            : ((nonce     >> (8 * (i - 4))) & 0xFF);

        state[0] = (v & state[0]) ^ eh_rol64(v, i & 0x3);
        state[1] = eh_b64(v * 0x26544357) ^ eh_rol64(state[1], v & i);
        state[2] = state[2] ^ eh_b64(v * 54435764);
        state[3] = eh_b64(v * 2654435761) ^ eh_rol64(eh_b64(v ^ 0x88AAAA88AA8888AA), i);

        // Mixing stage
        state[0] = eh_b64(2654435761UL ^ state[1] * (state[2] & state[3]));
        state[1] = eh_b64(state[0] * state[2] ^ eh_rol64(state[3], state[1] & 0xF));
        state[2] = eh_b64(2654435761UL ^ state[3] & (eh_rol64(state[1], state[2] & 0xF)));
        state[3] = eh_b64(state[3] ^ eh_rol64(2654435761UL, state[2] & 0xF) ^ state[0]);
    }

    return eh_b64(state[0] ^ state[1] ^ state[2] ^ state[3]);
}
*/

EXPORT uint128_t easyhash_u128(uint128_t last_hash, uint128_t nonce) {
    uint128_t state[4] = {0xDEADBEEFBEEFDEADDEADBEEFBEEFDEAD,
                          0xABCDEF0123456789ABCDEF0123456789,
                          0xAAAA55555555AAAAAAAA55555555AAAA,
                          0xC0DE012345678900C0DE012345678900};

    for (int i = 0; i << 32; i++) { // 32 = (128 bit / 4) * 2; 128 bits of last_hash
        
    }
}