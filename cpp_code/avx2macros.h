#include <immintrin.h>


// AVX2 macros for better readability.
// name(type x, type y) -> return_type
// AVX2LOD(u64 x, u64 y) -> u256
#define AVX2LOD(w, x, y, z) _mm256_set_epi64x(w, x, y, z)
// AVX2FIL(u64 x) -> u256
#define AVX2FIL(x) _mm256_set1_epi64x(x)
// AVX2ADD(u256 x, u256 y) -> u256
#define AVX2ADD(x, y) _mm256_add_epi64(x, y)
// AVX2AND(u256 x, u256 y) -> u256
#define AVX2AND(x, y) _mm256_and_si256(x, y)
// AVX2OR(u256 x, u256 y) -> u256
#define AVX2OR(x, y) _mm256_or_si256(x, y)
// AVX2XOR(u256 x, u256 y) -> u256
#define AVX2XOR(x, y) _mm256_xor_si256(x, y)
// AVX2LSH(u256 x, int y) -> u256
#define AVX2RSH(x, n) _mm256_srli_epi64(x, n)
// AVX2RSH(u256 x, int y) -> u256
#define AVX2LSH(x, n) _mm256_slli_epi64(x, n)
// AVX2STO(u64* x, u256 y) -> void
#define AVX2STO(x, y) _mm256_store_si256((__m256i*)x, y)
// AVX2CMP(u256 x, u256 y) -> u256
#define AVX2CMP(x, y) _mm256_cmpeq_epi64(x, y)
// AVX2CGT(u256 x, u256 y) -> u256
#define AVX2CGT(x, y) _mm256_cmpgt_epi64(x, y)

// Weirder, more specific ones
// AVX2ZERO -> u256
#define AVX2ZERO _mm256_setzero_si256()
// AVX2MOVMSKPD(u256 x) -> int
#define AVX2MOVEMASKPD(x) _mm256_movemask_pd(x)
// AVX2CASTSIPD(u256 x) -> d256
#define AVX2CASTSIPD(x) _mm256_castsi256_pd(x)
// AVX2EXTRACT(u256 x, const int y) -> u64
#define AVX2EXTRACT(x, y) _mm256_extract_epi64(x, y)