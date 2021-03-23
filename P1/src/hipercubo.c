#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <definitions.h>
#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

void assign_neighbours(int *neighbours, int size, int rank, int dimension);
float search_max_number(int *neighbours, int size,  int dimension, float send_number, int rank);

/* Main function */
int main(int argc, char **argv) {
    int rank, size, dimension, hypercube = false;
    float buf,  max_number;
    
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
        /* [Neighbour dimension 1, neighbour dimension 2, ... , neighbour dimension D...] */
        int neighbours[dimension];
        
        MPI_Recv(&buf, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        assign_neighbours(neighbours, size, rank, dimension);
        /*
        printf("Process %d neighbours: ",rank);
        for (int i = 0; i < dimension; i++) {
            printf("%d ",neighbours[i]);
        }
        printf("\n");
        */
        
        max_number = search_max_number(neighbours, size, dimension, buf, rank);

        //printf("Process %d number: %.2f: max: %.2f\n",rank, buf, max_number);
    }

    if (rank == 0) {
        printf("\n[Process %d] The maximum number is: %.2f\n\n", rank, max_number);
    }

    MPI_Finalize();
}

/* Function to compare which is the bigger number */
float compare_numbers(float number1, float number2) {

	if (number1 > number2) {
		return number1;
	} else {
		return number2;
	}
}

/* Search the max number sending the number assigned and asking to neighbours for their numbers */
float search_max_number(int *neighbours, int size, int dimension, float send_number, int rank) {
    
    MPI_Request request1;
	MPI_Request request2;
	MPI_Status status;

	float recv_number;
    
    for (int i = 0; i < dimension; i++) {
        MPI_Irecv(&recv_number, 1, MPI_FLOAT, neighbours[i], MPI_ANY_TAG, MPI_COMM_WORLD, &request1);
        MPI_Isend(&send_number, 1, MPI_FLOAT, neighbours[i], i, MPI_COMM_WORLD, &request2);
        MPI_Wait(&request1, &status);

		send_number = compare_numbers(send_number, recv_number);
		recv_number = send_number;
    }

    return recv_number;
}

/* Assign neighbouts using XOR operation */
void assign_neighbours(int *neighbours, int size, int rank, int dimension) {
    int cnt = 0;
    
    for (int i = 0; i < size; i++) {
        if (cnt == dimension) {
            break;
        } else {
            int res = rank^i;
            for (int j = 0; j < dimension; j++) {
                /* Check if only change 1 bit in res variable */
                if (res == (int) pow(2,j)) {
                    for (int k = 0; k < dimension; k++) {
                        /* Check the dimension to assign it in the right position into the array */
                        if ((i == rank - (int) pow(2,k)) || (i == rank + (int) pow(2,k)))  {
                            neighbours[k] = i;
                            cnt++;
                            break; 
                        }

                    }

                }
            }
        }
    }
}