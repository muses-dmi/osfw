//
// simple lock-free, single producer, single consumer, queue.

#pragma once

#include <memory>
#include <atomic>
#include <utility>

template<class T>
class lfspsc_queue {
private:
    const size_t size_;
    std::unique_ptr<T[]> data_;
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
public:
    lfspsc_queue(size_t s) : 
        size_{s}, data_{new T[s]}, head_{0}, tail_{0} {
    }

    ~lfspsc_queue() {      
    }

    //num of elements in ringbuffer (size - room())
    size_t available_space() {
        return (tail_ + size_ - head_) % size_;
    }

    //num of free spaces in ringbuffer (size - count())
    size_t free_space() {
        return (head_ + size_ - tail_) % size_;
    }

    bool push(T const& t) {
        size_t new_tail = (tail_ + 1) % size_;
        if (new_tail == head_) {
            return false; // full
        }
        data_[tail_] = t;
        tail_ = new_tail; 
        return true;
    }

    bool pop(T& dest) {
        try { 
            dest = data_[head_];
            head_ = (head_ + 1) % size_;  
            return true;
        }
        catch(...) { 
            return false; 
        }
    }
};