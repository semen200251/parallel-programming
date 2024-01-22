#!/bin/bash
#SBATCH -J c_mpi
#SBATCH -N 3
#SBATCH -p tornado
#SBATCH -o ./out_nodes
#SBATCH -e ./err
#SBATCH -t 00:01:00

mpicc datasets/MPIC/MPI.c -lm -o out
mpirun -n 30 ./out