// тесты на аллокатор из дз №2

#include <gtest/gtest.h>
#include <cstring>
#include <cstdint>

#include "allocator.hpp"

// простые базовые проверки

TEST(Allocator, InitAndAllocSimple)
{
    Allocator* a = init_allocator(100);
    ASSERT_NE(a, nullptr);

    char* p = alloc(a, 10);
    EXPECT_NE(p, nullptr);

    clear(a);
}

TEST(Allocator, FirstAllocAtBeginning)
{
    Allocator* a = init_allocator(64);
    char* p = alloc(a, 8);
    ASSERT_NE(p, nullptr);
    // первый блок должен начинаться прямо от начала памяти
    EXPECT_EQ(p, a->memory);
    clear(a);
}

TEST(Allocator, AllocsAreSequential)
{
    Allocator* a = init_allocator(100);
    char* p1 = alloc(a, 10);
    char* p2 = alloc(a, 20);
    char* p3 = alloc(a, 5);
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    ASSERT_NE(p3, nullptr);

    // стратегия линейная, блоки должны идти подряд
    EXPECT_EQ(p2 - p1, 10);
    EXPECT_EQ(p3 - p2, 20);
    EXPECT_EQ(p3 + 5, a->memory + 35);
    clear(a);
}

TEST(Allocator, TooBigAllocReturnsNullptr)
{
    Allocator* a = init_allocator(10);
    EXPECT_EQ(alloc(a, 11), nullptr); // на 1 больше размера
    // после неудачного alloc место никуда не делось, offset не сдвинулся
    char* p = alloc(a, 10);
    EXPECT_NE(p, nullptr);
    clear(a);
}

TEST(Allocator, FillExactlyWholeSize)
{
    Allocator* a = init_allocator(10);
    char* p = alloc(a, 10); // ровно весь размер
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(alloc(a, 1), nullptr); // больше ничего не влезает
    EXPECT_EQ(a->offset, 10u); // всё занято
    clear(a);
}

// граничные значения вокруг размера, с обеих сторон, и на двух разных размерах

TEST(Allocator, BoundaryAroundSize)
{
    Allocator* a = init_allocator(16);
    EXPECT_EQ(alloc(a, 17), nullptr); // чуть больше размера
    char* p = alloc(a, 15);            // чуть меньше
    ASSERT_NE(p, nullptr);
    char* p2 = alloc(a, 1); // добиваем ровно до конца
    EXPECT_NE(p2, nullptr);
    EXPECT_EQ(alloc(a, 1), nullptr);
    clear(a);
}

TEST(Allocator, BoundaryAroundBigSize)
{
    // то же самое но на большом размере
    Allocator* a = init_allocator(1000);
    EXPECT_EQ(alloc(a, 1001), nullptr);
    char* p = alloc(a, 999);
    ASSERT_NE(p, nullptr);
    EXPECT_NE(alloc(a, 1), nullptr);
    EXPECT_EQ(alloc(a, 1), nullptr);
    clear(a);
}

TEST(Allocator, TooBigAfterPartialFill)
{
    Allocator* a = init_allocator(30);
    ASSERT_NE(alloc(a, 10), nullptr);
    ASSERT_NE(alloc(a, 10), nullptr);
    // свободно осталось 10, значит 11 не влезет
    EXPECT_EQ(alloc(a, 11), nullptr);
    EXPECT_NE(alloc(a, 10), nullptr); // а 10 как раз влезет
    EXPECT_EQ(alloc(a, 1), nullptr);
    clear(a);
}

TEST(Allocator, zero_size_alloc)
{
    Allocator* a = init_allocator(10);
    // на блок нулевого размера места хватает всегда, и offset он не двигает
    char* p = alloc(a, 0);
    EXPECT_NE(p, nullptr);
    char* p2 = alloc(a, 10);
    EXPECT_NE(p2, nullptr);
    EXPECT_EQ(p2, p);
    clear(a);
}

TEST(Allocator, InitWithZeroSize)
{
    Allocator* a = init_allocator(0);
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(alloc(a, 1), nullptr);
    EXPECT_NE(alloc(a, 0), nullptr); // 0 байт влезает и в нулевой
    reset(a);
    EXPECT_EQ(alloc(a, 1), nullptr);
    clear(a);
}

TEST(Allocator, OneByteAllocator)
{
    Allocator* a = init_allocator(1);
    char* p = alloc(a, 1);
    ASSERT_NE(p, nullptr);
    p[0] = 'q';
    EXPECT_EQ(p[0], 'q');
    EXPECT_EQ(alloc(a, 1), nullptr);
    reset(a);
    EXPECT_NE(alloc(a, 1), nullptr);
    clear(a);
}

