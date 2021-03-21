#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <definitions.h>
#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

void assign_neighbours(int rank, int size, int neighbours[]);
int* fix_binary_number(int binary[], int truncated, int aux);
int* convert_decimal_to_binary(int number, int truncated);

/* Main function */
int main(int argc, char **argv) {
    int rank, size, truncated, hypercube = false;
    float buf;
    
	MPI_Request request;
    MPI_Status status;

    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        double res = log(size)/log(2);
        truncated = (int) res;

        if (truncated == res) {
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
        int neighbours[truncated];
        
        MPI_Recv(&buf, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        //printf("Process %d received %.2f\n", rank, buf);

        assign_neighbours(rank, log(size)/log(2), neighbours);
    }

    MPI_Finalize();
}

/* Fix numbers which length is less than the dimension of the network */
int* fix_binary_number(int binary[], int truncated, int aux) {
    int *b_digits;
    int i;
    int real_binary;

    for (i = 0; i < aux; i++) {
        b_digits[i] = 0;
    }

    for (int j = truncated-aux-1; 0 <= j; j--) {
        b_digits[i] = binary[j];
        i++;
    }

    return b_digits;
}

/* Convert a given decimal number to binary number */
int* convert_decimal_to_binary(int number, int truncated) {
    int binary[truncated];
    int i;
    int aux = truncated;
    int real_binary[truncated];

    for (i = 0; number > 0; i++) {
        binary[i] = number%2;
        number = number/2;
        aux--;
    }

    return fix_binary_number(binary, truncated, aux);
}

/* Assign neighours to processes */
void assign_neighbours(int rank, int truncated, int neighbours[]) {

    int* binary = convert_decimal_to_binary(rank, truncated);
    
    printf("Number %d --> ", rank);
    for (int i = 0; i < truncated; i++) {
        printf("%d",binary[i]);
    }
    printf("\n");
}