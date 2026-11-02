#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>

#include "matrix.hpp"

TEST(Matrix, Constructor)
{
    Matrix m(5, 3);
    EXPECT_EQ(m.getRows(), 5);
    EXPECT_EQ(m.getColumns(), 3);

    // изначально пусть будут нули, иначе в m1 из примера мусор
    for (size_t i = 0; i < m.getRows(); i++)
        for (size_t j = 0; j < m.getColumns(); j++)
            EXPECT_EQ(m[i][j], 0);
}

TEST(Matrix, SetAndGet)
{
    Matrix m(2, 3);
    m[0][0] = 1;
    m[1][2] = -7;
    EXPECT_EQ(m[0][0], 1);
    EXPECT_EQ(m[1][2], -7);

    const Matrix &const_m = m;
    EXPECT_EQ(const_m[1][2], -7);
}

TEST(Matrix, OutOfRange)
{
    Matrix m(2, 3);
    EXPECT_THROW(m[2][0], std::out_of_range);
    EXPECT_THROW(m[0][3], std::out_of_range);
    EXPECT_THROW(m[100][100], std::out_of_range);

    const Matrix &const_m = m;
    EXPECT_THROW(const_m[2][0], std::out_of_range);
    EXPECT_THROW(const_m[0][3], std::out_of_range);

    Matrix empty(0, 0);
    EXPECT_THROW(empty[0][0], std::out_of_range);
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

    m *= 0;
    EXPECT_EQ(m[0][0], 0);
    EXPECT_EQ(m[1][1], 0);
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

    // a и b не должны поменяться
    EXPECT_EQ(a[0][0], 1);
    EXPECT_EQ(b[0][0], 10);
}

TEST(Matrix, PlusDifferentSizes)
{
    Matrix a(2, 2);
    Matrix b(2, 3);
    Matrix c(3, 2);
    EXPECT_THROW(a + b, std::out_of_range);
    EXPECT_THROW(a + c, std::out_of_range);
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

    Matrix c(2, 3);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
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

TEST(Matrix, Copy)
{
    Matrix a(2, 2);
    a[0][1] = 7;
    Matrix b = a;
    EXPECT_TRUE(a == b);

    b[0][1] = 8;
    EXPECT_EQ(a[0][1], 7);
    EXPECT_EQ(b[0][1], 8);
}

TEST(Matrix, Assignment)
{
    Matrix a(2, 2);
    a[1][1] = 9;
    Matrix b(1, 3);
    b = a;

    EXPECT_EQ(b.getRows(), 2);
    EXPECT_EQ(b.getColumns(), 2);
    EXPECT_TRUE(a == b);

    b[1][1] = 10;
    EXPECT_EQ(a[1][1], 9);
    a = a;
    EXPECT_EQ(a[1][1], 9);
}
