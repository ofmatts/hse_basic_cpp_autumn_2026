#include "token_parser.hpp"

// максимум uint64_t (2^64 - 1). не хотел ради одной константы тащить <limits>
static const uint64_t MAX_UINT64 = 18446744073709551615ULL;

// проверяет что токен это число и заодно вычисляет его.
// false - если внутри не только цифры или не влезает в uint64_t
static bool parse_number(const std::string &token, uint64_t &value)
{
    if (token.empty())
        return false;

    // сначала убеждаюсь что всё цифры
    for (size_t i = 0; i < token.size(); i++)
    {
        if (token[i] < '0' || token[i] > '9')
            return false;
    }

    // теперь само число, с проверкой переполнения
    value = 0;
    for (size_t i = 0; i < token.size(); i++)
    {
        uint64_t d = token[i] - '0';
        // переполнение надо ловить до умножения: после него значение молча
        // завернётся по модулю 2^64 и проверка ничего не заметит
        if (value > (MAX_UINT64 - d) / 10)
            return false;
        value = value * 10 + d;
    }
    return true;
}

void TokenParser::SetStartCallback(func_ptr f)
{
    start_cb = f;
}

void TokenParser::SetEndCallback(func_ptr f)
{
    end_cb = f;
}

void TokenParser::SetDigitTokenCallback(func_digit_ptr f)
{
    digit_cb = f;
}

void TokenParser::SetStringTokenCallback(func_str_ptr f)
{
    str_cb = f;
}

void TokenParser::Parse(const std::string &text)
{
    if (start_cb != nullptr)
        start_cb();

    // режу текст на токены, разделители: пробел, таб, перевод строки
    std::string token = "";
    for (size_t i = 0; i < text.size(); i++)
    {
        char c = text[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') // \r на случай виндовых файлов
        {
            if (!token.empty())
            {
                uint64_t value;
                if (parse_number(token, value))
                {
                    if (digit_cb != nullptr)
                        digit_cb(value);
                }
                else
                {
                    if (str_cb != nullptr)
                        str_cb(token);
                }
                token = "";
            }
        }
        else
        {
            token += c;
        }
    }

    // последний токен, если текст кончился без разделителя
    if (token.size() > 0)
    {
        uint64_t value;
        if (parse_number(token, value))
        {
            if (digit_cb != nullptr)
                digit_cb(value);
        }
        else
        {
            if (str_cb != nullptr)
                str_cb(token);
        }
    }

    // printf("parse finished\n"); // дебаг

    if (end_cb != nullptr)
        end_cb();
}

// TODO: хотелось бы уметь принимать лямбды/функторы через std::function,
// но по заданию нужны именно указатели на функции, так что пока так

void parse(const std::string &text, func_digit_ptr digit_callback, func_str_ptr string_callback)
{
    // просто заворачиваю класс, чтобы был и вариант функцией из обязательной части
    TokenParser parser;
    parser.SetDigitTokenCallback(digit_callback);
    parser.SetStringTokenCallback(string_callback);
    parser.Parse(text);
}
