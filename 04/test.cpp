// тесты на mygrep из дз №4

#include <gtest/gtest.h>
#include <cstdio>  // remove
#include <fstream>
#include <string>
#include <vector>

#include "mygrep.hpp"

using namespace std;

// хелпер: пишу файл с содержимым. тесты гоняются в папке сборки,
// в конце каждого теста убираю за собой через remove
static void make_file(const string &name, const string &text)
{
    ofstream f(name);
    f << text;
}

// хелпер: собирает argv и дёргает parse_args
static bool try_parse(const vector<string> &args, Options &opts)
{
    // parse_args в argv ничего не пишет, так что каст литералов не страшен
    vector<char *> argv;
    argv.push_back((char *)"mygrep"); // argv[0], имя программы
    for (size_t i = 0; i < args.size(); i++)
        argv.push_back((char *)args[i].c_str());
    return parse_args((int)argv.size(), argv.data(), opts);
}

// (vector{...}) в сравнениях надо брать в скобки, иначе запятая разваливает
// макрос на куски. уже знаю из дз 3 :)

// ---------- contains ----------

TEST(Contains, Simple)
{
    EXPECT_TRUE(contains("hello world", "hello"));
    EXPECT_TRUE(contains("hello world", "world"));
    EXPECT_FALSE(contains("hello world", "worlds"));
}

TEST(Contains, StartMiddleEnd)
{
    // шаблон в начале, в середине и в конце строки
    EXPECT_TRUE(contains("abcXYZdef", "abc"));
    EXPECT_TRUE(contains("abcXYZdef", "XYZ"));
    EXPECT_TRUE(contains("abcXYZdef", "def"));
}

TEST(Contains, WholeLineMatch)
{
    // шаблон совпал со всей строкой целиком
    EXPECT_TRUE(contains("hello", "hello"));
    // и почти со всей, не хватает одного символа с любой стороны
    EXPECT_TRUE(contains("hello", "ello"));
    EXPECT_TRUE(contains("hello", "hell"));
}

TEST(Contains, PatternLongerThanLine)
{
    EXPECT_FALSE(contains("ab", "abc"));
    EXPECT_FALSE(contains("", "abc"));
}

TEST(Contains, EmptyPatternMatchesAll)
{
    // пустой шаблон совпадает с любой строкой и даже с пустой.
    // настоящий grep со строкой "" тоже печатает всё, проверял
    EXPECT_TRUE(contains("hello", "", false));
    EXPECT_TRUE(contains("", "", false));
}

TEST(Contains, EmptyLineAndNonEmptyPattern)
{
    EXPECT_FALSE(contains("", "a"));
}

TEST(Contains, SingleChar)
{
    // одиночный символ: в начале, в середине, в конце и когда его нет
    EXPECT_TRUE(contains("banana", "b"));
    EXPECT_TRUE(contains("banana", "n"));
    EXPECT_TRUE(contains("banana", "a"));
    EXPECT_FALSE(contains("banana", "x"));
}

TEST(Contains, ManyMatchesInOneLine)
{
    // сколько раз шаблон попал в строку не важно, строка то одна
    EXPECT_TRUE(contains("ababab", "ab"));
    EXPECT_TRUE(contains("aaaa", "aa"));
}

TEST(Contains, PartialWordLikeRosa)
{
    // совпадение внутри слова тоже считается, как "роза" в "проза"
    EXPECT_TRUE(contains("проза", "роза"));
    EXPECT_TRUE(contains("мороза", "роза"));
    // а вот просто рифма уже не подходит
    EXPECT_FALSE(contains("розы", "роза"));
}

TEST(Contains, CaseSensitiveWithoutFlag)
{
    // без -i регистр важен, это основной режим
    EXPECT_TRUE(contains("hello", "hello"));
    EXPECT_FALSE(contains("Hello", "hello"));
    EXPECT_FALSE(contains("HELLO", "hello"));
}

TEST(Contains, IgnoreCaseFlag)
{
    // из условия: Hello, hello и HELLO считаются одинаковыми
    EXPECT_TRUE(contains("Hello", "hello", true));
    EXPECT_TRUE(contains("HELLO", "hello", true));
    EXPECT_TRUE(contains("HeLLo", "hEllO", true));
    EXPECT_TRUE(contains("hello", "hello", true));
}

TEST(Contains, IgnoreCaseNotEverythingFits)
{
    // -i не значит что подойдёт что попало
    EXPECT_FALSE(contains("hallo", "hello", true));
    EXPECT_FALSE(contains("hell", "hello", true));
    EXPECT_FALSE(contains("", "hello", true));
}

