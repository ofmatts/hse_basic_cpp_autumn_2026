// дз №4: консольная утилита mygrep, упрощённый grep
// mygrep [-i] [-c] <шаблон> <файл> [<файл> ...]

#include <iostream>
#include <string>
#include <vector>

#include "mygrep.hpp"

using namespace std;

int main(int argc, char **argv)
{
    Options opts;
    if (!parse_args(argc, argv, opts))
    {
        cerr << "использование: mygrep [-i] [-c] <шаблон> <файл> [<файл> ...]" << endl;
        return 1;
    }

    // всю работу делает run_grep, main только печатает что она насобирала
    vector<string> out = run_grep(opts);
    for (size_t i = 0; i < out.size(); i++)
        cout << out[i] << endl;

    return 0;
}
