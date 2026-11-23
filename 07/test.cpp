#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>

#include "vector.hpp"

// штука чтобы считать сколько объектов сейчас живёт (ловим утечки и двойные удаления)
struct Counted
{
    static int alive;
    static int copies;
    int value;

    Counted(int v = 0) : value(v) { alive++; }
    Counted(const Counted &o) : value(o.value) { alive++; copies++; }
    Counted(Counted &&o) noexcept : value(o.value) { alive++; o.value = -1; }
    Counted &operator=(const Counted &o) { value = o.value; copies++; return *this; }
    Counted &operator=(Counted &&o) noexcept { value = o.value; o.value = -1; return *this; }
    ~Counted() { alive--; }
};
int Counted::alive = 0;
int Counted::copies = 0;

// ---------- общий vector ----------

TEST(Vector, DefaultConstructor)
{
    my::vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_EQ(v.data(), nullptr);
}

TEST(Vector, SizeConstructor)
{
    my::vector<int> v(5);
    EXPECT_EQ(v.size(), 5);
    EXPECT_FALSE(v.empty());
    for (size_t i = 0; i < v.size(); i++)
        EXPECT_EQ(v[i], 0);
}

TEST(Vector, SizeValueConstructor)
{
    my::vector<std::string> v(3, "abc");
    EXPECT_EQ(v.size(), 3);
    for (size_t i = 0; i < 3; i++)
        EXPECT_EQ(v[i], "abc");
}

TEST(Vector, InitializerList)
{
    my::vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[3], 4);
}

TEST(Vector, SubscriptWrite)
{
    my::vector<int> v = {1, 2, 3};
    v[1] = 20;
    EXPECT_EQ(v[1], 20);

    const my::vector<int> &c = v;
    EXPECT_EQ(c[1], 20);
}

TEST(Vector, FrontBack)
{
    my::vector<int> v = {7, 8, 9};
    EXPECT_EQ(v.front(), 7);
    EXPECT_EQ(v.back(), 9);

    v.front() = 1;
    v.back() = 3;
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[2], 3);

    const my::vector<int> &c = v;
    EXPECT_EQ(c.front(), 1);
    EXPECT_EQ(c.back(), 3);
}

TEST(Vector, SingleElementFrontBack)
{
    my::vector<int> v;
    v.push_back(42);
    EXPECT_EQ(&v.front(), &v.back());
}

TEST(Vector, Data)
{
    my::vector<int> v = {1, 2, 3};
    int *p = v.data();
    EXPECT_EQ(p[0], 1);
    EXPECT_EQ(p[2], 3);
    p[1] = 5;
    EXPECT_EQ(v[1], 5);
    EXPECT_EQ(p, &v[0]);

    const my::vector<int> &c = v;
    EXPECT_EQ(c.data(), p);
}

TEST(Vector, MaxSize)
{
    my::vector<int> v;
    EXPECT_GT(v.max_size(), 1000000);

    my::vector<char> c;
    EXPECT_GT(c.max_size(), v.max_size());
}

TEST(Vector, PushBack)
{
    my::vector<int> v;
    for (int i = 0; i < 100; i++)
    {
        v.push_back(i);
        EXPECT_EQ(v.size(), (size_t)i + 1);
        EXPECT_GE(v.capacity(), v.size());
    }
    for (int i = 0; i < 100; i++)
        EXPECT_EQ(v[i], i);
}

TEST(Vector, PushBackOwnElement)
{
    // ссылка на свой же элемент в момент перевыделения памяти
    my::vector<std::string> v;
    v.push_back("hello");
    for (int i = 0; i < 20; i++)
        v.push_back(v[0]);
    for (size_t i = 0; i < v.size(); i++)
        EXPECT_EQ(v[i], "hello");
}

TEST(Vector, PushBackMove)
{
    my::vector<std::string> v;
    std::string s = "some long string that does not fit into sso buffer";
    v.push_back(std::move(s));
    EXPECT_EQ(v[0], "some long string that does not fit into sso buffer");
    EXPECT_TRUE(s.empty());
}

