#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

#define BUFFER 2014
#define true 1
#define false 0

/* Open a file with a given permissions and returns a descriptor */
FILE* open_file(char filename[], char *permissions) {
	FILE *file;
	file = fopen(filename, permissions);

	if (file == NULL) {
		fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return file;
}

/* Sends to process != 0 if the topology is correct */
void send_network_topology_confirmation(int network_topology, int size, MPI_Request request) {
    for (int i = 1; i < size; i++) {
        MPI_Isend(&network_topology, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
    }
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