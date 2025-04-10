#!/bin/bash
OUTPUT_DIR="/home/harshitrawat/parallel/mpi/output"
if [ -d "$OUTPUT_DIR" ]; then
    rm -f "$OUTPUT_DIR"/*
    echo "All files in $OUTPUT_DIR have been removed."
else
    echo "Directory $OUTPUT_DIR does not exist."
fi
rm ./*.nfs*