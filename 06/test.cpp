#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "bigint.hpp"

// чтобы не писать каждый раз stringstream
static std::string str(const BigInt &x)
{
    std::stringstream ss;
    ss << x;
    return ss.str();
}

TEST(BigInt, DefaultIsZero)
{
    BigInt a;
    EXPECT_EQ(str(a), "0");
}

TEST(BigInt, FromInt)
{
    EXPECT_EQ(str(BigInt(0)), "0");
    EXPECT_EQ(str(BigInt(5)), "5");
    EXPECT_EQ(str(BigInt(-5)), "-5");
    EXPECT_EQ(str(BigInt(1000)), "1000");
    EXPECT_EQ(str(BigInt(-120034)), "-120034");
}

TEST(BigInt, FromIntLimits)
{
    EXPECT_EQ(str(BigInt(std::numeric_limits<int32_t>::max())), "2147483647");
    EXPECT_EQ(str(BigInt(std::numeric_limits<int32_t>::min())), "-2147483648");
}

TEST(BigInt, FromString)
{
    EXPECT_EQ(str(BigInt("123456789012345678901234567890")), "123456789012345678901234567890");
    EXPECT_EQ(str(BigInt(std::string("-98765432109876543210"))), "-98765432109876543210");
    EXPECT_EQ(str(BigInt("+17")), "17");
}

TEST(BigInt, FromStringLeadingZeros)
{
    EXPECT_EQ(str(BigInt("000123")), "123");
    EXPECT_EQ(str(BigInt("-0007")), "-7");
}

TEST(BigInt, FromStringZero)
{
    EXPECT_EQ(str(BigInt("0")), "0");
    EXPECT_EQ(str(BigInt("0000")), "0");
    // минус ноль - это просто ноль
    EXPECT_EQ(str(BigInt("-0")), "0");
    EXPECT_EQ(str(BigInt("-000")), "0");
    EXPECT_TRUE(BigInt("-0") == BigInt(0));
}

TEST(BigInt, FromStringBad)
{
    EXPECT_THROW(BigInt(""), std::invalid_argument);
    EXPECT_THROW(BigInt("-"), std::invalid_argument);
    EXPECT_THROW(BigInt("+"), std::invalid_argument);
    EXPECT_THROW(BigInt("12a3"), std::invalid_argument);
    EXPECT_THROW(BigInt("abc"), std::invalid_argument);
    EXPECT_THROW(BigInt("12 3"), std::invalid_argument);
    EXPECT_THROW(BigInt("1-2"), std::invalid_argument);
    EXPECT_THROW(BigInt("--5"), std::invalid_argument);
    EXPECT_THROW(BigInt("1.5"), std::invalid_argument);
}

TEST(BigInt, Example)
{
    // пример из задания
    BigInt a = 1;
    BigInt b("123456789012345678901234567890");
    BigInt c = a * b + 2;
    BigInt d;
    d = std::move(c);
    a = d + b;
    EXPECT_EQ(str(a), "246913578024691357802469135782");
}

TEST(BigInt, AddSimple)
{
    EXPECT_EQ(str(BigInt(2) + BigInt(3)), "5");
    EXPECT_EQ(str(BigInt(-2) + BigInt(-3)), "-5");
    EXPECT_EQ(str(BigInt(0) + BigInt(0)), "0");
    EXPECT_EQ(str(BigInt(0) + BigInt(7)), "7");
    EXPECT_EQ(str(BigInt(-7) + BigInt(0)), "-7");
}

TEST(BigInt, AddCarry)
{
    EXPECT_EQ(str(BigInt(9) + BigInt(1)), "10");
    EXPECT_EQ(str(BigInt("999999999999999999999") + BigInt(1)), "1000000000000000000000");
    EXPECT_EQ(str(BigInt("99999999999999999999") + BigInt("99999999999999999999")), "199999999999999999998");
}

TEST(BigInt, AddDifferentLength)
{
    EXPECT_EQ(str(BigInt("100000000000000000000") + BigInt(5)), "100000000000000000005");
    EXPECT_EQ(str(BigInt(5) + BigInt("100000000000000000000")), "100000000000000000005");
}

