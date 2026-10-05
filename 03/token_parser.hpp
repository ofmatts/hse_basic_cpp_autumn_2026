#ifndef TOKEN_PARSER_HPP
#define TOKEN_PARSER_HPP

#include <string>
#include <cstdint> // uint64_t

// дз №3, библиотека-парсер строк на токены
// токен это либо число (неотрицательное целое, влезающее в uint64_t), либо
// строка (всё остальное). разделители - пробел, табуляция, перевод строки.
// если число не влезает в uint64_t, оно считается строкой (так в задании)

// типы колбэков, как в условии
using func_ptr = void (*)();
using func_digit_ptr = void (*)(uint64_t);
using func_str_ptr = void (*)(const std::string &);

// вариант функцией из обязательной части задания
void parse(const std::string &text,
           func_digit_ptr digit_callback = nullptr,
           func_str_ptr string_callback = nullptr);

class TokenParser
{
public:
    TokenParser() = default;

    // колбэк вызывается перед парсингом
    void SetStartCallback(func_ptr f);
    // колбэк вызывается после парсинга
    void SetEndCallback(func_ptr f);
    // колбэк на токен-число
    void SetDigitTokenCallback(func_digit_ptr f);
    // колбэк на токен-строку
    void SetStringTokenCallback(func_str_ptr f);

    void Parse(const std::string &text);

private:
    func_ptr start_cb = nullptr;
    func_ptr end_cb = nullptr;
    func_digit_ptr digit_cb = nullptr;
    func_str_ptr str_cb = nullptr;
};

#endif // TOKEN_PARSER_HPP
