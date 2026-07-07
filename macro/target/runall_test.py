#!/usr/bin/env python3
"""
Python script to run the complete analysis chain for rest gas determination.

This script orchestrates the simulation, digitization, reconstruction, and analysis workflow.
It supports a two-stage scatter-gather workflow on SLURM using job arrays.
- 'worker'/'rerun_worker' modes: Run in parallel within a SLURM array job.
- 'merger' mode: Runs after workers are done to merge results and perform final analysis.
  The '--merger-stage' flag controls which merge step is performed for POCA workflows.
"""
import os
import sys
import subprocess
import argparse
import shlex
import json
from multiprocessing import Pool
from glob import glob


def run_command(command, log_file, output_file=None, env=None, check_output=True):
    """
    Executes a command and logs its output.
    Skips if output_file exists and check_output is True.
    """
    if check_output and output_file and os.path.exists(output_file):
        print(f"Output file {output_file} already exists. Skipping command.")
        print(f"  Command: {command}")
        return True
        
    print(f"Executing: {command}")
    print(f"Logging to: {log_file}")
    try:
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
        print(f"Error: Command not found. Make sure it is installed and in your PATH.", file=sys.stderr)
        with open(log_file, 'w') as f:
            f.write(f"Error: Command not found during execution of: {command}")
        return False

def merge_root_files(output_file, input_files, log_file):
    """Merges ROOT files using hadd."""
    if not input_files:
        print("No input files found to merge. Skipping.", file=sys.stderr)
        return False
    
    if os.path.exists(output_file):
         print(f"Merged file {output_file} already exists. Skipping merge.")
         return True

    command = f"hadd -f {output_file} {' '.join(input_files)}"
    return run_command(command, log_file, check_output=False)


def load_config(config_file):
    """Loads configuration from a JSON file."""
    if config_file and os.path.exists(config_file):
        print(f"Loading configuration from {config_file}")
        with open(config_file, 'r') as f:
            return json.load(f)
    print(f"Warning: Configuration file '{config_file}' not found. Using defaults and command-line arguments.")
    return {}

# --- Re-structured Workflow Functions ---

def poca_workflow_worker(p, use_mvd_str, out_prefix, log_path, reco_path, nevts_per_job):
    """Runs the parallelizable parts of the POCA workflow (Pass 1)."""
    print(f"\n--- Running POCA WORKER (Pass 1) for sub-job: {out_prefix} ---")
    sim_log, sim_output = os.path.join(log_path, f"{out_prefix}_sim.log"), os.path.join(reco_path, f"{out_prefix}_sim.root")
    sim_cmd = (f'root -l -q -b "prod_sim_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {nevts_per_job}, \\"{p.dec}\\", {p.mom}, {use_mvd_str}, '
               f'{p.ipx}, {p.ipy}, {p.ipz}, {p.use_restgas}, {p.theta_min}, {p.theta_max})"')
    if not run_command(sim_cmd, sim_log, sim_output): return False

    digi_log, digi_output = os.path.join(log_path, f"{out_prefix}_digi.log"), os.path.join(reco_path, f"{out_prefix}_digi.root")
    if not run_command(f'root -l -b -q "prod_aod_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str})"', digi_log, digi_output): return False

    reco_log, reco_output = os.path.join(log_path, f"{out_prefix}_reco.log"), os.path.join(reco_path, f"{out_prefix}_reco.root")
    if not run_command(f'root -l -b -q "reco_complete.C({nevts_per_job}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str})"', reco_log, reco_output): return False
    
    pid_log, pid_output = os.path.join(log_path, f"{out_prefix}_pid.log"), os.path.join(reco_path, f"{out_prefix}_pid.root")
    if not run_command(f'root -l -b -q "pid_complete.C({nevts_per_job}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str})"', pid_log, pid_output): return False
    return True

