#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <definitions.h>
#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

#define NUM_NEIGHBOURS 4

float compare_numbers(float number1, float number2);
void assign_neighbours(int *north_process, int *south_process, int *west_process,
							int *east_process, int l, int rank, int size);
int mod(int a, int b);
float search_min_number(int neighbours[], int size, float send_number);

/* Main function */
int main(int argc, char **argv) {

	int rank, size, truncated, toroidal = false;
	float buf, final_min_number;

	MPI_Status status;
	MPI_Request request;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		double value = sqrt(size);
		truncated = (int) value;

		/*Check if un toroidal network can be possible */
		if (truncated == value) {
			toroidal = true;
			send_network_topology_confirmation(toroidal, size, request);

			FILE* file = open_file("datos.dat","r");
			read_assign_values(file, size);

		} else {
			send_network_topology_confirmation(toroidal, size, request);
			fprintf(stderr, "Error, can't create toroidal network: %d processes\n", size);
		}

	} else {
		MPI_Recv(&toroidal, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}

	if (toroidal == true) {
		
		int north_process, south_process, west_process, east_process;

		/* Receive the number of the list */
		MPI_Recv(&buf, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		assign_neighbours(&north_process, &south_process, &west_process, &east_process, sqrt(size), rank, size);
		
		//printf("Proccess %d: %d %d %d %d\n", rank, north_process, south_process, east_process, west_process);
		
		int neighbours[NUM_NEIGHBOURS] = {north_process, south_process, west_process, east_process};

		/*
		printf("Process %d: ", rank);
		for (int i = 0; i < NUM_NEIGHBOURS; i++) {
			printf("%d ",neighbours[i]);
		}
		printf("\n");
		*/
		final_min_number = search_min_number(neighbours, size, buf);
		
		//printf("Process %d number: %.2f\n",rank, final_min_number);
	}

	if (rank == 0) {
		printf("\n[Process %d] The mininum number is: %.2f\n\n", rank, final_min_number);
	}

	MPI_Finalize();
	return 0;
}

/* Search a min number of column/row */
float search_min_number(int neighbours[], int size, float send_number) {
	
	MPI_Request request1, request2;
	MPI_Status status;
	int cnt = 0;
	float recv_number;
	
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i <= sqrt(size); i++) {
			MPI_Irecv(&recv_number, 1, MPI_FLOAT, neighbours[cnt+1], MPI_ANY_TAG, MPI_COMM_WORLD, &request1);
			MPI_Isend(&send_number, 1, MPI_FLOAT, neighbours[cnt], i, MPI_COMM_WORLD, &request2);
			MPI_Wait(&request1, &status);
			
			send_number = compare_numbers(send_number, recv_number);
			recv_number = send_number;
		
		}
		cnt+=2;
	}

	return recv_number;
}

/* Function to compare which is the smaller number */
float compare_numbers(float number1, float number2) {

	if (number1 < number2) {
		return number1;
	} else {
		return number2;
	}
}

/* Calculate the module of the divison */
int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

/* Assign neighours of rows to processes */
void assign_neighbours(int *north_process, int *south_process, int *west_process,
							int *east_process, int l, int rank, int size) {
	int row, number_position, j = 0;
	int row_numbers[l];
	row = rank/l;

	for (int i = row*l; i<=((row*l)+l-1); i++) {
		if (rank == i) {
			number_position = j;
		}
		row_numbers[j++] = i;
	}

	*north_process = mod(rank-l, size);
	*south_process = mod(rank+l, size);
	*west_process = row_numbers[mod(number_position-1, l)];
	*east_process = row_numbers[mod(number_position+1, l)];
}