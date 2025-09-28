#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdint>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <bitset>
#include <immintrin.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __linux__
#include <csignal>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include "avx512macros.h"

#ifdef _MSC_VER
#define ALIGN64 __declspec(align(64))
#else
#define ALIGN64 __attribute__((aligned(64)))
#endif

using u64 = uint64_t;
using u512 = __m512i;

u512 eh_hashu512(u512 input);

std::atomic<bool> global_stop(false);

struct FoundHash {
    u64 hash;
    int idx; // Figure out if it's the first or second hash, in AVX512 it will be 0-7
    bool found;
};

struct MiningResult {
    u64 hash;
    u64 nonce;
    double hashrate; // hashes per second
};

static inline FoundHash check_difficulty(u512 hashes, u64 mask) {
    ALIGN64 u64 split_hashes[8];
    u512 filled = AVX512FIL(mask);
    int lane_mask = AVX512CMP(AVX512AND(filled, hashes), AVX512ZERO);
    //std::cout << std::bitset<4>(lane_mask) << "\n";
    if (lane_mask != 0) {
        int idx = __builtin_ctz(lane_mask);
        AVX512STO(split_hashes, hashes);
        /*for (int i = 0; i < 4; i++) {
            std::cout << "Hash " << i << (((lane_mask >> i) & 1) ? " found:" : ":") << " 0x" << std::setw(16) << std::setfill('0') << std::hex << split_hashes[i] << "\n";
        }*/
        return {split_hashes[idx], idx, true};
    }
    return {0, 0, false};
}

// Note: this function is provided legacy, as it is not used. As such,
// DEPRECATED (use mine_thread for single-thread and mine_threaded for multiple).
// This function also lacks some optimizations like loop unrolling present in mine_thread.
MiningResult mine(u64 hash_input, int difficulty_bits) {
    u64 nonce = 0;
    u64 hashes_done = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    #define hi_n (hash_input ^ nonce)
    u512 hash_function_input = AVX512LOD(hi_n, hi_n+1, hi_n+2, hi_n+3, hi_n+4, hi_n+5, hi_n+6, hi_n+7);
    u64 mask = ~(((0xFFFFFFFFFFFFFFFFULL) << (64 - difficulty_bits)) - 1);
    FoundHash found_hash;

    while (true) {
        u512 hashes = eh_hashu512(hash_function_input);
        hashes_done += 8;

        if ((found_hash = check_difficulty(hashes, mask)).found) {
            auto end_time = std::chrono::high_resolution_clock::now();
            double seconds = std::chrono::duration<double>(end_time - start_time).count();
            double hashrate = hashes_done / seconds;

            return {found_hash.hash, nonce + found_hash.idx, hashrate};
        }

        hash_function_input = AVX512ADD(hash_function_input, AVX512FIL(2));
    }
}

