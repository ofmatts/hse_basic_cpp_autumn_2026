#ifndef VECTOR_BOOL_HPP
#define VECTOR_BOOL_HPP

// этот файл подключается из vector.hpp, отдельно его включать не надо

#include <cstdint>

namespace my
{

// специализация для bool: каждый элемент это один бит
template <>
class vector<bool>
{
    typedef uint64_t block_t;
    static const size_t BITS = 64;

    block_t *blocks_;
    size_t size_;     // в битах
    size_t capacity_; // тоже в битах, всегда кратно 64

    static size_t blocks_for(size_t bits)
    {
        return (bits + BITS - 1) / BITS;
    }

    // перевыделяет память под new_cap бит
    void relocate(size_t new_cap)
    {
        size_t nb = blocks_for(new_cap);
        block_t *fresh = nullptr;
        if (nb > 0)
        {
            fresh = new block_t[nb];
            for (size_t i = 0; i < nb; i++)
                fresh[i] = 0;
        }
        size_t used = blocks_for(size_);
        for (size_t i = 0; i < used && i < nb; i++)
            fresh[i] = blocks_[i];
        delete[] blocks_;
        blocks_ = fresh;
        capacity_ = nb * BITS;
    }

    bool get(size_t i) const
    {
        return (blocks_[i / BITS] >> (i % BITS)) & 1;
    }

    void set(size_t i, bool v)
    {
        block_t mask = (block_t)1 << (i % BITS);
        if (v)
            blocks_[i / BITS] |= mask;
        else
            blocks_[i / BITS] &= ~mask;
    }

public:
    // прокси, чтобы v[i] = true работало
    class reference
    {
        vector<bool> *v_;
        size_t pos_;

        bool get_value() const
        {
            return v_->get(pos_);
        }

    public:
        reference(vector<bool> *v, size_t pos) : v_(v), pos_(pos)
        {
        }

        reference(const reference &other) : v_(other.v_), pos_(other.pos_)
        {
        }

        reference &operator=(bool x)
        {
            v_->set(pos_, x);
            return *this;
        }

        // v[0] = v[1] должно копировать значение, а не прокси
        reference &operator=(const reference &other)
        {
            v_->set(pos_, other.get_value());
            return *this;
        }

        operator bool() const
        {
            return get_value();
        }

        void flip()
        {
            v_->set(pos_, !get_value());
        }
    };

    typedef bool value_type;

    vector() : blocks_(nullptr), size_(0), capacity_(0)
    {
    }

    explicit vector(size_t n, bool value = false) : blocks_(nullptr), size_(0), capacity_(0)
    {
        resize(n, value);
    }

    vector(std::initializer_list<bool> list) : blocks_(nullptr), size_(0), capacity_(0)
    {
        reserve(list.size());
        for (bool b : list)
            push_back(b);
    }

    vector(const vector &other) : blocks_(nullptr), size_(0), capacity_(0)
    {
        size_t nb = blocks_for(other.size_);
        if (nb > 0)
        {
            blocks_ = new block_t[nb];
            for (size_t i = 0; i < nb; i++)
                blocks_[i] = other.blocks_[i];
        }
        size_ = other.size_;
        capacity_ = nb * BITS;
    }

    vector(vector &&other) noexcept : blocks_(other.blocks_), size_(other.size_), capacity_(other.capacity_)
    {
        other.blocks_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    vector &operator=(const vector &other)
    {
        if (this != &other)
        {
            vector tmp(other);
            swap(tmp);
        }
        return *this;
    }

    vector &operator=(vector &&other) noexcept
    {
        if (this != &other)
        {
            delete[] blocks_;
            blocks_ = other.blocks_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.blocks_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    ~vector()
    {
        delete[] blocks_;
    }

    void swap(vector &other) noexcept
    {
        std::swap(blocks_, other.blocks_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    reference operator[](size_t i)
    {
        return reference(this, i);
    }

    bool operator[](size_t i) const
    {
        return get(i);
    }

    reference front()
    {
        return reference(this, 0);
    }

    bool front() const
    {
        return get(0);
    }

    reference back()
    {
        return reference(this, size_ - 1);
    }

    bool back() const
    {
        return get(size_ - 1);
    }

    // отдельных bool тут нет, поэтому отдаём сырые блоки с битами
    uint64_t *data()
    {
        return blocks_;
    }

    const uint64_t *data() const
    {
        return blocks_;
    }

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
        return static_cast<size_t>(-1) / 2;
    }

    size_t capacity() const
    {
        return capacity_;
    }

    void reserve(size_t n)
    {
        if (n > max_size())
            throw std::length_error("vector<bool>::reserve");
        if (n > capacity_)
            relocate(n);
    }

    void shrink_to_fit()
    {
        if (blocks_for(size_) * BITS < capacity_)
            relocate(size_);
    }

    void clear()
    {
        size_ = 0;
    }

    void push_back(bool value)
    {
        if (size_ == capacity_)
            relocate(capacity_ == 0 ? BITS : capacity_ * 2);
        set(size_, value);
        size_++;
    }

    void pop_back()
    {
        if (size_ > 0)
            size_--;
    }

    void emplace_back(bool value)
    {
        push_back(value);
    }

    // как и в общем vector позиция - индекс
    void emplace(size_t pos, bool value)
    {
        if (pos > size_)
            throw std::out_of_range("vector<bool>::emplace");

        push_back(false); // место под новый бит, size_ уже на 1 больше
        for (size_t i = size_ - 1; i > pos; i--)
            set(i, get(i - 1));
        set(pos, value);
    }

    void insert(size_t pos, bool value)
    {
        emplace(pos, value);
    }

    void resize(size_t n, bool value = false)
    {
        if (n < size_)
        {
            size_ = n;
            return;
        }
        reserve(n);
        while (size_ < n)
        {
            set(size_, value);
            size_++;
        }
    }

    void reverse()
    {
        if (size_ < 2)
            return;
        size_t i = 0;
        size_t j = size_ - 1;
        while (i < j)
        {
            bool a = get(i);
            bool b = get(j);
            set(i, b);
            set(j, a);
            i++;
            j--;
        }
    }
};

} // namespace my

#endif
