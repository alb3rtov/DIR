DIRSRC = src/
DIREXE = exec/

CC := mpicc

all: dirs toroide

dirs:
		mkdir -p $(DIREXE)

hipercubo: $(CC) $(DIRSRC)hipercubo.c -o $(DIREXE)hipercubo

toroide: $(DIRSRC)toroide.c
		$(CC) -o $(DIREXE)$@ $^

testhipercubo: 
			mpirun -n 4 ./$(DIREXE)hipercubo

testtoroide: 
			mpirun -n 4 ./$(DIREXE)toroide

clean:
	rm -rf *~ core $(DIREXE)