TEST(BigInt, AddDifferentSigns)
{
    EXPECT_EQ(str(BigInt(10) + BigInt(-3)), "7");
    EXPECT_EQ(str(BigInt(3) + BigInt(-10)), "-7");
    EXPECT_EQ(str(BigInt(-10) + BigInt(3)), "-7");
    EXPECT_EQ(str(BigInt(-3) + BigInt(10)), "7");
}

TEST(BigInt, AddToZero)
{
    BigInt r = BigInt("123456789012345678901234567890") + BigInt("-123456789012345678901234567890");
    EXPECT_EQ(str(r), "0");
    EXPECT_TRUE(r == BigInt(0));
    // и после этого можно спокойно прибавлять
    EXPECT_EQ(str(r + 5), "5");
    EXPECT_EQ(str(r + -5), "-5");
}

TEST(BigInt, AddInt)
{
    BigInt a("100000000000000000000");
    EXPECT_EQ(str(a + 1), "100000000000000000001");
    EXPECT_EQ(str(a + -1), "99999999999999999999");
    EXPECT_EQ(str(BigInt(-5) + 5), "0");
    EXPECT_EQ(str(a + std::numeric_limits<int32_t>::max()), "100000000002147483647");
    EXPECT_EQ(str(a + std::numeric_limits<int32_t>::min()), "99999999997852516352");
}

TEST(BigInt, SubSimple)
{
    EXPECT_EQ(str(BigInt(5) - BigInt(3)), "2");
    EXPECT_EQ(str(BigInt(3) - BigInt(5)), "-2");
    EXPECT_EQ(str(BigInt(-3) - BigInt(5)), "-8");
    EXPECT_EQ(str(BigInt(-3) - BigInt(-5)), "2");
    EXPECT_EQ(str(BigInt(3) - BigInt(-5)), "8");
    EXPECT_EQ(str(BigInt(0) - BigInt(5)), "-5");
    EXPECT_EQ(str(BigInt(5) - BigInt(0)), "5");
    EXPECT_EQ(str(BigInt(0) - BigInt(0)), "0");
}

TEST(BigInt, SubBorrow)
{
    EXPECT_EQ(str(BigInt(1000) - BigInt(1)), "999");
    EXPECT_EQ(str(BigInt("100000000000000000000") - BigInt(1)), "99999999999999999999");
    EXPECT_EQ(str(BigInt("1000000000000000000000") - BigInt("999999999999999999999")), "1");
    EXPECT_EQ(str(BigInt("123456789012345678901234567890") - BigInt("987654321098765432109876543210")),
              "-864197532086419753208641975320");
}

TEST(BigInt, SubSame)
{
    BigInt a("55555555555555555555");
    EXPECT_EQ(str(a - a), "0");
    EXPECT_EQ(str(a - BigInt("55555555555555555555")), "0");
    // результат должен быть нормальным нулём
    EXPECT_TRUE((a - a) == BigInt(0));
    EXPECT_TRUE((a - a) == -(a - a));
}

TEST(BigInt, SubInt)
{
    BigInt a("100000000000000000000");
    EXPECT_EQ(str(a - 1), "99999999999999999999");
    EXPECT_EQ(str(a - -1), "100000000000000000001");
    EXPECT_EQ(str(BigInt(3) - 10), "-7");
    EXPECT_EQ(str(BigInt(0) - std::numeric_limits<int32_t>::min()), "2147483648");
}

TEST(BigInt, MulSimple)
{
    EXPECT_EQ(str(BigInt(6) * BigInt(7)), "42");
    EXPECT_EQ(str(BigInt(-6) * BigInt(7)), "-42");
    EXPECT_EQ(str(BigInt(6) * BigInt(-7)), "-42");
    EXPECT_EQ(str(BigInt(-6) * BigInt(-7)), "42");
}

TEST(BigInt, MulZero)
{
    EXPECT_EQ(str(BigInt(0) * BigInt(5)), "0");
    EXPECT_EQ(str(BigInt(5) * BigInt(0)), "0");
    EXPECT_EQ(str(BigInt(0) * BigInt(0)), "0");
    // минус на ноль не должен давать "-0"
    EXPECT_EQ(str(BigInt(-5) * BigInt(0)), "0");
    EXPECT_EQ(str(BigInt(0) * BigInt(-5)), "0");
    EXPECT_EQ(str(BigInt("-123456789012345678901234567890") * 0), "0");
}

