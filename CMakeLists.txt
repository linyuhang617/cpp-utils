cmake_minimum_required(VERSION 3.10)
project(cpp-utils CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

find_package(Threads REQUIRED)

add_executable(demo_lru examples/demo_lru.cpp)
add_executable(test_lru tests/test_lru.cpp)

add_executable(demo_queue examples/demo_queue.cpp)
target_link_libraries(demo_queue Threads::Threads)

add_executable(test_queue tests/test_queue.cpp)
target_link_libraries(test_queue Threads::Threads)

add_executable(demo_pool examples/demo_pool.cpp)
target_link_libraries(demo_pool Threads::Threads)

add_executable(test_pool tests/test_pool.cpp)
target_link_libraries(test_pool Threads::Threads)

add_executable(bench benchmark/bench.cpp)
target_link_libraries(bench Threads::Threads)
