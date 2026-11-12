#include <gtest/gtest.h>

#include <cstdint>
#include <sstream>
#include <stdexcept>

#include "matrix.hpp"

TEST(Matrix, Sizes)
{
    Matrix m(5, 3);
    EXPECT_EQ(m.getRows(), 5u);
    EXPECT_EQ(m.getColumns(), 3u);
}

TEST(Matrix, ZeroAtStart)
{
    Matrix m(3, 4);
    for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 4; j++)
            EXPECT_EQ(m[i][j], 0);
}

TEST(Matrix, SetGet)
{
    Matrix m(5, 3);
    m[1][2] = 5;
    m[4][1] = -8;
    m[0][0] = 1;
    m[4][2] = 100;

    EXPECT_EQ(m[1][2], 5);
    EXPECT_EQ(m[4][1], -8);
    EXPECT_EQ(m[0][0], 1);
    EXPECT_EQ(m[4][2], 100);
    // остальное не задели
    EXPECT_EQ(m[1][1], 0);
    EXPECT_EQ(m[2][2], 0);
}

TEST(Matrix, ConstGet)
{
    Matrix m(2, 2);
    m[1][0] = 7;
    const Matrix &cm = m;
    EXPECT_EQ(cm[1][0], 7);
    EXPECT_EQ(cm[0][1], 0);
}

TEST(Matrix, OutOfRangeRows)
{
    Matrix m(2, 3);
    EXPECT_NO_THROW(m[1]);
    EXPECT_THROW(m[2], std::out_of_range);
    EXPECT_THROW(m[3][0], std::out_of_range);
    EXPECT_THROW(m[1000][0], std::out_of_range);
}

TEST(Matrix, OutOfRangeColumns)
{
    Matrix m(2, 3);
    EXPECT_NO_THROW(m[0][2]);
    EXPECT_THROW(m[0][3], std::out_of_range);
    EXPECT_THROW(m[1][4], std::out_of_range);
    EXPECT_THROW(m[1][1000], std::out_of_range);
}

TEST(Matrix, OutOfRangeConst)
{
    const Matrix m(2, 3);
    EXPECT_NO_THROW(m[1][2]);
    EXPECT_THROW(m[2][0], std::out_of_range);
    EXPECT_THROW(m[0][3], std::out_of_range);
}

TEST(Matrix, OutOfRangeWrite)
{
    Matrix m(2, 2);
    EXPECT_THROW(m[2][0] = 1, std::out_of_range);
    EXPECT_THROW(m[0][2] = 1, std::out_of_range);
}

TEST(Matrix, Empty)
{
    Matrix a(0, 0);
    EXPECT_EQ(a.getRows(), 0u);
    EXPECT_EQ(a.getColumns(), 0u);
    EXPECT_THROW(a[0], std::out_of_range);

    Matrix b(0, 5);
    EXPECT_THROW(b[0][0], std::out_of_range);

    // строки есть, а столбцов нет
    Matrix c(3, 0);
    EXPECT_NO_THROW(c[0]);
    EXPECT_THROW(c[0][0], std::out_of_range);
    EXPECT_THROW(c[3], std::out_of_range);
}

TEST(Matrix, OneByOne)
{
    Matrix m(1, 1);
    m[0][0] = 9;
    EXPECT_EQ(m[0][0], 9);
    EXPECT_THROW(m[1][0], std::out_of_range);
    EXPECT_THROW(m[0][1], std::out_of_range);
}

TEST(Matrix, Multiply)
{
    Matrix m(2, 2);
    m[0][0] = 1;
    m[0][1] = -2;
    m[1][0] = 0;
    m[1][1] = 5;
    m *= 3;

    EXPECT_EQ(m[0][0], 3);
    EXPECT_EQ(m[0][1], -6);
    EXPECT_EQ(m[1][0], 0);
    EXPECT_EQ(m[1][1], 15);
}

