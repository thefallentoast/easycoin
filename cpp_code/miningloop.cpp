#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdint>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>

#ifdef __cplusplus
extern "C" {
#endif

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

// Note: this function is provided legacy, as it is not used. As such,
// DEPRECATED (use mine_thread for single-thread and mine_threaded for multiple).
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

MiningResult mine_thread(u64 hash_input, int difficulty, int offset, int increment, std::atomic<bool>& stop_signal) {
    u64 nonce = offset;
    u64 hashes_done = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    while (!stop_signal.load(std::memory_order_relaxed)) {
        u64 hash = eh_hashu64(hash_input ^ nonce);
        hashes_done++;

        if (check_difficulty(hash, difficulty)) {
            // Notify all threads to stop
            stop_signal.store(true, std::memory_order_relaxed);

            auto end_time = std::chrono::high_resolution_clock::now();
            double seconds = std::chrono::duration<double>(end_time - start_time).count();
            double hashrate = hashes_done / seconds;

            return {hash, nonce, hashrate};
        }

        nonce += increment;
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

    MiningResult result = mine_threaded(hash_input, difficulty, 8);

    std::cout << "Multi-threaded: " << 8 << " threads: \n";
    std::cout << "Input:      0x"<< std::setw(16) << std::setfill('0') << std::hex << hash_input << "\n";
    std::cout << "Found hash: 0x"<< std::setw(16) << std::setfill('0') << std::hex << result.hash << "\n";
    std::cout << "Nonce: " << std::dec << result.nonce << "\n";
    std::cout << "Hashrate: " << result.hashrate / 1000000 << " MH/s\n";

    result = mine_threaded(hash_input, difficulty, 1);

    std::cout << "Single-threaded: \n";
    std::cout << "Input:      0x"<< std::setw(16) << std::setfill('0') << std::hex << hash_input << "\n";
    std::cout << "Found hash: 0x"<< std::setw(16) << std::setfill('0') << std::hex << result.hash << "\n";
    std::cout << "Nonce: " << std::dec << result.nonce << "\n";
    std::cout << "Hashrate: " << result.hashrate / 1000000 << " MH/s\n";

    return 0;
}

#ifdef __cplusplus
}
#endif