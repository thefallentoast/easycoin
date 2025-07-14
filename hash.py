import random
import string

def random_printable_string(n: int) -> str:
    return ''.join(random.choice(string.printable) for _ in range(n))

'''
Hash function for the EasyMine coin (and utilities)
'''

''' PSEUDOCODE 
def hash(input_string: str):
    result = 0xDEADBEEF
    for i, v in enumerate(input_string.encode()):
        temp = (result << i) ^ (v & result) | (i << (v & 0x3))
        temp <<= (v mod 2)
        result ^= temp
    return ((result * 2654435761) & 0xFFFFFFFF) ^ (result & 0x2654435761)

'''

def rol(x, n): return ((x << n) | (x >> (32 - n))) & 0xFFFFFFFF

def hash(input_string: str) -> int:
    assert(len(input_string)) < 16
    
    b32 = lambda x: x & 0xFFFFFFFF
    b16 = lambda x: x & 0xFFFF
    b4  = lambda x: x & 0xF
    
    state = [0xDEADBEEF, 0xABCDEF01, 0xAA5555AA, 0xC0DE9999]
    
    for i, v in enumerate(input_string.encode()):
        state[0] = (v & state[0]) ^ (rol(v, i & 0x3))
        state[1] = b32(v * 0x2654) ^ (rol(state[1], v & i))
        state[2] = state[2] ^ b32(v * 54435761)
        state[3] = b32(v * 2654435761) ^ rol(b32(v ^ 0x8AA8A88A), i)
        
        if i % 2 == 0:
            state[0] = b32(2654435761 ^ state[1] * (state[2] & state[3]))
            state[1] = b32(state[0] * state[2] ^ rol(state[3], b4(state[1])))
            state[2] = b32(2654435761 ^ state[3] & (rol(state[1], b4(state[2]))))
            state[3] = b32(state[3] ^ rol(2654435761, b4(state[2])) ^ state[0])
            
    return b32(state[0] ^ state[1] ^ state[2] ^ state[3])
'''
Arduino C code

#include <stdint.h>
#include <stddef.h>

uint32_t easyhash(const char *input, size_t len) {
    uint32_t result = 0xDEADBEEF;

    for (size_t i = 0; i < len; i++) {
        uint8_t v = (uint8_t)input[i];
        uint32_t safe_i = i & 31;  // avoid undefined shifts

        uint32_t temp = (result << safe_i) ^ (v & result) | (safe_i << (v & 0x3));
        temp <<= (v % 2);

        result ^= temp;
    }

    return ((result * 2654435761UL) & 0xFFFFFFFFUL) ^ (result & 0x2654435761UL);
}

'''
    
def hash_test() -> None:
    hashes = []
    for i in range(1000000):
        s = random_printable_string(8)
        h = hash(s)
        print(i)
        if s not in hashes:
            hashes.append(h)
        elif s in hashes:
            raise Exception("Collision found!")

if __name__ == "__main__":
    hash_test()
