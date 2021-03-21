#include "/usr/lib/x86_64-linux-gnu/openmpi/include/mpi.h"

#define BUFFER 2014
#define true 1
#define false 0

FILE* open_file(char filename[], char *permissions) {
	FILE *file;
	file = fopen(filename, permissions);

	if (file == NULL) {
		fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return file;
}

void send_network_topology_confirmation(int network_topology, int size, MPI_Request request) {
    for (int i = 1; i < size; i++) {
        MPI_Isend(&network_topology, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
    }
}