TEST(Vector, PopBack)
{
    my::vector<int> v = {1, 2, 3};
    v.pop_back();
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.back(), 2);
    v.pop_back();
    v.pop_back();
    EXPECT_TRUE(v.empty());
    v.pop_back(); // на пустом ничего не должно ломаться
    EXPECT_TRUE(v.empty());
}

TEST(Vector, Reserve)
{
    my::vector<int> v;
    v.reserve(50);
    EXPECT_GE(v.capacity(), 50);
    EXPECT_EQ(v.size(), 0);

    size_t cap = v.capacity();
    for (int i = 0; i < 50; i++)
        v.push_back(i);
    EXPECT_EQ(v.capacity(), cap); // перевыделения быть не должно

    v.reserve(10); // меньше не делаем
    EXPECT_EQ(v.capacity(), cap);
}

TEST(Vector, ReserveKeepsElements)
{
    my::vector<std::string> v = {"a", "b", "c"};
    v.reserve(100);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], "a");
    EXPECT_EQ(v[1], "b");
    EXPECT_EQ(v[2], "c");
}

TEST(Vector, ReserveTooBig)
{
    my::vector<int> v;
    EXPECT_THROW(v.reserve(v.max_size() + 1), std::length_error);
}

TEST(Vector, ShrinkToFit)
{
    my::vector<int> v;
    v.reserve(100);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 3);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[2], 3);
}

TEST(Vector, ShrinkToFitEmpty)
{
    my::vector<int> v;
    v.reserve(10);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_EQ(v.size(), 0);
    v.push_back(1); // после этого всё ещё работает
    EXPECT_EQ(v[0], 1);
}

TEST(Vector, InsertMiddle)
{
    my::vector<int> v = {1, 2, 4, 5};
    v.insert(2, 3);
    ASSERT_EQ(v.size(), 5);
    for (int i = 0; i < 5; i++)
        EXPECT_EQ(v[i], i + 1);
}

TEST(Vector, InsertBeginAndEnd)
{
    my::vector<int> v = {2, 3};
    v.insert(0, 1);
    v.insert(3, 4);
    ASSERT_EQ(v.size(), 4);
    for (int i = 0; i < 4; i++)
        EXPECT_EQ(v[i], i + 1);
}

TEST(Vector, InsertIntoEmpty)
{
    my::vector<std::string> v;
    v.insert(0, "x");
    ASSERT_EQ(v.size(), 1);
    EXPECT_EQ(v[0], "x");
}

TEST(Vector, InsertWithReallocation)
{
    my::vector<int> v;
    for (int i = 0; i < 10; i++)
        v.insert(0, i); // каждый раз в начало, несколько раз сработает рост
    ASSERT_EQ(v.size(), 10);
    for (int i = 0; i < 10; i++)
        EXPECT_EQ(v[i], 9 - i);
}

TEST(Vector, InsertOwnElement)
{
    my::vector<std::string> v = {"a", "b", "c"};
    v.insert(0, v[2]);
    ASSERT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], "c");
    EXPECT_EQ(v[1], "a");
    EXPECT_EQ(v[3], "c");
}

TEST(Vector, InsertOutOfRange)
{
    my::vector<int> v = {1, 2, 3};
    EXPECT_THROW(v.insert(4, 10), std::out_of_range);
    EXPECT_EQ(v.size(), 3);
}

TEST(Vector, InsertMove)
{
    my::vector<std::string> v = {"a", "c"};
    std::string s = "this string is long enough to be allocated on the heap";
    v.insert(1, std::move(s));
    EXPECT_EQ(v[1], "this string is long enough to be allocated on the heap");
    EXPECT_EQ(v[2], "c");
}

TEST(Vector, EmplaceBack)
{
    my::vector<std::string> v;
    v.emplace_back(3, 'z');
    v.emplace_back("abc");
    ASSERT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], "zzz");
    EXPECT_EQ(v[1], "abc");
}

