#!/bin/bash
rm init_out.txt
for i in 1 2 4 8 16 32 64;
do
	echo "gcc -O3 -fopenmp initialize.c -D INIT_CHUNK=$i -std=c99"
	gcc -O3 -fopenmp initialize.c -D INIT_CHUNK=$i -std=c99	-o gol
	printf "chunk size: $i\n" >> init_out.txt
	./gol 1024 1024 1000 560 >> init_out.txt
done
