#include <vector>
#include <stdexcept>
#include <random>
#include <sstream>
#include <mpi.h>
#include <iostream>
#include <fstream>

double local_start_time, local_end_time;

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

    local_start_time = MPI_Wtime();

    Matrix localC = localA * B;

    local_end_time = MPI_Wtime();

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

    if (argc != 6)
    {
        if (rank == 0)
            std::cerr << "Usage: " << argv[0] << " <rowsA> <colsA> <rowsB> <colsB> <n>" << std::endl;
        MPI_Finalize();
        return 1;
    }

    const int rowsA = std::stoi(argv[1]);
    const int colsA = std::stoi(argv[2]);
    const int rowsB = std::stoi(argv[3]);
    const int colsB = std::stoi(argv[4]);
    const int n = std::stoi(argv[5]);

    if (colsA != rowsB)
    {
        if (rank == 0)
            std::cerr << "Error: Number of columns in A must match number of rows in B." << std::endl;
        MPI_Finalize();
        return 1;
    }

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::ostringstream filename;
    filename << "timings-" << rowsA << "x" << colsA << "-" << rowsB << "x" << colsB << "-" << size << ".csv";
    std::ofstream csv_file;
    if (rank == 0)
    {
        csv_file.open(filename.str());
        csv_file << "run,process,local_time,total_time\n";
    }

    for (int run = 0; run < n; ++run)
    {
        Matrix A(rowsA, colsA);
        Matrix B(rowsB, colsB);
        Matrix C(rowsA, colsB);

        if (rank == 0)
        {
            A.rand();
            B.rand();
        }

        const double start_time = MPI_Wtime();
        MPI_Matrix_Multiplication(A, B, C);
        const double end_time = MPI_Wtime();

        const double local_time = local_end_time - local_start_time;
        std::vector<double> local_times(size);

        MPI_Gather(&local_time, 1, MPI_DOUBLE, local_times.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            for (int i = 0; i < size; ++i)
            {
                csv_file << run << "," << i << "," << local_times[i] << ",";
                if (i == 0)
                    csv_file << (end_time - start_time);
                csv_file << "\n";
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank == 0)
    {
        csv_file.close();
    }

    MPI_Finalize();

    return 0;
}