TEST(Contains, IgnoreCaseOnlyLatin)
{
    // известное ограничение: кириллица в utf-8, tolower по байтам её не
    // переключает, так что -i на неё не действует. в задании примеры латиницей
    EXPECT_FALSE(contains("Привет", "привет", true));
    // точное совпадение и с -i работает как обычно
    EXPECT_TRUE(contains("Привет", "Привет", true));
}

// ---------- parse_args ----------

TEST(Args, NoFlags)
{
    Options o;
    ASSERT_TRUE(try_parse({"hello", "1.txt"}, o));
    EXPECT_FALSE(o.ignore_case);
    EXPECT_FALSE(o.only_count);
    EXPECT_EQ(o.pattern, "hello");
    EXPECT_EQ(o.files, (vector<string>{"1.txt"}));
}

TEST(Args, FlagsSeparately)
{
    Options o;
    ASSERT_TRUE(try_parse({"-i", "-c", "hello", "1.txt"}, o));
    EXPECT_TRUE(o.ignore_case);
    EXPECT_TRUE(o.only_count);
    EXPECT_EQ(o.pattern, "hello");
}

TEST(Args, FlagsTogether)
{
    // -ic одним куском это то же самое
    Options o;
    ASSERT_TRUE(try_parse({"-ic", "hello", "1.txt"}, o));
    EXPECT_TRUE(o.ignore_case);
    EXPECT_TRUE(o.only_count);
}

TEST(Args, FlagsTogetherOtherOrder)
{
    // и в обратном порядке тоже
    Options o;
    ASSERT_TRUE(try_parse({"-ci", "hello", "1.txt"}, o));
    EXPECT_TRUE(o.ignore_case);
    EXPECT_TRUE(o.only_count);
}

TEST(Args, SameFlagTwice)
{
    // повтор флага ничего не ломает
    Options o;
    ASSERT_TRUE(try_parse({"-i", "-i", "hello", "1.txt"}, o));
    EXPECT_TRUE(o.ignore_case);
    EXPECT_FALSE(o.only_count);
}

TEST(Args, UnknownFlag)
{
    Options o;
    EXPECT_FALSE(try_parse({"-x", "hello", "1.txt"}, o));
}

TEST(Args, UnknownLetterInsideCombined)
{
    // -ix содержит незнакомую букву
    Options o;
    EXPECT_FALSE(try_parse({"-ix", "hello", "1.txt"}, o));
}

TEST(Args, NoArguments)
{
    Options o;
    EXPECT_FALSE(try_parse({}, o));
}

TEST(Args, OnlyFlagsNoPattern)
{
    Options o;
    EXPECT_FALSE(try_parse({"-ic"}, o));
}

TEST(Args, PatternWithoutFiles)
{
    Options o;
    EXPECT_FALSE(try_parse({"hello"}, o));
}

TEST(Args, ManyFiles)
{
    Options o;
    ASSERT_TRUE(try_parse({"hello", "a.txt", "b.txt", "c.txt"}, o));
    EXPECT_EQ(o.files, (vector<string>{"a.txt", "b.txt", "c.txt"}));
}

TEST(Args, FlagAfterPatternIsAFile)
{
    // всё что после шаблона считается файлами, даже если похоже на флаг
    Options o;
    ASSERT_TRUE(try_parse({"hello", "-i", "a.txt"}, o));
    EXPECT_FALSE(o.ignore_case);
    EXPECT_EQ(o.files, (vector<string>{"-i", "a.txt"}));
}

TEST(Args, PatternWithSpaces)
{
    // шаблон с пробелами приходит одним аргументом, тут всё само работает
    Options o;
    ASSERT_TRUE(try_parse({"hello world", "a.txt"}, o));
    EXPECT_EQ(o.pattern, "hello world");
}

TEST(Args, EmptyPatternAllowed)
{
    // пустой шаблон не ошибка, grep "" печатает все строки
    Options o;
    ASSERT_TRUE(try_parse({"", "a.txt"}, o));
    EXPECT_EQ(o.pattern, "");
    EXPECT_EQ(o.files, (vector<string>{"a.txt"}));
}

// ---------- grep_file ----------

TEST(GrepFile, SimpleMatch)
{
    make_file("t_simple.txt", "hello world\nbye\nhello again\n");
    Options opts;
    opts.pattern = "hello";
    vector<string> out;
    ASSERT_TRUE(grep_file("t_simple.txt", opts, out));
    EXPECT_EQ(out, (vector<string>{"hello world", "hello again"}));
    remove("t_simple.txt");
}

TEST(GrepFile, NoMatches)
{
    make_file("t_none.txt", "foo\nbar\n");
    Options opts;
    opts.pattern = "hello";
    vector<string> out;
    ASSERT_TRUE(grep_file("t_none.txt", opts, out));
    EXPECT_TRUE(out.empty());
    remove("t_none.txt");
}

