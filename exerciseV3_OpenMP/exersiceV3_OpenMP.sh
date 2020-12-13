#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks=20
#SBATCH --nodes=1
#SBATCH --time=12:00:00

gcc -fopenmp -O3 V3_OpenMP.c mmio.c -o a.out

export OMP_NUM_THREADS=$SLURM_NTASKS
time ./a.out youtube.mtx
