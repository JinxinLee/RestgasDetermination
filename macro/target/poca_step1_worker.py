import os
import sys
import shutil
from shared_utils import run_command, get_generated_events, parse_arguments

def check_and_run(command, log_file, temp_output_file, final_output_file):
    """Checks if the final output file exists before running a command."""
    if os.path.exists(final_output_file):
        print(f"Final output file {final_output_file} already exists. Skipping command.")
        return True
    return run_command(command, log_file, temp_output_file)

def run_poca_worker_step1(p, use_mvd_str, out_prefix, unified_log, temp_reco_path, temp_figure_path, final_reco_path, final_figure_path):
    """Runs the first step of the POCA workflow for a single worker job."""
    print(f"--- Running POCA Worker Step 1 for {out_prefix} ---")

    def get_paths(suffix):
        temp_file = os.path.join(temp_reco_path, f"{out_prefix}_{suffix}")
        final_file = os.path.join(final_reco_path, f"{out_prefix}_{suffix}")
        return temp_file, final_file

    # --- Simulation ---
    temp_sim_output, final_sim_output = get_paths("sim.root")
    sim_command = (
        f'root -l -q -b "prod_sim_hvmaps.C(\\"{os.path.join(temp_reco_path, out_prefix)}\\", {p.nevts}, \\"{p.dec}\\", {p.mom}, {use_mvd_str}, '
        f'{p.ipx}, {p.ipy}, {p.ipz}, {p.use_restgas}, {p.theta_min}, {p.theta_max}, \\"{p.restgas_profile}\\")"'
    )
    if not check_and_run(sim_command, unified_log, temp_sim_output, final_sim_output):
        sys.exit(1)

    # --- Digitization ---
    temp_digi_output, final_digi_output = get_paths("digi.root")
    digi_cmd = f'root -l -b -q "prod_aod_hvmaps.C(\\"{os.path.join(temp_reco_path, out_prefix)}\\", {use_mvd_str})"'
    if not check_and_run(digi_cmd, unified_log, temp_digi_output, final_digi_output):
        sys.exit(1)

    # --- Reconstruction ---
    temp_reco_output, final_reco_output = get_paths("reco.root")
    reco_cmd = f'root -l -b -q "reco_complete.C({p.nevts}, \\"{os.path.join(temp_reco_path, out_prefix)}\\", {use_mvd_str})"'
    if not check_and_run(reco_cmd, unified_log, temp_reco_output, final_reco_output):
        sys.exit(1)

    # --- PID ---
    temp_pid_output, final_pid_output = get_paths("pid.root")
    pid_cmd = f'root -l -b -q "pid_complete.C({p.nevts}, \\"{os.path.join(temp_reco_path, out_prefix)}\\", {use_mvd_str})"'
    if not check_and_run(pid_cmd, unified_log, temp_pid_output, final_pid_output):
        sys.exit(1)

    # --- Analysis for Vertex Fitting (ana_dpm.C) ---
    temp_ana_output, final_ana_output = get_paths("vtx_fit.json")
    ana_dpm_cmd = f'root -l -b -q "ana_dpm.C({p.nevts}, \\"{os.path.join(temp_reco_path, out_prefix)}\\", {use_mvd_str}, \\"{temp_figure_path}\\", \\"{out_prefix}\\", {p.mom})"'
    if not check_and_run(ana_dpm_cmd, unified_log, temp_ana_output, final_ana_output):
        sys.exit(1)
    
    print(f"--- POCA Worker Step 1 for {out_prefix} finished. ---")


def main():
    """Main execution function."""
    p = parse_arguments("Run the first step of the POCA analysis workflow for a single worker.")
    
    use_mvd_str = "true" if str(p.use_mvd_hvmaps).lower() == 'true' else "false"
    print(f"--- Starting POCA Step 1. Using MVD HVMAPS: {use_mvd_str} ---")

    # --- Path Setup for Worker ---
    slurm_job_id = os.environ.get('SLURM_JOB_ID', 'localjob')
    slurm_task_id = os.environ.get('SLURM_ARRAY_TASK_ID', '0')

    # Final paths on shared storage
    final_base_path = os.path.join(p.output_path, p.prefix)
    final_log_path = os.path.join(final_base_path, 'log')
    final_reco_path = os.path.join(final_base_path, 'reco')
    final_figure_path = os.path.join(final_base_path, 'figure')

    # Temporary paths on the worker node
    temp_dir_name = f"{p.prefix}_{slurm_job_id}_{slurm_task_id}"
    temp_base_path = os.path.join("/tmp", temp_dir_name)
    print(f"--- Worker mode detected. Using temporary path for reco/figure: {temp_base_path} ---")
    
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
    
    # Create unified log file for this worker
    unified_log = os.path.join(final_log_path, f"{out_prefix}_worker.log")
    
    try:
        run_poca_worker_step1(p, use_mvd_str, out_prefix, unified_log, temp_reco_path, temp_figure_path, final_reco_path, final_figure_path)
    finally:
        # Copy results from /tmp to final destination
        print(f"--- Copying reco/figure results from {temp_base_path} to {final_base_path} ---")
        try:
            os.makedirs(final_reco_path, exist_ok=True)
            os.makedirs(final_figure_path, exist_ok=True)
            
            # Copy reco files
            if os.path.exists(temp_reco_path):
                for item in os.listdir(temp_reco_path):
                    s = os.path.join(temp_reco_path, item)
                    d = os.path.join(final_reco_path, item)
                    if os.path.isfile(s):
                        shutil.copy2(s, d)

            # Copy figure files
            if os.path.exists(temp_figure_path):
                for item in os.listdir(temp_figure_path):
                    s = os.path.join(temp_figure_path, item)
                    d = os.path.join(final_figure_path, item)
                    if os.path.isfile(s):
                        shutil.copy2(s, d)

            print("--- Copy complete. Cleaning up temporary directory. ---")
            shutil.rmtree(temp_base_path)
        except Exception as e:
            print(f"Error during cleanup/copy for worker {slurm_task_id}: {e}", file=sys.stderr)

if __name__ == '__main__':
    main()
