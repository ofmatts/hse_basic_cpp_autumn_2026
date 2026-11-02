#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <cstdint>
#include <ostream>

class Matrix
{
    class ProxyRow
    {
        int32_t *data_;
        size_t columns_;

    public:
        ProxyRow(int32_t *data, size_t columns);

        int32_t &operator[](size_t column);
        const int32_t &operator[](size_t column) const;
    };

    size_t rows_;
    size_t columns_;
    int32_t *data_;

public:
    Matrix(size_t rows, size_t columns);
    Matrix(const Matrix &other);
    Matrix &operator=(const Matrix &other);
    ~Matrix();

    size_t getRows() const;
    size_t getColumns() const;

    ProxyRow operator[](size_t row);
    const ProxyRow operator[](size_t row) const;

    Matrix &operator*=(int32_t value);

    friend Matrix operator+(const Matrix &left, const Matrix &right);
    friend bool operator==(const Matrix &left, const Matrix &right);
    friend bool operator!=(const Matrix &left, const Matrix &right);
    friend std::ostream &operator<<(std::ostream &out, const Matrix &matrix);
};

#endif
