#include "bigint.hpp"

#include <stdexcept>

BigInt::BigInt() : digits_(nullptr), size_(0), minus_(false)
{
}

BigInt::BigInt(size_t size) : digits_(nullptr), size_(size), minus_(false)
{
    if (size_ > 0)
    {
        digits_ = new char[size_];
        for (size_t i = 0; i < size_; i++)
            digits_[i] = 0;
    }
}

BigInt::BigInt(int32_t value) : digits_(nullptr), size_(0), minus_(false)
{
    // через int64 иначе у INT32_MIN нельзя поменять знак
    int64_t x = value;
    if (x < 0)
    {
        minus_ = true;
        x = -x;
    }

    // у int32 максимум 10 цифр
    char tmp[12];
    size_t n = 0;
    while (x > 0)
    {
        tmp[n] = x % 10;
        n++;
        x /= 10;
    }

    if (n > 0)
    {
        digits_ = new char[n];
        for (size_t i = 0; i < n; i++)
            digits_[i] = tmp[i];
        size_ = n;
    }
}

BigInt::BigInt(const std::string &str) : digits_(nullptr), size_(0), minus_(false)
{
    size_t start = 0;
    bool neg = false;
    if (str.size() > 0 && (str[0] == '-' || str[0] == '+'))
    {
        neg = (str[0] == '-');
        start = 1;
    }

    if (start == str.size())
        throw std::invalid_argument("bad number");

    for (size_t i = start; i < str.size(); i++)
    {
        if (str[i] < '0' || str[i] > '9')
            throw std::invalid_argument("bad number");
    }

    size_ = str.size() - start;
    digits_ = new char[size_];
    for (size_t i = 0; i < size_; i++)
        digits_[i] = str[str.size() - 1 - i] - '0';

    minus_ = neg;
    trim();
}

BigInt::BigInt(const char *str) : BigInt(std::string(str))
{
}

BigInt::BigInt(const BigInt &other)
    : digits_(nullptr), size_(other.size_), minus_(other.minus_)
{
    if (size_ > 0)
    {
        digits_ = new char[size_];
        for (size_t i = 0; i < size_; i++)
            digits_[i] = other.digits_[i];
    }
}

BigInt::BigInt(BigInt &&other) noexcept
    : digits_(other.digits_), size_(other.size_), minus_(other.minus_)
{
    // other становится нулём
    other.digits_ = nullptr;
    other.size_ = 0;
    other.minus_ = false;
}

BigInt::~BigInt()
{
    delete[] digits_;
}

BigInt &BigInt::operator=(const BigInt &other)
{
    if (this == &other)
        return *this;

    char *new_digits = nullptr;
    if (other.size_ > 0)
    {
        new_digits = new char[other.size_];
        for (size_t i = 0; i < other.size_; i++)
            new_digits[i] = other.digits_[i];
    }

    delete[] digits_;
    digits_ = new_digits;
    size_ = other.size_;
    minus_ = other.minus_;
    return *this;
}

BigInt &BigInt::operator=(BigInt &&other) noexcept
{
    if (this == &other)
        return *this;

    delete[] digits_;

    digits_ = other.digits_;
    size_ = other.size_;
    minus_ = other.minus_;

    other.digits_ = nullptr;
    other.size_ = 0;
    other.minus_ = false;
    return *this;
}

// выкидываем старшие нули
void BigInt::trim()
{
    while (size_ > 0 && digits_[size_ - 1] == 0)
        size_--;

    if (size_ == 0)
    {
        delete[] digits_;
        digits_ = nullptr;
        minus_ = false;
    }
}

// сравнение по модулю: -1, 0 или 1
int BigInt::cmp_abs(const BigInt &a, const BigInt &b)
{
    if (a.size_ != b.size_)
        return a.size_ < b.size_ ? -1 : 1;

    for (size_t i = a.size_; i > 0; i--)
    {
        if (a.digits_[i - 1] != b.digits_[i - 1])
            return a.digits_[i - 1] < b.digits_[i - 1] ? -1 : 1;
    }
    return 0;
}

