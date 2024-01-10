#pragma once

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

namespace RDTSC_Clock {
    namespace internal {
        extern const double RDTSC_TICK_FREQ;
        extern uint64_t initialTimestamp;
        extern unsigned long long initialCycles;
    }
    void init();
    unsigned long long now(unsigned long long elapsedCycles);
}

