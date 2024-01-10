#pragma once

#include <chrono>
#include <cstdint>

namespace RDTSC_Clock {
    namespace internal {
        extern double RDTSC_TICK_FREQ;
        extern unsigned long long initialTimestamp;
        extern unsigned long long initialCycles;
    }
    void init();
    unsigned long long now(unsigned long long elapsedCycles);
    void exit();
}

