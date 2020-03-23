#include <iostream>
#include <cmath>
#include <mpi.h>

struct Body {
  double x, y, m, fx, fy;
};

int main(int argc, char** argv) {
  const int N = 20;
  Body body[N];
  MPI_Init(&argc, &argv);
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int begin = rank * (N / size);
  int end = (rank + 1) * (N / size);
  srand48(rank);
  for(int i=begin; i<end; i++) {
    body[i].x = drand48();
    body[i].y = drand48();
    body[i].m = drand48();
    body[i].fx = body[i].fy = 0;
  }
  MPI_Datatype MPI_BODY;
  MPI_Type_contiguous(5, MPI_DOUBLE, &MPI_BODY);
  MPI_Type_commit(&MPI_BODY);
  MPI_Allgather(&body[begin], end-begin, MPI_BODY, body, end-begin, MPI_BODY, MPI_COMM_WORLD);
  double fx[N] = {0}, fy[N] = {0};
  for(int i=begin; i<end; i++) {
    for(int j=0; j<N; j++) {
      if(i != j) {
        double rx = body[i].x - body[j].x;
        double ry = body[i].y - body[j].y;
        double r = std::sqrt(rx * rx + ry * ry);
        body[i].fx -= rx * body[j].m / (r * r * r);
        body[i].fy -= ry * body[j].m / (r * r * r);
      }
    }
  }
  MPI_Datatype MPI_FORCE;
  int blocksize[1] = {2}, displacement[1] = {3};
  MPI_Type_indexed(1, blocksize, displacement, MPI_DOUBLE, &MPI_FORCE);
  MPI_Type_create_resized(MPI_FORCE, 0, 5*sizeof(double), &MPI_FORCE);
  MPI_Type_commit(&MPI_FORCE);
  MPI_Allgather(&body[begin], end-begin, MPI_FORCE, body, end-begin, MPI_FORCE, MPI_COMM_WORLD);
  for(int i=0; i<N; i++) {
    if(rank==0) std::cout << i << " " << body[i].fx << " " << body[i].fy << std::endl;
  }
  MPI_Finalize();
}
