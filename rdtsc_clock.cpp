#include <iostream>
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
    internal::InitialState initialState;
    std::jthread worker;

    void sync(std::stop_token status, internal::InitialState& initialState) {
        while (!status.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            initialState.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
            initialState.cycles = __rdtscp(&temp);
        }
    }

    void init() {
        internal::RDTSC_TICK_FREQ = GET_RDTSC_TICK_FREQ();
        //std::cout << "RDTSC_TICK_FREQ: " << internal::RDTSC_TICK_FREQ << '\n';
        initialState.timestamp  = std::chrono::system_clock::now().time_since_epoch().count();
        initialState.cycles = __rdtscp(&temp);

        worker = std::jthread(sync, std::ref(initialState));
        /* Pin to specific CPU
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);
        int rc = pthread_setaffinity_np(worker.native_handle(), sizeof(cpu_set_t), &cpuset);
        */
    }

    unsigned long long now(unsigned long long elapsedCycles) {
        auto timestamp = (initialState.timestamp + static_cast<unsigned long long>(static_cast<long double>(elapsedCycles - initialState.cycles) / internal::RDTSC_TICK_FREQ));
        return timestamp;
    }

    void exit() {
        worker.request_stop();
        worker.detach();
    }
}