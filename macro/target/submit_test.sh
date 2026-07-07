#!/bin/bash

# ==============================================================================
# --- LAUNCHER SCRIPT ---
#
# USAGE: bash submit.sh
#
# This script's ONLY job is to count the config files and submit the
# 'slurm_master_script.sh' as a job array to Slurm.
# ==============================================================================

# --- Configuration ---
CONFIG_DIR="configs/"

echo "--- LAUNCHER MODE ---"

# Find and count config files
CONFIG_FILES=( "$CONFIG_DIR"/*.json )
N_CONFIGS=${#CONFIG_FILES[@]}

if [ "$N_CONFIGS" -eq 0 ]; then
    echo "Error: No config files (.json) found in directory '$CONFIG_DIR'" >&2
    exit 1
fi

echo "Found $N_CONFIGS configuration files to process."
echo "Submitting 'slurm_master_script.sh' as a master job array to SLURM..."

# Submit the master script as a job array.
# THIS IS THE KEY CHANGE: We are submitting a different, clean file.
sbatch --export=ALL --array=1-${N_CONFIGS} slurm_master_script.sh

echo "Master job array submitted. Use 'squeue -u \$USER' to monitor."
