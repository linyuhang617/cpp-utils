#pragma once
#include <list>
#include <unordered_map>
#include <stdexcept>
#include "Optional.h"

template <typename K, typename V>
class LRUCache {
public:
    explicit LRUCache(size_t capacity) {
        if (capacity == 0) throw std::invalid_argument("capacity must be > 0");
        capacity_ = capacity;
    }

    Optional<V> get(const K& key) {
        auto it = map_.find(key);
        if (it == map_.end()) return Optional<V>();
        list_.splice(list_.begin(), list_, it->second);
        return Optional<V>(it->second->second);
    }

    void put(const K& key, const V& value) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->second = value;
            list_.splice(list_.begin(), list_, it->second);
            return;
        }
        if (list_.size() == capacity_) {
            auto lru = list_.back();
            map_.erase(lru.first);
            list_.pop_back();
        }
        list_.emplace_front(key, value);
        map_[key] = list_.begin();
    }

    bool contains(const K& key) const { return map_.find(key) != map_.end(); }
    size_t size() const { return list_.size(); }
    size_t capacity() const { return capacity_; }
    void clear() { list_.clear(); map_.clear(); }

private:
    size_t capacity_;
    std::list<std::pair<K, V>> list_;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map_;
};
