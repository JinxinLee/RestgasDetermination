#!/bin/bash
#SBATCH --job-name=pnd_sim
#SBATCH --output=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.log
#SBATCH --error=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --array=1-1
#SBATCH --singularity-container=/cvmfs/vae.gsi.de/vae23/containers/user_container-develop.sif

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

# Run the python orchestrator
# The script will use the settings from a default config.json if no path is provided
python3 runall_prod_hvmaps.py configs/
