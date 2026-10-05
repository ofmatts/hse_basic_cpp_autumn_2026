// тесты на парсер из дз №3

#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include <vector>

#include "token_parser.hpp"

using namespace std;

// колбэки в парсере это обычные указатели на функции, состояние в них не
// передаёшь, поэтому результаты складываю в глобальные переменные и чищу
// их в начале каждого теста
static vector<uint64_t> got_digits;
static vector<string> got_strings;
static vector<string> calls; // общий лог вызовов: N42 = число, Sabc = строка
static int start_cnt = 0;
static int end_cnt = 0;

static void reset()
{
    got_digits.clear();
    got_strings.clear();
    calls.clear();
    start_cnt = 0;
    end_cnt = 0;
}

static void digit_cb(uint64_t value)
{
    got_digits.push_back(value);
    calls.push_back("N" + to_string(value));
}

static void string_cb(const string &token)
{
    got_strings.push_back(token);
    calls.push_back("S" + token);
}

static void start_cb()
{
    start_cnt++;
}

static void end_cb()
{
    end_cnt++;
}

// скобки вокруг (vector{...}) в EXPECT_EQ обязательны, иначе запятая внутри
// {} разваливает макрос на куски (полчаса на это убил пока понял)

// числа

TEST(Digits, SimpleNumbers)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.Parse("1234 0 42 100500");
    EXPECT_EQ(got_digits, (vector<uint64_t>{1234, 0, 42, 100500}));
    EXPECT_TRUE(got_strings.empty());
}

TEST(Digits, LeadingZeros)
{
    // 00323 из условия - это число 323
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.Parse("00323 000 007");
    EXPECT_EQ(got_digits, (vector<uint64_t>{323, 0, 7}));
}

TEST(Digits, Uint64MaxBoundary)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    // 18446744073709551615 = 2^64 - 1, самый максимум, ещё влезает
    p.Parse("18446744073709551615 18446744073709551614");
    EXPECT_EQ(got_digits, (vector<uint64_t>{18446744073709551615ULL, 18446744073709551614ULL}));
}

TEST(Digits, MaxPlusOneIsString)
{
    // максимум + 1 уже не влезает, по заданию это строка
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse("18446744073709551616");
    EXPECT_TRUE(got_digits.empty());
    EXPECT_EQ(got_strings, (vector<string>{"18446744073709551616"}));
}

TEST(Digits, VeryLongNumberIsString)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse("99999999999999999999999999999999999");
    EXPECT_EQ(got_strings, (vector<string>{"99999999999999999999999999999999999"}));
    EXPECT_TRUE(got_digits.empty());
}

TEST(Digits, OverflowWithLeadingZeros)
{
    // хитрый случай: нули спереди, а значение за максимумом. если проверять
    // переполнение по длине строки, такой токен можно ошибочно принять за число
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse("00018446744073709551615"); // нули + ровно максимум
    EXPECT_EQ(got_digits, (vector<uint64_t>{18446744073709551615ULL}));
    reset();
    p.Parse("00018446744073709551616"); // нули + максимум + 1
    EXPECT_EQ(got_strings, (vector<string>{"00018446744073709551616"}));
    EXPECT_TRUE(got_digits.empty());
}

TEST(Digits, BoundaryBothSidesTogether)
{
    // оба варианта рядом, чтобы точно не перепутать где число а где строка
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse("18446744073709551615 18446744073709551616");
    EXPECT_EQ(got_digits, (vector<uint64_t>{18446744073709551615ULL}));
    EXPECT_EQ(got_strings, (vector<string>{"18446744073709551616"}));
}

// строки

TEST(Strings, FromTask)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse("beatngu 4you");
    EXPECT_EQ(got_strings, (vector<string>{"beatngu", "4you"}));
    EXPECT_TRUE(got_digits.empty());
}

TEST(Strings, NotNumbersAreStrings)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    // минус, плюс, точка, буква в середине - всё не числа
    p.Parse("-1 +5 12.5 1e9 4you");
    EXPECT_EQ(got_strings, (vector<string>{"-1", "+5", "12.5", "1e9", "4you"}));
    EXPECT_TRUE(got_digits.empty());
}

TEST(Strings, SingleChars)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse("a 1 b 2");
    EXPECT_EQ(got_strings, (vector<string>{"a", "b"}));
    EXPECT_EQ(got_digits, (vector<uint64_t>{1, 2}));
}

// разделители

TEST(Separators, AllKinds)
{
    reset();
    TokenParser p;
    p.SetStringTokenCallback(string_cb);
    p.Parse("one two\tthree\nfour\t five");
    EXPECT_EQ(got_strings, (vector<string>{"one", "two", "three", "four", "five"}));
}

TEST(Separators, many_spaces_in_row)
{
    // подряд идущие разделители не должны рождать пустые токены
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.Parse("   1    2 3   ");
    EXPECT_EQ(got_digits, (vector<uint64_t>{1, 2, 3}));
}

TEST(Separators, TrailingNewline)
{
    // перенос в конце не должен давать лишний пустой токен
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.Parse("1\n2\n3\n");
    EXPECT_EQ(got_digits, (vector<uint64_t>{1, 2, 3}));
}

