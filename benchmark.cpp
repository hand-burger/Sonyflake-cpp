#include <iostream>
#include <thread>
#include <chrono>
#include "sonyflake.h"

int main() {
    Sonyflake sf(1);
    const int N = 100000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        sf.GetUniqueId();
    }

    auto end = std::chrono::high_resolution_clock::now();
    double secs = std::chrono::duration<double>(end - start).count();

    std::cout << "Generated " << N << " IDs in " << secs << " seconds\n";
    std::cout << "Throughput: " << (N / secs) / 1e3 << " thousand IDs/sec\n";
}
