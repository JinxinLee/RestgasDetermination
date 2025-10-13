import os
import sys
import subprocess
import json
from shared_utils import parse_arguments, get_default_params, load_config

def submit_sbatch(command, dep_job_id=None):
    """Submits a job to Slurm and returns the job ID."""
    if dep_job_id:
        command = f"sbatch --dependency=afterany:{dep_job_id} {command}"
    else:
        command = f"sbatch {command}"
    
    print(f"Submitting command: {command}")
    try:
        result = subprocess.run(command, shell=True, check=True, capture_output=True, text=True)
        job_id = result.stdout.strip().split()[-1]
        print(f"Successfully submitted job with ID: {job_id}")
        return job_id
    except subprocess.CalledProcessError as e:
        print(f"Error during submission for command: '{command}'", file=sys.stderr)
        print(f"sbatch stderr: {e.stderr}", file=sys.stderr)
        print(f"sbatch stdout: {e.stdout}", file=sys.stderr)
        sys.exit(1)

def main():
    """Main workflow orchestration function."""
    tasks = parse_arguments("Run the full analysis workflow using Slurm.", is_workflow_runner=True)
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    submit_script_path = os.path.join(script_dir, 'submit_sbatch.sh')

    for p in tasks:
        print(f"\n--- Processing config: {p.configfile} ---")
        # Create the full prefix and output directories
        #p.prefix = f"{p.dec}_{p.mom}GeV_{p.config.split('/')[-1].replace('.json', '')}"
        base_path = os.path.join(p.output_path, p.prefix)
        os.makedirs(base_path, exist_ok=True)
        
        # Save the configuration for this run
        config_path = os.path.join(base_path, 'config.json')
        # Create a dictionary from the namespace for saving
        config_to_save = vars(p)
        with open(config_path, 'w') as f:
            json.dump(config_to_save, f, indent=4)
        print(f"Saved configuration to {config_path}")

        # Construct the common arguments for the python scripts
        common_args = f"--config {config_path}"

        if p.back_prop_vertex == 'poca':
            print("\n--- Submitting POCA Workflow ---")
            # --- Step 1: POCA Worker Jobs (Array) ---
            print("--- Submitting POCA Step 1 Workers ---")
            poca_worker_cmd = f"--array=0-{p.njobs-1} {submit_script_path} python3 -u {os.path.join(script_dir, 'poca_step1_worker.py')} {common_args}"
            poca_worker_job_id = submit_sbatch(poca_worker_cmd)

            # --- Step 2: Merge POCA Files ---
            print("--- Submitting POCA Merge Job ---")
            poca_merge_cmd = f"{submit_script_path} python3 -u {os.path.join(script_dir, 'merge.py')} {common_args}"
            poca_merge_job_id = submit_sbatch(poca_merge_cmd, dep_job_id=poca_worker_job_id)

            # --- Step 3: POCA Analysis ---
            print("--- Submitting POCA Analysis Job ---")
            poca_ana_cmd = f"{submit_script_path} python3 -u {os.path.join(script_dir, 'poca_step2_analysis.py')} {common_args}"
            poca_ana_job_id = submit_sbatch(poca_ana_cmd, dep_job_id=poca_merge_job_id)
            print(f"POCA workflow submitted. Final analysis will be available after job {poca_ana_job_id} completes.")

        elif p.back_prop_vertex == 'mc':
            print("\n--- Submitting MC Workflow ---")
            # --- Step 1: MC Worker Jobs (Array) ---
            print("--- Submitting MC Step Workers ---")
            mc_worker_cmd = f"--array=0-{p.njobs-1} {submit_script_path} python3 -u {os.path.join(script_dir, 'mc_step_worker.py')} {common_args}"
            mc_worker_job_id = submit_sbatch(mc_worker_cmd)

            # --- Step 2: Merge MC Files ---
            print("--- Submitting MC Merge Job ---")
            mc_merge_cmd = f"{submit_script_path} python3 -u {os.path.join(script_dir, 'merge.py')} {common_args} --file-types par pid_final ana_final"
            mc_merge_job_id = submit_sbatch(mc_merge_cmd, dep_job_id=mc_worker_job_id)
            print(f"MC workflow submitted. Final merged file will be available after job {mc_merge_job_id} completes.")
        
        else:
            print(f"Error: Invalid back_prop_vertex option '{p.back_prop_vertex}' in {p.configfile}. Choose 'poca' or 'mc'.", file=sys.stderr)

    print(f"\n--- All workflows submitted successfully! ---")

if __name__ == '__main__':
    main()
