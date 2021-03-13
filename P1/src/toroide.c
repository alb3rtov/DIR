#include <stdio.h>
#include <math.h>
#include "mpi.h"

int main(int argc, char **argv) {
    
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		double value = sqrt(size);
		int truncated = (int) value;
		
		/*Check if un toroidal network can be possible*/
		if (truncated == value) {
			printf("Si\n");
		} else {
			printf("No\n");
		}

	} else {
		printf("Soy de los otros.\n");
	}

	MPI_Finalize();
	return 0;
}

