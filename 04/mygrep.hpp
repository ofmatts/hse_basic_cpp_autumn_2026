#ifndef MYGREP_HPP
#define MYGREP_HPP

#include <string>
#include <vector>

// дз №4, упрощённый grep. утилита ищет подстроку (шаблон) в файлах и печатает
// совпавшие строки. вызов:
//   mygrep [-i] [-c] <шаблон> <файл> [<файл> ...]
// -i - не смотреть на регистр, -c - печатать количество совпавших строк.
// флаги можно совать в один аргумент: -ic это то же что -i -c.
// флаги понимаются только до шаблона, всё что после него считается файлами.

// всё что напарсилось из командной строки
struct Options
{
    bool ignore_case = false;      // -i
    bool only_count = false;       // -c
    std::string pattern;           // что ищем
    std::vector<std::string> files; // где ищем
};

// разбор аргументов. false если аргументы плохие (нет шаблона/файла, кривой флаг)
bool parse_args(int argc, char **argv, Options &opts);

// правда если pattern лежит внутри line. наивный алгоритм, два цикла
bool contains(const std::string &line, const std::string &pattern, bool ignore_case = false);

// прогон по одному файлу, совпавшие строки кладутся в out (без \r на конце).
// если файл не открылся - печатает в stderr и возвращает false
bool grep_file(const std::string &filename, const Options &opts, std::vector<std::string> &out);

// прогон по всем файлам из opts, возвращает готовые строки вывода
// (префиксы с именами и счётчики уже приклеены). main их просто печатает
std::vector<std::string> run_grep(const Options &opts);

#endif // MYGREP_HPP