def poca_merger_stage1(p, use_mvd_str, log_path, reco_path, figure_path):
    """POCA MERGER (Stage 1): Merges initial results and fits the vertex."""
    print("\n--- Running POCA MERGER (Stage 1): Vertex Fitting ---")
    merged_pid_file = os.path.join(reco_path, f"{p.prefix}_pid_merged.root")
    pid_files_to_merge = sorted(glob(os.path.join(reco_path, f"{p.prefix}_*_pid.root")))
    merge_log = os.path.join(log_path, f"{p.prefix}_merge_pid.log")
    if not merge_root_files(merged_pid_file, pid_files_to_merge, merge_log): return False

    ana_log = os.path.join(log_path, f"{p.prefix}_ana_vtx.log")
    ana_output = os.path.join(reco_path, f"{p.prefix}_vtx_fit.json")
    merged_prefix_path = os.path.join(reco_path, f"{p.prefix}_pid_merged")
    ana_dpm_cmd = f'root -l -b -q "ana_dpm.C({p.nevts}, \\"{merged_prefix_path}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{p.prefix}\\")"'
    if not run_command(ana_dpm_cmd, ana_log, ana_output): return False
    return True
    
def poca_workflow_rerun_worker(p, use_mvd_str, out_prefix, log_path, reco_path):
    """POCA WORKER (Pass 2): Re-runs AOD using the fitted vertex."""
    print(f"\n--- Running POCA RE-RUN WORKER (Pass 2) for sub-job: {out_prefix} ---")
    json_fit_file = os.path.join(reco_path, f"{p.prefix}_vtx_fit.json")
    try:
        with open(json_fit_file, 'r') as f: fit_data = json.load(f)
        fit_env = {
            'FIT_VERTEX_X': str(fit_data['vertex_x']['mean']),
            'FIT_VERTEX_Y': str(fit_data['vertex_y']['mean']),
            'FIT_VERTEX_Z': str(fit_data['vertex_z']['mean'])
        }
    except Exception as e:
        print(f"FATAL: Worker could not read vertex fit file {json_fit_file}: {e}", file=sys.stderr)
        return False

    aod_complete_log = os.path.join(log_path, f"{out_prefix}_aod_complete.log")
    aod_complete_output = os.path.join(reco_path, f"{out_prefix}_pid_poca.root")
    worker_prefix_path = os.path.join(reco_path, out_prefix)
    if not run_command(f'root -l -b -q "prod_aod_complete.C(\\"{worker_prefix_path}\\", \\"fitvertex\\", {use_mvd_str})"', aod_complete_log, aod_complete_output, env=fit_env): return False
    return True

def poca_merger_stage2(p, use_mvd_str, log_path, reco_path, figure_path):
    """POCA MERGER (Stage 2): Merges final results and runs final analysis."""
    print("\n--- Running POCA MERGER (Stage 2): Final Analysis ---")
    merged_aod_final_file = os.path.join(reco_path, f"{p.prefix}_pid_poca_merged.root")
    aod_final_files_to_merge = sorted(glob(os.path.join(reco_path, f"{p.prefix}_*_pid_poca.root")))
    merge_aod_log = os.path.join(log_path, f"{p.prefix}_merge_poca.log")
    if not merge_root_files(merged_aod_final_file, aod_final_files_to_merge, merge_aod_log): return False
    
    json_fit_file = os.path.join(reco_path, f"{p.prefix}_vtx_fit.json")
    try:
        with open(json_fit_file, 'r') as f: fit_data = json.load(f)
        fit_env = { 'FIT_VERTEX_X': str(fit_data['vertex_x']['mean']), 'FIT_VERTEX_Y': str(fit_data['vertex_y']['mean']), 'FIT_VERTEX_Z': str(fit_data['vertex_z']['mean']) }
    except Exception as e:
        print(f"Error reading fit file {json_fit_file}: {e}", file=sys.stderr)
        return False

    ana_complete_log = os.path.join(log_path, f"{p.prefix}_ana_complete.log")
    ana_complete_output = os.path.join(reco_path, f"{p.prefix}_poca.root")
    merged_final_prefix_path = os.path.join(reco_path, f"{p.prefix}_pid_poca_merged")
    ana_complete_cmd = f'root -l -b -q "ana_complete.C({p.nevts}, \\"{merged_final_prefix_path}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{p.prefix}\\")"'
    if not run_command(ana_complete_cmd, ana_complete_log, ana_complete_output, env=fit_env): return False
    return True

