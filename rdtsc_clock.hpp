#pragma once

#include <chrono>
#include <cstdint>

namespace RDTSC_Clock {
    namespace internal {
        extern double RDTSC_TICK_FREQ;
        struct InitialState {
        unsigned long long timestamp;
        unsigned long long cycles;
        };
        static_assert(sizeof(InitialState) <= 16);
        extern InitialState initialState;
    }
    void init();
    unsigned long long now(unsigned long long elapsedCycles);
    void exit();
}

