#include <iostream>
#include <cmath>
#include <mpi.h>

int main(int argc, char** argv) {
  const int N = 20;
  double x[N], y[N], m[N], fx[N], fy[N];
  MPI_Init(&argc, &argv);
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int begin = rank * (N / size);
  int end = (rank + 1) * (N / size);
  srand48(rank);
  for(int i=begin; i<end; i++) {
    x[i] = drand48();
    y[i] = drand48();
    m[i] = drand48();
    fx[i] = fy[i] = 0;
  }
  MPI_Allgather( &x[begin], end-begin, MPI_DOUBLE,  x, end-begin, MPI_DOUBLE, MPI_COMM_WORLD);
  MPI_Allgather( &y[begin], end-begin, MPI_DOUBLE,  y, end-begin, MPI_DOUBLE, MPI_COMM_WORLD);
  MPI_Allgather( &m[begin], end-begin, MPI_DOUBLE,  m, end-begin, MPI_DOUBLE, MPI_COMM_WORLD);
  MPI_Allgather(&fx[begin], end-begin, MPI_DOUBLE, fx, end-begin, MPI_DOUBLE, MPI_COMM_WORLD);
  MPI_Allgather(&fy[begin], end-begin, MPI_DOUBLE, fy, end-begin, MPI_DOUBLE, MPI_COMM_WORLD);
  for(int i=0; i<N; i++) {
    for(int j=begin; j<end; j++) {
      if(i != j) {
        double rx = x[i] - x[j];
        double ry = y[i] - y[j];
        double r = std::sqrt(rx * rx + ry * ry);
        fx[i] -= rx * m[j] / (r * r * r);
        fy[i] -= ry * m[j] / (r * r * r);
      }
    }
  }
  MPI_Allreduce(MPI_IN_PLACE, fx, N, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(MPI_IN_PLACE, fy, N, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  for(int i=0; i<N; i++) {
    if(rank==0) std::cout << i << " " << fx[i] << " " << fy[i] << std::endl;
  }
  MPI_Finalize();
}