// складывает модули, знак не трогает
BigInt BigInt::add_abs(const BigInt &a, const BigInt &b)
{
    size_t n = a.size_ > b.size_ ? a.size_ : b.size_;
    BigInt res(n + 1);

    int carry = 0;
    for (size_t i = 0; i < n; i++)
    {
        int s = carry;
        if (i < a.size_)
            s += a.digits_[i];
        if (i < b.size_)
            s += b.digits_[i];

        res.digits_[i] = s % 10;
        carry = s / 10;
    }
    res.digits_[n] = carry;

    res.trim();
    return res;
}

// вычитает модули, надо чтобы |a| >= |b|
BigInt BigInt::sub_abs(const BigInt &a, const BigInt &b)
{
    BigInt res(a.size_);

    int borrow = 0;
    for (size_t i = 0; i < a.size_; i++)
    {
        int d = a.digits_[i] - borrow;
        if (i < b.size_)
            d -= b.digits_[i];

        if (d < 0)
        {
            d += 10;
            borrow = 1;
        }
        else
            borrow = 0;

        res.digits_[i] = d;
    }

    res.trim();
    return res;
}

BigInt BigInt::operator-() const
{
    BigInt res(*this);
    if (res.size_ > 0)
        res.minus_ = !res.minus_;
    return res;
}

BigInt BigInt::operator+(const BigInt &other) const
{
    if (minus_ == other.minus_)
    {
        BigInt res = add_abs(*this, other);
        if (res.size_ > 0)
            res.minus_ = minus_;
        return res;
    }

    // знаки разные, вычитаем из большего по модулю меньший
    int c = cmp_abs(*this, other);
    if (c == 0)
        return BigInt();

    BigInt res;
    if (c > 0)
    {
        res = sub_abs(*this, other);
        res.minus_ = minus_;
    }
    else
    {
        res = sub_abs(other, *this);
        res.minus_ = other.minus_;
    }
    return res;
}

BigInt BigInt::operator-(const BigInt &other) const
{
    return *this + (-other);
}

BigInt BigInt::operator*(const BigInt &other) const
{
    if (size_ == 0 || other.size_ == 0)
        return BigInt();

    BigInt res(size_ + other.size_);

    // обычное умножение в столбик, O(n^2)
    for (size_t i = 0; i < size_; i++)
    {
        int carry = 0;
        for (size_t j = 0; j < other.size_; j++)
        {
            int cur = res.digits_[i + j] + digits_[i] * other.digits_[j] + carry;
            res.digits_[i + j] = cur % 10;
            carry = cur / 10;
        }

        // перенос может уйти дальше одной клетки
        size_t k = i + other.size_;
        while (carry > 0)
        {
            int cur = res.digits_[k] + carry;
            res.digits_[k] = cur % 10;
            carry = cur / 10;
            k++;
        }
    }

    res.trim();
    if (res.size_ > 0)
        res.minus_ = (minus_ != other.minus_);
    return res;
}

BigInt BigInt::operator+(int32_t other) const
{
    return *this + BigInt(other);
}

BigInt BigInt::operator-(int32_t other) const
{
    return *this - BigInt(other);
}

BigInt BigInt::operator*(int32_t other) const
{
    return *this * BigInt(other);
}

bool BigInt::operator==(const BigInt &other) const
{
    if (minus_ != other.minus_)
        return false;
    return cmp_abs(*this, other) == 0;
}

bool BigInt::operator!=(const BigInt &other) const
{
    return !(*this == other);
}

bool BigInt::operator<(const BigInt &other) const
{
    if (minus_ != other.minus_)
        return minus_;  // отрицательное меньше положительного

    int c = cmp_abs(*this, other);
    if (minus_)
        return c > 0;
    return c < 0;
}

bool BigInt::operator>(const BigInt &other) const
{
    return other < *this;
}

bool BigInt::operator<=(const BigInt &other) const
{
    return !(other < *this);
}

bool BigInt::operator>=(const BigInt &other) const
{
    return !(*this < other);
}

std::ostream &operator<<(std::ostream &out, const BigInt &num)
{
    if (num.size_ == 0)
    {
        out << '0';
        return out;
    }

    if (num.minus_)
        out << '-';

    for (size_t i = num.size_; i > 0; i--)
        out << static_cast<char>('0' + num.digits_[i - 1]);

    return out;
}

