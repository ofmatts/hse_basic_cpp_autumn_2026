#include "allocator.hpp"

// единственный неприятный момент в задании: при повторном вызове init_allocator
// старая память должна освобождаться. но в функцию передаётся только размер,
// откуда знать про прошлый аллокатор? поэтому запоминаю его тут
static Allocator* last_allocator = nullptr;

Allocator* init_allocator(size_t maxSize)
{
    // повторный вызов - сначала убираем за прошлым
    if (last_allocator != nullptr)
    {
        delete[] last_allocator->memory;
        delete last_allocator;
        last_allocator = nullptr;
    }

    Allocator* result = new Allocator;
    result->memory = new char[maxSize];
    result->size = maxSize;
    result->offset = 0;

    last_allocator = result;
    return result;
}

char* alloc(Allocator *alloc, size_t size)
{
    if (alloc == nullptr)
        return nullptr;

    // проверку делаю через вычитание, а не через offset + size: size_t
    // беззнаковый, при огромном size сумма переполнится, обнулится и
    // места внезапно "хватит"
    if (size > alloc->size - alloc->offset)
        return nullptr;

    // printf("alloc: offset=%zu\n", alloc->offset);

    char* result = alloc->memory + alloc->offset;
    alloc->offset += size;
    return result;
}

void reset(Allocator *alloc)
{
    if (alloc == nullptr){ // на всякий случай, вдруг кто-нибудь вызовет с nullptr
        return;
    }

    // просто откатываем offset. память НЕ удаляем, это делает только clear!
    alloc->offset = 0;
}

void clear(Allocator *alloc)
{
    if (alloc == nullptr)
        return;

    delete[] alloc->memory;
    alloc->memory = nullptr;
    alloc->size = 0;
    alloc->offset = 0;

    // если чистим именно последний аллокатор, надо про него забыть,
    // иначе повторный init_allocator удалит его второй раз
    if (alloc == last_allocator)
        last_allocator = nullptr;

    delete alloc;
}

// TODO: можно ещё выравнивание по 8/16 байтам сделать, в задании не требуется,
// но для реального использования было бы полезно
