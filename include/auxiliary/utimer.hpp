#ifndef UTIMER_HPP
#define UTIMER_HPP

#include <iostream>
#include <chrono>

/**
 * @brief object that takes the time upon creation and, once destroyed, prints
 * the elapsed time from its creation
 */
class utimer {
private:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point stop;
    long elapsed;
    const std::string msg;

public:
    utimer(const std::string msg);
    ~utimer();
};

#endif