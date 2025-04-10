#!/bin/bash
NUM_PROCESSORS_LIST=(2 4 8 16 32 64)
# SIZE_OF_ARRAY_LIST=(100000000)

SIZE_OF_ARRAY_LIST=(10000000 20000000 30000000 40000000 50000000)
for i in {1..5}; do
    for NUM_PROCESSORS in "${NUM_PROCESSORS_LIST[@]}"; do
        for SIZE_OF_ARRAY in "${SIZE_OF_ARRAY_LIST[@]}"; do
            sbatch job-submit.sh $NUM_PROCESSORS $SIZE_OF_ARRAY
        done
    done
done