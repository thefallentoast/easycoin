#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>  // for std::fixed and std::setprecision
#include <cstdint>

// Assume you have this function implemented & linked properly
extern "C" uint32_t easyhash_u32pair(uint32_t last_hash, uint32_t nonce);

// Mining function similar to Python mine()
struct MineResult {
    uint32_t hash;
    uint32_t nonce;
};

uint32_t mine(uint32_t difficulty, uint32_t previous_hash) {
    using namespace std::chrono;

    auto start_time = steady_clock::now();
    uint32_t h = 0xDEADBEEF;
    uint64_t hash_count = 0;
    uint32_t nonce = 0;

    while (h > difficulty) {
        h = easyhash_u32pair(previous_hash, nonce);
        nonce++;
        hash_count++;

        auto now = steady_clock::now();
        duration<double> elapsed = now - start_time;
        if (elapsed.count() >= 1.0) {
            double mhashrate = hash_count / 1000000.0 / elapsed.count();
            std::cout << "Hashrate: " << std::fixed << std::setprecision(3)
                      << khashrate << " MH/s" << std::endl;
            // Reset timer and counter
            start_time = now;
            hash_count = 0;
        }
    }

    std::cout << "Found: 0x" << std::hex << h 
              << " || Difficulty: 0x" << difficulty 
              << " || Nonce: " << std::dec << nonce << std::endl;

    return nonce;
}

int main() {
    uint32_t difficulty = 0x0000000F;
    uint32_t previous_hash = 0x12345678;

    MineResult res1 = mine(difficulty, previous_hash);
    MineResult res2 = mine(difficulty, res1.hash);

    std::cout << "Chained result: Found hash 0x" << std::hex << res2.hash
              << " with nonce " << std::dec << res2.nonce << std::endl;

    return 0;
}
