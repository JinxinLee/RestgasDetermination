#!/bin/bash
#SBATCH --job-name=pnd_sim
#SBATCH --partition=long
#SBATCH --time=20:00:00
#SBATCH --output=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.log
#SBATCH --error=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=16G
#SBATCH --array=1-1
#SBATCH --singularity-container=/cvmfs/vae.gsi.de/vae23/containers/user_container-develop.sif

# Check if at least one argument is provided
if [ -z "$1" ]; then
    echo "Error: No command provided to execute."
    exit 1
fi

echo "--- Starting Slurm Job ---"
echo "Job ID: $SLURM_JOB_ID"
echo "Array Task ID: $SLURM_ARRAY_TASK_ID"
echo "Executing command: $@"
echo "--------------------------"

# Setup PandaRoot environment
# (You must source your environment script here)
source /lustre/panda/jili/oct19/build/config.sh -p 

#export ROOT_INCLUDE_PATH=/cvmfs/fairsoft.gsi.de/debian10/fairsoft/jun19p2/include/TGeant3:/cvmfs/fairsoft.gsi.de/debian10/fairroot/v18.2.1_fs_jun19p2/include:${ROOT_INCLUDE_PATH}

echo "--- JOB ENVIRONMENT AFTER FIX ---"
echo "ROOT_INCLUDE_PATH is now: $ROOT_INCLUDE_PATH"
echo "SIMPATH is "$SIMPATH
echo "FAIRROOTPATH is "$FAIRROOTPATH
echo "---------------------------------"

# Navigate to the directory of this script to ensure relative paths in python work
#SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
#cd "$SCRIPT_DIR"
#echo "SCRIPT_DIR is"$SCRIPT_DIR
cd /lustre/panda/jili/oct19/macro/target

# Run the command passed as arguments to this script
echo "Running python script..."
"$@"

echo "--- Slurm Job Finished ---"
