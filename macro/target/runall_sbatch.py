#!/usr/bin/env python3
"""
Python script to run the complete analysis chain for rest gas determination.

This script orchestrates the simulation, digitization, reconstruction, and analysis workflow.
It performs an initial analysis to determine the interaction vertex, then re-runs the
reconstruction and PID steps using this fitted vertex information for improved accuracy.

Usage:
  python3 runall_prod_hvmaps.py [prefix] [nevts] [gen] [pbeam]

Example:
  python3 runall_prod_hvmaps.py my_test 10000 pp_dd 8.9

Workflow:
1.  Run simulation (prod_sim_hvmaps.C)
2.  Run digitization (prod_aod_hvmaps.C)
3.  Run initial reconstruction (reco_complete.C)
4.  Run initial PID (pid_complete.C)
5.  Run analysis to fit the vertex (ana_dpm.C), which generates a JSON file.
6.  Set the FIT_RESULT_FILE environment variable pointing to the generated JSON.
7.  Re-run reconstruction with a new suffix "reco_from_fit" (reco_complete.C).
8.  Re-run PID with the new reco file and a new suffix "pid_from_fit" (pid_complete.C). The back-propagation step inside will use the environment variable.
"""
import os
import sys
import subprocess
import argparse
import shlex
import json
from multiprocessing import Pool


def run_command(command, log_file, output_file, env=None):
    """Executes a command and logs its output, skipping if the output file already exists."""
    if os.path.exists(output_file):
        print(f"Output file {output_file} already exists. Skipping command.")
        print(f"Executing: {command}")
        return True
        
    print(f"Executing: {command}")
    print(f"Logging to: {log_file}")
    try:
        # Use the provided environment, or the current one if None
        current_env = os.environ.copy()
        if env:
            current_env.update(env)
            
        with open(log_file, 'w') as f:
            process = subprocess.run(shlex.split(command), stdout=f, stderr=subprocess.STDOUT, check=True, text=True, env=current_env)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {command}", file=sys.stderr)
        print(f"See log for details: {log_file}", file=sys.stderr)
        return False
    except FileNotFoundError:
        print(f"Error: 'root' command not found. Make sure ROOT is installed and in your PATH.", file=sys.stderr)
        with open(log_file, 'w') as f:
            f.write("Error: 'root' command not found. Make sure ROOT is installed and in your PATH.")
        return False

def load_config(config_file):
    """Loads configuration from a JSON file."""
    if config_file and os.path.exists(config_file):
        print(f"Loading configuration from {config_file}")
        with open(config_file, 'r') as f:
            return json.load(f)
    print(f"Warning: Configuration file '{config_file}' not found. Using defaults and command-line arguments.")
    return {}

def get_generated_events(log_file):
    """Extracts the number of generated events from a log file."""
    try:
        with open(log_file, 'r') as f:
            for line in f:
                if "Number of events generated" in line:
                    return line.strip()
    except FileNotFoundError:
        return None
    return None

