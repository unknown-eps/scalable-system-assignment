#!/bin/bash
array_size=1000000
for i in {1..20}
do
    sbatch job-submit.sh $array_size
    sleep 5
    while squeue -u $USER | grep -q job-submit.sh; do
        sleep 10
    done
done
