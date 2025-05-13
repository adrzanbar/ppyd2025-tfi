#include <vector>
#include <iostream>
#include <sstream>
#include <random>

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

    std::string str() const
    {
        std::ostringstream oss;
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                oss << (*this)[i][j] << " ";
            }
            oss << "\n";
        }
        return oss.str();
    }

    void rand()
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::uniform_int_distribution<int> dis(0, 9);
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                (*this)[i][j] = dis(g);
            }
        }
    }
};

int main()
{
    std::cout << "Matrix A:" << std::endl;
    Matrix A(2, 3);
    A.rand();
    std::cout << A.str() << std::endl;
    std::cout << "Matrix B:" << std::endl;
    Matrix B(3, 2);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            B[i][j] = i * j;
        }
    }
    std::cout << B.str() << std::endl;
    std::cout << "Matrix C (A * B):" << std::endl;
    Matrix C = A * B;
    std::cout << C.str() << std::endl;
    return 0;
}