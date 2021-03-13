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

testhipercubo: 
			mpirun -n 3 ./$(DIREXE)hipercubo

testtoroide: 
			mpirun -n 3 ./$(DIREXE)toroide

clean:
	rm -rf *~ core $(DIREXE)