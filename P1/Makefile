DIRSRC = src/
DIREXE = exec/
DIRHEA := include/

CFLAGS := -I$(DIRHEA)
CC := mpicc
LDLIBS := -lm

all: dirs toroide hipercubo

dirs:
		mkdir -p $(DIREXE)

hipercubo: $(DIRSRC)hipercubo.c
		$(CC) -o $(DIREXE)$@ $^ $(LDLIBS) $(CFLAGS)

toroide: $(DIRSRC)toroide.c
		$(CC) -o $(DIREXE)$@ $^ $(LDLIBS) $(CFLAGS)

run_hipercubo: 
			mpirun -oversubscribe -n 4 ./$(DIREXE)hipercubo

run_toroide: 
			mpirun -oversubscribe -n 9 ./$(DIREXE)toroide

clean:
	rm -rf *~ core $(DIREXE)