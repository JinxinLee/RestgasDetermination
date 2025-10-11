import os
import sys
import json
import shutil
from shared_utils import run_command, parse_arguments



def run_poca_analysis_steps(p, use_mvd_str, reco_path, figure_path, log_path):
    """The actual analysis logic, operating on specific paths."""
    
    # --- Step 1: Read vertex from the user-provided JSON and set environment ---
    # The JSON file is expected to have been copied to the temp reco_path.
    final_vtx_json = os.path.join(reco_path, f"{p.prefix}_vtx_fit.json")
    fit_env = get_vertex_from_file(final_vtx_json)
    if not fit_env:
        print("Error: Failed to get fitted vertex. Aborting.", file=sys.stderr)
        sys.exit(1)

    # --- Step 2: Run prod_aod_complete.C with fitted vertex ---
    print("\n--- Running prod_aod_complete.C with fitted vertex ---")
    aod_complete_log = os.path.join(log_path, "prod_aod_complete.log")
    aod_complete_output = os.path.join(reco_path, f"{p.prefix}_pid_poca.root")
    # The input files for this macro (e.g., pid.root) should have been copied to the temp dir.
    aod_complete_cmd = (
        f'root -l -b -q "prod_aod_complete.C(\\"{os.path.join(reco_path, p.prefix)}\\", '
        f'\\"fitvertex\\", {use_mvd_str})"'
    )
    if not run_command(aod_complete_cmd, aod_complete_log, aod_complete_output, env=fit_env):
        print("Error: prod_aod_complete.C failed.", file=sys.stderr)
        sys.exit(1)
    print("--- prod_aod_complete.C finished. ---")

    # --- Step 3: Run ana_complete.C ---
    print("\n--- Running ana_complete.C ---")
    ana_complete_log = os.path.join(log_path, "ana_complete.log")
    ana_complete_output = os.path.join(reco_path, f"{p.prefix}_poca.root")
    ana_complete_cmd = (
        f'root -l -b -q "ana_complete.C({p.nevts}, \\"{os.path.join(reco_path, p.prefix)}\\", '
        f'{use_mvd_str}, \\"{figure_path}\\", \\"{p.prefix}\\")"'
    )
    if not run_command(ana_complete_cmd, ana_complete_log, ana_complete_output, env=fit_env):
        print("Error: ana_complete.C failed.", file=sys.stderr)
        sys.exit(1)
    print("--- ana_complete.C finished. ---")


def main():
    """Main execution function."""
    p = parse_arguments("Run the POCA analysis steps.")
    use_mvd_str = 'true' if p.use_mvd_hvmaps else 'false'

    # --- Path Setup ---
    slurm_job_id = os.environ.get('SLURM_JOB_ID', 'localjob_ana')
    
    # Final paths on shared storage
    final_base_path = os.path.join(p.output_path, p.prefix)
    final_log_path = os.path.join(final_base_path, 'log')
    final_reco_path = os.path.join(final_base_path, 'reco')
    final_figure_path = os.path.join(final_base_path, 'figure')
    os.makedirs(final_log_path, exist_ok=True)
    os.makedirs(final_reco_path, exist_ok=True)
    os.makedirs(final_figure_path, exist_ok=True)

    # Temporary paths on the worker node
    temp_dir_name = f"{p.prefix}_ana_{slurm_job_id}"
    temp_base_path = os.path.join("/tmp", temp_dir_name)
    print(f"--- Analysis job. Using temporary path for reco/figure: {temp_base_path} ---")
    
    temp_reco_path = os.path.join(temp_base_path, 'reco')
    temp_figure_path = os.path.join(temp_base_path, 'figure')
    os.makedirs(temp_reco_path, exist_ok=True)
    os.makedirs(temp_figure_path, exist_ok=True)

    try:
        # --- Prepare inputs in temp directory ---
        print(f"--- Preparing inputs by copying from {final_reco_path} to {temp_reco_path} ---")
        # We need the merged root files and the vertex fit json for the analysis.
        files_to_copy = [
            f"{p.prefix}_sim.root", 
            f"{p.prefix}_digi.root", 
            f"{p.prefix}_reco.root", 
            f"{p.prefix}_pid.root",
            f"{p.prefix}_vtx_fit.json"
        ]
        for fname in files_to_copy:
            src = os.path.join(final_reco_path, fname)
            dst = os.path.join(temp_reco_path, fname)
            if os.path.exists(src):
                print(f"Copying {src} to {dst}")
                shutil.copy(src, dst)
            else:
                # The vertex fit json is critical, so we exit if it's not found.
                if fname.endswith(".json"):
                    print(f"Error: Critical input file {src} not found!", file=sys.stderr)
                    sys.exit(1)
                print(f"Warning: Input file {src} not found. It might not be needed for this step.", file=sys.stderr)

        print("\n--- Starting POCA Step 2 Analysis in temporary directory ---")
        run_poca_analysis_steps(p, use_mvd_str, temp_reco_path, temp_figure_path, final_log_path)
        print("\n--- POCA Step 2 Analysis completed successfully. ---")

    finally:
        # --- Copy results back to final destination ---
        print(f"--- Copying results from {temp_base_path} to {final_base_path} ---")
        try:
            # dirs_exist_ok=True is crucial for python 3.8+
            if os.path.exists(temp_reco_path):
                shutil.copytree(temp_reco_path, final_reco_path, dirs_exist_ok=True)
            
            if os.path.exists(temp_figure_path):
                shutil.copytree(temp_figure_path, final_figure_path, dirs_exist_ok=True)

            print("--- Copy complete. Cleaning up temporary directory. ---")
            shutil.rmtree(temp_base_path)
        except Exception as e:
            print(f"Error during cleanup/copy for analysis job: {e}", file=sys.stderr)

