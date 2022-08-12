#include <iostream>
#include <chrono>

#include "auxiliary/utimer.hpp"


// constructor of class utimer
utimer::utimer(const std::string msg) : msg(msg) {
    start = std::chrono::system_clock::now();
}

// destructor of class utimer
utimer::~utimer() {
    stop = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    long musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout << msg << " computed in: " << musec << " micro-seconds" << std::endl;
}