TEST(Allocator, HugeSizeDoesNotOverflow)
{
    // если внутри проверять через offset + size, то size_t переполнится,
    // проверка пройдёт и всё поедет. проверяю что такого не происходит
    Allocator* a = init_allocator(100);
    EXPECT_EQ(alloc(a, SIZE_MAX), nullptr);
    // offset не уехал, весь размер всё ещё доступен
    char* p = alloc(a, 100);
    EXPECT_NE(p, nullptr);
    clear(a);
}

// работа с данными

TEST(Allocator, WriteAndReadData)
{
    Allocator* a = init_allocator(64);
    char* p = alloc(a, 16);
    ASSERT_NE(p, nullptr);
    memcpy(p, "hello allocator", 16);
    EXPECT_EQ(0, memcmp(p, "hello allocator", 16));

    char* p2 = alloc(a, 8);
    ASSERT_NE(p2, nullptr);
    memset(p2, 'x', 8);
    // запись во второй блок не должна ломать первый
    EXPECT_EQ(0, memcmp(p, "hello allocator", 16));
    clear(a);
}

TEST(Allocator, BlocksDoNotOverlap)
{
    Allocator* a = init_allocator(20);
    char* p1 = alloc(a, 10);
    char* p2 = alloc(a, 10);
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    memset(p1, 'a', 10);
    memset(p2, 'b', 10);
    EXPECT_EQ(p1[0], 'a');
    EXPECT_EQ(p1[9], 'a');
    EXPECT_EQ(p2[0], 'b');
    EXPECT_EQ(p2[9], 'b');
    clear(a);
}


// reset

TEST(ResetTest, AllowsToReuseMemory)
{
    Allocator* a = init_allocator(100);
    char* p1 = alloc(a, 50);
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(alloc(a, 50), nullptr);
    EXPECT_EQ(alloc(a, 1), nullptr); // всё заняли

    reset(a);
    EXPECT_EQ(a->offset, 0u); // откатились в начало

    // снова можно занять весь размер
    char* p2 = alloc(a, 100);
    EXPECT_NE(p2, nullptr);
    EXPECT_EQ(p2, p1); // и снова с самого начала
    clear(a);
}

TEST(ResetTest, DoesNotFreeMemory)
{
    Allocator* a = init_allocator(32);
    char* p = alloc(a, 32);
    ASSERT_NE(p, nullptr);
    reset(a);
    // reset не должен вызывать delete: память всё ещё наша и в неё можно писать
    memcpy(p, "still alive", 12);
    EXPECT_EQ(0, memcmp(p, "still alive", 12));
    clear(a);
}

TEST(ResetTest, DoubleResetIsOk)
{
    Allocator* a = init_allocator(50);
    ASSERT_NE(alloc(a, 50), nullptr);
    reset(a);
    reset(a); // второй reset подряд ничего не должен ломать
    EXPECT_NE(alloc(a, 50), nullptr);
    clear(a);
}

TEST(ResetTest, ResetOnEmptyAllocator)
{
    Allocator* a = init_allocator(10);
    reset(a); // ещё ничего не аллоцировали
    EXPECT_EQ(a->offset, 0u);
    EXPECT_NE(alloc(a, 10), nullptr);
    clear(a);
}

// повторный init и clear

TEST(Allocator, SecondInitReplacesMemory)
{
    Allocator* a = init_allocator(100);
    ASSERT_NE(alloc(a, 100), nullptr);

    // повторный init: старая память освобождается, выделяется новая
    a = init_allocator(10);
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(alloc(a, 11), nullptr); // размер уже новый
    EXPECT_NE(alloc(a, 10), nullptr);
    clear(a);
}

TEST(Allocator, InitAfterClearWorks)
{
    Allocator* a = init_allocator(100);
    clear(a);

    // после clear всё работает как в первый раз
    a = init_allocator(20);
    ASSERT_NE(a, nullptr);
    EXPECT_NE(alloc(a, 20), nullptr);
    EXPECT_EQ(alloc(a, 1), nullptr);
    clear(a);
}

TEST(Allocator, ManySmallAllocsInLoop)
{
    Allocator* a = init_allocator(1000);
    for (int i = 0; i < 100; i++)
    {
        char* p = alloc(a, 10);
        ASSERT_NE(p, nullptr) << "упал на итерации " << i;
        p[0] = 'x'; // в блок можно писать
    }
    EXPECT_EQ(alloc(a, 1), nullptr); // 100 * 10 = ровно весь размер

    reset(a);
    for (int i = 0; i < 100; i++)
    {
        EXPECT_NE(alloc(a, 10), nullptr);
    }
    clear(a);
}

