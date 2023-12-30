#include <iostream>
#include <fstream>
#include <cmath>
#include "mpi.h"

const int N = 10000;
double** a, * b, * c, * buf;
int size, rank, n;
MPI_Status st;

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    n = N / size;

    b = new double[N];
    c = new double[n];
    a = new double* [n];
    for (int i = 0; i < n; i++) {
        a[i] = new double[N];
    }
    buf = new double[n * N];

    if (rank == 0) {
        for (int p = 1; p < size; p++) {
            for (int i = 0, m = 0; i < n; i++) {
                for (int j = 0; j < N; j++) {
                    a[i][j] = (fabs(i - j) > 2 ? 0. : 1.);
                    buf[m++] = a[i][j];
                }
            }
            MPI_Send(buf, n * N, MPI_DOUBLE, p, 0, MPI_COMM_WORLD);
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < N; j++) {
                a[i][j] = (fabs(i - j) > 2 ? 0. : 1.);
            }
        }
        for (int j = 0; j < N; j++) {
            b[j] = 1.;
        }
    }
    else {
        MPI_Recv(buf, n * N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &st);
        for (int i = 0, m = 0; i < n; i++) {
            for (int j = 0; j < N; j++) {
                a[i][j] = buf[m++];
            }
        }
    }
    MPI_Bcast(b, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double time = MPI_Wtime();
    for (int i = 0; i < n; i++) {
        c[i] = 0.;
        for (int j = 0; j < N; j++) {
            c[i] += a[i][j] * b[j];
        }
    }
    time = MPI_Wtime() - time;

    if (rank != 0) {
        MPI_Send(c, n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }
    else {
        std::ofstream fout("result.txt");
        for (int i = 0; i < n; i++) {
            fout << 0 << "\t" << c[i] << std::endl;
        }
        for (int p = 1; p < size; p++) {
            MPI_Recv(c, n, MPI_DOUBLE, p, 1, MPI_COMM_WORLD, &st);
            for (int i = 0; i < n; i++) {
                fout << p << "\t" << c[i] << std::endl;
            }
        }
        fout.close();

        std::cout << "time = " << time << std::endl;
    }

    // MPI_Reduce(*sendbuf, *recvbuf, count, datatype, MPI_SUM, root=0, MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}