def mc_workflow_worker(p, use_mvd_str, out_prefix, log_path, reco_path, nevts_per_job):
    """Runs the parallelizable parts of the MC workflow."""
    print(f"\n--- Running MC WORKER for sub-job: {out_prefix} ---")
    sim_log, sim_output = os.path.join(log_path, f"{out_prefix}_sim.log"), os.path.join(reco_path, f"{out_prefix}_sim.root")
    sim_cmd = (f'root -l -q -b "prod_sim_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {nevts_per_job}, \\"{p.dec}\\", {p.mom}, {use_mvd_str}, '
               f'{p.ipx}, {p.ipy}, {p.ipz}, {p.use_restgas}, {p.theta_min}, {p.theta_max})"')
    if not run_command(sim_cmd, sim_log, sim_output): return False
    
    aod_complete_log = os.path.join(log_path, f"{out_prefix}_aod_complete.log")
    aod_complete_output = os.path.join(reco_path, f"{out_prefix}_pid_poca.root")
    worker_prefix_path = os.path.join(reco_path, out_prefix)
    if not run_command(f'root -l -b -q "prod_aod_complete.C(\\"{worker_prefix_path}\\", \\"mcvertex\\", {use_mvd_str})"', aod_complete_log, aod_complete_output): return False
    return True

def mc_workflow_merger(p, use_mvd_str, log_path, reco_path, figure_path):
    """Merges MC worker results and runs final analysis."""
    print("\n--- Running MC MERGER ---")
    merged_aod_file = os.path.join(reco_path, f"{p.prefix}_pid_poca_merged.root")
    files_to_merge = sorted(glob(os.path.join(reco_path, f"{p.prefix}_*_pid_poca.root")))
    merge_log = os.path.join(log_path, f"{p.prefix}_merge_poca.log")
    if not merge_root_files(merged_aod_file, files_to_merge, merge_log): return False

    ana_complete_log = os.path.join(log_path, f"{p.prefix}_ana_complete.log")
    ana_complete_output = os.path.join(reco_path, f"{p.prefix}_poca.root")
    merged_prefix_path = os.path.join(reco_path, f"{p.prefix}_pid_poca_merged")
    ana_complete_cmd = f'root -l -b -q "ana_complete.C({p.nevts}, \\"{merged_prefix_path}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{p.prefix}\\")"'
    if not run_command(ana_complete_cmd, ana_complete_log, ana_complete_output): return False
    return True


