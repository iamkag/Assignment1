#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --time=12:00:00

gcc V3_woParallel.c mmio.c -o a.out

time ./a.out vt2010.mtx
