#!/bin/bash
#SBATCH --job-name test
#SBATCH --nodelist=node[8]
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=8
#SBATCH --output /home/harshitrawat/parallel/mpi/output/%j.out
module load openmpi
make
INPUT_SIZE=$1
mpiexec ./mpi_main $INPUT_SIZE