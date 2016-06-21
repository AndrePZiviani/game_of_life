CFLAGS =-Wall -O3 

all: executable

debug: CFLAGS += -DDEBUG -g
debug: executable

executable:
	gcc $(CFLAGS) -std=c99 -o gol gol-serial.c 

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf gol

outclean:
	rm -rf *.out
