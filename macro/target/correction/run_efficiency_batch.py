import subprocess
import multiprocessing
import os
import time

# ================= Configuration section =================

# Your ROOT macro filename
MACRO_NAME = "efficiency_correction_2.C"
# Ensure the macro is in the current directory or provide an absolute path
MACRO_PATH = os.path.join(os.getcwd(), MACRO_NAME)

# Fixed MC reference files (used to compute efficiency)
MC_REF_TAG = "acc_fullgas"
MC_REF_RANGE = "1,500"  # MC range

# ================= Task generation section =================

DATASETS = []
DATA_RANGE = "1,100" 

# 1. Add a FullGas self-consistency check (as a reference)
DATASETS.append(("acc_fullgas", "acc_fullgas", DATA_RANGE))

# 2. Define tags for the cross-combinations
TAGS = [
    "acc_fullgas",
    "acc_p5",
    "acc_p10",
    "acc_p20",
    "acc_m5",
    "acc_m10",
    "acc_m20"
]

# 3. Generate all combinations (data x MC)
# This will generate all N×N combinations such as p5_p5, p5_m5, p5_p10
for data_tag in TAGS:
    for mc_tag in TAGS:
        DATASETS.append((data_tag, mc_tag, DATA_RANGE))

# If you only want to run specific combinations, you can filter or add them here, for example:
# DATASETS = [("acc_p5", "acc_m5", "1,100"), ("acc_p5", "acc_p10", "1,100")]

# Number of parallel processes (adjust based on your CPU cores)
MAX_WORKERS = 50 

# ===========================================

def run_task(dataset_info):
    data_tag, mc_tag, file_range = dataset_info
    
    # Parse the tag to obtain a short name
    data_short = data_tag.replace("acc_", "")
    mc_short = mc_tag.replace("acc_", "")
    
    # Construct the output filename: {data}_{mc}_log_full
    output_name = f"{data_short}_{mc_short}_log_full"
    
    # Build the file path strings
    # 1. Real data (the data to be corrected) -> use data_tag
    real_data_arg = f"../data/{data_tag}/reco/{data_tag}_%d_ana_final.root,{file_range}"
    
    # 2. MC Rec (the efficiency numerator) -> use mc_tag
    mc_rec_arg = f"../data/{mc_tag}/reco/{mc_tag}_%d_ana_final.root,{MC_REF_RANGE}"
    
    # 3. MC Gen (the efficiency denominator) -> use mc_tag
    mc_gen_arg = f"../data/{mc_tag}/reco/{mc_tag}_%d_sim.root,{MC_REF_RANGE}"
    
    # 4. Real data gen (for validation) -> use data_tag
    real_data_gen_arg = f"../data/{data_tag}/reco/{data_tag}_%d_sim.root,{file_range}"
    
    # Build the full ROOT command
    # Note: the arguments must be wrapped in double quotes and escaped correctly in the shell
    root_cmd = (
        f"root -b -q -l '{MACRO_NAME}("
        f"\"{real_data_arg}\", "
        f"\"{mc_rec_arg}\", "
        f"\"{mc_gen_arg}\", "
        f"\"{real_data_gen_arg}\", "
        f"\"{output_name}\")'"
    )
    
    print(f"Starting: {output_name}")
    print(f"CMD: {root_cmd}")
    
    start_time = time.time()
    
    # Execute the command
    # result = subprocess.run(root_cmd, shell=True, capture_output=True, text=True) # if you want to capture the output
    result = subprocess.run(root_cmd, shell=True)
    
    duration = time.time() - start_time
    
    if result.returncode == 0:
        print(f"✅ Finished {output_name} in {duration:.2f}s")
    else:
        print(f"❌ Failed {output_name} with code {result.returncode}")
        
    return result.returncode

def main():
    print(f"--- Starting Batch Analysis for {len(DATASETS)} datasets with {MAX_WORKERS} workers ---")
    
    # Run the tasks in parallel with a process pool
    with multiprocessing.Pool(processes=MAX_WORKERS) as pool:
        pool.map(run_task, DATASETS)
        
    print("--- All Tasks Completed ---")

if __name__ == "__main__":
    main()
