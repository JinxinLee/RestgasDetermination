# Two-Pass Vertex-Fitted Analysis Workflow

This document describes a specialized two-pass analysis workflow designed to improve track reconstruction by using a primary vertex fit from an initial analysis pass.

## Workflow Overview

The process is managed by the `runall_prod_hvmaps.py` script and can be broken down into two main stages:

1.  **Pass 1: Initial Vertex Finding**
    - The standard simulation, digitization, reconstruction, and PID chain is executed.
    - The `ana_dpm.C` macro is run on the output. This script performs a robust Gaussian fit to the POCA (Point of Closest Approach) vertex distribution of proton-antiproton pairs to determine the primary interaction vertex.
    - The fitted vertex coordinates (X, Y, Z) are saved to a JSON file: `<prefix>_<job_id>_vtx_fit.json`.

2.  **Pass 2: Vertex-Constrained Rerunning & Verification**
    - The `runall_prod_hvmaps.py` orchestrator script reads the coordinates from the JSON file.
    - It exports these coordinates as environment variables (`FIT_VERTEX_X`, `FIT_VERTEX_Y`, `FIT_VERTEX_Z`).
    - It then executes `prod_aod_complete.C`, which reruns the reconstruction and PID steps. The core tracking code (`PndPidTrackInfo`) is configured to use the vertex coordinates from the environment variables as a constraint, improving track parameter resolution.
    - Finally, `ana_complete.C` is run as a verification step. It generates histograms of the newly fitted vertex coordinates (`pvx`, `pvy`, `pvz`) to confirm the results of the second pass. These plots are saved as `<prefix>_<job_id>_vtx_verification.png`.

This two-pass approach allows for a more precise analysis by first establishing the primary event vertex and then using that information to refine the final physics results.

---

# Production Workflow Scripts

This directory contains scripts to run the `prod_hvmaps` simulation workflow, both locally and on a SLURM cluster like GSI Virgo.

## Scripts

- `runall_prod_hvmaps.sh`: The original bash script that runs the full sequence of ROOT macros.
- `runall_prod_hvmaps.py`: A Python wrapper that calls `runall_prod_hvmaps.sh`. It accepts the same arguments and is intended for consistency if a Python-based workflow is preferred.
- `submit_prod_hvmaps.py`: A Python script to generate and submit SLURM `sbatch` jobs. This is the recommended way to run the workflow in parallel on a cluster.

## How to Run on a SLURM Cluster (e.g., GSI Virgo)

The `submit_prod_hvmaps.py` script is the main tool for submitting jobs. It generates a temporary `sbatch` script that calls `runall_prod_hvmaps.sh` and submits it for you.

### Basic Usage

To submit a job array, use the `--array` flag. For example, to run 10 jobs:

```bash
python3 macro/target/submit_prod_hvmaps.py --array 1-10 --prefix 1234 --nevts 5000 --dec DPM2 --pbeam 8.9
```

This will create 10 jobs, each with a unique `SLURM_ARRAY_TASK_ID` from 1 to 10. The output files will be named using this ID, e.g., `1234_1_sim.root`, `1234_2_sim.root`, etc.

### Dry Run

Before submitting, you can check the generated `sbatch` script content with `--dry-run`:

```bash
python3 macro/target/submit_prod_hvmaps.py --dry-run --array 1-5
```

This will print the `sbatch` script to the console without actually submitting it.

### Command-line Options for `submit_prod_hvmaps.py`

- `--array`: (Required for cluster jobs) The job array specification (e.g., `1-100`, `1-10%5`).
- `--prefix`: Prefix for output files (default: `9999`).
- `--nevts`: Number of events per job (default: `1000`).
- `--dec`: Decay/generator string (default: `pp_dd`).
- `--pbeam`: Beam momentum (default: `8.9`).
- `--jobname`: SLURM job name (default: `pndsim`).
- `--time`: Time limit for each job (default: `8:00:00`).
- `--outbase`: Directory for SLURM log files (default: `data/slurmlog`).
- `--extra-sbatch`: A string of extra `#SBATCH` directives to add to the script, separated by newlines.

## Local Execution (for testing)

You can run the workflow for a single job locally by calling the shell script directly. This is useful for testing and debugging.

```bash
# Make sure the script is executable
chmod +x macro/target/runall_prod_hvmaps.sh

# Run with custom parameters
./macro/target/runall_prod_hvmaps.sh 9999 1000 pp_dd 8.9
```

You can also use the Python wrapper, which does the same thing:

```bash
python3 macro/target/runall_prod_hvmaps.py 9999 1000 pp_dd 8.9
```
