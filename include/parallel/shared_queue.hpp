#ifndef SHARED_QUEUE_HPP
#define SHARED_QUEUE_HPP

#include <mutex>
#include <queue>
#include <condition_variable>


template<typename T>
class shared_queue {
private:
    std::mutex m;
    std::condition_variable cond_var;
    std::queue<T> q;

public:
    shared_queue();
    ~shared_queue();
    void push(T frame);
    T pop();
};

#endif