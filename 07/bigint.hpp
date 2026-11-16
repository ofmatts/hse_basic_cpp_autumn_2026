#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

class BigInt
{
public:
    BigInt();
    BigInt(int32_t value);
    BigInt(const std::string &str);
    BigInt(const char *str);
    BigInt(const BigInt &other);
    BigInt(BigInt &&other) noexcept;
    ~BigInt();

    BigInt &operator=(const BigInt &other);
    BigInt &operator=(BigInt &&other) noexcept;

    BigInt operator-() const;

    BigInt operator+(const BigInt &other) const;
    BigInt operator-(const BigInt &other) const;
    BigInt operator*(const BigInt &other) const;

    BigInt operator+(int32_t other) const;
    BigInt operator-(int32_t other) const;
    BigInt operator*(int32_t other) const;

    bool operator==(const BigInt &other) const;
    bool operator!=(const BigInt &other) const;
    bool operator<(const BigInt &other) const;
    bool operator>(const BigInt &other) const;
    bool operator<=(const BigInt &other) const;
    bool operator>=(const BigInt &other) const;

    friend std::ostream &operator<<(std::ostream &out, const BigInt &num);

private:
    // цифры в десятичной системе, digits_[0] - самая младшая
    // ноль = size_ 0 и digits_ == nullptr (и минуса у нуля нет)
    char *digits_;
    size_t size_;
    bool minus_;

    explicit BigInt(size_t size);  // size нулевых цифр, потом надо будет trim()

    void trim();
    static int cmp_abs(const BigInt &a, const BigInt &b);
    static BigInt add_abs(const BigInt &a, const BigInt &b);
    static BigInt sub_abs(const BigInt &a, const BigInt &b);  // |a| >= |b|
};

std::ostream &operator<<(std::ostream &out, const BigInt &num);

#endif