def run_poca_workflow(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path):
    """Runs the full two-pass analysis workflow to fit the vertex."""
    print("\n--- Running POCA Workflow (Two-Pass Analysis) ---")

    # --- Step 1: Initial Workflow for Vertex Fitting ---
    
    # Simulation
    sim_log = os.path.join(log_path, f"{out_prefix}_sim.log")
    sim_output = os.path.join(reco_path, f"{out_prefix}_sim.root")
    sim_command = (
        f'root -l -q -b "prod_sim_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {p.nevts}, \\"{p.dec}\\", {p.mom}, {use_mvd_str}, '
        f'{p.ipx}, {p.ipy}, {p.ipz}, {p.use_restgas}, {p.theta_min}, {p.theta_max})"'
    )
    if not run_command(sim_command, sim_log, sim_output):
        sys.exit(1)
    
    num_ev_line = get_generated_events(sim_log)
    def append_nevents(log_file):
        if num_ev_line:
            with open(log_file, 'a') as f:
                f.write('\n' + num_ev_line)

    # Digitization
    digi_log = os.path.join(log_path, f"{out_prefix}_digi.log")
    digi_output = os.path.join(reco_path, f"{out_prefix}_digi.root")
    if not run_command(f'root -l -b -q "prod_aod_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str})"', digi_log, digi_output):
        sys.exit(1)
    append_nevents(digi_log)

    # Reconstruction
    reco_log = os.path.join(log_path, f"{out_prefix}_reco.log")
    reco_output = os.path.join(reco_path, f"{out_prefix}_reco.root")
    if not run_command(f'root -l -b -q "reco_complete.C({p.nevts}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str})"', reco_log, reco_output):
        sys.exit(1)
    append_nevents(reco_log)

    # PID
    pid_log = os.path.join(log_path, f"{out_prefix}_pid.log")
    pid_output = os.path.join(reco_path, f"{out_prefix}_pid.root")
    if not run_command(f'root -l -b -q "pid_complete.C({p.nevts}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str})"', pid_log, pid_output):
        sys.exit(1)
    append_nevents(pid_log)

    # Analysis for Vertex Fitting
    ana_log = os.path.join(log_path, f"{out_prefix}_ana.log")
    ana_output = os.path.join(reco_path, f"{out_prefix}_vtx_fit.json")
    ana_dpm_cmd = f'root -l -b -q "ana_dpm.C({p.nevts}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{out_prefix}\\")"'
    if not run_command(ana_dpm_cmd, ana_log, ana_output):
        sys.exit(1)
    append_nevents(ana_log)

    # --- Step 2: Re-run with Fitted Vertex ---
    print("\n--- Re-running Reco/PID with Fitted Vertex ---")
    
    json_fit_file = os.path.join(reco_path, f"{out_prefix}_vtx_fit.json")
    
    # Read vertex from JSON and set environment variables
    try:
        with open(json_fit_file, 'r') as f:
            fit_data = json.load(f)
        
        vtx_x = fit_data['vertex_x']['mean']
        vtx_y = fit_data['vertex_y']['mean']
        vtx_z = fit_data['vertex_z']['mean']

        print(f"Found fitted vertex: X={vtx_x}, Y={vtx_y}, Z={vtx_z}")

        fit_env = {
            'FIT_VERTEX_X': str(vtx_x),
            'FIT_VERTEX_Y': str(vtx_y),
            'FIT_VERTEX_Z': str(vtx_z)
        }
        print(f"Setting environment variables for fitted vertex.")

    except (FileNotFoundError, KeyError, json.JSONDecodeError) as e:
        print(f"Error: Could not read or parse fit results from {json_fit_file}.", file=sys.stderr)
        print(f"Reason: {e}", file=sys.stderr)
        print("Skipping re-run of reco and pid.", file=sys.stderr)
        sys.exit(1)

    # Re-run combined Reco and PID
    aod_complete_log = os.path.join(log_path, f"{out_prefix}_aod_complete.log")
    aod_complete_output = os.path.join(reco_path, f"{out_prefix}_pid_poca.root")
    if not run_command(f'root -l -b -q "prod_aod_complete.C(\\"{os.path.join(reco_path, out_prefix)}\\", \\"fitvertex\\", {use_mvd_str})"', aod_complete_log, aod_complete_output, env=fit_env):
        sys.exit(1)
    append_nevents(aod_complete_log)

    # Re-run final analysis
    ana_complete_log = os.path.join(log_path, f"{out_prefix}_ana_complete.log")
    ana_complete_output = os.path.join(reco_path, f"{out_prefix}_poca.root")
    ana_complete_cmd = f'root -l -b -q "ana_complete.C({p.nevts}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{out_prefix}\\")"'
    if not run_command(ana_complete_cmd, ana_complete_log, ana_complete_output, env=fit_env):
        sys.exit(1)
    append_nevents(ana_complete_log)

