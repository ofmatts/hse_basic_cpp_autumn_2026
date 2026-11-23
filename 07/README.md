# дз 7 (лекция 9) - свой vector

шаблонный vector в namespace my + специализация для bool (биты упакованы в uint64_t)

- operator[], front, back, data, empty, size, max_size, reserve, capacity, shrink_to_fit
- push_back, pop_back, insert, emplace, emplace_back, resize, reverse
- копирование и перемещение (конструкторы и операторы =)
- заодно сделал clear, swap, begin/end и конструкторы (n), (n, value), initializer_list

что нестандартного:

- insert и emplace принимают индекс, а не итератор
- reverse - метод вектора, разворачивает на месте
- в vector<bool> data() возвращает указатель на блоки uint64_t, а operator[] отдаёт прокси-объект
- память выделяется через operator new, элементы создаются placement new

файлы:
- vector.hpp - общий шаблон (в конце подключает vector_bool.hpp)
- vector_bool.hpp - специализация для bool
- test.cpp - тесты (gtest)
- CMakeLists.txt - сборка, googletest подтягивается через FetchContent

сборка и тесты:

    mkdir build && cd build
    cmake ..
    make
    ctest
