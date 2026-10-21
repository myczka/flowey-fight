#include "util.h"

#include <chrono>
#include <random>
#include <thread>

int getRand(int first, int last) {
    // Seeded once on the first call, then reused. One generator per thread.
    static thread_local std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<int> dist(first, last);
    return dist(gen);
}

void sleep(int delayMs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
}