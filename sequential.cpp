#include <vector>
#include <iostream>
#include <sstream>
#include <random>
#include <string>
#include <iomanip>

// Funciones para darle formato a los numeros para imprimirlos por pantalla.
std::string format_double(double number)
{
    std::ostringstream oss;
    oss << std::fixed << std::showpoint << std::setprecision(1);
    oss << number;
    return oss.str();
}

std::string center_num(double number, int total_ln)
{
    std::string num_str = format_double(number);
    int espacios = total_ln - num_str.length();
    int derecha = espacios / 2;
    int izquierda = espacios - derecha;
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
                double sum = 0.0;
                for (int k = 0; k < cols; ++k)
                {
                    sum += (*this)[i][k] * other[k][j];
                }
                result[i][j] = sum;
            }
        }
        return result;
    }

    void rand()
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::uniform_real_distribution<double> dis(0.0, 1.0);
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                (*this)[i][j] = dis(g);
            }
        }
    }

    int calculate_biggest_ln() const
    { // Convierte a string los valores de la matriz para medir la longitud de la cadena, devuelve el más grande.
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
    {
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

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0] << " <rowsA> <colsA> <rowsB> <colsB>" << std::endl;
        return 1;
    }

    const int rowsA = std::stoi(argv[1]);
    const int colsA = std::stoi(argv[2]);
    const int rowsB = std::stoi(argv[3]);
    const int colsB = std::stoi(argv[4]);

    if (colsA != rowsB)
    {
        std::cerr << "Error: Number of columns in A must match number of rows in B." << std::endl;
        return 1;
    }

    Matrix A(rowsA, colsA);
    A.rand();
    Matrix B(rowsB, colsB);
    B.rand();

    //start
    A * B;
    //end

    return 0;
}