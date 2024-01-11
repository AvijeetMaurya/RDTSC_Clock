#pragma once

#include <chrono>
#include <cstdint>

namespace RDTSC_Clock {
    namespace internal {
        extern double RDTSC_TICK_FREQ;
        extern unsigned long long timestamp;
    }
    void init();
    unsigned long long now();
    void exit();
}