def run_mc_workflow(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path):
    """Runs the simplified workflow using the MC vertex directly."""
    print("\n--- Running MC Workflow (Single Pass) ---")

    # --- Step 1: Simulation ---
    sim_log = os.path.join(log_path, f"{out_prefix}_sim.log")
    sim_output = os.path.join(reco_path, f"{out_prefix}_sim.root")
    sim_command = (
        f'root -l -q -b "prod_sim_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {p.nevts}, \\"{p.dec}\\", {p.mom}, {use_mvd_str}, '
        f'{p.ipx}, {p.ipy}, {p.ipz}, {p.use_restgas}, {p.theta_min}, {p.theta_max})"'
    )
    if not run_command(sim_command, sim_log, sim_output):
        sys.exit(1)

    num_ev_line = get_generated_events(sim_log)
    def append_nevents(log_file):
        if num_ev_line:
            with open(log_file, 'a') as f:
                f.write('\n' + num_ev_line)

    # --- Step 2: Combined AOD and Final Analysis ---
    
    # Run combined Reco and PID using MC vertex
    aod_complete_log = os.path.join(log_path, f"{out_prefix}_aod_complete.log")
    aod_complete_output = os.path.join(reco_path, f"{out_prefix}_pid_poca.root")
    if not run_command(f'root -l -b -q "prod_aod_complete.C(\\"{os.path.join(reco_path, out_prefix)}\\", \\"mcvertex\\", {use_mvd_str})"', aod_complete_log, aod_complete_output):
        sys.exit(1)
    append_nevents(aod_complete_log)

    # Run final analysis
    ana_complete_log = os.path.join(log_path, f"{out_prefix}_ana_complete.log")
    ana_complete_output = os.path.join(reco_path, f"{out_prefix}_poca.root")
    ana_complete_cmd = f'root -l -b -q "ana_complete.C({p.nevts}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{out_prefix}\\")"'
    if not run_command(ana_complete_cmd, ana_complete_log, ana_complete_output):
        sys.exit(1)
    append_nevents(ana_complete_log)

def run_poca_worker(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path):
    """Runs the worker part of the POCA workflow for a single job."""
    print(f"--- Running POCA Worker for {out_prefix} ---")

    # --- Step 1: Initial Workflow ---
    sim_log = os.path.join(log_path, f"{out_prefix}_sim.log")
    sim_output = os.path.join(reco_path, f"{out_prefix}_sim.root")
    sim_command = (
        f'root -l -q -b "prod_sim_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {p.nevts}, \\"{p.dec}\\", {p.mom}, {use_mvd_str}, '
        f'{p.ipx}, {p.ipy}, {p.ipz}, {p.use_restgas}, {p.theta_min}, {p.theta_max})"'
    )
    if not run_command(sim_command, sim_log, sim_output):
        sys.exit(1)

    num_ev_line = get_generated_events(sim_log)
    def append_nevents(log_file):
        if num_ev_line:
            with open(log_file, 'a') as f:
                f.write('\n' + num_ev_line)

    digi_log = os.path.join(log_path, f"{out_prefix}_digi.log")
    digi_output = os.path.join(reco_path, f"{out_prefix}_digi.root")
    if not run_command(f'root -l -b -q "prod_aod_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str})"', digi_log, digi_output):
        sys.exit(1)
    append_nevents(digi_log)

    reco_log = os.path.join(log_path, f"{out_prefix}_reco.log")
    reco_output = os.path.join(reco_path, f"{out_prefix}_reco.root")
    if not run_command(f'root -l -b -q "reco_complete.C({p.nevts}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str})"', reco_log, reco_output):
        sys.exit(1)
    append_nevents(reco_log)

    pid_log = os.path.join(log_path, f"{out_prefix}_pid.log")
    pid_output = os.path.join(reco_path, f"{out_prefix}_pid.root")
    if not run_command(f'root -l -b -q "pid_complete.C({p.nevts}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str})"', pid_log, pid_output):
        sys.exit(1)
    append_nevents(pid_log)

    # --- Step 2: Re-run with Fitted Vertex (Worker part) ---
    # This part is executed if the script is called with FIT_VERTEX env vars
    fit_vertex_x = os.environ.get('FIT_VERTEX_X')
    if fit_vertex_x:
        print("\n--- Re-running Reco/PID with Fitted Vertex (Worker) ---")
        fit_env = {
            'FIT_VERTEX_X': os.environ.get('FIT_VERTEX_X'),
            'FIT_VERTEX_Y': os.environ.get('FIT_VERTEX_Y'),
            'FIT_VERTEX_Z': os.environ.get('FIT_VERTEX_Z')
        }
        aod_complete_log = os.path.join(log_path, f"{out_prefix}_aod_complete.log")
        aod_complete_output = os.path.join(reco_path, f"{out_prefix}_pid_poca.root")
        if not run_command(f'root -l -b -q "prod_aod_complete.C(\\"{os.path.join(reco_path, out_prefix)}\\", \\"fitvertex\\", {use_mvd_str})"', aod_complete_log, aod_complete_output, env=fit_env):
            sys.exit(1)
        append_nevents(aod_complete_log)
    
    print(f"--- POCA Worker for {out_prefix} finished. ---")

