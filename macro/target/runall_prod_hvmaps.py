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

def get_generated_events(log_file):
    """Extracts the number of generated events from a log file."""
    try:
        with open(log_file, 'r') as f:
            for line in reversed(list(f)):
                if 'Generated Events' in line:
                    return line.strip()
    except FileNotFoundError:
        return None
    return None

def main():
    parser = argparse.ArgumentParser(description='Run the complete analysis chain for rest gas determination.')
    parser.add_argument('prefix', nargs='?', default='9999', help='Prefix for output files.')
    parser.add_argument('nevts', nargs='?', type=int, default=1000, help='Number of events to simulate.')
    parser.add_argument('dec', nargs='?', default='pp_dd', help='Name of EvtGen decay file or generator type (DPM/FTF/BOX).')
    parser.add_argument('mom', nargs='?', type=float, default=8.9, help='Momentum of the pbar-beam.')
    parser.add_argument('use_mvd_hvmaps', nargs='?', default='false', choices=['true', 'false'], help='Set to "true" to use new MVD with hvmaps, otherwise "false".')
    args = parser.parse_args()

    # The argument is already a string "true" or "false"
    use_mvd_str = args.use_mvd_hvmaps
    print(f"use_mvd_hvmaps={use_mvd_str}, so macros will use new MVD with hvmaps")

    # Environment setup
    print("SIMPATH is", os.environ.get('SIMPATH', 'Not set'))
    print("FAIRROOTPATH is", os.environ.get('FAIRROOTPATH', 'Not set'))

    slurm_id = os.environ.get('SLURM_ARRAY_TASK_ID', '1')
    out_prefix = f"data/dpm/{args.prefix}_{slurm_id}"
    
    os.makedirs(os.path.dirname(out_prefix), exist_ok=True)

    # --- Step 1: Initial Workflow ---
    print("\n--- Running Initial Workflow ---")
    
    # Simulation
    sim_log = f"{out_prefix}_sim.log"
    sim_output = f"{out_prefix}_sim.root"
    if not run_command(f'root -l -q -b "prod_sim_hvmaps.C(\\"{out_prefix}\\", {args.nevts}, \\"{args.dec}\\", {args.mom}, {use_mvd_str})"', sim_log, sim_output):
        sys.exit(1)
    
    num_ev_line = get_generated_events(sim_log)

    def append_nevents(log_file):
        if num_ev_line:
            with open(log_file, 'a') as f:
                f.write('\n' + num_ev_line)

    # Digitization
    digi_log = f"{out_prefix}_digi.log"
    digi_output = f"{out_prefix}_digi.root"
    if not run_command(f'root -l -b -q "prod_aod_hvmaps.C(\\"{out_prefix}\\", {use_mvd_str})"', digi_log, digi_output):
        sys.exit(1)
    append_nevents(digi_log)

    # Reconstruction
    reco_log = f"{out_prefix}_reco.log"
    reco_output = f"{out_prefix}_reco.root"
    if not run_command(f'root -l -b -q "reco_complete.C({args.nevts}, \\"{out_prefix}\\", {use_mvd_str})"', reco_log, reco_output):
        sys.exit(1)
    append_nevents(reco_log)

    # PID
    pid_log = f"{out_prefix}_pid.log"
    pid_output = f"{out_prefix}_pid.root"
    if not run_command(f'root -l -b -q "pid_complete.C({args.nevts}, \\"{out_prefix}\\", {use_mvd_str})"', pid_log, pid_output):
        sys.exit(1)
    append_nevents(pid_log)

    # Analysis for Vertex Fitting
    ana_log = f"{out_prefix}_ana.log"
    ana_output = f"{out_prefix}_vtx_fit.json"
    if not run_command(f'root -l -b -q "ana_dpm.C({args.nevts}, \\"{out_prefix}\\", {use_mvd_str})"', ana_log, ana_output):
        sys.exit(1)
    append_nevents(ana_log)

    # --- Step 2: Re-run with Fitted Vertex ---
    print("\n--- Re-running Reco/PID with Fitted Vertex ---")
    
    json_fit_file = f"{out_prefix}_vtx_fit.json"
    
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
    aod_complete_log = f"{out_prefix}_aod_complete.log"
    aod_complete_output = f"{out_prefix}_pid_poca.root"
    if not run_command(f'root -l -b -q "prod_aod_complete.C(\\"{out_prefix}\\", {use_mvd_str})"', aod_complete_log, aod_complete_output, env=fit_env):
        sys.exit(1)
    append_nevents(aod_complete_log)

    # Re-run final analysis
    ana_complete_log = f"{out_prefix}_ana_complete.log"
    ana_complete_output = f"{out_prefix}_poca.root"
    if not run_command(f'root -l -b -q "ana_complete.C({args.nevts}, \\"{out_prefix}\\", {use_mvd_str})"', ana_complete_log, ana_complete_output, env=fit_env):
        sys.exit(1)
    append_nevents(ana_complete_log)

    print("\nWorkflow completed successfully.")

if __name__ == '__main__':
    main()
