CFLAGS =-Wall -O3 
MPICC = mpicc
CC = mpicc

all: executable

debug: CFLAGS += -DDEBUG -g
debug: executable

executable:
	$(MPICC) $(CFLAGS) -std=c99 -o gol gol.c 

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf gol

outclean:
	rm -rf *.out