def run_mc_worker(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path):
    """Runs the worker part of the MC workflow for a single job."""
    print(f"--- Running MC Worker for {out_prefix} ---")

    # --- Step 1: Simulation ---
    sim_log = os.path.join(log_path, f"{out_prefix}_sim.log")
    sim_output = os.path.join(reco_path, f"{out_prefix}_sim.root")
    sim_command = (
        f'root -l -q -b "prod_sim_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {p.nevts}, \\"{p.dec}\\", {p.mom}, {use_mvd_str}, '
        f'{p.ipx}, {p.ipy}, {p.ipz}, {p.use_restgas}, {p.theta_min}, {p.theta_max})"'
    )
    if not run_command(sim_command, sim_log, sim_output):
        sys.exit(1)

    num_ev_line = get_generated_events(sim_log)
    def append_nevents(log_file):
        if num_ev_line:
            with open(log_file, 'a') as f:
                f.write('\n' + num_ev_line)

    # --- Step 2: Combined AOD ---
    aod_complete_log = os.path.join(log_path, f"{out_prefix}_aod_complete.log")
    aod_complete_output = os.path.join(reco_path, f"{out_prefix}_pid_poca.root")
    if not run_command(f'root -l -b -q "prod_aod_complete.C(\\"{os.path.join(reco_path, out_prefix)}\\", \\"mcvertex\\", {use_mvd_str})"', aod_complete_log, aod_complete_output):
        sys.exit(1)
    append_nevents(aod_complete_log)
    print(f"--- MC Worker for {out_prefix} finished. ---")

def run_poca_merge_step1(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path):
    """POCA Step 1: Merges initial files, fits vertex, and submits second stage."""
    print("\n--- Running POCA Merge Step 1: Initial Merge and Vertex Fit ---")
    njobs = p.njobs

    # Merge PID files
    merged_pid_file = os.path.join(reco_path, f"{out_prefix_base}_pid.root")
    pid_files_to_merge = [os.path.join(reco_path, f"{out_prefix_base}_{i+1}_pid.root") for i in range(njobs)]
    json_str = json.dumps(pid_files_to_merge).replace('"', '\\"')
    merge_command = f'root -l -b -q \'merge_files.C("{merged_pid_file}", "{json_str}")\''
    merge_log = os.path.join(log_path, f"{out_prefix_base}_merge_pid.log")
    if not run_command(merge_command, merge_log, merged_pid_file):
        print("Error during initial PID merging. Exiting.", file=sys.stderr)
        sys.exit(1)

    # Run Analysis for Vertex Fitting on merged file
    ana_log = os.path.join(log_path, f"{out_prefix_base}_ana.log")
    ana_output = os.path.join(reco_path, f"{out_prefix_base}_vtx_fit.json")
    total_nevts = p.nevts * njobs
    ana_dpm_cmd = f'root -l -b -q "ana_dpm.C({total_nevts}, \\"{os.path.join(reco_path, out_prefix_base)}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{out_prefix_base}\\")"'
    if not run_command(ana_dpm_cmd, ana_log, ana_output):
        sys.exit(1)

    # Read vertex info and submit next stages
    print("\n--- Submitting second stage jobs ---")
    try:
        with open(ana_output, 'r') as f:
            fit_data = json.load(f)
        vtx_x = fit_data['vertex_x']['mean']
        vtx_y = fit_data['vertex_y']['mean']
        vtx_z = fit_data['vertex_z']['mean']
        print(f"Found fitted vertex: X={vtx_x}, Y={vtx_y}, Z={vtx_z}")

        # Submit worker jobs for the second pass and get job ID
        worker2_command = (
            f"sbatch --export=ALL,FIT_VERTEX_X={vtx_x},FIT_VERTEX_Y={vtx_y},FIT_VERTEX_Z={vtx_z} "
            f"--array=1-{njobs} submit_sbatch.sh {p.configfile} worker"
        )
        print(f"Executing submission command for second pass workers: {worker2_command}")
        result = subprocess.run(shlex.split(worker2_command), check=True, capture_output=True, text=True)
        worker2_job_id = result.stdout.strip().split()[-1]
        print(f"--- Second stage worker jobs submitted with Job ID: {worker2_job_id} ---")

        # Submit the final merge (merge2) job, dependent on the second worker stage
        merge2_command = f"sbatch --dependency=afterok:{worker2_job_id} submit_sbatch.sh {p.configfile} merge2"
        print(f"Executing submission command for final merge: {merge2_command}")
        subprocess.run(shlex.split(merge2_command), check=True)
        print(f"--- Final merge job (merge2) submitted and will run after second stage workers complete. ---")

    except (FileNotFoundError, KeyError, json.JSONDecodeError, subprocess.CalledProcessError, IndexError) as e:
        print(f"Error during second stage submission: {e}", file=sys.stderr)
        sys.exit(1)

