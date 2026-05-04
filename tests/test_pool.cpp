#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include "utils/ObjectPool.h"

struct MyObj { int value = 0; };

void test_reuse() {
    ObjectPool<MyObj> pool(1);
    MyObj* ptr1;
    {
        auto g = pool.acquire();
        ptr1 = g.get();
    }
    auto g2 = pool.acquire();
    assert(g2.get() == ptr1); // 同一個物件被重用
    std::cout << "[PASS] object reuse\n";
}

void test_move_semantics() {
    ObjectPool<MyObj> pool(1);
    auto g1 = pool.acquire();
    MyObj* ptr = g1.get();
    auto g2 = std::move(g1);
    assert(g1.get() == nullptr);
    assert(g2.get() == ptr);
    std::cout << "[PASS] move semantics\n";
}

void test_concurrent() {
    ObjectPool<MyObj> pool(4);
    const int THREADS = 8;
    const int OPS = 100;
    std::vector<std::thread> threads;

    for (int i = 0; i < THREADS; i++) {
        threads.emplace_back([&]() {
            for (int j = 0; j < OPS; j++) {
                auto g = pool.acquire();
                g->value++;
            }
        });
    }
    for (auto& t : threads) t.join();
    std::cout << "[PASS] concurrent acquire/release\n";
}

int main() {
    test_reuse();
    test_move_semantics();
    test_concurrent();
    std::cout << "\nAll tests passed.\n";
    return 0;
}