TEST(Separators, WindowsLineEndings)
{
    // \r на всякий случай тоже считаю разделителем (виндовые файлы)
    reset();
    TokenParser p;
    p.SetStringTokenCallback(string_cb);
    p.Parse("a\r\nb");
    EXPECT_EQ(got_strings, (vector<string>{"a", "b"}));
}

TEST(Separators, OnlySeparators)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse(" \t \n \t\n ");
    EXPECT_TRUE(got_digits.empty());
    EXPECT_TRUE(got_strings.empty());
}

TEST(Separators, EmptyText)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse("");
    EXPECT_TRUE(got_digits.empty());
    EXPECT_TRUE(got_strings.empty());
}

// порядок токенов и длинные штуки

TEST(Parser, tokens_in_order)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse("abc 123 def 45 ghi 0");
    // calls - общий лог вызовов, так проверяю точный порядок, а не два
    // отдельных вектора (в них порядок как раз теряется)
    EXPECT_EQ(calls, (vector<string>{"Sabc", "N123", "Sdef", "N45", "Sghi", "N0"}));
}

TEST(Parser, LongTokens)
{
    reset();
    TokenParser p;
    p.SetStringTokenCallback(string_cb);
    string big(500, 'x');
    p.Parse(big + " " + big);
    ASSERT_EQ(got_strings.size(), 2u);
    EXPECT_EQ(got_strings[0].size(), 500u);
    EXPECT_EQ(got_strings[1].size(), 500u);
}

// колбэки

TEST(Callbacks, OnlyDigitCallback)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.Parse("abc 123 def 456");
    // строки без колбэка просто пропускаются
    EXPECT_EQ(got_digits, (vector<uint64_t>{123, 456}));
    EXPECT_TRUE(got_strings.empty());
}

TEST(Callbacks, OnlyStringCallback)
{
    reset();
    TokenParser p;
    p.SetStringTokenCallback(string_cb);
    p.Parse("abc 123 def 456");
    EXPECT_EQ(got_strings, (vector<string>{"abc", "def"}));
    EXPECT_TRUE(got_digits.empty());
}

TEST(Callbacks, NoCallbacksAtAll)
{
    reset();
    TokenParser p;
    // главное что не падает
    p.Parse("abc 123 xyz 42");
    EXPECT_TRUE(got_digits.empty());
    EXPECT_TRUE(got_strings.empty());
}

TEST(Callbacks, ParserCanBeReused)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.SetStartCallback(start_cb);
    p.SetEndCallback(end_cb);
    p.Parse("1 a");
    p.Parse("2 b");
    // колбэки между вызовами сохраняются
    EXPECT_EQ(got_digits, (vector<uint64_t>{1, 2}));
    EXPECT_EQ(got_strings, (vector<string>{"a", "b"}));
    EXPECT_EQ(start_cnt, 2);
    EXPECT_EQ(end_cnt, 2);
}

TEST(Callbacks, ChangeCallbackBetweenParses)
{
    reset();
    TokenParser p;
    p.SetDigitTokenCallback(digit_cb);
    p.SetStringTokenCallback(string_cb);
    p.Parse("abc 123");
    EXPECT_EQ(got_digits, (vector<uint64_t>{123}));
    EXPECT_EQ(got_strings, (vector<string>{"abc"}));

    // выключаю строковый колбэк и парсю снова
    p.SetStringTokenCallback(nullptr);
    reset();
    p.Parse("def 456");
    EXPECT_EQ(got_digits, (vector<uint64_t>{456}));
    EXPECT_TRUE(got_strings.empty());
}

// старт/энд колбэки

TEST(StartEnd, CalledOncePerParse)
{
    reset();
    TokenParser p;
    p.SetStartCallback(start_cb);
    p.SetEndCallback(end_cb);
    p.SetDigitTokenCallback(digit_cb);
    p.Parse("1 2 3");
    EXPECT_EQ(start_cnt, 1);
    EXPECT_EQ(end_cnt, 1);
}

TEST(StartEnd, CalledEvenOnEmptyText)
{
    reset();
    TokenParser p;
    p.SetStartCallback(start_cb);
    p.SetEndCallback(end_cb);
    p.Parse("");
    EXPECT_EQ(start_cnt, 1);
    EXPECT_EQ(end_cnt, 1);
}

TEST(StartEnd, NotCalledWhenNotSet)
{
    // по умолчанию их нет, дёргаться не должны
    reset();
    TokenParser p;
    p.Parse("abc 123");
    EXPECT_EQ(start_cnt, 0);
    EXPECT_EQ(end_cnt, 0);
}

// свободная функция из задания

TEST(FreeFunction, ParsesWithBothCallbacks)
{
    reset();
    parse("hello 42", digit_cb, string_cb);
    EXPECT_EQ(got_strings, (vector<string>{"hello"}));
    EXPECT_EQ(got_digits, (vector<uint64_t>{42}));
}

TEST(FreeFunction, NullptrsAreOk)
{
    reset();
    parse("test 123", nullptr, nullptr); // оба колбэка не заданы
    EXPECT_TRUE(got_digits.empty());
    EXPECT_TRUE(got_strings.empty());
}

TEST(FreeFunction, LikeInTaskExample)
{
    // как в примере из условия: parse(line, nullptr, parse_string)
    reset();
    parse("beatngu 4you 42", nullptr, string_cb);
    EXPECT_EQ(got_strings, (vector<string>{"beatngu", "4you"}));
    EXPECT_TRUE(got_digits.empty());
}
