#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>

class Matrix
{
    class ProxyRow
    {
    private:
        int32_t *data_;
        size_t cols_;

    public:
        ProxyRow() : data_(nullptr), cols_(0) {}
        ProxyRow(int32_t *data, size_t cols) : data_(data), cols_(cols) {}

        int32_t &operator[](size_t j);
        const int32_t &operator[](size_t j) const;
    };

public:
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix &other);
    ~Matrix();

    Matrix &operator=(const Matrix &other);

    size_t getRows() const { return rows_count_; }
    size_t getColumns() const { return cols_count_; }

    ProxyRow &operator[](size_t i);
    const ProxyRow &operator[](size_t i) const;

    Matrix &operator*=(int32_t k);

    Matrix operator+(const Matrix &other) const;

    bool operator==(const Matrix &other) const;
    bool operator!=(const Matrix &other) const;

private:
    size_t rows_count_;
    size_t cols_count_;
    int32_t *data_;
    ProxyRow *rows_;

    void fill_rows();
};

std::ostream &operator<<(std::ostream &out, const Matrix &m);

#endif
