#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>

/**
 * @brief object that takes the time upon creation and, once destroyed, prints
 * the elapsed time from its creation
 */
template <typename T>
class timer {
private:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point stop;
    const std::string msg;

public:
    // constructor
    timer(const std::string msg) :
        msg(msg), start(std::chrono::system_clock::now()) {}
    
    // destructor
    ~timer() {
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
};

#endif