TEST(Vector, Emplace)
{
    my::vector<std::string> v = {"a", "d"};
    v.emplace(1, 2, 'c'); // строка "cc"
    ASSERT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], "a");
    EXPECT_EQ(v[1], "cc");
    EXPECT_EQ(v[2], "d");

    v.emplace(3, "end");
    EXPECT_EQ(v.back(), "end");
    EXPECT_THROW(v.emplace(10, "bad"), std::out_of_range);
}

TEST(Vector, EmplaceNoCopy)
{
    Counted::copies = 0;
    {
        my::vector<Counted> v;
        v.reserve(10);
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace(0, 3);
        EXPECT_EQ(Counted::copies, 0);
        EXPECT_EQ(v[0].value, 3);
        EXPECT_EQ(v[1].value, 1);
        EXPECT_EQ(v[2].value, 2);
    }
    EXPECT_EQ(Counted::alive, 0);
}

TEST(Vector, UniquePtr)
{
    // тип который нельзя копировать
    my::vector<std::unique_ptr<int>> v;
    v.push_back(std::unique_ptr<int>(new int(1)));
    v.emplace_back(new int(3));
    v.insert(1, std::unique_ptr<int>(new int(2)));
    ASSERT_EQ(v.size(), 3);
    EXPECT_EQ(*v[0], 1);
    EXPECT_EQ(*v[1], 2);
    EXPECT_EQ(*v[2], 3);
    v.reverse();
    EXPECT_EQ(*v[0], 3);
    v.resize(1);
    EXPECT_EQ(v.size(), 1);
}

TEST(Vector, ResizeBigger)
{
    my::vector<int> v = {1, 2};
    v.resize(5);
    ASSERT_EQ(v.size(), 5);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 0);
    EXPECT_EQ(v[4], 0);
}

TEST(Vector, ResizeWithValue)
{
    my::vector<int> v = {1, 2};
    v.resize(4, 9);
    ASSERT_EQ(v.size(), 4);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 9);
    EXPECT_EQ(v[3], 9);
}

TEST(Vector, ResizeWithOwnElement)
{
    my::vector<std::string> v = {"abc"};
    v.resize(20, v[0]); // при росте старый буфер умирает
    ASSERT_EQ(v.size(), 20);
    for (size_t i = 0; i < 20; i++)
        EXPECT_EQ(v[i], "abc");
}

TEST(Vector, ResizeSmaller)
{
    my::vector<int> v = {1, 2, 3, 4, 5};
    size_t cap = v.capacity();
    v.resize(2);
    ASSERT_EQ(v.size(), 2);
    EXPECT_EQ(v.back(), 2);
    EXPECT_EQ(v.capacity(), cap); // память не отдаём
}

TEST(Vector, ResizeSameAndZero)
{
    my::vector<int> v = {1, 2, 3};
    v.resize(3);
    EXPECT_EQ(v.size(), 3);
    v.resize(0);
    EXPECT_TRUE(v.empty());
}

TEST(Vector, Reverse)
{
    my::vector<int> v = {1, 2, 3, 4, 5};
    v.reverse();
    for (int i = 0; i < 5; i++)
        EXPECT_EQ(v[i], 5 - i);
}

TEST(Vector, ReverseEven)
{
    my::vector<std::string> v = {"a", "b", "c", "d"};
    v.reverse();
    EXPECT_EQ(v[0], "d");
    EXPECT_EQ(v[1], "c");
    EXPECT_EQ(v[2], "b");
    EXPECT_EQ(v[3], "a");
}

TEST(Vector, ReverseTrivial)
{
    my::vector<int> empty;
    empty.reverse();
    EXPECT_TRUE(empty.empty());

    my::vector<int> one = {5};
    one.reverse();
    EXPECT_EQ(one[0], 5);
}

