/* Pract2  RAP 09/10    Javier Ayllon*/

#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h> 
#include <assert.h>   
#include <unistd.h>   
#define NIL (0)       
#define NUM_WORKERS_PROCESS 5
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
      int buf[5];
      int errcodes[NUM_WORKERS_PROCESS];

      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_get_parent( &commPadre );

      if ( (commPadre==MPI_COMM_NULL)
            && (rank==0) )  {
            MPI_Comm_spawn("exec/pract2", MPI_ARGV_NULL, NUM_WORKERS_PROCESS, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, errcodes);
	      initX();
            printf("Soy el padre %d\n", rank);
	      /* Codigo del maestro */

            /*for (int i = 0; i < 400; i++) {
                  MPI_Recv();
                  dibujaPunto(buf[0], buf[1], buf[2], buf[3], buf[4]);
            }*/

	      /*En algun momento dibujamos puntos en la ventana algo como
	      dibujaPunto(x,y,r,g,b);  */

            //}

      }

      else {
            MPI_Info info;
            MPI_File fh;
            MPI_Datatype arraytype;
            MPI_Offset disp;
            MPI_Status status;
            
            printf("Hijo %d\n", rank);
            //MPI_Comm_get_parent(&intercomm);
            /* Codigo de todos los trabajadores */
            /* El archivo sobre el que debemos trabajar es foto.dat */

            MPI_File_open(MPI_COMM_WORLD, "data/foto.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
            MPI_File_set_view(fh, disp, MPI_UNSIGNED_CHAR, arraytype, "native", MPI_INFO_NULL);
            MPI_File_read(fh, buf, 5, MPI_UNSIGNED_CHAR, &status);

            printf("%s\n",buf);
            //MPI_Send();
      }

      MPI_Finalize();
}

