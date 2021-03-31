/* Pract2  RAP 09/10    Javier Ayllon*/
#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>

#define NIL (0)
#define NUM_WORKERS_PROCESS 1
#define FILENAME "data/foto.dat"

/*Variables Globales */
XColor colorX;
Colormap mapacolor;
char cadenaColor[]="#000000";
Display *dpy;
Window w;
GC gc;

/*Funciones auxiliares */

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

void dibujaPunto(int x,int y, int r, int g, int b) {

      sprintf(cadenaColor,"#%.2X%.2X%.2X",r,g,b);
      XParseColor(dpy, mapacolor, cadenaColor, &colorX);
      XAllocColor(dpy, mapacolor, &colorX);
      XSetForeground(dpy, gc, colorX.pixel);
      XDrawPoint(dpy, w, gc,x,y);
      XFlush(dpy);

}

/* Programa principal */
int main (int argc, char *argv[]) {

      int rank, size, tag;
      MPI_Comm commPadre, intercomm;
      MPI_Status status;
      int buffer[5];
      int errcodes[NUM_WORKERS_PROCESS];

      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_get_parent( &commPadre );

      if ( (commPadre==MPI_COMM_NULL)
            && (rank==0) )  {
            MPI_Comm_spawn("exec/pract2", MPI_ARGV_NULL, NUM_WORKERS_PROCESS, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, errcodes);
	      initX();
            //printf("Soy el padre %d\n", rank);
	      /* Codigo del maestro */

            /*for (int x = 0; i < 400; x++) {
                  for (int y = 0; i < 400; y++) {
                        MPI_Recv();
                        MPI_Recv(&toroidal, 1, MPI_INT, 0, MPI_ANY_TAG, intercomm, &status);
                        dibujaPunto(x, y, buf[0], buf[1], buf[2]);
                  }
            }*/
            //dibujaPunto(200, 200, 235, 61, 24);
            
            
            for (int i = 0; i < 160000; i++) {
                  MPI_Recv(&buffer, 5, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, intercomm, &status);
                  dibujaPunto(buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
            }
            
            //sleep(5);
	      /*En algun momento dibujamos puntos en la ventana algo como
	      dibujaPunto(x,y,r,g,b);  */

            //}

      }

      else {

            int bufsize, nrchar;
            char *buf;          /* Buffer for reading */
            MPI_Offset filesize;
            MPI_File myfile;    /* Shared file */
            MPI_Status status;  /* Status returned from read */
            MPI_Request request;
            MPI_Comm_get_parent( &commPadre );
            /* Open the file */
            MPI_File_open (MPI_COMM_WORLD, FILENAME, MPI_MODE_RDONLY,
                        MPI_INFO_NULL, &myfile);
            /* Get the size of the file */
            MPI_File_get_size(myfile, &filesize);
            /* Calculate how many elements that is */
            filesize = filesize/sizeof(char);
            /* Calculate how many elements each processor gets */
            bufsize = filesize/NUM_WORKERS_PROCESS;
            /* Allocate the buffer to read to, one extra for terminating null char */
            buf = (char *) malloc((bufsize+1)*sizeof(char));
            /* Set the file view */   
            MPI_File_set_view(myfile, rank*bufsize*sizeof(char), MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, 
                        "native", MPI_INFO_NULL);
            /* Read from the file */
            MPI_File_read(myfile, buf, bufsize, MPI_UNSIGNED_CHAR, &status);
            /* Find out how many elemyidnts were read */
            MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &nrchar);
            /* Set terminating null char in the string */
            buf[nrchar] = (char)0;
            
            //printf("  %c\n", buf[5]);
            //printf("Process %2d read %d characters: ", rank, nrchar);
            /* Close the file */
            MPI_File_close(&myfile);

            int cnt = 0;

            for (int x = 0; x < 400; x++) {
                  for (int y = 0; y < nrchar/(3*400); y++) {
                        buffer[0] = x;
                        buffer[1] = y; 
                        buffer[2] = buf[cnt];
                        buffer[3] = buf[cnt+1];
                        buffer[4] = buf[cnt+2];
                        MPI_Send(&buffer, 5, MPI_INT, 0, x*y, commPadre);
                        cnt++;
                  }
            }

            //MPI_Isend(&buffer, 5, MPI_INT, 0, i, MPI_COMM_WORLD, &request);
            

            /*
            MPI_File fh;
            MPI_Offset filesize;
            MPI_Status status;
            int bufsize, *buffer, k = 0;
            */
            //printf("Hijo %d\n", rank);

            //lines = size_child_comm/400;

            /* Codigo de todos los trabajadores */
            /* El archivo sobre el que debemos trabajar es foto.dat */
            /*
            MPI_File_open(MPI_COMM_WORLD, "data/foto.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
            MPI_File_get_size(fh, &filesize);
            filesize = filesize / sizeof(char);
            bufsize = filesize / NUM_WORKERS_PROCESS;
            printf("%d\n",bufsize);
            buffer = (int *) malloc(bufsize * sizeof(char));
            
            MPI_File_set_view(fh, rank * bufsize * sizeof(char), MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, "native", MPI_INFO_NULL);
            MPI_File_read(fh, buffer, bufsize, MPI_UNSIGNED_CHAR, &status);
            MPI_File_close(&fh);
            */
            //printf("%d %d %d %d %d\n",buffer[0],buffer[1],buffer[2], buf[3], buf[4]);

            /*
            for (int x = 0; x < 400; x++) {
                  for (int y = 0; y < 400/NUM_WORKERS_PROCESS; y++) {
                        buf[0] = x;
                        buf[1] = y;

                        for (int i = 0; i < 3; i++) {
                              buf[cnt++] = buffer[k++];
                        }
                        cnt = 0;
                        
                        MPI_Send(buf,);
                  }
            }
            */
      }

      MPI_Finalize();
}