TEST(Vector, CopyConstructor)
{
    my::vector<std::string> a = {"x", "y", "z"};
    my::vector<std::string> b(a);
    ASSERT_EQ(b.size(), 3);
    EXPECT_EQ(b[1], "y");
    EXPECT_NE(a.data(), b.data());

    b[0] = "changed";
    EXPECT_EQ(a[0], "x"); // глубокое копирование
}

TEST(Vector, CopyAssignment)
{
    my::vector<int> a = {1, 2, 3};
    my::vector<int> b = {9, 9, 9, 9, 9};
    b = a;
    ASSERT_EQ(b.size(), 3);
    EXPECT_EQ(b[2], 3);
    a[0] = 100;
    EXPECT_EQ(b[0], 1);
}

TEST(Vector, SelfAssignment)
{
    my::vector<int> a = {1, 2, 3};
    my::vector<int> &ref = a;
    a = ref;
    ASSERT_EQ(a.size(), 3);
    EXPECT_EQ(a[1], 2);
}

TEST(Vector, CopyEmpty)
{
    my::vector<int> a;
    my::vector<int> b(a);
    EXPECT_TRUE(b.empty());
    b = a;
    EXPECT_TRUE(b.empty());
}

TEST(Vector, MoveConstructor)
{
    my::vector<std::string> a = {"x", "y"};
    std::string *old = a.data();
    my::vector<std::string> b(std::move(a));
    EXPECT_EQ(b.data(), old); // буфер переехал, а не скопировался
    ASSERT_EQ(b.size(), 2);
    EXPECT_EQ(b[1], "y");
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.capacity(), 0);

    a.push_back("again"); // после перемещения пользоваться можно
    EXPECT_EQ(a[0], "again");
}

TEST(Vector, MoveAssignment)
{
    my::vector<std::string> a = {"x", "y", "z"};
    my::vector<std::string> b = {"old"};
    std::string *old = a.data();
    b = std::move(a);
    EXPECT_EQ(b.data(), old);
    ASSERT_EQ(b.size(), 3);
    EXPECT_EQ(b[2], "z");
    EXPECT_TRUE(a.empty());
}

TEST(Vector, ReturnFromFunction)
{
    auto make = []() {
        my::vector<int> v;
        for (int i = 0; i < 10; i++)
            v.push_back(i);
        return v;
    };
    my::vector<int> r = make();
    ASSERT_EQ(r.size(), 10);
    EXPECT_EQ(r[9], 9);
}

TEST(Vector, NoLeaks)
{
    Counted::alive = 0;
    {
        my::vector<Counted> v;
        for (int i = 0; i < 20; i++)
            v.push_back(Counted(i));
        v.insert(5, Counted(100));
        v.pop_back();
        v.resize(30);
        v.resize(10);
        v.shrink_to_fit();
        my::vector<Counted> w(v);
        my::vector<Counted> z(std::move(w));
        v = z;
        EXPECT_EQ(Counted::alive, 20); // v и z по 10 штук
    }
    EXPECT_EQ(Counted::alive, 0);
}

TEST(Vector, ReallocMovesNotCopies)
{
    // у Counted перемещение noexcept, значит при росте копий быть не должно
    my::vector<Counted> v;
    v.reserve(4);
    for (int i = 0; i < 4; i++)
        v.push_back(Counted(i));
    Counted::copies = 0;
    v.push_back(Counted(4)); // тут рост
    EXPECT_EQ(Counted::copies, 0);
    EXPECT_EQ(v[3].value, 3);
    EXPECT_EQ(v[4].value, 4);
}

struct Bomb
{
    static int left;
    Counted c;
    Bomb() : c(1) {}
    Bomb(const Bomb &o) : c(o.c)
    {
        if (left == 0)
            throw std::runtime_error("boom");
        left--;
    }
};
int Bomb::left = 0;

