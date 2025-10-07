#!/bin/bash
#SBATCH --job-name=pnd_sim
#SBATCH --partition=long
#SBATCH --time=08:00:00
#SBATCH --output=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.log
#SBATCH --error=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=2  # <--apply cores for parallel jobs
#SBATCH --mem=4G
#SBATCH --array=1-1
#SBATCH --singularity-container=/cvmfs/vae.gsi.de/vae23/containers/user_container-develop.sif

# Setup PandaRoot environment
# (You must source your environment script here)
source /lustre/panda/jili/oct19/build/config.sh -p 

echo "--- JOB ENVIRONMENT AFTER FIX ---"
echo "ROOT_INCLUDE_PATH is now: $ROOT_INCLUDE_PATH"
echo "SIMPATH is "$SIMPATH
echo "FAIRROOTPATH is "$FAIRROOTPATH
echo "---------------------------------"

# Navigate to the macro directory
cd /lustre/panda/jili/oct19/macro/target

# Run the python orchestrator with the -u flag for real-time output
# and -j2 for 2 parallel jobs, matching the --cpus-per-task=2 request.
python3 -u runall_prod_hvmaps.py configs/ -j 2
