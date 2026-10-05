// демо из условия: читает строки со стандартного входа и парсит их

#include <iostream>
#include <string>

#include "token_parser.hpp"

using namespace std;

static void print_digit(uint64_t value)
{
    cout << "число: " << value << endl;
}

static void print_string(const string &token)
{
    cout << "строка: " << token << endl;
}

int main()
{
    TokenParser parser;
    parser.SetDigitTokenCallback(print_digit);
    parser.SetStringTokenCallback(print_string);
    // можно было и свободной функцией parse(line, nullptr, print_string),
    // но через класс можно ещё и старт/энд колбэки вешать

    string line;
    while (getline(cin, line))
    {
        parser.Parse(line);
    }

    return 0;
}
