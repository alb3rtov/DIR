#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

#define BUFFER 2014
#define true 1
#define false 0

FILE* open_file(char filename[], char *permissions);
void read_assign_values(FILE *file, int size);
int compareNumbers(double buf, double number);
void send_toroidal_confirmation(int toroidal, int size, MPI_Request request);

/* Main function */
int main(int argc, char **argv) {

	int rank, size, truncated, toroidal = false;
	float buf;

	MPI_Status status;
	MPI_Request request;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		double value = sqrt(size);
		truncated = (int) value;

		/*Check if un toroidal network can be possible*/
		if (truncated == value) {
			toroidal = true;
			send_toroidal_confirmation(toroidal, size, request);

			FILE* file = open_file("datos.dat","r");
			read_assign_values(file, size);

		} else {
			toroidal = false;
			send_toroidal_confirmation(toroidal, size, request);
			fprintf(stderr, "Error, can't create toroidal network: %d processes\n", size);
		}

	} else {
		MPI_Recv(&toroidal, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}

	if (toroidal == 1) {
		double recv_number;
		double min;
		int north_processor;
		int south_processor;
		int west_processor;
		int east_processor;

		/* Receive the number of the list */
		MPI_Recv(&buf, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		printf("Process %d receive: %.2f\n", rank, buf);

		//assignNeighbour(&north_processor, &south_processor, &west_processor, &east_processor, size, rank);

		/* Loop for North-South path *//*
		for (int i = 1; i < truncated; i++){
			MPI_Irecv(&recv_number, 0, MPI_FLOAT, south_processor, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Bsend(&buf, 1, MPI_FLOAT, north_processor, 0, MPI_COMM_WORLD);
			min = compareNumbers(buf, recv_number);
		}*/

		/* Loop for East-West path *//*
		for (int i = 1; i < truncated; i++){
			MPI_Irecv(&recv_number, 0, MPI_FLOAT, east_processor, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Bsend(&buf, 1, MPI_FLOAT, west_processor, 0, MPI_COMM_WORLD);
			min = compareNumbers(buf, recv_number);
		}*/

	}

	MPI_Finalize();
	return 0;
}

/* Loop to notify process if the network can be toroidal */
void send_toroidal_confirmation(int toroidal, int size, MPI_Request request) {
	for (int i = 1; i < size; i++) {
		MPI_Isend(&toroidal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
	}
}

/* Function to compare which is the smaller number */
int compareNumbers(double buf, double number) {

}

/* Open the file and returns the descriptor */
FILE* open_file(char filename[], char *permissions) {
	FILE *file;
	file = fopen(filename, permissions);

	if (file == NULL) {
	fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
	exit(EXIT_FAILURE);
	}

	return file;
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