def run_for_config(p):
    """Main controller function for a given configuration."""
    use_mvd_str = p.use_mvd_hvmaps
    print(f"\n--- Starting Workflow for Prefix: {p.prefix} from config: {p.configfile} ---")
    print(f"--- Running in MODE: {p.mode} (Stage: {p.merger_stage if p.mode == 'merger' else 'N/A'}) ---")
    
    base_path, log_path, reco_path, figure_path = [os.path.join(p.output_path, p.prefix, d) for d in ['', 'log', 'reco', 'figure']]
    os.makedirs(log_path, exist_ok=True); os.makedirs(reco_path, exist_ok=True); os.makedirs(figure_path, exist_ok=True)
    success = False

    if p.mode in ['worker', 'rerun_worker']:
        task_id = os.environ.get('SLURM_ARRAY_TASK_ID', '1')
        out_prefix = f"{p.prefix}_{task_id}"
        if p.nevts % p.num_array_jobs != 0:
            print(f"Warning: Total events ({p.nevts}) is not divisible by number of jobs ({p.num_array_jobs}).", file=sys.stderr)
        nevts_per_job = p.nevts // p.num_array_jobs

        if p.mode == 'worker':
            if p.back_prop_vertex == 'poca': success = poca_workflow_worker(p, use_mvd_str, out_prefix, log_path, reco_path, nevts_per_job)
            else: success = mc_workflow_worker(p, use_mvd_str, out_prefix, log_path, reco_path, nevts_per_job)
        else: # rerun_worker
            if p.back_prop_vertex == 'poca': success = poca_workflow_rerun_worker(p, use_mvd_str, out_prefix, log_path, reco_path)

    elif p.mode == 'merger':
        config_save_path = os.path.join(base_path, 'config.json')
        with open(config_save_path, 'w') as f: json.dump({k: v for k, v in vars(p).items() if k not in ['configfiles', 'configfile', 'jobs']}, f, indent=4)
        print(f"Saved final configuration to {config_save_path}")

        if p.back_prop_vertex == 'poca':
            if p.merger_stage == 1: success = poca_merger_stage1(p, use_mvd_str, log_path, reco_path, figure_path)
            else: success = poca_merger_stage2(p, use_mvd_str, log_path, reco_path, figure_path)
        else: # mc (only has one merger stage)
            success = mc_workflow_merger(p, use_mvd_str, log_path, reco_path, figure_path)
    
    else:
        print(f"Error: Invalid mode '{p.mode}'.", file=sys.stderr); sys.exit(1)

    if success: print(f"\n--- Mode '{p.mode}' for Prefix: {p.prefix} completed successfully. ---")
    else: print(f"\n--- Mode '{p.mode}' for Prefix: {p.prefix} FAILED. ---", file=sys.stderr); sys.exit(1)


def main():
    conf_parser = argparse.ArgumentParser(add_help=False)
    conf_parser.add_argument('configfiles', type=str, nargs='*', default=['config.json'], help='Path to config JSON file(s) or directories.')
    conf_parser.add_argument('--mode', choices=['worker', 'merger', 'rerun_worker'], default='worker', help='Execution mode for SLURM jobs.')
    conf_parser.add_argument('--num-array-jobs', type=int, default=1, help='Total number of jobs in the SLURM array.')
    conf_parser.add_argument('--merger-stage', type=int, choices=[1, 2], default=1, help='Which merger stage to run for POCA workflow.')
    conf_args, _ = conf_parser.parse_known_args()
    
    parser = argparse.ArgumentParser(description='Run the complete analysis chain.', parents=[conf_parser])
    # Define all other arguments...
    parser.add_argument('--prefix', type=str); parser.add_argument('--nevts', type=int); parser.add_argument('--dec', type=str)
    parser.add_argument('--mom', type=float); parser.add_argument('--use_mvd_hvmaps', choices=['true', 'false'])
    parser.add_argument('--ipx', type=float); parser.add_argument('--ipy', type=float); parser.add_argument('--ipz', type=float)
    parser.add_argument('--use_restgas', choices=['true', 'false']); parser.add_argument('--theta_min', type=float)
    parser.add_argument('--theta_max', type=float); parser.add_argument('--back_prop_vertex', choices=['poca', 'mc'])
    parser.add_argument('--output_path', type=str)

    config_path = conf_args.configfiles[0] if conf_args.configfiles else 'config.json'
    if os.path.isdir(config_path):
        config_file = sorted(glob(os.path.join(config_path, '*.json')))[0]
    else:
        config_file = config_path

    if not os.path.exists(config_file): print(f"Error: Config file not found at '{config_file}'", file=sys.stderr); sys.exit(1)

    params = {
        'prefix': 'test', 'nevts': 1000, 'dec': 'pp_dd', 'mom': 4.06, 'use_mvd_hvmaps': 'false',
        'ipx': 0.0, 'ipy': 0.0, 'ipz': 0.0, 'use_restgas': 'false', 'theta_min': 0.0, 'theta_max': 180.0,
        'back_prop_vertex': 'poca', 'output_path': 'data'
    }
    config = load_config(config_file)
    params.update(config)
    parser.set_defaults(**params)
    
    p = parser.parse_args()
    p.configfile = config_file
    
    run_for_config(p)
    print("\n--- Analysis script finished ---")

if __name__ == '__main__':
    main()
