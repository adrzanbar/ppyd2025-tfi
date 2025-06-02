#!/bin/bash

# Matrix sizes (square matrices)
sizes=(1000 2000 3000 4000 8000)
# Number of processes
procs=(2 4 6 8 10 12 14 16)
# Number of runs per experiment
n=3

# Path to the parallel executable
EXEC="./parallel"

for size in "${sizes[@]}"; do
    for p in "${procs[@]}"; do
        echo "Running: size=${size}x${size}, procs=${p}, runs=${n}"
        mpirun -np $p $EXEC $size $size $size $size $n
    done
done