#ifndef SHARED_QUEUE_HPP
#define SHARED_QUEUE_HPP

#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>


template<typename T>
class shared_queue {
private:
    std::mutex m;
    std::condition_variable cond_var;
    std::queue<T*> q;
    std::atomic<bool> finished;

public:
    // constructor
    shared_queue() : finished(false) {}

    // destructor
    ~shared_queue() {}

    // getter for "finished"
    bool get_finished() { return finished; /* atomic */ }

    /**
     * @brief push a frame in the queue assuring access synchronization
     * 
     * Creates a "unique_lock", pushes the frame into the queue and releases
     * the lock notifying a waiting thread through a condition variable.
     * 
     * @param frame the pointer to the frame to be pushed in the queue
     */
    void push(T *frame) {
        std::unique_lock<std::mutex> lk(m);
        q.push(frame);
        cond_var.notify_one();
    }

    /**
     * @brief pop a frame from the queue assuring access synchronization
     * 
     * Creates a "unique_lock", through a condition variable it waits while
     * the queue is empty or the main thread finished pushing frames.
     * Then, if the queue isn't empty, it pops a frame and returns it.
     * 
     * @return the pointer to the frame popped from the queue (or nullptr)
     */
    T * pop() {
        std::unique_lock<std::mutex> lk(m);
        
        cond_var.wait(lk, [this](){ return !q.empty() || finished; });
        
        if (!q.empty()) {   // q can be empty if finished is true
            T *frame = q.front();
            q.pop();
            return frame;
        }
        return nullptr; // if q is empty and finished is true
    }

    /**
     * @brief get the size of the queue assuring access synchronization
     * 
     * @return the size of the queue
     */
    size_t size() {
        std::lock_guard<std::mutex> lk(m);
        return q.size();
    }

    /**
     * @brief checks whether the queue is empty assuring access synchronization
     * 
     * @return true if the queue is empty
     * @return false if the queue is not empty
     */
    bool empty() {
        std::lock_guard<std::mutex> lk(m);
        return q.empty();
    }

    // set finished to true and notifies all waiting threads
    void no_more_pushes() {
        finished = true;    // atomic
        cond_var.notify_all();
    }
};

#endif