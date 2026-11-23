#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstddef>
#include <initializer_list>
#include <new>
#include <stdexcept>
#include <utility>

namespace my
{

template <class T>
class vector
{
    T *data_;
    size_t size_;
    size_t capacity_;

    // выделяет сырую память под n штук, конструкторы не вызывает
    static T *allocate(size_t n)
    {
        if (n == 0)
            return nullptr;
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    static void deallocate(T *p)
    {
        ::operator delete(p);
    }

    // переезд в новый буфер на new_cap элементов (new_cap >= size_)
    void relocate(size_t new_cap)
    {
        T *fresh = allocate(new_cap);
        for (size_t i = 0; i < size_; i++)
        {
            new (fresh + i) T(std::move_if_noexcept(data_[i]));
            data_[i].~T();
        }
        deallocate(data_);
        data_ = fresh;
        capacity_ = new_cap;
    }

    size_t next_capacity() const
    {
        return capacity_ == 0 ? 1 : capacity_ * 2;
    }

public:
    typedef T value_type;
    typedef T *iterator;
    typedef const T *const_iterator;

    vector() : data_(nullptr), size_(0), capacity_(0)
    {
    }

    explicit vector(size_t n) : data_(nullptr), size_(0), capacity_(0)
    {
        reserve(n);
        try
        {
            for (size_t i = 0; i < n; i++)
            {
                new (data_ + i) T();
                size_++; // увеличиваю по одному, чтобы знать сколько успело создаться
            }
        }
        catch (...)
        {
            // деструктор при исключении в конструкторе не вызовется, чистим сами
            clear();
            deallocate(data_);
            throw;
        }
    }

    vector(size_t n, const T &value) : data_(nullptr), size_(0), capacity_(0)
    {
        reserve(n);
        try
        {
            for (size_t i = 0; i < n; i++)
            {
                new (data_ + i) T(value);
                size_++;
            }
        }
        catch (...)
        {
            clear();
            deallocate(data_);
            throw;
        }
    }

    vector(std::initializer_list<T> list) : data_(nullptr), size_(0), capacity_(0)
    {
        reserve(list.size());
        try
        {
            for (const T &x : list)
            {
                new (data_ + size_) T(x);
                size_++;
            }
        }
        catch (...)
        {
            clear();
            deallocate(data_);
            throw;
        }
    }

    vector(const vector &other) : data_(nullptr), size_(0), capacity_(0)
    {
        reserve(other.size_);
        try
        {
            for (size_t i = 0; i < other.size_; i++)
            {
                new (data_ + i) T(other.data_[i]);
                size_++;
            }
        }
        catch (...)
        {
            clear();
            deallocate(data_);
            throw;
        }
    }

    vector(vector &&other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    vector &operator=(const vector &other)
    {
        if (this != &other)
        {
            vector tmp(other); // copy and swap
            swap(tmp);
        }
        return *this;
    }

    vector &operator=(vector &&other) noexcept
    {
        if (this != &other)
        {
            clear();
            deallocate(data_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    ~vector()
    {
        clear();
        deallocate(data_);
    }

    void swap(vector &other) noexcept
    {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    T &operator[](size_t i)
    {
        return data_[i];
    }

    const T &operator[](size_t i) const
    {
        return data_[i];
    }

    T &front()
    {
        return data_[0];
    }

    const T &front() const
    {
        return data_[0];
    }

    T &back()
    {
        return data_[size_ - 1];
    }

    const T &back() const
    {
        return data_[size_ - 1];
    }

    T *data()
    {
        return data_;
    }

    const T *data() const
    {
        return data_;
    }

    iterator begin() { return data_; }
    iterator end() { return data_ + size_; }
    const_iterator begin() const { return data_; }
    const_iterator end() const { return data_ + size_; }

    bool empty() const
    {
        return size_ == 0;
    }

    size_t size() const
    {
        return size_;
    }

    size_t max_size() const
    {
        return static_cast<size_t>(-1) / sizeof(T);
    }

    size_t capacity() const
    {
        return capacity_;
    }

    void reserve(size_t n)
    {
        if (n > max_size())
            throw std::length_error("vector::reserve");
        if (n > capacity_)
            relocate(n);
    }

    void shrink_to_fit()
    {
        if (capacity_ > size_)
            relocate(size_);
    }

    void clear()
    {
        for (size_t i = 0; i < size_; i++)
            data_[i].~T();
        size_ = 0;
    }

    // создаёт элемент прямо в конце
    template <class... Args>
    void emplace_back(Args &&...args)
    {
        if (size_ == capacity_)
        {
            // новый элемент строю в новом буфере до переезда, иначе push_back(v[0])
            // сломается, аргумент может ссылаться на элемент старого буфера
            size_t new_cap = next_capacity();
            T *fresh = allocate(new_cap);
            new (fresh + size_) T(std::forward<Args>(args)...);
            for (size_t i = 0; i < size_; i++)
            {
                new (fresh + i) T(std::move_if_noexcept(data_[i]));
                data_[i].~T();
            }
            deallocate(data_);
            data_ = fresh;
            capacity_ = new_cap;
        }
        else
        {
            new (data_ + size_) T(std::forward<Args>(args)...);
        }
        size_++;
    }

    void push_back(const T &value)
    {
        emplace_back(value);
    }

    void push_back(T &&value)
    {
        emplace_back(std::move(value));
    }

    void pop_back()
    {
        if (size_ == 0)
            return;
        size_--;
        data_[size_].~T();
    }

    // pos - индекс, а не итератор. вставляет перед элементом pos, pos == size() это просто в конец
    template <class... Args>
    void emplace(size_t pos, Args &&...args)
    {
        if (pos > size_)
            throw std::out_of_range("vector::emplace");

        if (pos == size_)
        {
            emplace_back(std::forward<Args>(args)...);
            return;
        }

        // сначала делаю временный объект, т.к. args может быть ссылкой внутрь самого вектора
        T tmp(std::forward<Args>(args)...);

        if (size_ == capacity_)
            relocate(next_capacity());

        // последний элемент уезжает в сырую память, остальные сдвигаю присваиванием
        new (data_ + size_) T(std::move(data_[size_ - 1]));
        for (size_t i = size_ - 1; i > pos; i--)
            data_[i] = std::move(data_[i - 1]);
        data_[pos] = std::move(tmp);
        size_++;
    }

    void insert(size_t pos, const T &value)
    {
        emplace(pos, value);
    }

    void insert(size_t pos, T &&value)
    {
        emplace(pos, std::move(value));
    }

    void resize(size_t n)
    {
        if (n < size_)
        {
            while (size_ > n)
                pop_back();
        }
        else if (n > size_)
        {
            reserve(n);
            while (size_ < n)
            {
                new (data_ + size_) T();
                size_++;
            }
        }
    }

    void resize(size_t n, const T &value)
    {
        if (n < size_)
        {
            while (size_ > n)
                pop_back();
        }
        else if (n > size_)
        {
            T copy(value); // value может лежать внутри самого вектора
            reserve(n);
            while (size_ < n)
            {
                new (data_ + size_) T(copy);
                size_++;
            }
        }
    }

    // разворачивает элементы в обратном порядке на месте
    void reverse()
    {
        if (size_ < 2)
            return;
        size_t i = 0;
        size_t j = size_ - 1;
        while (i < j)
        {
            std::swap(data_[i], data_[j]);
            i++;
            j--;
        }
    }
};

} // namespace my

#include "vector_bool.hpp"

#endif