TEST(BigInt, MulOne)
{
    BigInt a("123456789012345678901234567890");
    EXPECT_EQ(str(a * 1), "123456789012345678901234567890");
    EXPECT_EQ(str(a * -1), "-123456789012345678901234567890");
    EXPECT_EQ(str(BigInt(1) * a), "123456789012345678901234567890");
}

TEST(BigInt, MulBig)
{
    BigInt a("123456789012345678901234567890");
    BigInt b("987654321098765432109876543210");
    EXPECT_EQ(str(a * b), "121932631137021795226185032733622923332237463801111263526900");
    EXPECT_EQ(str(a * -b), "-121932631137021795226185032733622923332237463801111263526900");
    EXPECT_EQ(str(-a * -b), "121932631137021795226185032733622923332237463801111263526900");
}

TEST(BigInt, MulCarries)
{
    // много девяток, переносы идут через все разряды
    EXPECT_EQ(str(BigInt("99999999999999999999") * BigInt("99999999999999999999")),
              "9999999999999999999800000000000000000001");
    EXPECT_EQ(str(BigInt(99999) * BigInt(99999)), "9999800001");
    EXPECT_EQ(str(BigInt("10000000000") * BigInt("10000000000")), "100000000000000000000");
}

TEST(BigInt, MulInt)
{
    BigInt a("12345678901234567890");
    EXPECT_EQ(str(a * 10), "123456789012345678900");
    EXPECT_EQ(str(a * -10), "-123456789012345678900");
    EXPECT_EQ(str(BigInt(2) * std::numeric_limits<int32_t>::max()), "4294967294");
    EXPECT_EQ(str(BigInt(std::numeric_limits<int32_t>::min()) * std::numeric_limits<int32_t>::min()),
              "4611686018427387904");
}

TEST(BigInt, Factorial)
{
    BigInt f = 1;
    for (int i = 1; i <= 30; i++)
        f = f * i;
    EXPECT_EQ(str(f), "265252859812191058636308480000000");
}

TEST(BigInt, Power)
{
    BigInt p = 1;
    for (int i = 0; i < 100; i++)
        p = p * 2;
    EXPECT_EQ(str(p), "1267650600228229401496703205376");
}

TEST(BigInt, UnaryMinus)
{
    EXPECT_EQ(str(-BigInt(5)), "-5");
    EXPECT_EQ(str(-BigInt(-5)), "5");
    EXPECT_EQ(str(-BigInt("123456789012345678901234567890")), "-123456789012345678901234567890");
    EXPECT_EQ(str(-(-BigInt("123456789012345678901234567890"))), "123456789012345678901234567890");
}

TEST(BigInt, UnaryMinusZero)
{
    EXPECT_EQ(str(-BigInt(0)), "0");
    EXPECT_EQ(str(-BigInt()), "0");
    EXPECT_TRUE(-BigInt(0) == BigInt(0));
}

TEST(BigInt, UnaryMinusDoesNotChangeOriginal)
{
    BigInt a = 7;
    BigInt b = -a;
    EXPECT_EQ(str(a), "7");
    EXPECT_EQ(str(b), "-7");
}

TEST(BigInt, Equal)
{
    EXPECT_TRUE(BigInt(5) == BigInt(5));
    EXPECT_FALSE(BigInt(5) == BigInt(6));
    EXPECT_FALSE(BigInt(5) == BigInt(-5));
    EXPECT_TRUE(BigInt(-5) == BigInt(-5));
    EXPECT_TRUE(BigInt() == BigInt(0));
    EXPECT_TRUE(BigInt("123456789012345678901234567890") == BigInt("123456789012345678901234567890"));
    // отличаются только последней цифрой
    EXPECT_FALSE(BigInt("123456789012345678901234567890") == BigInt("123456789012345678901234567891"));
    // отличаются длиной
    EXPECT_FALSE(BigInt("1000") == BigInt("10000"));
}

TEST(BigInt, NotEqual)
{
    EXPECT_TRUE(BigInt(5) != BigInt(6));
    EXPECT_TRUE(BigInt(5) != BigInt(-5));
    EXPECT_FALSE(BigInt(5) != BigInt(5));
    EXPECT_FALSE(BigInt(0) != BigInt(-0));
}

