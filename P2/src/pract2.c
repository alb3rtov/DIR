#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>

#include "filters_values.h"

#define NIL (0)
#define NUM_WORKERS_PROCESS 3
#define FILENAME "data/foto.dat"

/* Global variables */
XColor colorX;
Colormap mapacolor;
char cadenaColor[]="#000000";
Display *dpy;
Window w;
GC gc;

/* Auxiliar functions */
void initX() {
      dpy = XOpenDisplay(NIL);
      assert(dpy);

      int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
      int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

      w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
                                     400, 400, 0, blackColor, blackColor);
      XSelectInput(dpy, w, StructureNotifyMask);
      XMapWindow(dpy, w);
      gc = XCreateGC(dpy, w, 0, NIL);
      XSetForeground(dpy, gc, whiteColor);
      for(;;) {
            XEvent e;
            XNextEvent(dpy, &e);
            if (e.type == MapNotify)
                  break;
      }
      mapacolor = DefaultColormap(dpy, 0);
}

/* Draw a pixel into de window */
void dibujaPunto(int x,int y, int r, int g, int b) {
      sprintf(cadenaColor,"#%.2X%.2X%.2X",r,g,b);
      XParseColor(dpy, mapacolor, cadenaColor, &colorX);
      XAllocColor(dpy, mapacolor, &colorX);
      XSetForeground(dpy, gc, colorX.pixel);
      XDrawPoint(dpy, w, gc,x,y);
      XFlush(dpy);
}

/* Select filter */
int get_num_filter() {
      int num_filter = 0;
      
      printf("\nFiltros:\n");
      printf("- (1) Sin filtro\n");
      printf("- (2) Blanco y negro\n");
      printf("- (3) Sepia\n");
      printf("- (4) Color invertido\n");
      printf("Introduzca un número de filtro: \n");
      scanf("%d", &num_filter);

      return num_filter;
}

/* Normal image, without filter */
void set_no_filter(int *buffer, unsigned char *buf, int cnt) {
      buffer[2] = buf[cnt];
      buffer[3] = buf[cnt+1];
      buffer[4] = buf[cnt+2];
}

/* Set grayscale filter for each pixel */
void set_bw_filter(int *buffer, unsigned char *buf, int cnt) {
      buffer[2] = buf[cnt]*BLACK_WHITE_VALUE + buf[cnt+1]*BLACK_WHITE_VALUE + buf[cnt+2]*BLACK_WHITE_VALUE;
      buffer[3] = buf[cnt]*BLACK_WHITE_VALUE + buf[cnt+1]*BLACK_WHITE_VALUE + buf[cnt+2]*BLACK_WHITE_VALUE;
      buffer[4] = buf[cnt]*BLACK_WHITE_VALUE + buf[cnt+1]*BLACK_WHITE_VALUE + buf[cnt+2]*BLACK_WHITE_VALUE;
}

/* Set sepia filter for each pixel */
void set_sepia_filter(int *buffer, unsigned char *buf, int cnt) {

      buffer[2] = buf[cnt]*RED_SEPIA_R + buf[cnt+1]*RED_SEPIA_G + buf[cnt+2]*RED_SEPIE_B; /* Red */
      if (buffer[2] > 255) {
            buffer[2] = 255;
      }

      buffer[3] = buf[cnt]*GREEN_SEPIA_R + buf[cnt+1]*GREEN_SEPIA_G + buf[cnt+2]*GREEN_SEPIA_B; /* Green */
      if (buffer[3] > 255) {
            buffer[3] = 255;
      }

      buffer[4] = buf[cnt]*BLUE_SEPIA_R + buf[cnt+1]*BLUE_SEPIA_G + buf[cnt+2]*BLUE_SEPIA_B; /* Blue */
      if (buffer[4] > 255) {
            buffer[4] = 255;
      }
}

/* Set inverted colors filter */
void set_inverted_filter(int *buffer, unsigned char *buf, int cnt) {
      buffer[2] = 255-buf[cnt];
      buffer[3] = 255-buf[cnt+1];
      buffer[4] = 255-buf[cnt+2];
}

/* Switch that calls the filter selected */
void select_filter(int *buffer, unsigned char *buf, int cnt, int num_filter) {
      switch (num_filter) {
      case 1:
            set_no_filter(buffer, buf, cnt);
            break;
      case 2:
            set_bw_filter(buffer, buf, cnt);
            break;
      case 3:
            set_sepia_filter(buffer, buf, cnt);
            break;
      case 4:
            set_inverted_filter(buffer, buf, cnt);
            break;
      default:
            set_no_filter(buffer, buf, cnt); /* No filter by default */
            break;
      }
}

