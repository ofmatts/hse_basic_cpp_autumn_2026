# дз 2 - аллокатор с линейным выделением памяти

- allocator.hpp / allocator.cpp - сам аллокатор
- test.cpp - тесты (gtest)
- CMakeLists.txt - сборка, googletest подтягивается сам через FetchContent 

как собрать и прогнать тесты:

    mkdir build && cd build
    cmake ..
    make
    ctest
