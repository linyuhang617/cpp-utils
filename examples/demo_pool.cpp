#include <iostream>
#include "utils/ObjectPool.h"

struct MyObj {
    int value = 0;
    explicit MyObj(int v = 0) : value(v) {}
};

int main() {
    // 預設 factory（new T()）
    std::cout << "=== Default factory ===\n";
    ObjectPool<MyObj> pool(4);
    std::cout << "after prealloc:   available = " << pool.available() << "\n";
    {
        auto a = pool.acquire();
        auto b = pool.acquire();
        auto c = pool.acquire();
        auto d = pool.acquire();
        std::cout << "inside scope:     available = " << pool.available() << "\n";
        a->value = 42;
        std::cout << "a->value = " << a->value << "\n";
        auto e = pool.acquire();
        std::cout << "after 5th:        available = " << pool.available() << "\n";
    }
    std::cout << "after scope ends: available = " << pool.available() << "\n";

    // 自訂 factory
    std::cout << "\n=== Custom factory (value=99) ===\n";
    ObjectPool<MyObj> pool2(2, []{ return new MyObj(99); });
    {
        auto g = pool2.acquire();
        std::cout << "g->value = " << g->value << "\n"; // 99
    }

    return 0;
}
