#include <iostream>
#include "parallel/shared_queue.hpp"


// constructor
template<typename T>
shared_queue<T>::shared_queue() {}

// destructor
template<typename T>
shared_queue<T>::~shared_queue() {}

/**
 * @brief push a frame in the queue
 * 
 * Creates a lock guard, pushes the frame into the queue
 * and releases the lock notifying all waiting threads through
 * a condition variable.
 * 
 * @param frame the pointer to the frame to be pushed in the queue
 */
template<typename T>
void shared_queue<T>::push(T frame) {
    std::unique_lock<std::mutex> lk(m);
    q.push(frame);
    cond_var.notify_all();
}

/**
 * @brief pop a frame from the queue
 * 
 * Creates a lock guard, uses a condition variable to check
 * whether the queue is empty and in case it waits.
 * If the queue isn't empty, it pops a frame from the queue
 * and returns it.
 * 
 * @return the pointer to the frame popped from the queue
 */
template<typename T>
T shared_queue<T>::pop(bool *finished) {
    std::unique_lock<std::mutex> lk(m);
    
    // cond_var.wait(exit_mutex, [this](){ return !q.empty(); });

    while (!(q.empty() && *finished)) {
        std::cout << "IN POP: " << this->q.empty() << " " << *finished << std::endl;
        cond_var.wait(lk);
    }
    
    if (!q.empty()) {
        T frame = q.front();
        q.pop();
        return frame;
    }
    return nullptr;
}


template<typename T>
size_t shared_queue<T>::size() {
    std::lock_guard<std::mutex> lk(m);
    return q.size();
}

template<typename T>
bool shared_queue<T>::empty() {
    std::lock_guard<std::mutex> lk(m);
    return q.empty();
}
