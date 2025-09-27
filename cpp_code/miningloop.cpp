#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdint>

using u64 = uint64_t;

u64 eh_hashu64(u64 input);

bool check_difficulty(u64 hash, int difficulty_bits) {
    // Top 'difficulty_bits' must be zero
    u64 mask = (1ULL << (64 - difficulty_bits)) - 1;
    u64 upper_mask = ~mask;
    return (hash & upper_mask) == 0;
}

struct MiningResult {
    u64 hash;
    u64 nonce;
    double hashrate; // hashes per second
};

MiningResult mine(u64 hash_input, int difficulty) {
    u64 nonce = 0;
    u64 hashes_done = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    while (true) {
        u64 hash = eh_hashu64(hash_input ^ nonce);
        hashes_done++;

        if (check_difficulty(hash, difficulty)) {
            auto end_time = std::chrono::high_resolution_clock::now();
            double seconds = std::chrono::duration<double>(end_time - start_time).count();
            double hashrate = hashes_done / seconds;

            return {hash, nonce, hashrate};
        }

        nonce++;
    }
}

int main() {
    u64 hash_input = 0x019590326;
    int difficulty = 24;

    MiningResult result = mine(hash_input, difficulty);

    std::cout << "Input: 0x"<< std::setw(16) << std::setfill('0') << std::hex << hash_input << "\n";
    std::cout << "Found hash: 0x"<< std::setw(16) << std::setfill('0') << std::hex << result.hash << "\n";
    std::cout << "Nonce: " << std::dec << result.nonce << "\n";
    std::cout << "Hashrate: " << result.hashrate / 1000000 << " MH/s\n";

    return 0;
}