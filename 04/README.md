# дз 4 - упрощённый grep (утилита mygrep)

ищет подстроку в файлах и печатает совпавшие строки, как настоящий grep:

    mygrep [-i] [-c] <шаблон> <файл> [<файл> ...]

- -i - игнорировать регистр (только латиница, кириллица в utf-8 так не переключается)
- -c - печатать количество совпавших строк вместо самих строк
- флаги можно совать в один аргумент: -ic = -i -c
- если файл не открылся - сообщение в stderr, остальные файлы обрабатываются дальше

- mygrep.hpp / mygrep.cpp - вся логика: parse_args, contains (наивный поиск), grep_file, run_grep
- main.cpp - тонкий main, разбор аргументов и печать
- test.cpp - тесты (gtest)
- CMakeLists.txt - сборка, googletest подтягивается сам через FetchContent 

как собрать и прогнать тесты:

    mkdir build && cd build
    cmake ..
    make
    ctest

пощупать руками:

    printf 'Hello world\nbye\n' > a.txt
    ./mygrep hello a.txt
    ./mygrep -ic HELLO a.txt
