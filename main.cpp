#include "rdtsc_clock.hpp"

#include <iostream>

int main() {
    unsigned int temp;
    RDTSC_Clock::init();
    while (true) {  
        auto now = std::chrono::system_clock::now();
        volatile long long rdtscp_timestamp = RDTSC_Clock::now();
        long long system_timestamp = now.time_since_epoch().count();
        //std::cout << "Delta: " << rdtscp_timestamp - system_timestamp << '\n';
    }
    RDTSC_Clock::exit();
}