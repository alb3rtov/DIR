#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <definitions.h>

#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

void read_assign_values(FILE *file, int size);
float compare_numbers(float number1, float number2);
void assign_neighbours(int *north_process, int *south_process, int *west_process,
							int *east_process, int l, int rank, int size);
int mod(int a, int b);
float search_min_number(int f_process, int s_process, int size, float send_number);

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
		float min_number_column;
		int north_process, south_process, west_process, east_process;

		/* Receive the number of the list */
		MPI_Recv(&buf, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		assign_neighbours(&north_process, &south_process, &west_process, &east_process, sqrt(size), rank, size);
		min_number_column = search_min_number(north_process, south_process, size, buf);
		final_min_number = search_min_number(west_process, east_process, size, min_number_column);

		//printf(" - [%d (%.2f)] Min number: %.2f\n", rank, buf, final_min_number);
	}

	if (rank == 0) {
		printf("\n[Process %d] The mininum number is: %.2f\n\n", rank, final_min_number);
	}

	MPI_Finalize();
	return 0;
}

/* Search a min number of column/row */
float search_min_number(int f_process, int s_process, int size, float send_number) {
	
	MPI_Request request1;
	MPI_Request request2;
	MPI_Status status;

	float recv_number;
	int received = false;
	int flag = 0;

	for (int i = 1; i <= sqrt(size) - 1; i++){
		MPI_Irecv(&recv_number, 1, MPI_FLOAT, f_process, MPI_ANY_TAG, MPI_COMM_WORLD, &request1);
		MPI_Isend(&send_number, 1, MPI_FLOAT, s_process, i, MPI_COMM_WORLD, &request2);
		MPI_Wait(&request1, &status);

		send_number = compare_numbers(send_number, recv_number);
		recv_number = send_number;
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

/* Read and assign numbers to child processes*/
void read_assign_values(FILE *file, int size) {
	char buffer[BUFFER];
	float buf;

	fgets(buffer, BUFFER, file);
	char * token = strtok(buffer, ",");
	
	while ((token != NULL) && (size > 0)) {
		size--;
		buf = atof(token);
		MPI_Bsend(&buf, 1, MPI_FLOAT, size, 0, MPI_COMM_WORLD);
		token = strtok(NULL, ",");
	}
	fclose(file);
}