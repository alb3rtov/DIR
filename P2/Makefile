DIRSRC = src/
DIREXE = exec/
DIRHEA := include/

CC := mpicc
LDLIBS := -lX11

all: dirs pract2

dirs:
		mkdir -p $(DIREXE)

pract2: $(DIRSRC)pract2.c
		$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

run_pract2: 
			mpirun -oversubscribe -n 1 ./$(DIREXE)pract2

clean:
	rm -rf *~ core $(DIREXE)