#include "rdtsc_clock.hpp"

#include <x86intrin.h>
#include <thread>

int main() {
    unsigned int temp;
    timespec ts;
    RDTSC_Clock::init();
    while (true) {  
        auto now = std::chrono::system_clock::now();
        long long system_timestamp = now.time_since_epoch().count();
        unsigned long long cycles = __rdtscp(&temp);
        long long rdtscp_timestamp = RDTSC_Clock::now(cycles);
        std::cout << "Delta: " << rdtscp_timestamp - system_timestamp << '\n';
    }
}