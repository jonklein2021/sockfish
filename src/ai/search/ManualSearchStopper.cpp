#include "ManualSearchStopper.h"

void ManualSearchStopper::reset() {
    stopped.store(false, std::memory_order_relaxed);
}

void ManualSearchStopper::overrideAndAbort() {
    stopped.store(true, std::memory_order_relaxed);
}

bool ManualSearchStopper::isStopped() {
    return stopped.load(std::memory_order_relaxed);
}
