#include <cassert>
#include <stdexcept>
#include <iostream>
#include "utils/LRUCache.h"

void test_capacity_zero() {
    bool thrown = false;
    try { LRUCache<int,int> c(0); }
    catch (const std::invalid_argument&) { thrown = true; }
    assert(thrown);
    std::cout << "[PASS] capacity=0 throws\n";
}

void test_capacity_one() {
    LRUCache<int,int> c(1);
    c.put(1, 10);
    assert(c.get(1).value() == 10);
    c.put(2, 20);
    assert(!c.get(1).has_value());
    assert(c.get(2).value() == 20);
    std::cout << "[PASS] capacity=1\n";
}

void test_hit_miss() {
    LRUCache<int,int> c(3);
    c.put(1, 1);
    assert(c.get(1).value() == 1);
    assert(!c.get(99).has_value());
    std::cout << "[PASS] hit/miss\n";
}

void test_eviction_order() {
    LRUCache<int,int> c(3);
    c.put(1, 1); c.put(2, 2); c.put(3, 3);
    c.get(1);
    c.put(4, 4);
    assert(!c.get(2).has_value());
    assert(c.get(1).has_value());
    assert(c.get(3).has_value());
    assert(c.get(4).has_value());
    std::cout << "[PASS] eviction order\n";
}

void test_contains() {
    LRUCache<int,int> c(3);
    c.put(1, 1);
    assert(c.contains(1));
    assert(!c.contains(99));
    std::cout << "[PASS] contains\n";
}

void test_size_capacity() {
    LRUCache<int,int> c(3);
    assert(c.size() == 0);
    assert(c.capacity() == 3);
    c.put(1, 1); c.put(2, 2);
    assert(c.size() == 2);
    std::cout << "[PASS] size/capacity\n";
}

void test_clear() {
    LRUCache<int,int> c(3);
    c.put(1, 1); c.put(2, 2);
    c.clear();
    assert(c.size() == 0);
    assert(!c.contains(1));
    std::cout << "[PASS] clear\n";
}

int main() {
    test_capacity_zero();
    test_capacity_one();
    test_hit_miss();
    test_eviction_order();
    test_contains();
    test_size_capacity();
    test_clear();
    std::cout << "\nAll tests passed.\n";
    return 0;
}
