# Compile and Execute Repository Code.

# Serial execution with GNU.
- Compile:  
```
gcc source_code.c mmio.c -o a.out
```
- Execute:  
```
time ./a.out nameofMTXmatrix.mtx
```

# Parallel execution with OpenMP.
- Compile for MacOS:  
```
clang -Xpreprocessor -fopenmp -O3 V3_OpenMP.c mmio.c -lomp
```
- Execute for MacOS:  
```
time ./a.out nameofMTXmatrix.mtx
```

# Parallel execution with OpenCilk.
- Compile for MacOS:  
```
xcrun userpath/clang -fcilkplus source_code.c mmio.c -o a.out
```
- Execute for MacOS:  
```
time ./a.out nameofMTXmatrix.mtx
```

In order to connect at a HPC cluster and execute your code, you have to create a bash.sh file.  
In the following line a template of a bash.sh file is provided.

```bash
#!/bin/bash
#SBATCH --partition=batch       # partition of job submission
#SBATCH --ntasks=20             # number of CPUs per Node
#SBATCH --nodes=1               # number of nodes you would like to use(1 for OpenMP due to its architecture)  
#SBATCH --time=12:00:00         # Wall time

gcc -fopenmp -O3 V3_OpenMP.c mmio.c -o a.out

export OMP_NUM_THREADS=$SLURM_NTASKS
time ./a.out youtube.mtx
```