def run_poca_merge_step2(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path):
    """POCA Step 2: Merges final AOD files and runs final analysis."""
    print("\n--- Running POCA Merge Step 2: Final Merge and Analysis ---")
    njobs = p.njobs
    total_nevts = p.nevts * njobs

    # Merge final AOD files
    merged_aod_file = os.path.join(reco_path, f"{out_prefix_base}_pid_poca.root")
    aod_files_to_merge = [os.path.join(reco_path, f"{out_prefix_base}_{i+1}_pid_poca.root") for i in range(njobs)]
    json_str_aod = json.dumps(aod_files_to_merge).replace('"', '\\"')
    merge_aod_command = f'root -l -b -q \'merge_files.C("{merged_aod_file}", "{json_str_aod}")\''
    merge_aod_log = os.path.join(log_path, f"{out_prefix_base}_merge_aod.log")
    if not run_command(merge_aod_command, merge_aod_log, merged_aod_file):
        print("Error during final AOD merging. Exiting.", file=sys.stderr)
        sys.exit(1)

    # Run final analysis on the final merged file
    ana_complete_log = os.path.join(log_path, f"{out_prefix_base}_ana_complete.log")
    ana_complete_output = os.path.join(reco_path, f"{out_prefix_base}_poca.root")
    ana_complete_cmd = f'root -l -b -q "ana_complete.C({total_nevts}, \\"{os.path.join(reco_path, out_prefix_base)}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{out_prefix_base}\\")"'
    if not run_command(ana_complete_cmd, ana_complete_log, ana_complete_output):
        sys.exit(1)

def run_poca_merge(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path):
    """Dispatches to the correct POCA merge step based on the mode."""
    if p.mode == 'merge1':
        run_poca_merge_step1(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path)
    elif p.mode == 'merge2':
        run_poca_merge_step2(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path)
    else: # Original local/sequential logic
        # This part is now effectively legacy, as cluster submission is preferred.
        # For simplicity, we can keep it as a single block for local testing.
        run_poca_merge_step1_local_equivalent(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path)
        run_poca_merge_step2(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path)

def run_poca_merge_step1_local_equivalent(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path):
    """The first part of the original run_poca_merge, for local execution."""
    print("\n--- Running POCA Merge Step 1 (Local): Initial Merge and Vertex Fit ---")
    njobs = p.njobs
    
    # Merge PID files
    merged_pid_file = os.path.join(reco_path, f"{out_prefix_base}_pid.root")
    pid_files_to_merge = [os.path.join(reco_path, f"{out_prefix_base}_{i+1}_pid.root") for i in range(njobs)]
    json_str = json.dumps(pid_files_to_merge).replace('"', '\\"')
    merge_command = f'root -l -b -q \'merge_files.C("{merged_pid_file}", "{json_str}")\''
    merge_log = os.path.join(log_path, f"{out_prefix_base}_merge_pid.log")
    if not run_command(merge_command, merge_log, merged_pid_file):
        sys.exit(1)

    # Run Analysis for Vertex Fitting on merged file
    ana_log = os.path.join(log_path, f"{out_prefix_base}_ana.log")
    ana_output = os.path.join(reco_path, f"{out_prefix_base}_vtx_fit.json")
    total_nevts = p.nevts * njobs
    ana_dpm_cmd = f'root -l -b -q "ana_dpm.C({total_nevts}, \\"{os.path.join(reco_path, out_prefix_base)}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{out_prefix_base}\\")"'
    if not run_command(ana_dpm_cmd, ana_log, ana_output):
        sys.exit(1)

    # --- Step 2: Re-run with Fitted Vertex ---
    print("\n--- Re-running Reco/PID with Fitted Vertex (Local) ---")
    try:
        with open(ana_output, 'r') as f:
            fit_data = json.load(f)
        vtx_x = fit_data['vertex_x']['mean']
        vtx_y = fit_data['vertex_y']['mean']
        vtx_z = fit_data['vertex_z']['mean']
        print(f"Found fitted vertex: X={vtx_x}, Y={vtx_y}, Z={vtx_z}")
        
        fit_env = {
            'FIT_VERTEX_X': str(vtx_x),
            'FIT_VERTEX_Y': str(vtx_y),
            'FIT_VERTEX_Z': str(vtx_z)
        }
        # In local mode, we just run the second stage directly.
        # We need to re-run the worker tasks sequentially.
        for i in range(njobs):
            slurm_id = str(i + 1)
            out_prefix = f"{out_prefix_base}_{slurm_id}"
            print(f"\n--- Running second stage for worker {slurm_id} (local) ---")
            aod_complete_log = os.path.join(log_path, f"{out_prefix}_aod_complete.log")
            aod_complete_output = os.path.join(reco_path, f"{out_prefix}_pid_poca.root")
            if not run_command(f'root -l -b -q "prod_aod_complete.C(\\"{os.path.join(reco_path, out_prefix)}\\", \\"fitvertex\\", {use_mvd_str})"', aod_complete_log, aod_complete_output, env=fit_env):
                sys.exit(1)

    except (FileNotFoundError, KeyError, json.JSONDecodeError) as e:
        print(f"Error: Could not read or parse fit results from {ana_output}.", file=sys.stderr)
        sys.exit(1)

