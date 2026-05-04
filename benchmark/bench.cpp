#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include "utils/LRUCache.h"
#include "utils/ThreadSafeQueue.h"
#include "utils/ObjectPool.h"

using Clock = std::chrono::steady_clock;

// ── LRUCache ──────────────────────────────────────────────
void bench_lru() {
    std::cout << "=== LRUCache ===\n";
    const int N = 10'000'000;

    // 100% hit rate
    {
        LRUCache<int,int> cache(1000);
        for (int i = 0; i < 1000; i++) cache.put(i, i);
        auto t0 = Clock::now();
        for (int i = 0; i < N; i++) cache.get(i % 1000);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "  100% hit  " << N << " ops: " << ms << "ms"
                  << "  (" << (N / (ms ? ms : 1) / 1000) << "M ops/s)\n";
    }

    // 50% hit rate
    {
        LRUCache<int,int> cache(500);
        for (int i = 0; i < 500; i++) cache.put(i, i);
        auto t0 = Clock::now();
        for (int i = 0; i < N; i++) cache.get(i % 1000);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "  50% hit   " << N << " ops: " << ms << "ms"
                  << "  (" << (N / (ms ? ms : 1) / 1000) << "M ops/s)\n";
    }
}

// ── ThreadSafeQueue ───────────────────────────────────────
void bench_queue() {
    std::cout << "\n=== ThreadSafeQueue ===\n";
    const int N = 1'000'000;

    auto run = [&](const char* label, size_t capacity) {
        ThreadSafeQueue<int> q(capacity);
        std::atomic<int> consumed(0);
        auto t0 = Clock::now();

        std::vector<std::thread> producers, consumers;
        for (int i = 0; i < 4; i++)
            producers.emplace_back([&]() {
                for (int j = 0; j < N / 4; j++) q.push(j);
            });
        for (int i = 0; i < 4; i++)
            consumers.emplace_back([&]() {
                while (true) {
                    auto v = q.pop();
                    if (!v.has_value()) break;
                    consumed++;
                }
            });

        for (auto& t : producers) t.join();
        q.close();
        for (auto& t : consumers) t.join();

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "  " << label << " " << N << " msgs: " << ms << "ms"
                  << "  (" << (consumed.load() / (ms ? ms : 1) / 1000) << "M msg/s)\n";
    };

    run("unbounded  ", 0);
    run("bounded=256", 256);
}

// ── ObjectPool ────────────────────────────────────────────
void bench_pool() {
    std::cout << "\n=== ObjectPool ===\n";
    const int N = 1'000'000;

    struct Obj { int x = 0; };

    // pool
    {
        ObjectPool<Obj> pool(1);
        auto t0 = Clock::now();
        for (int i = 0; i < N; i++) {
            auto g = pool.acquire();
            g->x++;
        }
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "  pool       " << N << " ops: " << ms << "ms"
                  << "  (" << (N / (ms ? ms : 1) / 1000) << "M ops/s)\n";
    }

    // new/delete
    {
        auto t0 = Clock::now();
        for (int i = 0; i < N; i++) {
            Obj* o = new Obj();
            o->x++;
            delete o;
        }
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count();
        std::cout << "  new/delete " << N << " ops: " << ms << "ms"
                  << "  (" << (N / (ms ? ms : 1) / 1000) << "M ops/s)\n";
    }
}

int main() {
    bench_lru();
    bench_queue();
    bench_pool();
    return 0;
}
