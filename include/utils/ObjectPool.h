#pragma once
#include <stack>
#include <mutex>
#include <functional>

template <typename T>
class ObjectPool {
public:
    class PoolGuard {
    public:
        PoolGuard(ObjectPool<T>& pool, T* obj) : pool_(pool), obj_(obj) {}
        ~PoolGuard() {
            if (obj_) pool_.return_object(obj_);
        }
        PoolGuard(PoolGuard&& other) : pool_(other.pool_), obj_(other.obj_) {
            other.obj_ = nullptr;
        }
        PoolGuard& operator=(PoolGuard&& other) {
            if (this != &other) {
                if (obj_) pool_.return_object(obj_);
                obj_ = other.obj_;
                other.obj_ = nullptr;
            }
            return *this;
        }
        PoolGuard(const PoolGuard&) = delete;
        PoolGuard& operator=(const PoolGuard&) = delete;
        T* get() { return obj_; }
        T* operator->() { return obj_; }
        T& operator*() { return *obj_; }
    private:
        ObjectPool<T>& pool_;
        T* obj_;
    };

    using Factory = std::function<T*()>;

    explicit ObjectPool(size_t prealloc = 0, Factory factory = []{ return new T(); })
        : factory_(factory) {
        for (size_t i = 0; i < prealloc; i++)
            pool_.push(factory_());
    }

    ~ObjectPool() {
        while (!pool_.empty()) {
            delete pool_.top();
            pool_.pop();
        }
    }

    PoolGuard acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        T* obj = pool_.empty() ? factory_() : [&]{
            T* o = pool_.top(); pool_.pop(); return o;
        }();
        return PoolGuard(*this, obj);
    }

    size_t available() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.size();
    }

private:
    friend class PoolGuard;

    void return_object(T* obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.push(obj);
    }

    Factory factory_;
    std::stack<T*> pool_;
    mutable std::mutex mutex_;
};
