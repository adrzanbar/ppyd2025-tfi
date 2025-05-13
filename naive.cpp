#include <vector>
#include <iostream>
#include <sstream>
#include <random>
#include <string>
#include <iomanip>

// Funciones para darle formato a los numeros para imprimirlos por pantalla.
std::string format_double(double number) {
    std::ostringstream oss;
    oss << std::fixed << std::showpoint << std::setprecision(1);
    oss << number;
    return oss.str();
}

std::string center_num(double number, int total_ln) {
    std::string num_str = format_double(number);
    int espacios = total_ln - num_str.length();
    int izquierda = espacios / 2;
    int derecha = espacios - izquierda;
    return std::string(izquierda, ' ') + num_str + std::string(derecha, ' ');
}
// ---

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

    int calculate_biggest_ln() const
    {   // Convierte a string los valores de la matriz para medir la longitud de la cadena, devuelve el más grande.
        int max_len = 0;
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                std::string val_str = format_double((*this)[i][j]);
                int len = val_str.length();
                if (len > max_len)
                {
                    max_len = len;
                }
            }
        }
        return max_len;
    }

    std::string str() const
    {   // Imprime el contenido de la matriz por pantalla.
        int biggest_ln = calculate_biggest_ln();

        std::ostringstream oss;
        for (int i = 0; i < rows; ++i)
        {
            // oss << "|";
            for (int j = 0; j < cols; ++j)
            {
                oss << center_num((*this)[i][j], biggest_ln) << " ";
            }
            oss << "\n";
        }
        return oss.str();
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