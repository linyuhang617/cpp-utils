#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include "utils/ThreadSafeQueue.h"

void test_fifo_order() {
    ThreadSafeQueue<int> q;
    q.push(1); q.push(2); q.push(3);
    assert(q.pop().value() == 1);
    assert(q.pop().value() == 2);
    assert(q.pop().value() == 3);
    std::cout << "[PASS] fifo order\n";
}

void test_try_pop_empty() {
    ThreadSafeQueue<int> q;
    assert(!q.try_pop().has_value());
    q.push(42);
    assert(q.try_pop().value() == 42);
    assert(!q.try_pop().has_value());
    std::cout << "[PASS] try_pop empty\n";
}

void test_close_unblocks_pop() {
    ThreadSafeQueue<int> q;
    bool got_nullopt = false;
    std::thread consumer([&]() {
        auto val = q.pop();
        got_nullopt = !val.has_value();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    q.close();
    consumer.join();
    assert(got_nullopt);
    std::cout << "[PASS] close unblocks pop\n";
}

void test_pop_drains_before_close() {
    ThreadSafeQueue<int> q;
    q.push(1); q.push(2);
    q.close();
    assert(q.pop().value() == 1);
    assert(q.pop().value() == 2);
    assert(!q.pop().has_value());
    std::cout << "[PASS] pop drains before close\n";
}

void test_bounded_blocks_producer() {
    ThreadSafeQueue<int> q(3);
    q.push(1); q.push(2); q.push(3);
    bool producer_blocked = true;
    std::thread producer([&]() {
        q.push(4);
        producer_blocked = false;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    assert(producer_blocked);
    q.pop();
    producer.join();
    assert(!producer_blocked);
    std::cout << "[PASS] bounded blocks producer\n";
}

void test_pop_for_timeout() {
    ThreadSafeQueue<int> q;
    auto t0 = std::chrono::steady_clock::now();
    auto val = q.pop_for(std::chrono::milliseconds(50));
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - t0).count();
    assert(!val.has_value());
    assert(ms >= 30 && ms <= 200);
    std::cout << "[PASS] pop_for timeout (" << ms << "ms)\n";
}

void test_pop_for_gets_value() {
    ThreadSafeQueue<int> q;
    std::thread producer([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        q.push(99);
    });
    auto val = q.pop_for(std::chrono::milliseconds(200));
    assert(val.has_value());
    assert(val.value() == 99);
    producer.join();
    std::cout << "[PASS] pop_for gets value\n";
}

int main() {
    test_fifo_order();
    test_try_pop_empty();
    test_close_unblocks_pop();
    test_pop_drains_before_close();
    test_bounded_blocks_producer();
    test_pop_for_timeout();
    test_pop_for_gets_value();
    std::cout << "\nAll tests passed.\n";
    return 0;
}
