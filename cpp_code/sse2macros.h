#include <emmintrin.h>



// SSE2 macros for better readability.
// name(type x, type y) -> return_type
// SSE2LOD(u64 x, u64 y) -> u128
#define SSE2LOD(x, y) _mm_set_epi64x(x, y)
// SSE2FIL(u64 x) -> u128
#define SSE2FIL(x) _mm_set1_epi64x(x)
// SSE2ADD(u128 x, u128 y) -> u128
#define SSE2ADD(x, y) _mm_add_epi64(x, y)
// SSE2OR(u128 x, u128 y) -> u128
#define SSE2OR(x, y) _mm_or_si128(x, y)
// SSE2XOR(u128 x, u128 y) -> u128
#define SSE2XOR(x, y) _mm_xor_si128(x, y)
// SSE2LSH(u128 x, int y) -> u128
#define SSE2RSH(x, n) _mm_srli_epi64(x, n)
// SSE2RSH(u128 x, int y) -> u128
#define SSE2LSH(x, n) _mm_slli_epi64(x, n)
// SSE2STO(u64* x, u128 y) -> void
#define SSE2STO(x, y) _mm_store_si128((__m128i*)x, y)

