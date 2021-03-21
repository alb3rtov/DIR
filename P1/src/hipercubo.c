#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <definitions.h>
#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

void assign_neighbours(int rank, int dimension, int *binary_number);
void convert_decimal_to_binary(int number, int dimension, int *binary_number);

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
        int binary_number[dimension];
        
        assign_neighbours(rank, dimension, binary_number);
       
        printf("Number %d --> ", rank);
        for (int i = 0; i < dimension; i++) {
            printf("%d",binary_number[i]);
        }
        printf("\n");
        //MPI_Recv(&buf, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    }

    MPI_Finalize();
}

/* Convert a given decimal number to binary number */
void convert_decimal_to_binary(int number, int dimension, int *binary_number) {
    int i, j;
    int aux = dimension;
    int binary[dimension];
    
    for (i = 0; number > 0; i++) {
        binary[i] = number%2;
        number = number/2;
        aux--;
    }
    
    /* Fix numbers which length is less than the dimension of the network */
    for (j = 0; j < aux; j++) {
        binary_number[j] = 0;
    }
    
    for (int k = i - 1; k >= 0; k--) {
        binary_number[j++] = binary[k];
    }
}

/* Assign neighours to processes */
void assign_neighbours(int rank, int dimension, int *binary_number) {
    convert_decimal_to_binary(rank, dimension, binary_number);
    



}