TEST(BigInt, Less)
{
    EXPECT_TRUE(BigInt(1) < BigInt(2));
    EXPECT_FALSE(BigInt(2) < BigInt(1));
    EXPECT_FALSE(BigInt(2) < BigInt(2));
    EXPECT_TRUE(BigInt(-2) < BigInt(1));
    EXPECT_FALSE(BigInt(1) < BigInt(-2));
    EXPECT_TRUE(BigInt(-3) < BigInt(-2));
    EXPECT_FALSE(BigInt(-2) < BigInt(-3));
    EXPECT_FALSE(BigInt(-2) < BigInt(-2));
    EXPECT_TRUE(BigInt(-1) < BigInt(0));
    EXPECT_TRUE(BigInt(0) < BigInt(1));
    EXPECT_FALSE(BigInt(0) < BigInt(0));
}

TEST(BigInt, LessDifferentLength)
{
    EXPECT_TRUE(BigInt(999) < BigInt(1000));
    EXPECT_FALSE(BigInt(1000) < BigInt(999));
    EXPECT_TRUE(BigInt(-1000) < BigInt(-999));
    EXPECT_FALSE(BigInt(-999) < BigInt(-1000));
    EXPECT_TRUE(BigInt("99999999999999999999") < BigInt("100000000000000000000"));
    EXPECT_TRUE(BigInt("-100000000000000000000") < BigInt("-99999999999999999999"));
}

TEST(BigInt, LessSameLengthDifferentDigit)
{
    EXPECT_TRUE(BigInt("123456789012345678901234567890") < BigInt("123456789012345678901234567891"));
    EXPECT_FALSE(BigInt("123456789012345678901234567891") < BigInt("123456789012345678901234567890"));
    EXPECT_TRUE(BigInt("1999") < BigInt("2000"));
    EXPECT_TRUE(BigInt("-2000") < BigInt("-1999"));
    EXPECT_FALSE(BigInt("2000") < BigInt("1999"));
}

TEST(BigInt, Greater)
{
    EXPECT_TRUE(BigInt(2) > BigInt(1));
    EXPECT_FALSE(BigInt(1) > BigInt(2));
    EXPECT_FALSE(BigInt(1) > BigInt(1));
    EXPECT_TRUE(BigInt(1) > BigInt(-2));
    EXPECT_TRUE(BigInt(-2) > BigInt(-3));
    EXPECT_FALSE(BigInt(-3) > BigInt(-2));
    EXPECT_TRUE(BigInt(0) > BigInt(-1));
}

TEST(BigInt, LessEqual)
{
    EXPECT_TRUE(BigInt(1) <= BigInt(2));
    EXPECT_TRUE(BigInt(2) <= BigInt(2));
    EXPECT_FALSE(BigInt(3) <= BigInt(2));
    EXPECT_TRUE(BigInt(-3) <= BigInt(-3));
    EXPECT_TRUE(BigInt(-3) <= BigInt(-2));
    EXPECT_FALSE(BigInt(-2) <= BigInt(-3));
    EXPECT_TRUE(BigInt(0) <= BigInt(-0));
}

TEST(BigInt, GreaterEqual)
{
    EXPECT_TRUE(BigInt(2) >= BigInt(1));
    EXPECT_TRUE(BigInt(2) >= BigInt(2));
    EXPECT_FALSE(BigInt(1) >= BigInt(2));
    EXPECT_TRUE(BigInt(-3) >= BigInt(-3));
    EXPECT_TRUE(BigInt(-2) >= BigInt(-3));
    EXPECT_FALSE(BigInt(-3) >= BigInt(-2));
    EXPECT_TRUE(BigInt(0) >= BigInt(-0));
}


TEST(BigInt, CopyConstructor)
{
    BigInt a("123456789012345678901234567890");
    BigInt b(a);
    EXPECT_TRUE(a == b);

    // b должен быть независимой копией
    b = b + 1;
    EXPECT_EQ(str(a), "123456789012345678901234567890");
    EXPECT_EQ(str(b), "123456789012345678901234567891");
}

