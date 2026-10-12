#include "mygrep.hpp"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>

// буквы в нижний регистр. работает только для латиницы: в utf-8 кириллица
// многобайтовая, tolower по байтам её не переключает. в задании примеры латиницей
static std::string to_lower(std::string s)
{
    for (size_t i = 0; i < s.size(); i++)
        s[i] = std::tolower((unsigned char)s[i]); // каст нужен, с отрицательными char вообще ub
    return s;
}

bool contains(const std::string &line, const std::string &pattern, bool ignore_case)
{
    // для -i просто привожу обе строки к нижнему регистру и ищу как обычно
    if (ignore_case)
        return contains(to_lower(line), to_lower(pattern), false);

    // пустой шаблон совпадает с чем угодно, настоящий grep себя так же ведёт
    if (pattern.empty())
        return true;

    // наивный поиск: прикладываю шаблон к каждой позиции строки и сравниваю
    for (size_t i = 0; i + pattern.size() <= line.size(); i++)
    {
        bool same = true;
        for (size_t j = 0; j < pattern.size(); j++)
        {
            if (line[i + j] != pattern[j])
            {
                same = false;
                break;
            }
        }
        if (same)
            return true;
    }
    return false;
}

bool parse_args(int argc, char **argv, Options &opts)
{
    opts = Options(); // сбрасываю на случай если opts уже использовался

    int i = 1; // argv[0] это имя программы
    for (; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg.size() > 1 && arg[0] == '-')
        {
            // несколько флагов в одном аргументе: -ic = -i -c
            for (size_t j = 1; j < arg.size(); j++)
            {
                if (arg[j] == 'i')
                    opts.ignore_case = true;
                else if (arg[j] == 'c')
                    opts.only_count = true;
                else
                    return false; // незнакомый флаг
            }
        }
        else
        {
            break; // первый не-флаг считаю шаблоном, дальше только файлы
        }
    }

    if (i == argc)
        return false; // аргументы кончились а шаблона не было

    opts.pattern = argv[i];
    i++;

    for (; i < argc; i++)
        opts.files.push_back(argv[i]);

    return opts.files.size() > 0; // хотя бы один файл нужен
}

bool grep_file(const std::string &filename, const Options &opts, std::vector<std::string> &out)
{
    out.clear(); // на всякий случай

    // в задании прямо сказано проверить существование до открытия
    if (!std::filesystem::exists(filename))
    {
        std::cerr << "mygrep: " << filename << ": файл не найден" << std::endl;
        return false;
    }

    std::ifstream file(filename);
    if (!file.is_open())
    {
        // файл есть, но открыть не вышло, например нет прав
        std::cerr << "mygrep: " << filename << ": не удалось открыть" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // getline режет только по \n, у виндовых строк на конце остаётся \r.
        // срезаю его, иначе он уедет в вывод и сломает поиск по концу слова
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        // std::cerr << "debug: [" << line << "]" << std::endl; // на этом баг с \r и поймал

        if (contains(line, opts.pattern, opts.ignore_case))
            out.push_back(line);
    }

    return true;
}

std::vector<std::string> run_grep(const Options &opts)
{
    std::vector<std::string> result;

    // если файлов несколько, перед строкой печатается имя файла, как в grep
    bool many = opts.files.size() > 1;

    for (size_t i = 0; i < opts.files.size(); i++)
    {
        std::vector<std::string> matched;
        if (!grep_file(opts.files[i], opts, matched))
            continue; // про битый файл уже написали в stderr, дальше по списку

        if (opts.only_count)
        {
            if (many)
                result.push_back(opts.files[i] + ":" + std::to_string(matched.size()));
            else
                result.push_back(std::to_string(matched.size()));
        }
        else
        {
            for (size_t j = 0; j < matched.size(); j++)
            {
                if (many)
                    result.push_back(opts.files[i] + ":" + matched[j]);
                else
                    result.push_back(matched[j]);
            }
        }
    }

    return result;
}

// TODO: к заданию "по желанию" прилагались бойер-мур и кмп. наивный поиск это
// O(n*m), но строки короткие, для дз и так летает
