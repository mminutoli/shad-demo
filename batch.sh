#!/bin/bash

#SBATCH --job-name=jobname      # Job name
#SBATCH --output=jobname.%j.out # Stdout (%j expands to jobId)
#SBATCH --error=jobname.%j.err  # Stderr (%j expands to jobId)
#SBATCH --nodes=8               # Number of nodes requested
#SBATCH --cpu-freq=Performance  # Set CPU governor
#SBATCH --time=12:00:00         # walltime
#SBATCH --partition=compute     # Partition

## LOAD MODULES ##
module purge
module load gcc/7.1.0
module load openmpi

mpiexec -n 8 --npernode 1 --mca mpi_cuda_support 0 $1 $2 --gmt_num_workers 17 --gmt_num_helpers 2 --gmt_mtasks_per_queue 8388608 --gmt_mtasks_res_block_rem 1048576
