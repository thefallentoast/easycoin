#include <immintrin.h>


// AVX512 macros for better readability.
// name(type x, type y) -> return_type
// AVX512LOD(u64 a, u64 b, u64 c, u64 d, u64 w, u64 x, u64 y, u64 z) -> u512
#define AVX512LOD(a, b, c, d, w, x, y, z) _mm512_set_epi64(a, b, c, d, w, x, y, z)
// AVX512FIL(u64 x) -> u512
#define AVX512FIL(x) _mm512_set1_epi64(x)
// AVX512ADD(u512 x, u512 y) -> u512
#define AVX512ADD(x, y) _mm512_add_epi64(x, y)
// AVX512AND(u512 x, u512 y) -> u512
#define AVX512AND(x, y) _mm512_and_si512(x, y)
// AVX512OR(u512 x, u512 y) -> u512
#define AVX512OR(x, y) _mm512_or_si512(x, y)
// AVX512XOR(u512 x, u512 y) -> u512
#define AVX512XOR(x, y) _mm512_xor_si512(x, y)
// AVX512LSH(u512 x, int y) -> u512
#define AVX512RSH(x, n) _mm512_srli_epi64(x, n)
// AVX512RSH(u512 x, int y) -> u512
#define AVX512LSH(x, n) _mm512_slli_epi64(x, n)
// AVX512STO(u64* x, u512 y) -> void
#define AVX512STO(x, y) _mm512_store_si512((__m512i*)x, y)
// AVX512CMP(u512 x, u512 y) -> u512
#define AVX512CMP(x, y) _mm512_cmpeq_epi64_mask(x, y)
// AVX512CGT(u512 x, u512 y) -> u512
#define AVX512CGT(x, y) _mm512_cmpgt_epi64(x, y)

// Weirder, more specific ones
// AVX512ZERO -> u512
#define AVX512ZERO _mm512_setzero_si512()
// AVX512MOVMSKPD(u512 x) -> int
#define AVX512MOVEMASKPD(x) _mm512_movemask_pd(x)
// AVX512CASTSIPD(u512 x) -> d512
#define AVX512CASTSIPD(x) _mm512_castsi512_pd(x)
// AVX512EXTRACT(u512 x, const int y) -> u64
#define AVX512EXTRACT(x, y) _mm512_extract_epi64(x, y)