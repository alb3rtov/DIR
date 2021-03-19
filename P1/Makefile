DIRSRC = src/
DIREXE = exec/

CC := mpicc
LDLIBS := -lm

all: dirs toroide

dirs:
		mkdir -p $(DIREXE)

hipercubo: $(CC) $(DIRSRC)hipercubo.c -o $(DIREXE)hipercubo

toroide: $(DIRSRC)toroide.c
		$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

run_hipercubo: 
			mpirun -oversubscribe -n 9 ./$(DIREXE)hipercubo

run_toroide: 
			mpirun -oversubscribe -n 9 ./$(DIREXE)toroide

clean:
	rm -rf *~ core $(DIREXE)