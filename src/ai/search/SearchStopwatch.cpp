#include "SearchStopwatch.h"

#include <chrono>

SearchStopwatch::SearchStopwatch(int timeLimitMs)
    : timeLimit(timeLimitMs) {}

void SearchStopwatch::reset() {
    startTime = std::chrono::steady_clock::now();
    stopped.store(false, std::memory_order_relaxed);
}

void SearchStopwatch::overrideAndAbort() {
    stopped.store(true, std::memory_order_relaxed);
}

bool SearchStopwatch::isStopped() {
    if (stopped.load(std::memory_order_relaxed)) {
        return true;
    }

    if (std::chrono::steady_clock::now() - startTime >= timeLimit) {
        stopped.store(true, std::memory_order_relaxed);
        return true;
    }

    return false;
}
