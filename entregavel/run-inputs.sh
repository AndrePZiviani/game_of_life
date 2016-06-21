#!/bin/bash
printf "      N    M    seed    output file\n"
printf "./gol 2048 2048 500     teste-0.out\n"
./gol 512 512 4000 teste-0.out
cat stats.out
printf "\n./gol 2048 2048 32400   teste-1.out\n"
./gol 512 512 8000 teste-1.out
cat stats.out
printf "\n./gol 4096 4096 500     teste-2.out\n"
./gol 1024 512 4000 teste-2.out
cat stats.out
printf "\n./gol 4096 4096 32400   teste-3.out\n"
./gol 1024 512 8000 teste-3.out
cat stats.out
printf "\n./gol 8192 8192 500     teste-4.out\n"
./gol 1024 1024 4000 teste-4.out
cat stats.out
printf "\n./gol 8192 8192 32400   teste-5.out\n"
./gol 1024 1024 8000 teste-5.out
cat stats.out
printf "\n"
