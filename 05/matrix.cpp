#include "matrix.hpp"

#include <stdexcept>

Matrix::ProxyRow::ProxyRow(int32_t *data, size_t columns)
    : data_(data), columns_(columns)
{
}

int32_t &Matrix::ProxyRow::operator[](size_t column)
{
    if (column >= columns_)
        throw std::out_of_range("");
    return data_[column];
}

const int32_t &Matrix::ProxyRow::operator[](size_t column) const
{
    if (column >= columns_)
        throw std::out_of_range("");
    return data_[column];
}

Matrix::Matrix(size_t rows, size_t columns)
    : rows_(rows), columns_(columns), data_(new int32_t[rows * columns]())
{
}

Matrix::Matrix(const Matrix &other)
    : rows_(other.rows_), columns_(other.columns_),
      data_(new int32_t[other.rows_ * other.columns_])
{
    for (size_t i = 0; i < rows_ * columns_; i++)
        data_[i] = other.data_[i];
}

Matrix &Matrix::operator=(const Matrix &other)
{
    if (this == &other)
        return *this;

    int32_t *new_data = new int32_t[other.rows_ * other.columns_];
    for (size_t i = 0; i < other.rows_ * other.columns_; i++)
        new_data[i] = other.data_[i];

    delete[] data_;
    data_ = new_data;
    rows_ = other.rows_;
    columns_ = other.columns_;
    return *this;
}

Matrix::~Matrix()
{
    delete[] data_;
}

size_t Matrix::getRows() const
{
    return rows_;
}

size_t Matrix::getColumns() const
{
    return columns_;
}

Matrix::ProxyRow Matrix::operator[](size_t row)
{
    if (row >= rows_)
        throw std::out_of_range("");
    return ProxyRow(data_ + row * columns_, columns_);
}

const Matrix::ProxyRow Matrix::operator[](size_t row) const
{
    if (row >= rows_)
        throw std::out_of_range("");
    return ProxyRow(data_ + row * columns_, columns_);
}

Matrix &Matrix::operator*=(int32_t value)
{
    for (size_t i = 0; i < rows_ * columns_; i++)
        data_[i] *= value;
    return *this;
}

Matrix operator+(const Matrix &left, const Matrix &right)
{
    if (left.rows_ != right.rows_ || left.columns_ != right.columns_)
        throw std::out_of_range("");

    Matrix result(left.rows_, left.columns_);
    for (size_t i = 0; i < left.rows_ * left.columns_; i++)
        result.data_[i] = left.data_[i] + right.data_[i];
    return result;
}

bool operator==(const Matrix &left, const Matrix &right)
{
    if (left.rows_ != right.rows_ || left.columns_ != right.columns_)
        return false;

    for (size_t i = 0; i < left.rows_ * left.columns_; i++)
    {
        if (left.data_[i] != right.data_[i])
            return false;
    }
    return true;
}

bool operator!=(const Matrix &left, const Matrix &right)
{
    return !(left == right);
}

std::ostream &operator<<(std::ostream &out, const Matrix &matrix)
{
    for (size_t i = 0; i < matrix.rows_; i++)
    {
        for (size_t j = 0; j < matrix.columns_; j++)
        {
            if (j != 0)
                out << ' ';
            out << matrix.data_[i * matrix.columns_ + j];
        }
        out << '\n';
    }
    return out;
}
