#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <iostream>
//#include <chrono>
#include <iomanip>  // for std::fixed and std::setprecision
#include <cstdint>
#include <thread>
#include <vector>
#include <atomic>

extern "C" struct Result_u32 {
    std::atomic<bool> found{false};
    std::atomic<uint32_t> nonce{0};
    std::atomic<uint32_t> hash{0xFFFFFFFF};
    std::atomic<uint32_t> hashcount{0};
};

// Assume you have this function implemented & linked properly
extern "C" uint32_t easyhash_u32(uint32_t last_hash, uint32_t nonce);

extern "C" void mine_thread_u32(uint32_t difficulty, uint32_t previous_hash, uint32_t start_nonce, uint32_t step, Result_u32& result_u32) {
    //using namespace std::chrono;

    //auto start_time = steady_clock::now();
    uint32_t h = 0xDEADBEEF;
    uint32_t hash_count = 0;
    uint32_t nonce = start_nonce;
    double mhashrate = 0;

    while (h > difficulty && !result_u32.found.load() && __builtin_expect(nonce < UINT32_MAX - step, 1)) {
        h = easyhash_u32(previous_hash, nonce);
        hash_count++;

        /*auto now = steady_clock::now();
        duration<double> elapsed = now - start_time;
        if (elapsed.count() >= 1.0) {
            mhashrate = hash_count / 1000000.0 / elapsed.count();
            std::cout << "Hashrate: " << std::fixed << std::setprecision(3)
                      << mhashrate << " MH/s" << std::endl << std::flush;
            // Reset timer and counter
            start_time = now;
            hash_count = 0;
        }*/

        nonce += step;
        
    }
    if (!result_u32.found.exchange(true) && __builtin_expect(nonce < UINT32_MAX - step, 1)) {  // Only first thread to find result sets this
        result_u32.nonce.store(nonce);
        result_u32.hash.store(h);
    }
    result_u32.hashcount.fetch_add(hash_count); // Add the hashcount to the total
    
    return;  // Exit thread once found

    /*std::cout << "Found: 0x" << std::hex << h 
              << " || Difficulty: 0x" << difficulty 
              << " || Nonce: " << std::dec << nonce - 1 << std::endl << std::flush;*/

    /*
    if (out_nonce) *out_nonce = nonce - 1;
    if (out_hashcount) *out_hashcount = hash_count;
    if (out_hash) *out_hash = h;*/
    
}

extern "C" EXPORT void mine_u32(uint32_t difficulty, uint32_t previous_hash, uint32_t num_threads, uint32_t* out_nonce, uint32_t* out_hash, uint32_t* out_hashcount) {
    Result_u32 result_u32;
    std::vector<std::thread> threads;

    for (uint32_t i = 0; i < num_threads; ++i) {
        // Each thread starts at nonce = i and increments by num_threads (work division)
        threads.emplace_back(mine_thread_u32, difficulty, previous_hash, i, num_threads, std::ref(result_u32));
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    *out_nonce = result_u32.nonce.load();
    *out_hash = result_u32.hash.load();
    *out_hashcount = result_u32.hashcount.load();
}

int main() {
    uint32_t difficulty = 0x00000007;
    uint32_t previous_hash = 0x6;

    uint32_t out_nonce_1 = 0;
    uint32_t out_nonce_2 = 0;

    uint32_t out_hash_1 = 0;
    uint32_t out_hash_2 = 0;

    uint32_t out_mhashrate_1 = 0;
    uint32_t out_mhashrate_2 = 0;

    mine_u32(difficulty, previous_hash, 8, &out_nonce_1, &out_hash_1, &out_mhashrate_1);
    mine_u32(difficulty, out_hash_1, 8, &out_nonce_2, &out_hash_2, &out_mhashrate_2);

    std::cout << "Chained result_u32: Found hash 0x" << std::hex << out_hash_2
              << " with nonce " << std::dec << out_nonce_2 << std::endl;
    std::cout << "Input to previous was hash 0x" << std::hex << out_hash_1
              << " with nonce to find it " << std::dec << out_nonce_1 << std::endl;
    std::cout << "Hash counts: " << std::dec << out_mhashrate_1 << " || "
              << std::dec << out_mhashrate_2 << std::endl;

    return 0;
}
