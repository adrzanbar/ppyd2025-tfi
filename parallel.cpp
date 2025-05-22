#include <vector>
#include <stdexcept>
#include <random>
#include <sstream>
#include <mpi.h>

struct Matrix
{
    int rows;
    int cols;
    std::vector<double> values;

    Matrix(int r, int c) : rows(r), cols(c), values(r * c) {}

    double *operator[](int row)
    {
        return &values[row * cols];
    }

    const double *operator[](int row) const
    {
        return &values[row * cols];
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
        std::uniform_int_distribution<int> dis(0, 9);
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                (*this)[i][j] = dis(g);
            }
        }
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
            oss << std::endl;
        }
        return oss.str();
    }

    double *data() { return values.data(); }
    const double *data() const { return values.data(); }
};

void MPI_Matrix_Multiplication(const Matrix &A, const Matrix &B, Matrix &C)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int base_rows = A.rows / size;
    int remainder = A.rows % size;

    std::vector<int> sendcounts(size);
    std::vector<int> displs(size);
    std::vector<int> recvcounts(size);
    std::vector<int> recvdispls(size);

    int offset = 0;
    int recv_offset = 0;

    for (int i = 0; i < size; ++i)
    {
        int rows_i = base_rows + (i < remainder ? 1 : 0);
        sendcounts[i] = rows_i * A.cols;
        displs[i] = offset;
        offset += sendcounts[i];

        recvcounts[i] = rows_i * B.cols;
        recvdispls[i] = recv_offset;
        recv_offset += recvcounts[i];
    }

    int local_rows = base_rows + (rank < remainder ? 1 : 0);
    Matrix localA(local_rows, A.cols);

    MPI_Scatterv(
        A.data(),
        sendcounts.data(),
        displs.data(),
        MPI_DOUBLE,
        localA.data(),
        sendcounts[rank],
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    MPI_Bcast((void *)B.data(), B.rows * B.cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    Matrix localC = localA * B;

    MPI_Gatherv(
        localC.data(),
        recvcounts[rank],
        MPI_DOUBLE,
        C.data(),
        recvcounts.data(),
        recvdispls.data(),
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);
};

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0] << " <rowsA> <colsA> <rowsB> <colsB>" << std::endl;
        MPI_Finalize();
        return 1;
    }

    const int rowsA = std::stoi(argv[1]);
    const int colsA = std::stoi(argv[2]);
    const int rowsB = std::stoi(argv[3]);
    const int colsB = std::stoi(argv[4]);

    Matrix A(rowsA, colsA);
    Matrix B(rowsB, colsB);
    Matrix C(rowsA, colsB);

    if (rank == 0)
    {
        A.rand();
        B.rand();

        std::cout << "Matrix A:" << std::endl;
        std::cout << A.str() << std::endl;

        std::cout << "Matrix B:" << std::endl;
        std::cout << B.str() << std::endl;
    }

    MPI_Matrix_Multiplication(A, B, C);

    if (rank == 0)
    {
        std::cout << "Result Matrix C:" << std::endl;
        std::cout << C.str() << std::endl;
    }

    MPI_Finalize();

    return 0;
}