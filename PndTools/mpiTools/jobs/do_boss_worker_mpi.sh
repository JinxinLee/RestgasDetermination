#!/bin/bash
qsub run_boss_worker_mpi.job -q long -N BossWorkerJobs -l nodes=20:ppn=2 -v JOBFILE="$HOME/jobs/jobs_emcboxgen.in",SEED=0 -j oe -o $HOME/jobs/output/
