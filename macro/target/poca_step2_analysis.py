import os
import sys
import json
import shutil
from shared_utils import run_command, parse_arguments

def check_and_run(command, log_file, temp_output_file, final_output_file, env=None):
    """Checks if the final output file exists before running a command."""
    if os.path.exists(final_output_file):
        print(f"Final output file {final_output_file} already exists. Skipping command.")
        return True
    return run_command(command, log_file, temp_output_file, env=env)

def get_vertex_from_file(json_file):
    """
    Reads vertex position from a single JSON file and returns it as a dictionary
    for environment variables.
    """
    print(f"Reading vertex information from {json_file}")
    try:
        with open(json_file, 'r') as f:
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
        return fit_env

    except (FileNotFoundError, KeyError, json.JSONDecodeError) as e:
        print(f"Error: Could not read or parse fit results from {json_file}.", file=sys.stderr)
        print(f"Reason: {e}", file=sys.stderr)
        return None

def run_poca_analysis_steps(p, use_mvd_str, out_prefix, unified_log, temp_reco_path, temp_figure_path, final_reco_path, final_figure_path):
    """The actual analysis logic, operating on specific paths."""
    
    # --- Step 1: Get vertex environment ---
    final_vtx_json = os.path.join(final_reco_path, f"{out_prefix}_vtx_fit.json")
    fit_env = get_vertex_from_file(final_vtx_json)
    if not fit_env:
        print("Error: Failed to get fitted vertex. Aborting.", file=sys.stderr)
        sys.exit(1)

    def get_paths(suffix):
        temp_file = os.path.join(temp_reco_path, f"{out_prefix}_{suffix}")
        final_file = os.path.join(final_reco_path, f"{out_prefix}_{suffix}")
        return temp_file, final_file

    # --- Step 2: Run prod_aod_complete.C ---
    temp_aod_output, final_aod_output = get_paths("pid_final.root")
    aod_cmd = (
        f'root -l -b -q "prod_aod_complete.C(\\"{os.path.join(temp_reco_path, out_prefix)}\\", '
        f'\\"fitvertex\\", {use_mvd_str})"'
    )
    if not check_and_run(aod_cmd, unified_log, temp_aod_output, final_aod_output, env=fit_env):
        print(f"Error: POCA AOD completion (prod_aod_complete.C) for {out_prefix} failed.", file=sys.stderr)
        sys.exit(1)

    # --- Step 3: Run ana_complete.C ---
    # Always run analysis step, even if output exists
    # First, ensure all required input files are in the temp directory
    required_files = ["par.root", "pid_final.root"]
    for file_suffix in required_files:
        temp_file = os.path.join(temp_reco_path, f"{out_prefix}_{file_suffix}")
        final_file = os.path.join(final_reco_path, f"{out_prefix}_{file_suffix}")
        
        # If file doesn't exist in temp but exists in final, copy it
        if not os.path.exists(temp_file) and os.path.exists(final_file):
            print(f"Copying required input file {final_file} to {temp_file} for analysis")
            shutil.copy2(final_file, temp_file)
    
    temp_ana_output, final_ana_output = get_paths("poca.root")
    ana_cmd = (
        f'root -l -b -q "ana_complete.C({p.nevts}, \\"{os.path.join(temp_reco_path, out_prefix)}\\", '
        f'{use_mvd_str}, \\"{temp_figure_path}\\", \\"{out_prefix}\\", '
        f'{p.ipx}, {p.ipy}, {p.ipz}, \\"{final_reco_path}/..\\\")"'
    )
    # Use run_command directly instead of check_and_run to force execution
    if not run_command(ana_cmd, unified_log, temp_ana_output, env=fit_env):
        print(f"Error: POCA analysis (ana_complete.C) for {out_prefix} failed.", file=sys.stderr)
        sys.exit(1)

def main():
    """Main execution function."""
    p = parse_arguments("Run the second step of the POCA analysis workflow for a single worker.")
    
    use_mvd_str = "true" if str(p.use_mvd_hvmaps).lower() == 'true' else "false"
    print(f"--- Starting POCA Step 2. Using MVD HVMAPS: {use_mvd_str} ---")

    # --- Path Setup for Worker ---
    slurm_job_id = os.environ.get('SLURM_JOB_ID', 'localjob')
    slurm_task_id = os.environ.get('SLURM_ARRAY_TASK_ID', '0')

    # Final paths on shared storage
    final_base_path = os.path.join(p.output_path, p.prefix)
    final_log_path = os.path.join(final_base_path, 'log')
    final_reco_path = os.path.join(final_base_path, 'reco')
    final_figure_path = os.path.join(final_base_path, 'figure')

    # Temporary paths on the worker node
    temp_dir_name = f"{p.prefix}_mc_{slurm_job_id}_{slurm_task_id}"
    temp_base_path = os.path.join("/tmp", temp_dir_name)
    print(f"--- MC Worker mode detected. Using temporary path for reco/figure: {temp_base_path} ---")
    
    temp_reco_path = os.path.join(temp_base_path, 'reco')
    temp_figure_path = os.path.join(temp_base_path, 'figure')
    
    os.makedirs(final_log_path, exist_ok=True)
    os.makedirs(temp_reco_path, exist_ok=True)
    os.makedirs(temp_figure_path, exist_ok=True)

    # --- Naming and Execution ---
    prefix_parts = p.prefix.split('_')
    naming_prefix = f"{prefix_parts[0]}_{prefix_parts[1]}" if len(prefix_parts) >= 2 else p.prefix
    out_prefix = f"{naming_prefix}_{slurm_task_id}"
    
    print(f"\n--- Starting POCA Worker Job {slurm_task_id} for Prefix: {p.prefix} (using file prefix: {out_prefix}) ---")
    
    # Create unified log file for this analysis step
    unified_log = os.path.join(final_log_path, f"{out_prefix}_analysis.log")
    
    try:
        # The MC step no longer needs the POCA results from the shared drive,
        # as it's a self-contained simulation from start to finish.
        # The old logic for copying files is removed.

        run_poca_analysis_steps(p, use_mvd_str, out_prefix, unified_log, temp_reco_path, temp_figure_path, final_reco_path, final_figure_path)
    finally:
        # Copy results from /tmp to final destination
        print(f"--- Copying POCA results from {temp_base_path} to {final_base_path} ---")
        try:
            os.makedirs(final_reco_path, exist_ok=True)
            os.makedirs(final_figure_path, exist_ok=True)
            
            if os.path.exists(temp_reco_path):
                shutil.copytree(temp_reco_path, final_reco_path, dirs_exist_ok=True)

            if os.path.exists(temp_figure_path):
                shutil.copytree(temp_figure_path, final_figure_path, dirs_exist_ok=True)

            print("--- Copy complete. Cleaning up temporary directory. ---")
            shutil.rmtree(temp_base_path)
        except Exception as e:
            print(f"Error during cleanup/copy for MC worker {slurm_task_id}: {e}", file=sys.stderr)

if __name__ == '__main__':
    main()