TEST(BigInt, CopyConstructorZero)
{
    BigInt a;
    BigInt b(a);
    EXPECT_EQ(str(b), "0");
    EXPECT_TRUE(a == b);
}

TEST(BigInt, CopyAssign)
{
    BigInt a("-123456789012345678901234567890");
    BigInt b = 5;
    b = a;
    EXPECT_TRUE(a == b);
    EXPECT_EQ(str(b), "-123456789012345678901234567890");

    a = a + 1;
    EXPECT_EQ(str(b), "-123456789012345678901234567890");
}

TEST(BigInt, CopyAssignToShorterAndLonger)
{
    BigInt big("123456789012345678901234567890");
    BigInt small = 3;

    small = big;
    EXPECT_EQ(str(small), "123456789012345678901234567890");

    BigInt big2("99999999999999999999999999");
    big2 = BigInt(3);
    EXPECT_EQ(str(big2), "3");
}

TEST(BigInt, CopyAssignZero)
{
    BigInt a("123456789012345678901234567890");
    BigInt z;
    a = z;
    EXPECT_EQ(str(a), "0");
}

TEST(BigInt, SelfAssign)
{
    BigInt a("123456789012345678901234567890");
    BigInt &ref = a;
    a = ref;
    EXPECT_EQ(str(a), "123456789012345678901234567890");

    a = std::move(ref);
    EXPECT_EQ(str(a), "123456789012345678901234567890");
}

TEST(BigInt, ChainAssign)
{
    BigInt a, b, c;
    a = b = c = BigInt("-777777777777777777777");
    EXPECT_EQ(str(a), "-777777777777777777777");
    EXPECT_EQ(str(b), "-777777777777777777777");
    EXPECT_EQ(str(c), "-777777777777777777777");
}

TEST(BigInt, MoveConstructor)
{
    BigInt a("123456789012345678901234567890");
    BigInt b(std::move(a));
    EXPECT_EQ(str(b), "123456789012345678901234567890");

    // после перемещения a - валидный ноль
    EXPECT_EQ(str(a), "0");
    EXPECT_TRUE(a == BigInt(0));
    a = BigInt(15);
    EXPECT_EQ(str(a), "15");
}

TEST(BigInt, MoveAssign)
{
    BigInt a("-123456789012345678901234567890");
    BigInt b("5");
    b = std::move(a);
    EXPECT_EQ(str(b), "-123456789012345678901234567890");
    EXPECT_EQ(str(a), "0");

    // перемещённый объект можно использовать
    a = b + 1;
    EXPECT_EQ(str(a), "-123456789012345678901234567889");
}

TEST(BigInt, MoveZero)
{
    BigInt a;
    BigInt b(std::move(a));
    EXPECT_EQ(str(b), "0");

    BigInt c = 5;
    c = std::move(b);
    EXPECT_EQ(str(c), "0");
}

TEST(BigInt, ReturnFromFunction)
{
    auto make = []() {
        BigInt x("99999999999999999999");
        return x + 1;
    };
    BigInt r = make();
    EXPECT_EQ(str(r), "100000000000000000000");
}

TEST(BigInt, Output)
{
    std::stringstream ss;
    ss << BigInt(12) << ' ' << BigInt(-34) << ' ' << BigInt(0);
    EXPECT_EQ(ss.str(), "12 -34 0");
}

TEST(BigInt, ConstOperands)
{
    const BigInt a = 10;
    const BigInt b = 20;
    EXPECT_EQ(str(a + b), "30");
    EXPECT_EQ(str(a - b), "-10");
    EXPECT_EQ(str(a * b), "200");
    EXPECT_EQ(str(-a), "-10");
    EXPECT_TRUE(a < b);
}

TEST(BigInt, LongNumbers)
{
    // 1000 цифр
    std::string s(1000, '9');
    BigInt a(s);
    EXPECT_EQ(str(a), s);

    std::string expected = "1" + std::string(1000, '0');
    EXPECT_EQ(str(a + 1), expected);
    EXPECT_EQ(str((a + 1) - 1), s);

    // (10^1000 - 1)^2 = 10^2000 - 2 * 10^1000 + 1
    std::string sq = std::string(999, '9') + "8" + std::string(999, '0') + "1";
    EXPECT_EQ(str(a * a), sq);
}

