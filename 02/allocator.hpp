#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <cstddef> // size_t

// аллокатор со стратегией линейного выделения памяти - в init_allocator один раз берём большой кусок через new, а дальше при каждом alloc просто двигаем смещение вперёд
struct Allocator
{
    char* memory;  // начало выделенной памяти
    size_t size;   // её размер
    size_t offset; // сдвиг от начала, отсюда начинается свободное место
};

Allocator* init_allocator(size_t maxSize);
char* alloc(Allocator *alloc, size_t size);
void reset(Allocator *alloc);
void clear(Allocator *alloc);

#endif // ALLOCATOR_HPP

