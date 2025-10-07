#!/bin/bash
#SBATCH --job-name=pnd_sim
#SBATCH --partition=main
#SBATCH --output=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.log
#SBATCH --error=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --array=1-1
#SBATCH --singularity-container=/cvmfs/vae.gsi.de/vae23/containers/user_container-develop.sif

# Check for required arguments
if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Error: Missing arguments. Usage: $0 <config_file> <mode>"
    exit 1
fi

CONFIG_FILE=$1
MODE=$2

echo "--- Starting Slurm Job ---"
echo "Job ID: $SLURM_JOB_ID"
echo "Array Task ID: $SLURM_ARRAY_TASK_ID"
echo "Config File: $CONFIG_FILE"
echo "Mode: $MODE"
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

# Navigate to the macro directory
cd /lustre/panda/jili/oct19/macro/target

# Run the python orchestrator with the provided config file and mode
echo "Running python script..."
python3 runall_sbatch.py "$CONFIG_FILE" --mode "$MODE"

echo "--- Slurm Job Finished ---"

