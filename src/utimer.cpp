#include <iostream>
#include <chrono>


class utimer {
private:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point stop;
    const std::string msg;

public:
    utimer(const std::string msg) : msg(msg) {
        start = std::chrono::system_clock::now();
    }

    ~utimer() {
        stop = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = stop - start;
        auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        std::cout << msg << " computed in: " << musec << " micro-seconds" << std::endl;
    }
};