# Advanced Analysis Workflow Orchestrator

This document describes a configurable, multi-workflow analysis pipeline managed by the `runall_prod_hvmaps.py` script. It supports different analysis strategies through a centralized configuration system and is designed for both local testing and large-scale cluster production.

## Core Concepts

### 1. Central Orchestrator
The `runall_prod_hvmaps.py` script is the single entry point for the entire workflow. It reads configurations, resolves parameters, creates the necessary directory structure, and executes the required ROOT macros in sequence.

### 2. Configuration Management
All parameters are managed through a `config.json` file. This allows for easy switching between different physics scenarios without modifying the scripts.

### 3. Parameter Hierarchy
The script uses a three-tiered parameter system, providing maximum flexibility:
1.  **Hardcoded Defaults**: Sensible defaults are defined within the script.
2.  **`config.json`**: Values in this file override the hardcoded defaults.
3.  **Command-Line Arguments**: Arguments passed directly to the script (e.g., `--nevts 10000`) will override both the defaults and the `config.json` values.

## Available Workflows

The `back_prop_vertex` parameter in `config.json` controls which analysis workflow is executed.

### A. POCA Workflow (`"back_prop_vertex": "poca"`)
This is a **two-pass analysis** designed to achieve the highest possible track resolution by using a fitted primary vertex.

-   **Pass 1: Initial Vertex Finding**
    1.  The standard simulation, digitization, reconstruction, and PID chain is executed.
    2.  `ana_dpm.C` is run to analyze proton-antiproton pairs and calculate their Point of Closest Approach (POCA) vertex.
    3.  The fitted vertex coordinates (X, Y, Z) are saved to `<output_path>/<prefix>/reco/<prefix>_<job_id>_vtx_fit.json`.

-   **Pass 2: Vertex-Constrained Rerunning**
    1.  The script reads the vertex coordinates from the JSON file.
    2.  It exports these coordinates as environment variables (`FIT_VERTEX_X`, `FIT_VERTEX_Y`, `FIT_VERTEX_Z`).
    3.  `prod_aod_complete.C` is executed with the `"fitvertex"` option. This reruns the reconstruction and PID, using the vertex from the environment variables as a constraint.
    4.  `ana_complete.C` is run as a final verification step, generating plots to confirm the results.

### B. MC Workflow (`"back_prop_vertex": "mc"`)
This is a simplified **single-pass analysis** that uses the Monte Carlo truth vertex for back-propagation. It is faster and useful for studies where vertex fitting is not required.

1.  The simulation is run.
2.  `prod_aod_complete.C` is executed with the `"mcvertex"` option. This runs the reconstruction and PID using the MC truth vertex.
3.  `ana_complete.C` is run for final analysis.

## Configuration (`config.json`)

All workflow parameters can be set in `config.json`:

| Parameter          | Type    | Description                                                                       |
| ------------------ | ------- | --------------------------------------------------------------------------------- |
| `prefix`           | string  | A name for the job, used to create a dedicated output subdirectory.               |
| `nevts`            | integer | Number of events to generate per job.                                             |
| `dec`              | string  | EvtGen decay file name or a generator type (e.g., `DPM2`, `FTF`, `BOX`).           |
| `mom`              | float   | Momentum of the antiproton beam in GeV/c.                                         |
| `use_mvd_hvmaps`   | string  | `"true"` or `"false"`. Selects the MVD parameter file (`all_hvmaps.par` or `all.par`). |
| `ipx`, `ipy`, `ipz`| float   | Coordinates of the interaction point (IP) in cm.                                  |
| `use_restgas`      | string  | `"true"` or `"false"`. If true, enables the rest gas target simulation (`TargetMode=6`). |
| `theta_min`, `theta_max` | float | Minimum and maximum theta angles (in degrees) for the DPM generator.            |
| `back_prop_vertex` | string  | Workflow selector. Can be `"poca"` or `"mc"`.                                     |
| `output_path`      | string  | The absolute base path where all output will be stored.                           |

## Output Directory Structure

The script creates a structured output to keep results organized. The structure is:

```
<output_path>/
└── <prefix>/
    ├── log/
    │   ├── <prefix>_<job_id>_sim.log
    │   └── ...
    ├── reco/
    │   ├── <prefix>_<job_id>_sim.root
    │   ├── <prefix>_<job_id>_vtx_fit.json
    │   └── ...
    └── figure/
        ├── <prefix>_<job_id>_vtx_fit.png
        └── ...
```

## Usage

### Local Execution
For testing and debugging, you can run the script directly. You can override any parameter from `config.json` using command-line flags.

```bash
# Run with default settings from config.json
python3 macro/target/runall_prod_hvmaps.py

# Override the number of events and prefix for a specific run
python3 macro/target/runall_prod_hvmaps.py --nevts 5000 --prefix my_local_test
```

### SLURM Cluster Execution
The script is designed to work with SLURM job arrays. It automatically detects the `SLURM_ARRAY_TASK_ID` environment variable to create unique output file names for each job in the array.

**Example `sbatch` script (`submit.sh`):**
```bash
#!/bin/bash
#SBATCH --job-name=pnd_sim_array
#SBATCH --output=/lustre/panda/jili/slurmlog/pnd_sim_%A_%a.log
#SBATCH --error=/lustre/panda/jili/slurmlog/pnd_sim_%A_%a.err
#SBATCH --time=08:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=4G
#SBATCH --array=1-100

# Setup PandaRoot environment
# (You must source your environment script here)
# e.g., source /path/to/your/pandaroot/build/config.sh

# Navigate to the macro directory
cd /path/to/your/RestgasDetermination/macro/target

# Run the python orchestrator
# The script will use the settings from config.json
python3 runall_prod_hvmaps.py
```

To submit the job array, simply run:
```bash
sbatch submit.sh
```
This will launch 100 jobs, and the Python script will handle the unique naming and output paths for each one.
