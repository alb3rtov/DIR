#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <definitions.h>
#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

void calculate_neighbours(int *neighbours, int size, int rank, int dimension);

/* Main function */
int main(int argc, char **argv) {
    int rank, size, dimension, hypercube = false;
    float buf;
    
	MPI_Request request;
    MPI_Status status;

    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        double res = log(size)/log(2);
        dimension = (int) res;

        if (dimension == res) {
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
        dimension = log(size)/log(2);
        int neighbours[dimension];
        
        calculate_neighbours(neighbours, size, rank, dimension);

        printf("Process %d neighbours: ",rank);
        for (int i = 0; i < dimension; i++) {
            printf("%d ",neighbours[i]);
        }
        printf("\n");
        //MPI_Recv(&buf, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        
    }

    MPI_Finalize();
}

void calculate_neighbours(int *neighbours, int size, int rank, int dimension) {
    int cnt = 0;
    
    for (int i = 0; i < size; i++) {
        if (cnt == dimension) {
            break;
        } else {
            int res = rank^i;
            if (res == 1 || res == 2 || res == 4) {
                neighbours[cnt++] = i;
            }
        }
    }
}