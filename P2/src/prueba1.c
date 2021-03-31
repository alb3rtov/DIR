#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int myrank, numprocs, bufsize, *buf, count;
    MPI_File thefile;
    MPI_Status status;
    MPI_Offset filesize;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    
    MPI_File_open(MPI_COMM_WORLD, "data/prueba.txt", MPI_MODE_RDONLY, MPI_INFO_NULL, &thefile);
    MPI_File_get_size(thefile, &filesize);

    filesize = filesize / sizeof(int);
    //printf("filesize para cada proceso: %d", filesize);
    bufsize = filesize / numprocs;
    //printf("bufsize: %d\n", bufsize);
    buf = (int *) malloc(bufsize * sizeof(int));
    
    MPI_File_set_view(thefile, myrank * bufsize * sizeof(int), MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
    MPI_File_read(thefile, buf, bufsize, MPI_INT, &status);
    MPI_Get_count(&status, MPI_INT, &count);
    
    //printf("process %d read %d ints\n", myrank, count);
    if (myrank == 0) {
        printf("%d\n",buf[1]);
    }
    MPI_File_close(&thefile);
    MPI_Finalize();
    return 0;
}