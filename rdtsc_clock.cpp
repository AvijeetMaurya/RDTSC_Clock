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

    double internal::RDTSC_TICK_TO_NS;
    unsigned int temp;
    unsigned long long internal::timestamp;
    std::jthread worker;

    void sync(std::stop_token status, unsigned long long& initialTimestamp) {
        while (!status.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            initialTimestamp = std::chrono::system_clock::now().time_since_epoch().count() - (__rdtscp(&temp) * internal::RDTSC_TICK_TO_NS);
        }
    }

    void init() {
        internal::RDTSC_TICK_TO_NS = 1 / GET_RDTSC_TICK_FREQ();
        //std::cout << "RDTSC_TICK_FREQ: " << internal::RDTSC_TICK_FREQ << '\n';
        internal::timestamp = std::chrono::system_clock::now().time_since_epoch().count() - (__rdtscp(&temp) * internal::RDTSC_TICK_TO_NS);

        worker = std::jthread(sync, std::ref(internal::timestamp));
        /* Pin to specific CPU
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);
        int rc = pthread_setaffinity_np(worker.native_handle(), sizeof(cpu_set_t), &cpuset);
        */
    }

    unsigned long long now() {
        return internal::timestamp + static_cast<unsigned long long>(__rdtscp(&temp) * internal::RDTSC_TICK_TO_NS);
    }

    void exit() {
        worker.request_stop();
        worker.detach();
    }
}