TEST(Matrix, MultiplyByZeroOneMinusOne)
{
    Matrix m(2, 3);
    for (size_t i = 0; i < 2; i++)
        for (size_t j = 0; j < 3; j++)
            m[i][j] = i * 3 + j + 1;

    Matrix orig = m;
    m *= 1;
    EXPECT_TRUE(m == orig);

    m *= -1;
    EXPECT_EQ(m[0][0], -1);
    EXPECT_EQ(m[1][2], -6);

    m *= 0;
    for (size_t i = 0; i < 2; i++)
        for (size_t j = 0; j < 3; j++)
            EXPECT_EQ(m[i][j], 0);
}

TEST(Matrix, MultiplyReturnsSelf)
{
    Matrix m(1, 2);
    m[0][1] = 2;
    (m *= 2) *= 3;
    EXPECT_EQ(m[0][1], 12);
}

TEST(Matrix, MultiplyEmpty)
{
    Matrix m(0, 0);
    m *= 5;
    EXPECT_EQ(m.getRows(), 0u);
}

TEST(Matrix, Plus)
{
    Matrix a(2, 2);
    Matrix b(2, 2);
    a[0][0] = 1;
    a[0][1] = 2;
    a[1][0] = 3;
    a[1][1] = 4;
    b[0][0] = 10;
    b[1][1] = -4;

    Matrix c = a + b;
    EXPECT_EQ(c[0][0], 11);
    EXPECT_EQ(c[0][1], 2);
    EXPECT_EQ(c[1][0], 3);
    EXPECT_EQ(c[1][1], 0);

    // исходные не поменялись
    EXPECT_EQ(a[0][0], 1);
    EXPECT_EQ(b[0][0], 10);
    EXPECT_EQ(b[1][1], -4);
}

TEST(Matrix, PlusChain)
{
    Matrix a(1, 2);
    a[0][0] = 1;
    a[0][1] = 2;
    Matrix r = a + a + a;
    EXPECT_EQ(r[0][0], 3);
    EXPECT_EQ(r[0][1], 6);
}

TEST(Matrix, PlusDifferentSizes)
{
    Matrix a(2, 2);
    Matrix b(2, 3);
    Matrix c(3, 2);
    Matrix d(3, 3);
    EXPECT_THROW(a + b, std::out_of_range);
    EXPECT_THROW(a + c, std::out_of_range);
    EXPECT_THROW(a + d, std::out_of_range);
}

TEST(Matrix, PlusEmpty)
{
    Matrix a(0, 0);
    Matrix b(0, 0);
    Matrix c = a + b;
    EXPECT_EQ(c.getRows(), 0u);
    EXPECT_EQ(c.getColumns(), 0u);
}

TEST(Matrix, PlusBigValues)
{
    Matrix a(1, 1);
    Matrix b(1, 1);
    a[0][0] = 1000000000;
    b[0][0] = 1000000000;
    Matrix c = a + b;
    EXPECT_EQ(c[0][0], 2000000000);
}

TEST(Matrix, Equality)
{
    Matrix a(2, 2);
    Matrix b(2, 2);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);

    a[1][0] = 42;
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);

    b[1][0] = 42;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(Matrix, EqualitySelf)
{
    Matrix a(3, 3);
    a[2][2] = 1;
    EXPECT_TRUE(a == a);
    EXPECT_FALSE(a != a);
}

TEST(Matrix, EqualityFirstAndLastElementDiffers)
{
    Matrix a(3, 3);
    Matrix b(3, 3);
    b[2][2] = 1;
    EXPECT_TRUE(a != b);

    Matrix c(3, 3);
    Matrix d(3, 3);
    d[0][0] = 1;
    EXPECT_TRUE(c != d);
}

TEST(Matrix, EqualityDifferentSizes)
{
    Matrix a(2, 3);
    Matrix b(3, 2);   // элементов столько же, но форма другая
    Matrix c(2, 2);
    Matrix d(1, 6);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
    EXPECT_FALSE(a == d);
    EXPECT_TRUE(a != d);
}

TEST(Matrix, EqualityEmpty)
{
    Matrix a(0, 0);
    Matrix b(0, 0);
    EXPECT_TRUE(a == b);
}