if __name__ == '__main__':
    main()

import os
import sys
import json
import shutil
from shared_utils import run_command, parse_arguments

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

def run_poca_worker_step2(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path):
    """
    Runs the actual analysis logic for POCA Step 2.
    This function is designed to be called from main() and operates in the temp directory.
    """
    print(f"--- Running POCA Analysis Step 2 for {out_prefix} ---")

    # --- Step 1: Read vertex from the user-provided JSON and set environment ---
    # The JSON file is expected to have been copied to the temp reco_path.
    vtx_json_path = os.path.join(reco_path, f"{out_prefix}_vtx_fit.json")
    fit_env = get_vertex_from_file(vtx_json_path)
    if not fit_env:
        print("Error: Failed to get fitted vertex. Aborting.", file=sys.stderr)
        sys.exit(1)

    # --- Step 2: Run prod_aod_complete.C with fitted vertex ---
    aod_complete_log = os.path.join(log_path, f"{out_prefix}_aod_complete.log")
    aod_complete_output = os.path.join(reco_path, f"{out_prefix}_pid_poca.root")
    # For MC, we use the "mcvertex" option
    aod_complete_cmd = (
        f'root -l -b -q "prod_aod_complete.C(\\"{os.path.join(reco_path, out_prefix)}\\", '
        f'\\"fitvertex\\", {use_mvd_str})"'
    )
    if not run_command(aod_complete_cmd, aod_complete_log, aod_complete_output):
        print(f"Error: MC AOD completion (prod_aod_complete.C) for {out_prefix} failed.", file=sys.stderr)
        sys.exit(1)

    # --- Step 3: Final Analysis ---
    ana_complete_log = os.path.join(log_path, f"{out_prefix}_ana_complete.log")
    ana_complete_output = os.path.join(reco_path, f"{out_prefix}_poca.root")
    ana_complete_cmd = (
        f'root -l -b -q "ana_complete.C({p.nevts_mc}, \\"{os.path.join(reco_path, out_prefix)}\\", '
        f'{use_mvd_str}, \\"{figure_path}\\", \\"{out_prefix}\\")"'
    )
    if not run_command(ana_complete_cmd, ana_complete_log, ana_complete_output):
        print(f"Error: MC final analysis (ana_complete.C) for {out_prefix} failed.", file=sys.stderr)
        sys.exit(1)

    print(f"--- MC Worker for {out_prefix} finished. ---")

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
    
    log_path = final_log_path
    reco_path = os.path.join(temp_base_path, 'reco')
    figure_path = os.path.join(temp_base_path, 'figure')
    
    os.makedirs(log_path, exist_ok=True)
    os.makedirs(reco_path, exist_ok=True)
    os.makedirs(figure_path, exist_ok=True)

    # --- Naming and Execution ---
    prefix_parts = p.prefix.split('_')
    naming_prefix = f"{prefix_parts[0]}_{prefix_parts[1]}" if len(prefix_parts) >= 2 else p.prefix
    out_prefix = f"{naming_prefix}_{slurm_task_id}"
    
    print(f"\n--- Starting POCA Worker Job {slurm_task_id} for Prefix: {p.prefix} (using file prefix: {out_prefix}) ---")
    
    try:
        # The MC step no longer needs the POCA results from the shared drive,
        # as it's a self-contained simulation from start to finish.
        # The old logic for copying files is removed.

        run_poca_worker_step2(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path)
    finally:
        # Copy results from /tmp to final destination
        print(f"--- Copying POCA results from {temp_base_path} to {final_base_path} ---")
        try:
            os.makedirs(final_reco_path, exist_ok=True)
            os.makedirs(final_figure_path, exist_ok=True)
            
            if os.path.exists(reco_path):
                shutil.copytree(reco_path, final_reco_path, dirs_exist_ok=True)
            
            if os.path.exists(figure_path):
                shutil.copytree(figure_path, final_figure_path, dirs_exist_ok=True)

            print("--- Copy complete. Cleaning up temporary directory. ---")
            shutil.rmtree(temp_base_path)
        except Exception as e:
            print(f"Error during cleanup/copy for MC worker {slurm_task_id}: {e}", file=sys.stderr)

if __name__ == '__main__':
    main()
