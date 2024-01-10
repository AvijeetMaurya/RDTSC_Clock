#include <iostream>
#include <semaphore>
#include <thread>

#include <x86intrin.h>
#include <cpuid.h>

#include "rdtsc_clock.hpp"

namespace RDTSC_Clock {
    static double GET_RDTSC_TICK_FREQ() {
        unsigned int eax_denominator, ebx_numerator, ecx_hz, edx;
        __get_cpuid(0x15, &eax_denominator, &ebx_numerator, &ecx_hz, &edx);
        return ((static_cast<double>(ecx_hz) * ebx_numerator) / eax_denominator) / 1e9;
    }

    double internal::RDTSC_TICK_FREQ;
    unsigned int temp;
    std::binary_semaphore initialState{1};
    unsigned long long internal::initialTimestamp;
    unsigned long long internal::initialCycles;
    std::jthread worker;

    void sync(std::stop_token status, unsigned long long& initialTimestamp, unsigned long long& initialCycles) {
        while (!status.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            initialState.acquire();
            initialTimestamp = std::chrono::system_clock::now().time_since_epoch().count();
            initialCycles = __rdtscp(&temp);
            initialState.release();
        }
    }

    void init() {
        internal::RDTSC_TICK_FREQ = GET_RDTSC_TICK_FREQ();
        std::cout << "RDTSC_TICK_FREQ: " << internal::RDTSC_TICK_FREQ << '\n';
        internal::initialTimestamp  = std::chrono::system_clock::now().time_since_epoch().count();
        internal::initialCycles = __rdtscp(&temp);

        worker = std::jthread(sync, std::ref(internal::initialTimestamp), std::ref(internal::initialCycles));
        /* Pin to specific CPU
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);
        int rc = pthread_setaffinity_np(worker.native_handle(), sizeof(cpu_set_t), &cpuset);
        */
    }

    unsigned long long now(unsigned long long elapsedCycles) {
        initialState.acquire();
        auto timestamp = (internal::initialTimestamp + static_cast<unsigned long long>(static_cast<long double>(elapsedCycles - internal::initialCycles) / internal::RDTSC_TICK_FREQ));
        initialState.release();
        return timestamp;
    }

    void exit() {
        worker.request_stop();
    }
}