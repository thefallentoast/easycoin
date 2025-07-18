#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <iostream>
//#include <chrono>
#include <iomanip>  // for std::fixed and std::setprecision
#include <cstdint>
#include <windows.h>
#include <cstring>
#include <thread>
#include <vector>
#include <errno.h>
#include <atomic>

extern "C" struct Result_u64 {
    std::atomic<bool> found{false};
    std::atomic<uint64_t> nonce{0};
    std::atomic<uint64_t> hash{0xFFFFFFFFFFFFFFFF};
    std::atomic<uint64_t> hashcount{0};
};

extern "C" uint64_t easyhash_u64(uint64_t last_hash, uint64_t nonce);

extern "C" void mine_thread_u64(uint64_t difficulty, uint64_t previous_hash, uint64_t start_nonce, uint64_t step, Result_u64& result_u64) {
    //using namespace std::chrono;

    //auto start_time = steady_clock::now();
    uint64_t h = 0xDEADBEEF;
    uint64_t hash_count = 0;
    uint64_t nonce = start_nonce;

    while (h > difficulty && !result_u64.found.load() && __builtin_expect(nonce < UINT64_MAX - step, 1)) {
        h = easyhash_u64(previous_hash, nonce);
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
    if (!result_u64.found.exchange(true) && __builtin_expect(nonce < UINT64_MAX - step, 1)) {  // Only first thread to find result sets this
        result_u64.nonce.store(nonce);
        result_u64.hash.store(h);
    }
    result_u64.hashcount.fetch_add(hash_count); // Add the hashcount to the total
    
    return;  // Exit thread once found

    /*std::cout << "Found: 0x" << std::hex << h 
              << " || Difficulty: 0x" << difficulty 
              << " || Nonce: " << std::dec << nonce - 1 << std::endl << std::flush;*/

    /*
    if (out_nonce) *out_nonce = nonce - 1;
    if (out_hashcount) *out_hashcount = hash_count;
    if (out_hash) *out_hash = h;*/
    
}

extern "C" EXPORT void mine_u64(uint64_t difficulty, uint64_t previous_hash, uint64_t num_threads, uint64_t* out_nonce, uint64_t* out_hash, uint64_t* out_hashcount) {
    Result_u64 result_u64;
    std::vector<std::thread> threads;

    int hardware_concurrency = std::thread::hardware_concurrency();

    if (num_threads == 0) {
        num_threads = hardware_concurrency;
    }

    for (uint64_t i = 0; i < num_threads; ++i) {
        // Each thread starts at nonce = i and increments by num_threads (work division)
        #ifdef __linux__
        threads.emplace_back(mine_thread_u64, difficulty, previous_hash, i, num_threads, std::ref(result_u64));
        #elif defined(_WIN32)
        threads.emplace_back([=, &result_u64]() {
            // Optional: print which thread this is
            //std::cout << "Thread " << i << " started\n";

            // Set affinity
            DWORD_PTR mask = 1ull << (i % std::thread::hardware_concurrency());
            HANDLE handle = GetCurrentThread();
            SetThreadAffinityMask(handle, mask);

            // Run actual mining logic
            mine_thread_u64(difficulty, previous_hash, i, num_threads, result_u64);
            });
        #endif
    

#ifdef __linux__
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        CPU_SET(i % hardware_concurrency, &cpuset);

        pthread_t handle = threads.back().native_handle();
        int rc = pthread_setaffinity_np(handle, sizeof(cpu_set_t), &cpuset);
        if (rc != 0) {
            std::cerr << "Failed to set affinity for thread " << i << ": " << std::strerror(rc) << "\n";
        }
#endif
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    *out_nonce = result_u64.nonce.load();
    *out_hash = result_u64.hash.load();
    *out_hashcount = result_u64.hashcount.load();
}