TEST(GrepFile, IgnoreCase)
{
    make_file("t_case.txt", "Hello world\nbye\nHELLO again\n");
    Options opts;
    opts.pattern = "hello";
    opts.ignore_case = true;
    vector<string> out;
    ASSERT_TRUE(grep_file("t_case.txt", opts, out));
    EXPECT_EQ(out, (vector<string>{"Hello world", "HELLO again"}));
    remove("t_case.txt");
}

TEST(GrepFile, EmptyPatternMatchesEverything)
{
    make_file("t_all.txt", "foo\n\nbar\n"); // пустая строка в середине тоже строка
    Options opts;
    opts.pattern = "";
    vector<string> out;
    ASSERT_TRUE(grep_file("t_all.txt", opts, out));
    EXPECT_EQ(out, (vector<string>{"foo", "", "bar"}));
    remove("t_all.txt");
}

TEST(GrepFile, WindowsLineEndings)
{
    // у виндовых строк \r на конце, он не должен уезжать в вывод
    make_file("t_crlf.txt", "hello\r\nbye\n");
    Options opts;
    opts.pattern = "hello";
    vector<string> out;
    ASSERT_TRUE(grep_file("t_crlf.txt", opts, out));
    EXPECT_EQ(out, (vector<string>{"hello"}));
    remove("t_crlf.txt");
}

TEST(GrepFile, MissingFile)
{
    Options opts;
    opts.pattern = "hello";
    vector<string> out;
    // про это пишется в stderr, но проверять сам текст не буду, хватит false
    EXPECT_FALSE(grep_file("t_no_such_file_here.txt", opts, out));
}

// ---------- run_grep ----------

TEST(RunGrep, SingleFileNoPrefix)
{
    make_file("t_one.txt", "hello\nbye\n");
    Options opts;
    opts.pattern = "hello";
    opts.files.push_back("t_one.txt");
    vector<string> out = run_grep(opts);
    EXPECT_EQ(out, (vector<string>{"hello"}));
    remove("t_one.txt");
}

TEST(RunGrep, ManyFilesGetPrefix)
{
    make_file("t_a.txt", "hello\nbye\n");
    make_file("t_b.txt", "hello world\nnothing\n");
    Options opts;
    opts.pattern = "hello";
    opts.files.push_back("t_a.txt");
    opts.files.push_back("t_b.txt");
    vector<string> out = run_grep(opts);
    EXPECT_EQ(out, (vector<string>{"t_a.txt:hello", "t_b.txt:hello world"}));
    remove("t_a.txt");
    remove("t_b.txt");
}

TEST(RunGrep, CountMode)
{
    make_file("t_cnt.txt", "hello\nhello\nbye\n");
    Options opts;
    opts.pattern = "hello";
    opts.only_count = true;
    opts.files.push_back("t_cnt.txt");
    vector<string> out = run_grep(opts);
    EXPECT_EQ(out, (vector<string>{"2"}));
    remove("t_cnt.txt");
}

TEST(RunGrep, CountModeManyFiles)
{
    make_file("t_c1.txt", "hello\nhello\n");
    make_file("t_c2.txt", "hello\nbye\n");
    Options opts;
    opts.pattern = "hello";
    opts.only_count = true;
    opts.files.push_back("t_c1.txt");
    opts.files.push_back("t_c2.txt");
    vector<string> out = run_grep(opts);
    EXPECT_EQ(out, (vector<string>{"t_c1.txt:2", "t_c2.txt:1"}));
    remove("t_c1.txt");
    remove("t_c2.txt");
}

TEST(RunGrep, MissingFileSkipped)
{
    make_file("t_ok.txt", "hello\n");
    Options opts;
    opts.pattern = "hello";
    opts.files.push_back("t_no_such_file_here.txt");
    opts.files.push_back("t_ok.txt");
    // битый файл пишется в stderr и пропускается, остальные идут дальше
    // файлов в списке два, поэтому у уцелевшего строка выходит с префиксом
    vector<string> out = run_grep(opts);
    EXPECT_EQ(out, (vector<string>{"t_ok.txt:hello"}));
    remove("t_ok.txt");
}

// ---------- и всё вместе ----------

TEST(AllTogether, ParseAndRun)
{
    // прогон целиком: флаги одним куском, -i и -c сразу
    make_file("t_full.txt", "Hello\nHELLO\nbye\n");
    Options opts;
    ASSERT_TRUE(try_parse({"-ic", "hello", "t_full.txt"}, opts));
    vector<string> out = run_grep(opts);
    EXPECT_EQ(out, (vector<string>{"2"}));
    remove("t_full.txt");
}
