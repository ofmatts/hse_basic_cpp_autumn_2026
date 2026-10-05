# дз 3 - библиотека-парсер токенов

- token_parser.hpp / token_parser.cpp - сам парсер: класс TokenParser + свободная функция parse() из обязательной части
- test.cpp - тесты (gtest)
- main.cpp - демо из условия, читает строки со stdin и зовёт парсер
- CMakeLists.txt - сборка, googletest подтягивается сам через FetchContent 

как собрать и прогнать тесты:

    mkdir build && cd build
    cmake ..
    make
    ctest

запуск демо:

    echo "beatngu 4you 42" | ./parser_demo
