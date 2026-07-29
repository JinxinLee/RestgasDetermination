# Target Analysis & Rest Gas Workflow Orchestrator

This directory (`macro/target/`) contains the complete execution pipeline, ROOT macros, Python orchestrator scripts, SLURM batch submission tools, and post-processing correction utilities for the **Rest Gas Determination** project in PandaRoot.

---

## Directory Structure

```text
macro/target/
├── Central Orchestrator & Config Utilities
│   ├── runall_prod_hvmaps.py     # Main Python orchestrator for local & multi-config processing
│   ├── generate_configs.py       # Configuration file matrix generator
│   ├── batch_generate_configs.py # Batch configuration generator for parameter scans
│   ├── shared_utils.py           # Shared helper functions (logging, ROOT execution, parsing)
│   └── config.json               # Default configuration file template
│
├── Core ROOT Execution Macros
│   ├── prod_sim_hvmaps.C         # Geant4 MC simulation (TargetMode=8 for rest gas)
│   ├── reco_complete.C           # Track finding (STT + Apollonius triplet) & Kalman fitting
│   ├── pid_complete.C            # Pass 1 PID correlator execution (no back-propagation)
│   ├── ana_dpm.C                 # Pass 1 POCA vertex calculation & Gaussian vertex fitting
│   ├── prod_aod_complete.C       # Pass 2 combined Digi+Reco+PID (uses fitvertex/mcvertex)
│   └── ana_complete.C            # Pass 2 final analysis, NTuple filling & vertex verification
│
├── SLURM / Cluster Execution
│   ├── submit.sh                 # SLURM job array submission script
│   └── merge.py                  # Utility to merge parallel ROOT output files via `hadd`
│
├── Correction & Post-Processing (`correction/`)
│   ├── efficiency_correction_2.C # Enhanced efficiency & rest gas profile correction macro
│   ├── efficiency_correction.C   # Standard 9-panel efficiency correction macro
│   ├── efficiency_correction_steps.C # Iterative efficiency reweighting macro
│   └── run_efficiency_batch.py   # Batch execution wrapper for efficiency corrections
│
└── Auxiliary & Development Scripts
    ├── ConvertTrackToRhoCandList.C # Track candidate converter utility
    ├── houghPlusApolloniusTripletTrackFinder.C # Experimental tracking macro
    ├── standardPlusApolloniusTripletTrackFinder.C # Experimental tracking macro
    ├── digi_complete.C           # Standalone digitization macro (superseded)
    ├── prod_aod_hvmaps.C         # Standalone Digi/AOD macro (used in Pass 1)
    └── pid_new.C                 # Experimental PID macro (superseded)
```

---

## Workflow Strategies

The execution pipeline supports two workflow strategies, controlled by the `"back_prop_vertex"` parameter in `.json` configuration files:

### 1. POCA Workflow (`"back_prop_vertex": "poca"`) — Two-Pass Analysis

Designed for real data and realistic vertex-unconstrained scenarios:

- **Pass 1: Initial Vertex Finding**
  1. `prod_sim_hvmaps.C`: Runs Geant4 simulation using `TargetMode=8` (rest gas target profile).
  2. `prod_aod_hvmaps.C`: Runs digitization.
  3. `reco_complete.C`: Reconstructs tracks using GenFit2.
  4. `pid_complete.C`: Runs initial PID without track back-propagation.
  5. `ana_dpm.C`: Calculates the Point of Closest Approach (POCA) between reconstructed $p\bar{p}$ track pairs, performs iterative Gaussian vertex fitting, and exports the fitted mean vertex $(X, Y, Z)$ to `<prefix>_vtx_fit.json`.

- **Pass 2: Vertex-Constrained Refinement**
  1. `runall_prod_hvmaps.py` parses `<prefix>_vtx_fit.json`.
  2. Passes the `event_poca` tree in `<prefix>_boost.root` to the second pass using `POCA_VERTEX_FILE`.
  3. `prod_aod_complete.C`: Re-runs Digi + Reco + PID using option `"fitvertex"`. `PndPidCorrelator` reads the matching event ID and propagates each event's tracks to its own POCA.
  4. `ana_complete.C`: Generates final NTuples (`*_ana_final.root`) and verification plots.

