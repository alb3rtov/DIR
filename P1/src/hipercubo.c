#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <definitions.h>
#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

/* Main function */
int main(int argc, char **argv) {
    int rank, size, trucanted, hypercube = false;
    float buf;

	MPI_Request request;
    MPI_Status status;

    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        double res = log(size)/log(2);
        trucanted = (int) res;

        if (trucanted == res) {
            hypercube = true;
            send_network_topology_confirmation(hypercube, size, request);

            FILE* file = open_file("datos.dat", "r");
            read_assign_values(file, size);

        } else {
            send_network_topology_confirmation(hypercube, size, request);
            fprintf(stderr, "Error, can't create toroidal network: %d processes\n", size);
        }
    } else {
        MPI_Recv(&hypercube, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }

    if (hypercube == true) {
        MPI_Recv(&buf, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("Process %d received %.2f\n", rank, buf);
    }

    MPI_Finalize();
}