TEST(Vector, ConstructorException)
{
    // если копирование кинуло, уже созданные элементы должны уничтожиться
    Counted::alive = 0;
    {
        Bomb proto;
        Bomb::left = 3;
        EXPECT_THROW(my::vector<Bomb> v(10, proto), std::runtime_error);
        EXPECT_EQ(Counted::alive, 1); // остался только proto
    }
    EXPECT_EQ(Counted::alive, 0);
}

TEST(Vector, Iterators)
{
    my::vector<int> v = {1, 2, 3, 4};
    int sum = 0;
    for (int x : v)
        sum += x;
    EXPECT_EQ(sum, 10);
    EXPECT_EQ(v.end() - v.begin(), 4);
}

TEST(Vector, Swap)
{
    my::vector<int> a = {1, 2};
    my::vector<int> b = {3, 4, 5};
    a.swap(b);
    EXPECT_EQ(a.size(), 3);
    EXPECT_EQ(b.size(), 2);
    EXPECT_EQ(a[0], 3);
    EXPECT_EQ(b[1], 2);
}

TEST(Vector, Clear)
{
    my::vector<int> v = {1, 2, 3};
    size_t cap = v.capacity();
    v.clear();
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.capacity(), cap);
}

TEST(Vector, VectorOfVectors)
{
    my::vector<my::vector<int>> v;
    for (int i = 0; i < 5; i++)
    {
        my::vector<int> inner;
        for (int j = 0; j <= i; j++)
            inner.push_back(j);
        v.push_back(inner);
    }
    ASSERT_EQ(v.size(), 5);
    EXPECT_EQ(v[4].size(), 5);
    EXPECT_EQ(v[2][2], 2);
    v.insert(0, my::vector<int>(3, 7));
    EXPECT_EQ(v[0][1], 7);
    EXPECT_EQ(v[1].size(), 1);
}

TEST(Vector, LargeAmount)
{
    my::vector<int> v;
    for (int i = 0; i < 100000; i++)
        v.push_back(i);
    ASSERT_EQ(v.size(), 100000);
    EXPECT_EQ(v[0], 0);
    EXPECT_EQ(v[99999], 99999);
}

// ---------- vector<bool> ----------

TEST(VectorBool, DefaultConstructor)
{
    my::vector<bool> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
}

TEST(VectorBool, SizeConstructor)
{
    my::vector<bool> v(10);
    EXPECT_EQ(v.size(), 10);
    for (size_t i = 0; i < 10; i++)
        EXPECT_FALSE(v[i]);

    my::vector<bool> t(70, true);
    EXPECT_EQ(t.size(), 70);
    for (size_t i = 0; i < 70; i++)
        EXPECT_TRUE(t[i]);
}

TEST(VectorBool, InitializerList)
{
    my::vector<bool> v = {true, false, true, true};
    ASSERT_EQ(v.size(), 4);
    EXPECT_TRUE(v[0]);
    EXPECT_FALSE(v[1]);
    EXPECT_TRUE(v[2]);
    EXPECT_TRUE(v[3]);
}

TEST(VectorBool, IsBitPacked)
{
    // 1000 бит должны влезать в 16 блоков по 64, а не в 1000 байт
    my::vector<bool> v(1000);
    EXPECT_GE(v.capacity(), 1000);
    EXPECT_LT(v.capacity(), 1000 + 64);
}

TEST(VectorBool, SetAndGet)
{
    my::vector<bool> v(130);
    v[0] = true;
    v[63] = true;
    v[64] = true;
    v[129] = true;
    for (size_t i = 0; i < 130; i++)
    {
        bool expected = (i == 0 || i == 63 || i == 64 || i == 129);
        EXPECT_EQ((bool)v[i], expected) << "i = " << i;
    }
    v[63] = false;
    EXPECT_FALSE(v[63]);
    EXPECT_TRUE(v[64]); // соседний блок не задет
}

TEST(VectorBool, ProxyAssignFromProxy)
{
    my::vector<bool> v = {true, false};
    v[1] = v[0];
    EXPECT_TRUE(v[1]);
    v[0] = false;
    EXPECT_TRUE(v[1]); // скопировалось значение, а не ссылка
}