### 2. MC Workflow (`"back_prop_vertex": "mc"`) — Single-Pass Analysis

Used for simulation reference studies:

1. `prod_sim_hvmaps.C`: Runs Geant4 simulation.
2. `prod_aod_complete.C`: Runs combined Digi + Reco + PID with option `"mcvertex"`, setting `PndPidCorrelator` to use the true MC vertex for track back-propagation.
3. `ana_complete.C`: Produces final NTuples and plots.

---

## Configuration System

All parameters are specified in JSON configuration files, with a 3-tier precedence hierarchy:
1. **Hardcoded Defaults** (in Python scripts)
2. **JSON Config File**
3. **Command-Line Arguments** (override JSON settings)

### Parameter Reference

| Parameter | Type | Default | Description |
| --- | --- | --- | --- |
| `prefix` | string | `"test"` | Job prefix name; output files are stored under `<output_path>/<prefix>/` |
| `nevts` | int | `1000` | Number of events to generate per job |
| `dec` | string | `"pp_dd"` | Decay file or generator mode (`DPM`, `DPM1`, `DPM2`, `FTF`, `BOX`) |
| `mom` | float | `4.06` | Antiproton beam momentum (GeV/c) |
| `use_mvd_hvmaps` | string | `"false"` | `"true"` to use HVMAPS MVD parameter file (`all_hvmaps.par`) |
| `ipx`, `ipy`, `ipz` | float | `0.0` | Target Interaction Point coordinates (cm) |
| `use_restgas` | string | `"false"` | `"true"` to enable rest gas density profile (`TargetMode=8`) |
| `restgas_profile` | string | `"restgas_16012024_with_cryopump.txt"` | Profile basename under `input/`, or an absolute path |
| `theta_min` | float | `0.0` | Minimum polar angle for DPM generator (degrees) |
| `theta_max` | float | `180.0` | Maximum polar angle for DPM generator (degrees) |
| `back_prop_vertex` | string | `"poca"` | Workflow selector: `"poca"` (two-pass) or `"mc"` (single-pass) |
| `output_path` | string | `"data"` | Base directory for log, reco, and figure outputs |

---

## Usage Instructions

### 1. Generating Configuration Files

Generate matrix scans over beam momentum, vertex position, or event counts:

```bash
# Single parameter matrix scan
python3 generate_configs.py \
    --type point \
    --vertex poca \
    --moms 4.06 8.9 \
    --ipzs -10.0 0.0 10.0 \
    --nevts 10000 \
    --output-dir configs/scan_vtx
```

### 2. Local Workflow Execution

Run single or multiple configurations locally:

```bash
# Run a single configuration file
python3 runall_prod_hvmaps.py configs/scan_vtx/point_poca_4.06_0.0_0.0_0.0.json

# Run all configuration files in a directory in parallel using 4 workers
python3 runall_prod_hvmaps.py configs/scan_vtx/ -j 4

# Override event count for all config runs
python3 runall_prod_hvmaps.py configs/scan_vtx/ --nevts 2000
```

### 3. Cluster Execution (SLURM / Virgo)

For cluster production, submit array jobs using `submit.sh`:

```bash
# Submit SLURM job array on the cluster
sbatch submit.sh
```

Example `submit.sh`:
```bash
#!/bin/bash
#SBATCH --job-name=pnd_sim
#SBATCH --partition=long
#SBATCH --time=20:00:00
#SBATCH --output=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.log
#SBATCH --error=/lustre/panda/jili/oct19/macro/target/data/slurmlog/pnd_sim_%A_%a.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=24G
#SBATCH --array=1-500
#SBATCH --singularity-container=/cvmfs/vae.gsi.de/vae23/containers/user_container-develop.sif

# Setup PandaRoot environment
source /lustre/panda/jili/oct19/build/config.sh -p

cd /lustre/panda/jili/oct19/macro/target
python3 -u runall_prod_hvmaps.py acc_p20.json
```

