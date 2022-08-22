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
    long elapsed;
    const std::string msg;

public:
    timer(const std::string msg);
    ~timer();
};

#endif