TEST(VectorBool, Flip)
{
    my::vector<bool> v = {true, false};
    v[0].flip();
    v[1].flip();
    EXPECT_FALSE(v[0]);
    EXPECT_TRUE(v[1]);
}

TEST(VectorBool, ConstSubscript)
{
    my::vector<bool> v = {false, true};
    const my::vector<bool> &c = v;
    EXPECT_FALSE(c[0]);
    EXPECT_TRUE(c[1]);
    EXPECT_FALSE(c.front());
    EXPECT_TRUE(c.back());
}

TEST(VectorBool, FrontBack)
{
    my::vector<bool> v = {false, true, false};
    EXPECT_FALSE(v.front());
    EXPECT_FALSE(v.back());
    v.front() = true;
    v.back() = true;
    EXPECT_TRUE(v[0]);
    EXPECT_TRUE(v[2]);
}

TEST(VectorBool, Data)
{
    my::vector<bool> v(3);
    v[0] = true;
    v[2] = true;
    ASSERT_NE(v.data(), nullptr);
    EXPECT_EQ(v.data()[0], 5u); // 101 в двоичной
}

TEST(VectorBool, PushBackPopBack)
{
    my::vector<bool> v;
    for (int i = 0; i < 200; i++)
        v.push_back(i % 3 == 0);
    ASSERT_EQ(v.size(), 200);
    for (int i = 0; i < 200; i++)
        EXPECT_EQ((bool)v[i], i % 3 == 0) << "i = " << i;

    v.pop_back();
    EXPECT_EQ(v.size(), 199);
    while (!v.empty())
        v.pop_back();
    v.pop_back(); // пустой
    EXPECT_EQ(v.size(), 0);
}

TEST(VectorBool, PushBackAfterPop)
{
    // после pop_back в старом бите может остаться единица, push_back должен её перезаписать
    my::vector<bool> v;
    v.push_back(true);
    v.pop_back();
    v.push_back(false);
    EXPECT_FALSE(v[0]);
}

TEST(VectorBool, MaxSize)
{
    my::vector<bool> v;
    EXPECT_GT(v.max_size(), 1000000);
}

TEST(VectorBool, ReserveAndShrink)
{
    my::vector<bool> v;
    v.reserve(500);
    EXPECT_GE(v.capacity(), 500);
    EXPECT_EQ(v.size(), 0);

    v.push_back(true);
    v.push_back(false);
    v.push_back(true);
    v.shrink_to_fit();
    EXPECT_GE(v.capacity(), 3);
    EXPECT_LE(v.capacity(), 64);
    ASSERT_EQ(v.size(), 3);
    EXPECT_TRUE(v[0]);
    EXPECT_FALSE(v[1]);
    EXPECT_TRUE(v[2]);

    EXPECT_THROW(v.reserve(v.max_size() + 1), std::length_error);
}

TEST(VectorBool, ShrinkEmpty)
{
    my::vector<bool> v;
    v.reserve(100);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 0);
}

TEST(VectorBool, Insert)
{
    my::vector<bool> v = {true, true, true};
    v.insert(1, false);
    ASSERT_EQ(v.size(), 4);
    EXPECT_TRUE(v[0]);
    EXPECT_FALSE(v[1]);
    EXPECT_TRUE(v[2]);
    EXPECT_TRUE(v[3]);

    v.insert(0, false);
    v.insert(5, false);
    ASSERT_EQ(v.size(), 6);
    EXPECT_FALSE(v[0]);
    EXPECT_TRUE(v[1]);
    EXPECT_FALSE(v[5]);
    EXPECT_THROW(v.insert(10, true), std::out_of_range);
}

TEST(VectorBool, InsertAcrossBlocks)
{
    my::vector<bool> v(64, true); // ровно один полный блок
    v.insert(10, false);
    ASSERT_EQ(v.size(), 65);
    for (size_t i = 0; i < 65; i++)
        EXPECT_EQ((bool)v[i], i != 10) << "i = " << i;
}

