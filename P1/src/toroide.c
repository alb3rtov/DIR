#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

#define BUFFER 2014

FILE* open_file(char filename[], char *permissions);
void read_assign_values(FILE *file, int size);

/* Main function */
int main(int argc, char **argv) {
    
	int rank, size;
	float buf;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		double value = sqrt(size);
		int truncated = (int) value;

		/*Check if un toroidal network can be possible*/
		if (truncated == value) {
			FILE* file = open_file("datos.dat","r");
			read_assign_values(file, size);
		} else {
			fprintf(stderr, "Error, can't create toroidal network\n");
			
		}

	} else {
		MPI_Recv(&buf, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		printf("Process %d receive: %.2f\n",rank,buf);
	}

	MPI_Finalize();
	return 0;
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
	
	while ((token != NULL) && (size != 0)) {
		size--;
		buf = atof(token);
		MPI_Send(&buf, 1, MPI_FLOAT, size, 0, MPI_COMM_WORLD);
		token = strtok(NULL, ",");
	}

	fclose(file);
}