MiningResult mine_thread(u64 hash_input, int difficulty_bits, int offset, int increment, std::atomic<bool>& stop_signal) {
    u64 nonce = offset;
    int nonce_increment = increment * 8;
    u64 hashes_done = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    #define hi_n (hash_input ^ nonce)
    u512 hash_function_input = AVX512LOD(hi_n, hi_n+1, hi_n+2, hi_n+3, hi_n+4, hi_n+5, hi_n+6, hi_n+7);
    u64 mask = ~((1ULL << (64 - difficulty_bits)) - 1);
    //std::cout << "Mask: " << std::setw(16) << std::setfill('0') << std::hex << mask << "\n";
    FoundHash found_hash;
    u512 hashes;

    while (!stop_signal.load(std::memory_order_relaxed) && !global_stop.load(std::memory_order_relaxed)) {
        
        // 64 iterations (64*4*8=2048 hashes per memory load)
        for (int i = 0; i < 64; i++) {
            // Unroll 4 times
            hashes = eh_hashu512(hash_function_input);
            hashes_done += 8;

            if ((found_hash = check_difficulty(hashes, mask)).found) {
                /*std::cout << "Found index: " << found_hash.idx << "\n";
                std::cout << "Found hash: " << found_hash.hash << "\n";*/
                auto end_time = std::chrono::high_resolution_clock::now();
                double seconds = std::chrono::duration<double>(end_time - start_time).count();
                double hashrate = hashes_done / seconds;

                stop_signal.store(true);

                return {found_hash.hash, nonce + found_hash.idx, hashrate};
            }

            hash_function_input = AVX512ADD(hash_function_input, AVX512FIL(nonce_increment));
            hashes = eh_hashu512(hash_function_input);
            hashes_done += 8;

            if ((found_hash = check_difficulty(hashes, mask)).found) {
                auto end_time = std::chrono::high_resolution_clock::now();
                double seconds = std::chrono::duration<double>(end_time - start_time).count();
                double hashrate = hashes_done / seconds;

                stop_signal.store(true);

                return {found_hash.hash, nonce + found_hash.idx, hashrate};
            }

            hash_function_input = AVX512ADD(hash_function_input, AVX512FIL(nonce_increment));
            hashes = eh_hashu512(hash_function_input);
            hashes_done += 8;

            if ((found_hash = check_difficulty(hashes, mask)).found) {
                auto end_time = std::chrono::high_resolution_clock::now();
                double seconds = std::chrono::duration<double>(end_time - start_time).count();
                double hashrate = hashes_done / seconds;

                stop_signal.store(true);

                return {found_hash.hash, nonce + found_hash.idx, hashrate};
            }

            hash_function_input = AVX512ADD(hash_function_input, AVX512FIL(nonce_increment));
            hashes = eh_hashu512(hash_function_input);
            hashes_done += 8;

            if ((found_hash = check_difficulty(hashes, mask)).found) {
                auto end_time = std::chrono::high_resolution_clock::now();
                double seconds = std::chrono::duration<double>(end_time - start_time).count();
                double hashrate = hashes_done / seconds;

                stop_signal.store(true);

                return {found_hash.hash, nonce + found_hash.idx, hashrate};
            }

            hash_function_input = AVX512ADD(hash_function_input, AVX512FIL(nonce_increment));
            
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(end_time - start_time).count();
    double hashrate = hashes_done / seconds;

    // Return a default result if stopped before finding
    return {0xFFFFFFFFFFFFFFFF, 0, hashrate};
}

MiningResult mine_threaded(u64 hash_input, int difficulty, int num_threads) {
    std::vector<std::thread> threads;
    std::vector<MiningResult> results(num_threads);
    std::atomic<bool> stop_signal(false);

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

#ifdef __linux__
void signal_handler(int signum) {
    global_stop.store(true);
    std::cout << "Stopping. \n";
}
#elif defined(_WIN32)
BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        global_stop.store(true);
        return TRUE;
    }
    return FALSE;
}
#endif

int main() {
    u64 hash_input = 0x07895173F;
    int difficulty = 22;

    #ifdef __linux__
    std::signal(SIGINT, signal_handler);
    #elif defined(_WIN32)
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    #endif

    /*std::cout << "Hash test\n";
    u512 hashes = eh_hashu512(AVX512FIL(0xFFFF));
    ALIGN64 u64 hashes_split[4]; AVX512STO(hashes_split, hashes);
    std::cout << "Raw hashes: ";
    for (int i = 0; i < 4; i++)
        std::cout << std::hex << hashes_split[i] << " ";
    std::cout << "\n";

    FoundHash check = check_difficulty(hashes, 0xC000000000000000ULL);
    std::cout << "Do these fit? " << check.found << "\n";
    */

    bool local_stop = false;
    while (!local_stop) {
        local_stop = global_stop.load(std::memory_order_relaxed);
        std::cout << "Mining test\n"; 

        MiningResult result = mine_threaded(hash_input, difficulty, 8);

        std::cout << "Multi-threaded (8): \n";
        std::cout << "Input:      0x"<< std::setw(16) << std::setfill('0') << std::hex << hash_input << "\n";
        std::cout << "Found hash: 0x"<< std::setw(16) << std::setfill('0') << std::hex << result.hash << "\n";
        std::cout << "Nonce: " << std::dec << result.nonce << "\n";
        std::cout << "Hashrate: " << result.hashrate / 1000000 << " MH/s\n";

        if ((hash_input == 0) && (result.hash == 0)) return 0;

        hash_input = result.hash;

        std::cout << "Should" << (local_stop ? " " : "n't ") << "stop.\n";
    }
    return 0;
}

#ifdef __cplusplus
}
#endif