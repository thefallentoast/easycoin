#include <iostream>
#ifdef _MSC_VER
    #include <intrin.h>
#else
    #include <cpuid.h>
#endif

void check_simd() {
    unsigned int eax, ebx, ecx, edx;

#ifdef _MSC_VER
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    eax = cpuInfo[0]; ebx = cpuInfo[1]; ecx = cpuInfo[2]; edx = cpuInfo[3];
#else
    if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        std::cout << "CPUID not supported\n";
        return;
    }
#endif

    std::cout << "CPU SIMD support:\n";

    std::cout << "SSE:      " << ((edx & (1 << 25)) ? "Yes" : "No") << "\n";
    std::cout << "SSE2:     " << ((edx & (1 << 26)) ? "Yes" : "No") << "\n";
    std::cout << "SSE3:     " << ((ecx & (1 << 0)) ? "Yes" : "No") << "\n";
    std::cout << "SSSE3:    " << ((ecx & (1 << 9)) ? "Yes" : "No") << "\n";
    std::cout << "SSE4.1:   " << ((ecx & (1 << 19)) ? "Yes" : "No") << "\n";
    std::cout << "SSE4.2:   " << ((ecx & (1 << 20)) ? "Yes" : "No") << "\n";
    std::cout << "AVX:      " << ((ecx & (1 << 28)) ? "Yes" : "No") << "\n";
    std::cout << "AVX2:     ";
#ifdef __GNUC__
    // AVX2 is bit 5 of EBX in CPUID leaf 7, subleaf 0
    unsigned int eax7, ebx7, ecx7, edx7;
    if (__get_cpuid_count(7, 0, &eax7, &ebx7, &ecx7, &edx7))
        std::cout << ((ebx7 & (1 << 5)) ? "Yes" : "No") << "\n";
    else
        std::cout << "Unknown\n";
#else
    std::cout << "Unknown\n";
#endif
}

int main() {
    check_simd();
    return 0;
}
