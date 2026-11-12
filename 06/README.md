# дз 6 (лекция 7) - класс Matrix

матрица из int32_t, память через new/delete, без vector

- getRows / getColumns
- m[i][j] - через прокси-класс ProxyRow, вылет за границы -> std::out_of_range
- *=  на число
- + двух матриц (если размеры разные - тоже out_of_range)
- == и !=
- вывод в ostream (числа через пробел, каждая строка матрицы с новой строки)

файлы:
- matrix.hpp / matrix.cpp - сам класс
- test.cpp - тесты (gtest)
- CMakeLists.txt - сборка, googletest подтягивается через FetchContent

сборка и тесты:

    mkdir build && cd build
    cmake ..
    make
    ctest
