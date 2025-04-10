#!/bin/bash
#SBATCH --job-name demo_openmp
#SBATCH --tasks-per-node 32
#SBATCH --nodelist node8
#SBATCH --output /home/harshitrawat/parallel/openmp/output/job-%j.out
#SBATCH --open-mode=append
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <number_of_threads> <size_of_array>"
    exit 1
fi
NUM_THREADS=$1
ARRAY_SIZE=$2
echo "Number of threads: $NUM_THREADS"
cd $SLURM_SUBMIT_DIR
./psum $NUM_THREADS $ARRAY_SIZE
