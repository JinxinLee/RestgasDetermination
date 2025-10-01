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

def run_for_config(p):
    """Runs the entire workflow for a given parameter set."""
    # The argument is already a string "true" or "false"
    use_mvd_str = p.use_mvd_hvmaps
    print(f"\n--- Starting Workflow for Prefix: {p.prefix} from config: {p.configfile} ---")
    for key, value in vars(p).items():
        # Don't print arguments that are not workflow parameters
        if key not in ['configfiles', 'configfile', 'jobs']:
            print(f"{key}: {value}")
    print("-------------------------------------------------")

    # --- Path Setup ---
    # Construct the base path for this specific job prefix
    base_path = os.path.join(p.output_path, p.prefix)
    
    log_path = os.path.join(base_path, 'log')
    reco_path = os.path.join(base_path, 'reco')
    figure_path = os.path.join(base_path, 'figure')

    os.makedirs(log_path, exist_ok=True)
    os.makedirs(reco_path, exist_ok=True)
    os.makedirs(figure_path, exist_ok=True)

    # Save the final configuration for this run
    config_save_path = os.path.join(base_path, 'config.json')
    with open(config_save_path, 'w') as f:
        # Convert Namespace to dict for saving, filter out helper args
        config_to_save = {k: v for k, v in vars(p).items() if k not in ['configfiles', 'configfile', 'jobs']}
        json.dump(config_to_save, f, indent=4)
    print(f"Saved final configuration to {config_save_path}")

    slurm_id = os.environ.get('SLURM_ARRAY_TASK_ID', '1')
    # Note: out_prefix no longer contains the subdirectories
    out_prefix = f"{p.prefix}_{slurm_id}"
    
    # --- Workflow Selection ---
    if p.back_prop_vertex == 'poca':
        run_poca_workflow(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path)
    elif p.back_prop_vertex == 'mc':
        run_mc_workflow(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path)
    else:
        print(f"Error: Invalid back_prop_vertex option '{p.back_prop_vertex}'. Choose 'poca' or 'mc'.", file=sys.stderr)
        sys.exit(1)

    print(f"\n--- Workflow for Prefix: {p.prefix} completed successfully. ---")


def main():
    # --- Argument Parser Setup ---
    # This parser handles the config file path(s) and parallel jobs argument first.
    conf_parser = argparse.ArgumentParser(add_help=False)
    conf_parser.add_argument('configfiles', type=str, nargs='*', default=['config.json'],
                             help='One or more paths to configuration JSON files or directories containing them. Defaults to "config.json".')
    conf_parser.add_argument('-j', '--jobs', type=int, default=1,
                             help='Number of parallel jobs to run. Defaults to 1 (sequential).')
    
    # Parse known args to get the config file paths and any other command-line args.
    conf_args, remaining_argv = conf_parser.parse_known_args()
    
    # --- Main Parser ---
    # This parser defines all possible arguments.
    parser = argparse.ArgumentParser(
        description='Run the complete analysis chain for rest gas determination.',
        parents=[conf_parser] # Inherit arguments for help message
    )
    
    # Define all arguments that can be set via command line or config file
    parser.add_argument('--prefix', type=str, help='Prefix for output files.')
    parser.add_argument('--nevts', type=int, help='Number of events to simulate.')
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
    # Expand directories into a list of config files
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
            expanded_config_files.append(path)

    tasks = []
    for config_file in expanded_config_files:
        # 1. Start with hardcoded defaults
        params = {
            'prefix': 'test', 'nevts': 1000, 'dec': 'pp_dd', 'mom': 4.06,
            'use_mvd_hvmaps': 'false', 'ipx': 0.0, 'ipy': 0.0, 'ipz': 0.0,
            'use_restgas': 'false', 'theta_min': 0.0, 'theta_max': 180.0,
            'back_prop_vertex': 'poca', 'output_path': 'data'
        }

        # 2. Load configuration from the current file
        config = load_config(config_file)
        params.update(config)

        # 3. Override with any command-line arguments
        # We need to parse the remaining_argv for each loop iteration
        # to apply the command-line overrides correctly.
        # We set the defaults to the merged (default + config) params.
        parser.set_defaults(**params)
        # We parse args from remaining_argv, and also add back the configfiles and jobs
        # from the initial parse so they are present in the final namespace.
        p = parser.parse_args(remaining_argv + [
            '--jobs', str(conf_args.jobs)
        ])
        # Add the specific config file used for this task, for reference
        p.configfile = config_file
        tasks.append(p)

    # --- Workflow Execution ---
    num_jobs = conf_args.jobs
    print(f"\nFound {len(tasks)} configuration(s) to run. Starting {num_jobs} parallel job(s).")

    if num_jobs > 1 and len(tasks) > 1:
        # Parallel execution
        with Pool(processes=num_jobs) as pool:
            pool.map(run_for_config, tasks)
    else:
        # Sequential execution
        for task in tasks:
            run_for_config(task)

    print("\nAll workflows completed.")