TEST(VectorBool, Emplace)
{
    my::vector<bool> v;
    v.emplace(0, true);
    v.emplace_back(false);
    v.emplace(1, true);
    ASSERT_EQ(v.size(), 3);
    EXPECT_TRUE(v[0]);
    EXPECT_TRUE(v[1]);
    EXPECT_FALSE(v[2]);
    EXPECT_THROW(v.emplace(4, true), std::out_of_range);
}

TEST(VectorBool, Resize)
{
    my::vector<bool> v = {true, false};
    v.resize(5, true);
    ASSERT_EQ(v.size(), 5);
    EXPECT_TRUE(v[0]);
    EXPECT_FALSE(v[1]);
    EXPECT_TRUE(v[2]);
    EXPECT_TRUE(v[4]);

    v.resize(1);
    EXPECT_EQ(v.size(), 1);
    v.resize(3); // новые должны быть false, хоть там раньше и были единицы
    EXPECT_TRUE(v[0]);
    EXPECT_FALSE(v[1]);
    EXPECT_FALSE(v[2]);

    v.resize(0);
    EXPECT_TRUE(v.empty());
}

TEST(VectorBool, Reverse)
{
    my::vector<bool> v = {true, true, false, false, false};
    v.reverse();
    EXPECT_FALSE(v[0]);
    EXPECT_FALSE(v[1]);
    EXPECT_FALSE(v[2]);
    EXPECT_TRUE(v[3]);
    EXPECT_TRUE(v[4]);
}

TEST(VectorBool, ReverseLong)
{
    my::vector<bool> v(100);
    for (size_t i = 0; i < 100; i++)
        v[i] = (i < 30);
    v.reverse();
    for (size_t i = 0; i < 100; i++)
        EXPECT_EQ((bool)v[i], i >= 70) << "i = " << i;

    my::vector<bool> e;
    e.reverse();
    EXPECT_TRUE(e.empty());
}

TEST(VectorBool, Copy)
{
    my::vector<bool> a = {true, false, true};
    my::vector<bool> b(a);
    ASSERT_EQ(b.size(), 3);
    EXPECT_TRUE(b[2]);
    b[0] = false;
    EXPECT_TRUE(a[0]);

    my::vector<bool> c(200, true);
    c = a;
    ASSERT_EQ(c.size(), 3);
    EXPECT_FALSE(c[1]);

    my::vector<bool> &ref = c;
    c = ref; // само в себя
    EXPECT_EQ(c.size(), 3);
}

TEST(VectorBool, Move)
{
    my::vector<bool> a(100, true);
    my::vector<bool> b(std::move(a));
    EXPECT_EQ(b.size(), 100);
    EXPECT_TRUE(b[99]);
    EXPECT_TRUE(a.empty());

    my::vector<bool> c = {false};
    c = std::move(b);
    EXPECT_EQ(c.size(), 100);
    EXPECT_TRUE(c[50]);
    EXPECT_TRUE(b.empty());

    a.push_back(true); // использовать после move можно
    EXPECT_TRUE(a[0]);
}

TEST(VectorBool, Clear)
{
    my::vector<bool> v(10, true);
    v.clear();
    EXPECT_TRUE(v.empty());
    v.push_back(false);
    EXPECT_FALSE(v[0]);
}

TEST(VectorBool, Swap)
{
    my::vector<bool> a = {true};
    my::vector<bool> b = {false, false};
    a.swap(b);
    EXPECT_EQ(a.size(), 2);
    EXPECT_EQ(b.size(), 1);
    EXPECT_TRUE(b[0]);
}

TEST(VectorBool, InsideOtherVector)
{
    my::vector<my::vector<bool>> v;
    v.push_back(my::vector<bool>(3, true));
    v.push_back(my::vector<bool>(2, false));
    EXPECT_TRUE(v[0][2]);
    EXPECT_FALSE(v[1][1]);
}
