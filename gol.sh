#!/bin/bash
#SBATCH -p 7d
#SBATCH -n 144
#SBATCH -t 1-00:00:01

source /etc/profile
module load intel/5.0.3.048
#module load gcc/4.9 mvapich2/2.2

#cat /proc/self/status
srun  /home/apz11/gol/gol 10000 10000 12 12 1000000

exit 0