int check_pixels_division(int bufsize) {
      int truncanted;
      double result;

      result = (double) bufsize/(3*400);
      truncanted = (int) result;
      //printf("%d %f\n",truncanted, result);
      while (result != truncanted) {
            
            bufsize--;
            result = (double) bufsize/(3*400);
            truncanted = (int) result;
            //printf("%d\n",bufsize);
      }

      return bufsize;
}

/* Main function */
int main (int argc, char *argv[]) {

      int rank, size, tag, num_filter;
      MPI_Comm commPadre, intercomm;
      MPI_Status status;
      int buffer[5];
      int errcodes[NUM_WORKERS_PROCESS];
      
      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_get_parent( &commPadre );

      if ((commPadre==MPI_COMM_NULL)
            && (rank==0) )  {
            
            MPI_Comm_spawn("exec/pract2", MPI_ARGV_NULL, NUM_WORKERS_PROCESS, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, errcodes);
	      
            /* Set and send the number of filter */
            num_filter = get_num_filter();
            for (int i = 0; i < NUM_WORKERS_PROCESS; i++) {
                  MPI_Send(&num_filter, 1, MPI_INT, i, i, intercomm);  
            }
            
            initX();
            
            for (int i = 0; i < 160000; i++) {
                  MPI_Recv(&buffer, 5, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, intercomm, &status);
                  dibujaPunto(buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
            }
            
            printf("Presiona una tecla para continuar...\n");
            getchar();getchar();
      }

      else {
            int bufsize, nrchar, cnt = 0;
            unsigned char *buf;  /* Buffer for reading */
            MPI_Offset filesize;
            MPI_File myfile;    /* Shared file */
            MPI_Status status;  /* Status returned from read */
            MPI_Request request;
            
            MPI_Recv(&num_filter, 1, MPI_INT, 0, MPI_ANY_TAG, commPadre, &status);
            MPI_Comm_get_parent( &commPadre );

            MPI_File_open (MPI_COMM_WORLD, FILENAME, MPI_MODE_RDONLY, MPI_INFO_NULL, &myfile); /* Open the file */
            MPI_File_get_size(myfile, &filesize);  /* Get the size of the file */
            
            filesize = filesize/sizeof(unsigned char); /* Calculate how many elements that is */
            bufsize = filesize/NUM_WORKERS_PROCESS; /* Calculate how many elements each processor gets */

            bufsize = check_pixels_division(bufsize);

            /*int diff = 0;
            if (rank == NUM_WORKERS_PROCESS) {
                  if (bufsize*NUM_WORKERS_PROCESS != filesize) {
                        diff = filesize - (bufsize*NUM_WORKERS_PROCESS);
                        bufsize = bufsize + diff;
                  }
                  //printf("%d - %d\n",diff, bufsize);
            }*/

          
            

            buf = (unsigned char *) malloc((bufsize+1)*sizeof(unsigned char)); /* Allocate the buffer to read to, one extra for terminating null char */
            //printf("%d (%d)\n",bufsize,filesize);
            MPI_File_set_view(myfile, rank*bufsize*sizeof(unsigned char), MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, 
                        "native", MPI_INFO_NULL); /* Set the file view */   
            MPI_File_read(myfile, buf, bufsize, MPI_UNSIGNED_CHAR, &status); /* Read from the file */
            MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &nrchar); /* Find out how many elemyidnts were read */
            
            buf[nrchar] = (unsigned char)0; /* Set terminating null char in the string */
            MPI_File_close(&myfile); /* Close the file */
            //printf("[%d] %d - %d (%d)\n",rank,nrchar,bufsize,filesize);
            for (int y = (bufsize*rank)/(3*400); y < ((bufsize*rank)/(3*400))+(bufsize/(3*400)) ; y++) {
                  for (int x = 0; x < 400; x++) {
                        buffer[0] = x;
                        buffer[1] = y;   
                        select_filter(buffer, buf, cnt, num_filter);
                        MPI_Send(&buffer, 5, MPI_INT, 0, x*y, commPadre);                    
                        cnt+=3;
                  }
            }
      }

      MPI_Finalize();
}

