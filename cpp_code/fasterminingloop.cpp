#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdint>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <immintrin.h>

#include "avx2macros.h"

using u64 = uint64_t;
using u256 = __m256i;

u256 eh_hashu256(u256 input);

struct FoundHash {
    u64 hash;
    int idx; // Figure out if it's the first or second hash, in AVX2 it will be 0-3
    bool found;
};

struct MiningResult {
    u64 hash;
    u64 nonce;
    double hashrate; // hashes per second
};

FoundHash check_difficulty(u256 hashes, u64 mask) {
    u64 outputs[4];
    u64 hashes_split[4];
    AVX2STO(hashes_split, hashes);
    AVX2STO(outputs, AVX2CGT(AVX2FIL(mask), hashes));
    if        (outputs[0]) {
        return {hashes_split[0], 0, true};
    } else if (outputs[1]) {
        return {hashes_split[1], 1, true};
    } else if (outputs[2]) {
        return {hashes_split[2], 2, true};
    } else if (outputs[3]) {
        return {hashes_split[3], 3, true};
    } else {
        return {0, 0, false};
    }
}

// Note: this function is provided legacy, as it is not used. As such,
// DEPRECATED (use mine_thread for single-thread and mine_threaded for multiple).
MiningResult mine(u64 hash_input, int difficulty_bits) {
    u64 nonce = 0;
    u64 hashes_done = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    u256 hash_function_input = AVX2LOD(hash_input ^ nonce, (hash_input ^ nonce) + 1, (hash_input ^ nonce) + 2, (hash_input ^ nonce) + 3);
    u64 mask = ~((1ULL << (64 - difficulty_bits)) - 1);
    FoundHash found_hash;

    while (true) {
        u256 hashes = eh_hashu256(hash_function_input);
        hashes_done += 4;

        if ((found_hash = check_difficulty(hashes, mask)).found) {
            auto end_time = std::chrono::high_resolution_clock::now();
            double seconds = std::chrono::duration<double>(end_time - start_time).count();
            double hashrate = hashes_done / seconds;

            return {found_hash.hash, nonce + found_hash.idx, hashrate};
        }

        hash_function_input = AVX2ADD(hash_function_input, AVX2FIL(2));
    }
}

MiningResult mine_thread(u64 hash_input, int difficulty_bits, int offset, int increment, std::atomic<bool>& stop_signal) {
    u64 nonce = offset;
    int nonce_increment = increment * 4;
    u64 hashes_done = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    u256 hash_function_input = AVX2LOD(hash_input ^ nonce, (hash_input ^ nonce) + 1, (hash_input ^ nonce) + 2, (hash_input ^ nonce) + 3);
    u64 mask = ~((1ULL << (64 - difficulty_bits)) - 1);
    FoundHash found_hash;

    while (!stop_signal.load(std::memory_order_relaxed)) {
        u256 hashes = eh_hashu256(hash_function_input);
        hashes_done += 4;

        if ((found_hash = check_difficulty(hashes, mask)).found) {
            auto end_time = std::chrono::high_resolution_clock::now();
            double seconds = std::chrono::duration<double>(end_time - start_time).count();
            double hashrate = hashes_done / seconds;

            return {found_hash.hash, nonce + found_hash.idx, hashrate};
        }

        hash_function_input = AVX2ADD(hash_function_input, AVX2FIL(nonce_increment));
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(end_time - start_time).count();
    double hashrate = hashes_done / seconds;

    // Return a default result if stopped before finding
    return {0xFFFFFFFFFFFFFFFF, 0, hashrate};
}

MiningResult mine_threaded(u64 hash_input, int difficulty, int num_threads) {
    std::atomic<bool> stop_signal(false);
    std::vector<std::thread> threads;
    std::vector<MiningResult> results(num_threads);

    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([&, i]() {
            // Offset each thread's starting nonce so they don't overlap
            results[i] = mine_thread(hash_input, difficulty, i, num_threads, stop_signal);
        });
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    double final_hashrate = 0.0;
    MiningResult final_result = {0, 0, 0.0};

    // Find the result that isn't zero
    for (const auto& res : results) {
        final_hashrate += res.hashrate;
        if (res.hash != 0xFFFFFFFFFFFFFFFF) {  // <-- check which thread actually found the solution
            final_result = res;
        }
    }

    final_result.hashrate = final_hashrate;
    return final_result;
}

int main() {
    u64 hash_input = 0x019590326;
    int difficulty = 24;

    std::cout << "Mining test\n"; 

    MiningResult result = mine_threaded(hash_input, difficulty, 8);

    std::cout << "Single-threaded: \n";
    std::cout << "Input:      0x"<< std::setw(16) << std::setfill('0') << std::hex << hash_input << "\n";
    std::cout << "Found hash: 0x"<< std::setw(16) << std::setfill('0') << std::hex << result.hash << "\n";
    std::cout << "Nonce: " << std::dec << result.nonce << "\n";
    std::cout << "Hashrate: " << result.hashrate / 1000000 << " MH/s\n";

    return 0;
}