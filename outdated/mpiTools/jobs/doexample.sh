#!/bin/bash
qsub run_boss_worker_mpi.job -N example -l nodes=4:ppn=8 -v JOBFILE="$HOME/jobs/jobs_example.in",MPIVERSION="1" -j oe -o $HOME/jobs/output/
