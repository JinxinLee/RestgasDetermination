import os
import sys
import shutil
from shared_utils import run_command, parse_arguments

def run_mc_worker(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path):
    """Runs the MC simulation and reconstruction for a single worker job."""
    print(f"--- Running MC Worker for {out_prefix} ---")

    # --- Step 1: Simulation ---
    sim_log = os.path.join(log_path, f"{out_prefix}_sim.log")
    sim_output = os.path.join(reco_path, f"{out_prefix}_sim.root")
    sim_command = (
        f'root -l -q -b "prod_sim_hvmaps.C(\\"{os.path.join(reco_path, out_prefix)}\\", {p.nevts_mc}, \\"{p.dec}\\", {p.mom}, {use_mvd_str}, '
        f'{p.ipx}, {p.ipy}, {p.ipz}, {p.use_restgas}, {p.theta_min}, {p.theta_max})"'
    )
    if not run_command(sim_command, sim_log, sim_output):
        print(f"Error: MC simulation (prod_sim_hvmaps.C) for {out_prefix} failed.", file=sys.stderr)
        sys.exit(1)

    # --- Step 2: Combined AOD ---
    aod_complete_log = os.path.join(log_path, f"{out_prefix}_aod_complete.log")
    aod_complete_output = os.path.join(reco_path, f"{out_prefix}_pid_poca.root")
    # For MC, we use the "mcvertex" option
    aod_complete_cmd = (
        f'root -l -b -q "prod_aod_complete.C(\\"{os.path.join(reco_path, out_prefix)}\\", '
        f'\\"mcvertex\\", {use_mvd_str})"'
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
    p = parse_arguments("Run the MC simulation step for a single worker.")
    
    use_mvd_str = 'true' if p.use_mvd_hvmaps else 'false'

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
    
    print(f"\n--- Starting MC Worker Job {slurm_task_id} for Prefix: {p.prefix} (using file prefix: {out_prefix}) ---")
    
    try:
        # The MC step no longer needs the POCA results from the shared drive,
        # as it's a self-contained simulation from start to finish.
        # The old logic for copying files is removed.
        
        run_mc_worker(p, use_mvd_str, out_prefix, log_path, reco_path, figure_path)
    finally:
        # Copy results from /tmp to final destination
        print(f"--- Copying MC results from {temp_base_path} to {final_base_path} ---")
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
