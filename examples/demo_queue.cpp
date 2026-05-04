#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include "utils/ThreadSafeQueue.h"

void demo_unbounded() {
    std::cout << "=== Unbounded ===\n";
    ThreadSafeQueue<int> q;
    const int N = 100;

    std::thread producer([&]() {
        for (int i = 0; i < N; i++) q.push(i);
        q.close();
    });

    int consumed = 0;
    std::thread consumer([&]() {
        while (q.pop().has_value()) consumed++;
    });

    producer.join();
    consumer.join();
    std::cout << "produced: " << N << ", consumed: " << consumed << "\n";
}

void demo_bounded() {
    std::cout << "\n=== Bounded (capacity=5, 6 producers) ===\n";
    ThreadSafeQueue<int> q(5);

    std::vector<std::thread> producers;
    for (int i = 0; i < 6; i++) {
        producers.emplace_back([&, i]() {
            auto t0 = std::chrono::steady_clock::now();
            q.push(i);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - t0).count();
            std::cout << "  producer " << i << " waited " << ms << "ms\n";
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    for (int i = 0; i < 6; i++) q.pop();
    q.close();

    for (auto& t : producers) t.join();
}

int main() {
    demo_unbounded();
    demo_bounded();
    return 0;
}
