# дз 7 (лекция 8) - класс BigInt

большие целые числа, цифры лежат в динамическом массиве (new/delete, без vector)

- конструкторы от int32_t, std::string и const char*, на плохую строку кидается std::invalid_argument
- копирование и перемещение
- + - * (BigInt с BigInt и BigInt с int32_t справа), унарный минус
- сравнения только BigInt с BigInt
- вывод в ostream

умножение в столбик, O(n^2)

файлы:
- bigint.hpp / bigint.cpp - сам класс
- test.cpp - тесты (gtest)
- CMakeLists.txt - сборка, googletest подтягивается через FetchContent

сборка и тесты:

    mkdir build && cd build
    cmake ..
    make
    ctest