def run_for_config(p):
    """Runs the entire workflow for a given parameter set."""
    # The argument is already a string "true" or "false"
    use_mvd_str = p.use_mvd_hvmaps
    
    # --- Path Setup ---
    # Construct the base path for this specific job prefix
    base_path = os.path.join(p.output_path, p.prefix)
    
    log_path = os.path.join(base_path, 'log')
    reco_path = os.path.join(base_path, 'reco')
    figure_path = os.path.join(base_path, 'figure')

    os.makedirs(log_path, exist_ok=True)
    os.makedirs(reco_path, exist_ok=True)
    os.makedirs(figure_path, exist_ok=True)

    # --- Naming Prefix for files ---
    # The directory path uses the full prefix, but the filenames inside use a shortened version.
    prefix_parts = p.prefix.split('_')
    if len(prefix_parts) >= 2:
        naming_prefix = f"{prefix_parts[0]}_{prefix_parts[1]}"
    else:
        naming_prefix = p.prefix

    # --- Mode Dispatch ---
    if p.mode == 'local':
        print(f"\n--- Starting Local Workflow for Prefix: {p.prefix} ---")
        # Save config for local run consistency
        config_save_path = os.path.join(base_path, 'config.json')
        with open(config_save_path, 'w') as f:
            json.dump(vars(p), f, indent=4)
        print(f"Saved final configuration to {config_save_path}")
        
        # Local mode uses the full prefix for simplicity, as it's not split into jobs.
        out_prefix = p.prefix
        if p.back_prop_vertex == 'poca':
            run_poca_workflow(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path)
        elif p.back_prop_vertex == 'mc':
            run_mc_workflow(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path)
    
    elif p.mode == 'submit':
        print(f"\n--- Submitting jobs for config: {p.configfile} ---")
        # Save the final configuration for this run
        config_save_path = os.path.join(base_path, 'config.json')
        with open(config_save_path, 'w') as f:
            config_to_save = {k: v for k, v in vars(p).items() if k not in ['configfiles', 'jobs', 'mode', 'configfile']}
            json.dump(config_to_save, f, indent=4)
        print(f"Saved final configuration to {config_save_path}")

        # Submit the first stage of worker jobs and get the job ID
        submit_command = f"sbatch --array=1-{p.njobs} submit_sbatch.sh {config_save_path} worker"
        print(f"Executing submission command: {submit_command}")
        try:
            result = subprocess.run(shlex.split(submit_command), check=True, capture_output=True, text=True)
            job_id = result.stdout.strip().split()[-1]
            print(f"--- Worker jobs submitted with Job ID: {job_id} ---")

            # Determine the correct merge mode based on the workflow
            merge_mode = 'merge'
            if p.back_prop_vertex == 'poca':
                merge_mode = 'merge1'

            # After workers are submitted, submit the merge step as a dependent job
            merge_submit_command = f"sbatch --dependency=afterok:{job_id} submit_sbatch.sh {config_save_path} {merge_mode}"
            print(f"Executing merge submission command: {merge_submit_command}")
            subprocess.run(shlex.split(merge_submit_command), check=True)
            print(f"--- Merge job ({merge_mode}) submitted and will run after workers complete. ---")

        except (subprocess.CalledProcessError, IndexError) as e:
            print(f"Error submitting jobs to Slurm: {e}", file=sys.stderr)
            sys.exit(1)

    elif p.mode == 'worker':
        slurm_id = os.environ.get('SLURM_ARRAY_TASK_ID', '1')
        # Use the shortened naming_prefix for the output file names
        out_prefix = f"{naming_prefix}_{slurm_id}"
        print(f"\n--- Starting Worker Job {slurm_id} for Prefix: {p.prefix} (using file prefix: {out_prefix}) ---")
        if p.back_prop_vertex == 'poca':
            run_poca_worker(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path)
        elif p.back_prop_vertex == 'mc':
            run_mc_worker(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path)

    elif p.mode in ['merge', 'merge1', 'merge2']:
        # The base prefix for merging should be the shortened naming_prefix
        out_prefix_base = naming_prefix
        print(f"\n--- Starting Merge Step for Prefix: {p.prefix} (mode: {p.mode}, file base prefix: {out_prefix_base}) ---")
        if p.back_prop_vertex == 'poca':
            # run_poca_merge will dispatch to step1 or step2 based on mode
            run_poca_merge(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path)
        elif p.back_prop_vertex == 'mc':
            # MC workflow only has a single merge step
            run_mc_merge(p, use_mvd_str, out_prefix_base, log_path, reco_path, figure_path)

    else:
        print(f"Error: Unknown mode '{p.mode}'", file=sys.stderr)
        sys.exit(1)
    print(f"\n--- Workflow for config {p.configfile} in mode {p.mode} completed. ---")


