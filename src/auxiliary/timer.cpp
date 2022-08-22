#include <iostream>
#include <chrono>

#include "auxiliary/timer.hpp"


// constructor of class timer
template <typename T>
timer<T>::timer(const std::string msg) : msg(msg) {
    start = std::chrono::system_clock::now();
}

// destructor of class timer
template <typename T>
timer<T>::~timer() {
    stop = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    long casted_elapsed = std::chrono::duration_cast<T>(elapsed).count();
   
    // save the correct unit of time
    std::string unit;
    if (std::is_same<T, std::chrono::seconds>::value)
        unit = " s";
    else if (std::is_same<T, std::chrono::milliseconds>::value)
        unit = " ms";
    else if (std::is_same<T, std::chrono::microseconds>::value)
        unit = " us";
    else if (std::is_same<T, std::chrono::nanoseconds>::value)
        unit = " ns";
    else
        unit = "unknown";
    
    // print the elapsed time
    std::cout << msg << ": " << casted_elapsed << unit << std::endl;
}