Each SLURM array job receives a unique `SLURM_ARRAY_TASK_ID`, which `runall_prod_hvmaps.py` automatically uses to format output filenames (e.g. `<prefix>_1_sim.root`, `<prefix>_2_sim.root`).

### 4. Merging Parallel Output Files

Merge task output ROOT files after array job completion:

```bash
# Merge ROOT files in a specified directory
python3 merge.py /lustre/panda/user/data/restgas_scan/reco/
```

### 5. Apply Efficiency Correction (`efficiency_correction_2.C`)

Extract corrected rest gas density profiles using `correction/efficiency_correction_2.C`:

```bash
cd correction/

# Basic usage with wildcard patterns:
root -b -q -l 'efficiency_correction_2.C( \
    "../data/restgas/reco/restgas_*_ana_final.root", \
    "../data/uniform_mc/reco/uniform_*_ana_final.root", \
    "../data/uniform_mc/reco/uniform_*_sim.root", \
    "../data/restgas/reco/restgas_*_sim.root", \
    "restgas_profile_corrected" \
)'

# Usage with explicit file range strings (e.g., files 1 to 100):
root -b -q -l 'efficiency_correction_2.C( \
    "../data/restgas/reco/restgas_%d_ana_final.root,1,100", \
    "../data/uniform_mc/reco/uniform_%d_ana_final.root,1,500", \
    "../data/uniform_mc/reco/uniform_%d_sim.root,1,500", \
    "../data/restgas/reco/restgas_%d_sim.root,1,100", \
    "restgas_profile_corrected" \
)'
```

Features of `efficiency_correction_2.C`:
- **File Range Syntax**: Supports explicit index ranges like `"path_%d.root,start,end"`
- **Custom Non-Uniform Binning**: High bin density in the central interaction region ($|z| \le 30\text{ cm}$) and wider bins for extended beam pipe coverage ($-300\text{ cm} \le z \le 600\text{ cm}$)
- **Multiple Correction Strategies**: Reco-Eff, Hybrid, and Bias-corrected output distributions

Produces:
- `restgas_profile_corrected.png` — Diagnostic plots
- `restgas_profile_corrected.root` — Saved ROOT histograms and profiles

---

## Output File Structure

```text
<output_path>/
└── <prefix>/
    ├── config.json                          # Final resolved JSON configuration
    ├── log/
    │   ├── <prefix>_<job_id>_sim.log        # Simulation log
    │   ├── <prefix>_<job_id>_digi.log       # Digitization log
    │   ├── <prefix>_<job_id>_reco.log       # Reconstruction log
    │   ├── <prefix>_<job_id>_pid.log        # Initial PID log
    │   ├── <prefix>_<job_id>_ana.log        # Vertex fitting log
    │   ├── <prefix>_<job_id>_aod_complete.log  # Pass 2 AOD log
    │   └── <prefix>_<job_id>_ana_complete.log  # Pass 2 final analysis log
    ├── reco/
    │   ├── <prefix>_<job_id>_sim.root       # Geant4 MC output (pndsim tree)
    │   ├── <prefix>_<job_id>_digi.root      # Digitized detector hits
    │   ├── <prefix>_<job_id>_reco.root      # Reconstructed tracks
    │   ├── <prefix>_<job_id>_pid.root       # Pass 1 PID output
    │   ├── <prefix>_<job_id>_boost.root     # Pass 1 NTuples plus event_poca tree
    │   ├── <prefix>_<job_id>_vtx_fit.json   # Pass 1 fitted vertex (X, Y, Z mean & sigma)
    │   ├── <prefix>_<job_id>_pid_final.root # Pass 2 vertex-constrained PID output
    │   └── <prefix>_<job_id>_ana_final.root # Pass 2 final NTuples (ntpDp tree)
    └── figure/
        ├── <prefix>_<job_id>_vtx_fit.png    # Pass 1 Gaussian vertex fit plots
        └── <prefix>_<job_id>_vtx_verification.png # Pass 2 vertex verification plots
```
