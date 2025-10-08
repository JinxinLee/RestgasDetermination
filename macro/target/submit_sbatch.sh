#!/bin/bash
#SBATCH --job-name=pnd_sim
#SBATCH --partition=main
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --array=1-1
#SBATCH --singularity-container=/cvmfs/vae.gsi.de/vae23/containers/user_container-develop.sif

# --- Dynamic Path Setup ---
# Get the directory where this script is located
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
echo "Script directory is: $SCRIPT_DIR"

# Set the working directory to the script's location
cd "$SCRIPT_DIR"
echo "Working directory changed to: $(pwd)"

# Define log paths relative to the script directory
LOG_DIR="$SCRIPT_DIR/slurmlog"
mkdir -p "$LOG_DIR" # Ensure the log directory exists
#SBATCH --output=slurmlog/pnd_sim_%A_%a.log
#SBATCH --error=slurmlog/pnd_sim_%A_%a.err
# --------------------------

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

# Setup PandaRoot environment using a path relative to the script's location
# Assumes the build directory is at ../../build relative to macro/target
source "$SCRIPT_DIR/../../build/config.sh" -p 

#export ROOT_INCLUDE_PATH=/cvmfs/fairsoft.gsi.de/debian10/fairsoft/jun19p2/include/TGeant3:/cvmfs/fairsoft.gsi.de/debian10/fairroot/v18.2.1_fs_jun19p2/include:${ROOT_INCLUDE_PATH}

echo "--- JOB ENVIRONMENT AFTER FIX ---"
echo "ROOT_INCLUDE_PATH is now: $ROOT_INCLUDE_PATH"
echo "SIMPATH is "$SIMPATH
echo "FAIRROOTPATH is "$FAIRROOTPATH
echo "---------------------------------"

# Run the python orchestrator with the provided config file and mode
echo "Running python script..."
python3 runall_sbatch.py "$CONFIG_FILE" --mode "$MODE"

echo "--- Slurm Job Finished ---"

