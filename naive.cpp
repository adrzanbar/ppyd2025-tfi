#include <vector>
#include <iostream>

struct Matrix
{
    int rows;
    int cols;
    std::vector<double> data;

    Matrix(int r, int c) : rows(r), cols(c), data(r * c) {}

    double *operator[](int row)
    {
        return &data[row * cols];
    }

    const double *operator[](int row) const
    {
        return &data[row * cols];
    }

    Matrix operator*(const Matrix &other) const
    {
        if (cols != other.rows)
        {
            throw std::invalid_argument("Matrix dimensions do not match for multiplication.");
        }
        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < other.cols; ++j)
            {
                for (int k = 0; k < cols; ++k)
                {
                    result[i][j] += (*this)[i][k] * other[k][j];
                }
            }
        }
        return result;
    }
};