#include <iostream>
#include "utils/LRUCache.h"

int main() {
    LRUCache<int, std::string> cache(3);
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");
    std::cout << "After put 1,2,3 (capacity=3):\n";
    std::cout << "  get(1) = " << cache.get(1).value_or("nullopt") << "\n";
    std::cout << "  get(2) = " << cache.get(2).value_or("nullopt") << "\n";
    cache.put(4, "four");
    std::cout << "\nAfter put(4) -> evicts key 3:\n";
    std::cout << "  get(3) = " << cache.get(3).value_or("nullopt") << "\n";
    std::cout << "  get(4) = " << cache.get(4).value_or("nullopt") << "\n";
    return 0;
}
