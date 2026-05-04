#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "Optional.h"

template <typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(size_t capacity = 0)
        : closed_(false), capacity_(capacity) {}

    void push(T value) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            not_full_.wait(lock, [this]{
                return capacity_ == 0 || queue_.size() < capacity_ || closed_;
            });
            if (closed_) return;
            queue_.push(std::move(value));
        }
        not_empty_.notify_one();
    }

    Optional<T> pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this]{ return !queue_.empty() || closed_; });
        if (queue_.empty()) return Optional<T>();
        T val = std::move(queue_.front());
        queue_.pop();
        lock.unlock();
        not_full_.notify_one();
        return Optional<T>(val);
    }

    Optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return Optional<T>();
        T val = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return Optional<T>(val);
    }

    template <typename Rep, typename Period>
    Optional<T> pop_for(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        bool got = not_empty_.wait_for(lock, timeout,
            [this]{ return !queue_.empty() || closed_; });
        if (!got || queue_.empty()) return Optional<T>();
        T val = std::move(queue_.front());
        queue_.pop();
        lock.unlock();
        not_full_.notify_one();
        return Optional<T>(val);
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    bool closed_;
    size_t capacity_;
};
