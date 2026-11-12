#include "matrix.hpp"

#include <stdexcept>

int32_t &Matrix::ProxyRow::operator[](size_t j)
{
    if (j >= cols_)
        throw std::out_of_range("");
    return data_[j];
}

const int32_t &Matrix::ProxyRow::operator[](size_t j) const
{
    if (j >= cols_)
        throw std::out_of_range("");
    return data_[j];
}

Matrix::Matrix(size_t rows, size_t cols)
    : rows_count_(rows), cols_count_(cols)
{
    data_ = new int32_t[rows * cols];
    rows_ = new ProxyRow[rows];

    for (size_t i = 0; i < rows * cols; i++)
        data_[i] = 0;

    fill_rows();
}

Matrix::Matrix(const Matrix &other)
    : rows_count_(other.rows_count_), cols_count_(other.cols_count_)
{
    data_ = new int32_t[rows_count_ * cols_count_];
    rows_ = new ProxyRow[rows_count_];

    for (size_t i = 0; i < rows_count_ * cols_count_; i++)
        data_[i] = other.data_[i];

    fill_rows();
}

Matrix::~Matrix()
{
    delete[] rows_;
    delete[] data_;
}

// проставляем в каждую строку указатель на её начало в data_
void Matrix::fill_rows()
{
    for (size_t i = 0; i < rows_count_; i++)
        rows_[i] = ProxyRow(data_ + i * cols_count_, cols_count_);
}

Matrix &Matrix::operator=(const Matrix &other)
{
    if (this == &other)
        return *this;

    // сначала выделяем новое, потом удаляем старое
    int32_t *new_data = new int32_t[other.rows_count_ * other.cols_count_];
    ProxyRow *new_rows = nullptr;
    try
    {
        new_rows = new ProxyRow[other.rows_count_];
    }
    catch (...)
    {
        delete[] new_data;
        throw;
    }

    for (size_t i = 0; i < other.rows_count_ * other.cols_count_; i++)
        new_data[i] = other.data_[i];

    delete[] rows_;
    delete[] data_;

    data_ = new_data;
    rows_ = new_rows;
    rows_count_ = other.rows_count_;
    cols_count_ = other.cols_count_;
    fill_rows();

    return *this;
}

Matrix::ProxyRow &Matrix::operator[](size_t i)
{
    if (i >= rows_count_)
        throw std::out_of_range("");
    return rows_[i];
}

const Matrix::ProxyRow &Matrix::operator[](size_t i) const
{
    if (i >= rows_count_)
        throw std::out_of_range("");
    return rows_[i];
}

Matrix &Matrix::operator*=(int32_t k)
{
    for (size_t i = 0; i < rows_count_ * cols_count_; i++)
        data_[i] *= k;
    return *this;
}

Matrix Matrix::operator+(const Matrix &other) const
{
    // про размеры в задании ничего нет, кидаю то же исключение
    if (rows_count_ != other.rows_count_ || cols_count_ != other.cols_count_)
        throw std::out_of_range("");

    Matrix res(rows_count_, cols_count_);
    for (size_t i = 0; i < rows_count_ * cols_count_; i++)
        res.data_[i] = data_[i] + other.data_[i];
    return res;
}

bool Matrix::operator==(const Matrix &other) const
{
    if (rows_count_ != other.rows_count_ || cols_count_ != other.cols_count_)
        return false;

    for (size_t i = 0; i < rows_count_ * cols_count_; i++)
    {
        if (data_[i] != other.data_[i])
            return false;
    }
    return true;
}

bool Matrix::operator!=(const Matrix &other) const
{
    return !(*this == other);
}

std::ostream &operator<<(std::ostream &out, const Matrix &m)
{
    for (size_t i = 0; i < m.getRows(); i++)
    {
        for (size_t j = 0; j < m.getColumns(); j++)
        {
            if (j != 0)
                out << " ";
            out << m[i][j];
        }
        out << "\n";
    }
    return out;
}
