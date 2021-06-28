#!/bin/bash

export OMP_NUM_THREADS=4
gcc -fopenmp grep.c -o grep