def main():
    print("--- Starting analysis script ---") # Add this line
    # --- Argument Parser Setup ---
    # This parser handles the config file path(s) and other top-level arguments.
    conf_parser = argparse.ArgumentParser(add_help=False)
    conf_parser.add_argument('configfiles', type=str, nargs='*', default=['config.json'],
                             help='One or more paths to configuration JSON files or directories containing them. Defaults to "config.json".')
    conf_parser.add_argument('-j', '--jobs', type=int, default=1,
                             help='Number of parallel jobs to run for local mode. Defaults to 1 (sequential).')
    conf_parser.add_argument('--mode', choices=['local', 'submit', 'worker', 'merge', 'merge1', 'merge2'], default='local',
                             help='Execution mode: "local" (default), "submit" to cluster, "worker" for cluster job, "merge" for post-processing.')

    # Parse known args to get the config file paths and other command-line args.
    conf_args, remaining_argv = conf_parser.parse_known_args()
    
    # --- Main Parser ---
    # This parser defines all possible arguments that can be in a config file or on the command line.
    parser = argparse.ArgumentParser(
        description='Run the complete analysis chain for rest gas determination.',
        parents=[conf_parser] # Inherit arguments for help message
    )
    
    # Define all arguments that can be set via command line or config file
    parser.add_argument('--prefix', type=str, help='Prefix for output files.')
    parser.add_argument('--nevts', type=int, help='Number of events to simulate per job.')
    parser.add_argument('--njobs', type=int, help='Number of jobs for cluster submission.')
    parser.add_argument('--dec', type=str, help='Name of EvtGen decay file or generator type (DPM/FTF/BOX).')
    parser.add_argument('--mom', type=float, help='Momentum of the pbar-beam.')
    parser.add_argument('--use_mvd_hvmaps', choices=['true', 'false'], help='Set to "true" to use new MVD with hvmaps.')
    parser.add_argument('--ipx', type=float, help='IP x coordinate.')
    parser.add_argument('--ipy', type=float, help='IP y coordinate.')
    parser.add_argument('--ipz', type=float, help='IP z coordinate.')
    parser.add_argument('--use_restgas', choices=['true', 'false'], help='Set to "true" to use restgas target.')
    parser.add_argument('--theta_min', type=float, help='Theta min for DPM generator.')
    parser.add_argument('--theta_max', type=float, help='Theta max for DPM generator.')
    parser.add_argument('--back_prop_vertex', choices=['poca', 'mc'], help='Vertex source for back propagation.')
    parser.add_argument('--output_path', type=str, help='Base path for output files.')

    # --- Task Preparation ---
    # In worker or merge mode, we expect exactly one config file.
    if conf_args.mode in ['worker', 'merge', 'merge1', 'merge2']:
        if len(conf_args.configfiles) != 1:
            print(f"Error: '{conf_args.mode}' mode requires exactly one config file, but {len(conf_args.configfiles)} were given.", file=sys.stderr)
            sys.exit(1)
        expanded_config_files = conf_args.configfiles
    else: # For local or submit mode, expand directories
        expanded_config_files = []
        for path in conf_args.configfiles:
            if os.path.isdir(path):
                print(f"Searching for config files in directory: {path}")
                for root, _, files in os.walk(path):
                    for file in files:
                        if file.endswith('.json'):
                            full_path = os.path.join(root, file)
                            expanded_config_files.append(full_path)
                            print(f"  Found config file: {full_path}")
            elif os.path.isfile(path):
                expanded_config_files.append(path)
            else:
                # If the path doesn't exist, it might be the default 'config.json'.
                # We'll pass it along and let load_config show a warning if it's not found.
                if conf_args.mode not in ['worker', 'merge', 'merge1', 'merge2']:
                     expanded_config_files.append(path)

    if not expanded_config_files and conf_args.mode not in ['worker', 'merge', 'merge1', 'merge2']:
        print(f"Warning: No config files found in the provided paths: {conf_args.configfiles}")

    tasks = []
    for config_file in expanded_config_files:
        # 1. Start with hardcoded defaults
        params = {
            'prefix': 'test', 'nevts': 1000, 'njobs': 10, 'dec': 'pp_dd', 'mom': 4.06,
            'use_mvd_hvmaps': 'false', 'ipx': 0.0, 'ipy': 0.0, 'ipz': 0.0,
            'use_restgas': 'false', 'theta_min': 0.0, 'theta_max': 180.0,
            'back_prop_vertex': 'poca', 'output_path': 'data'
        }

        # 2. Load configuration from the current file
        config = load_config(config_file)
        params.update(config)

        # 3. Override with any command-line arguments
        parser.set_defaults(**params)
        # The main config file path is now the first element of configfiles
        all_args = remaining_argv + conf_args.configfiles
        p = parser.parse_args(all_args)
        
        # Add helper attributes to the namespace
        p.configfile = config_file
        p.mode = conf_args.mode
        p.jobs = conf_args.jobs
        tasks.append(p)

    # --- Workflow Execution ---
    if not tasks:
        if conf_args.mode in ['worker', 'merge', 'merge1', 'merge2']:
             # This case should be handled earlier, but as a safeguard:
             print(f"Error: No config file provided for '{conf_args.mode}' mode.", file=sys.stderr)
        else:
            print("\nError: No configuration files found or processed. Exiting.", file=sys.stderr)
            print(f"Searched in paths provided: {conf_args.configfiles}", file=sys.stderr)
        sys.exit(1)

    print("\n--- Parsed Arguments ---")
    # Print the arguments for the first task as a representative sample
    if tasks:
        print("Sample configuration for the first task:")
        for key, value in vars(tasks[0]).items():
            # Don't print arguments that are not workflow parameters
            if key not in ['configfiles']:
                 print(f"  {key}: {value}")
    print("------------------------\n")
        
    # For submit, worker, merge modes, we typically run one task at a time from the command line
    if conf_args.mode in ['submit', 'worker', 'merge', 'merge1', 'merge2']:
        if len(tasks) > 1:
            print(f"Warning: Running in '{conf_args.mode}' mode, but multiple configs were found. Only processing the first one: {tasks[0].configfile}", file=sys.stderr)
        run_for_config(tasks[0])
    # Local mode can run multiple configs in parallel or sequentially
    else:
        num_jobs = conf_args.jobs
        if num_jobs > 1 and len(tasks) > 1:
            print(f"Found {len(tasks)} configuration(s) to run. Starting {num_jobs} parallel job(s) in local mode.")
            # Parallel execution
            with Pool(processes=num_jobs) as pool:
                pool.map(run_for_config, tasks)
        else:
            print(f"Found {len(tasks)} configuration(s) to run sequentially in local mode.")
            # Sequential execution
            for task in tasks:
                run_for_config(task)

    print("\nAll workflows completed.")
    print("--- Analysis script finished ---") # Add this line


if __name__ == '__main__':
    main()
