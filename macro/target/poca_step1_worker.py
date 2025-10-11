import os
import sys
import shutil
from shared_utils import run_command, get_generated_events, parse_arguments

def run_poca_worker_step1(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path):
    """Runs the first step of the POCA workflow for a single worker job."""
    print(f"--- Running POCA Worker Step 1 for {out_prefix} ---")

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

    # Analysis for Vertex Fitting (ana_dpm.C)
    ana_log = os.path.join(log_path, f"{out_prefix}_ana_dpm.log")
    ana_output = os.path.join(reco_path, f"{out_prefix}_vtx_fit.json")
    ana_dpm_cmd = f'root -l -b -q "ana_dpm.C({p.nevts}, \\"{os.path.join(reco_path, out_prefix)}\\", {use_mvd_str}, \\"{figure_path}\\", \\"{out_prefix}\\")"'
    if not run_command(ana_dpm_cmd, ana_log, ana_output):
        sys.exit(1)
    append_nevents(ana_log)
    
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
        run_poca_worker_step1(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path)
    finally:
        # Copy results from /tmp to final destination
        print(f"--- Copying reco/figure results from {temp_base_path} to {final_base_path} ---")
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
            print(f"Error during cleanup/copy for worker {slurm_task_id}: {e}", file=sys.stderr)

if __name__ == '__main__':
    main()