TEST(Matrix, Output)
{
    Matrix m(2, 3);
    int x = 1;
    for (size_t i = 0; i < 2; i++)
        for (size_t j = 0; j < 3; j++)
            m[i][j] = x++;

    std::ostringstream out;
    out << m;
    EXPECT_EQ(out.str(), "1 2 3\n4 5 6\n");
}

TEST(Matrix, OutputNegativeOneElement)
{
    Matrix m(1, 1);
    m[0][0] = -15;
    std::ostringstream out;
    out << m;
    EXPECT_EQ(out.str(), "-15\n");
}

TEST(Matrix, OutputEmpty)
{
    Matrix m(0, 0);
    std::ostringstream out;
    out << m;
    EXPECT_EQ(out.str(), "");
}

TEST(Matrix, OutputChain)
{
    Matrix a(1, 2);
    a[0][0] = 1;
    a[0][1] = 2;
    std::ostringstream out;
    out << a << "|" << a;
    EXPECT_EQ(out.str(), "1 2\n|1 2\n");
}

TEST(Matrix, OutputSum)
{
    Matrix a(1, 2);
    a[0][0] = 1;
    a[0][1] = 2;
    std::ostringstream out;
    out << a + a;
    EXPECT_EQ(out.str(), "2 4\n");
}

TEST(Matrix, Copy)
{
    Matrix a(2, 2);
    a[0][1] = 7;
    Matrix b = a;
    EXPECT_TRUE(a == b);

    b[0][1] = 8;
    EXPECT_EQ(a[0][1], 7);
    EXPECT_EQ(b[0][1], 8);
    EXPECT_TRUE(a != b);
}

TEST(Matrix, Assign)
{
    Matrix a(2, 2);
    a[1][1] = 9;
    Matrix b(1, 3);
    b = a;

    EXPECT_EQ(b.getRows(), 2u);
    EXPECT_EQ(b.getColumns(), 2u);
    EXPECT_TRUE(a == b);

    b[1][1] = 10;
    EXPECT_EQ(a[1][1], 9);

    // после присваивания границы тоже новые
    EXPECT_THROW(b[0][2], std::out_of_range);
    EXPECT_THROW(b[2][0], std::out_of_range);
}

TEST(Matrix, AssignSelf)
{
    Matrix a(2, 2);
    a[1][1] = 9;
    Matrix &ref = a;
    a = ref;
    EXPECT_EQ(a[1][1], 9);
    EXPECT_EQ(a.getRows(), 2u);
}

TEST(Matrix, AssignToBigger)
{
    Matrix a(1, 1);
    a[0][0] = 3;
    Matrix b(4, 4);
    b[3][3] = 1;
    b = a;
    EXPECT_EQ(b.getRows(), 1u);
    EXPECT_EQ(b.getColumns(), 1u);
    EXPECT_EQ(b[0][0], 3);
    EXPECT_THROW(b[3][3], std::out_of_range);
}

TEST(Matrix, AssignChain)
{
    Matrix a(1, 1);
    a[0][0] = 4;
    Matrix b(2, 2);
    Matrix c(3, 3);
    c = b = a;
    EXPECT_TRUE(b == a);
    EXPECT_TRUE(c == a);
}

TEST(Matrix, ExampleFromTask)
{
    const size_t rows = 5;
    const size_t cols = 3;

    Matrix m(rows, cols);
    EXPECT_EQ(m.getRows(), 5u);
    EXPECT_EQ(m.getColumns(), 3u);

    m[1][2] = 5;
    int32_t x = m[4][1];
    EXPECT_EQ(x, 0);

    m *= 3;
    EXPECT_EQ(m[1][2], 15);

    Matrix m1(rows, cols);
    EXPECT_FALSE(m1 == m);

    Matrix m2 = m1 + m;
    EXPECT_TRUE(m2 == m);

    std::ostringstream out;
    out << m2;
    EXPECT_EQ(out.str(), "0 0 0\n0 0 15\n0 0 0\n0 0 0\n0 0 0\n");
}

