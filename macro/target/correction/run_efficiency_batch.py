import subprocess
import multiprocessing
import os
import time

# ================= 配置区域 =================

# 你的 ROOT 宏文件名
MACRO_NAME = "efficiency_correction_2.C"
# 确保宏文件在当前目录或提供绝对路径
MACRO_PATH = os.path.join(os.getcwd(), MACRO_NAME)

# 固定的 MC 基准文件 (用于计算效率)
MC_REF_TAG = "acc_fullgas"
MC_REF_RANGE = "1,500"  # MC 范围

# ================= 任务生成区域 =================

DATASETS = []
DATA_RANGE = "1,100" 

# 1. 添加 FullGas 自洽性检验 (作为参考)
DATASETS.append(("acc_fullgas", "acc_fullgas", DATA_RANGE))

# 2. 定义需要交叉组合的 Tags
TAGS = [
    "acc_fullgas",
    "acc_p5",
    "acc_p10",
    "acc_p20",
    "acc_m5",
    "acc_m10",
    "acc_m20"
]

# 3. 生成所有组合 (Data x MC)
# 这将生成 p5_p5, p5_m5, p5_p10 等所有 N*N 种组合
for data_tag in TAGS:
    for mc_tag in TAGS:
        DATASETS.append((data_tag, mc_tag, DATA_RANGE))

# 如果只想运行特定的组合，可以在这里手动过滤或添加，例如：
# DATASETS = [("acc_p5", "acc_m5", "1,100"), ("acc_p5", "acc_p10", "1,100")]

# 并行进程数 (根据你的 CPU 核心数调整)
MAX_WORKERS = 50 

# ===========================================

def run_task(dataset_info):
    data_tag, mc_tag, file_range = dataset_info
    
    # 解析 Tag 获取简短名称
    data_short = data_tag.replace("acc_", "")
    mc_short = mc_tag.replace("acc_", "")
    
    # 构造输出文件名: {data}_{mc}_log_full
    output_name = f"{data_short}_{mc_short}_log_full"
    
    # 构造文件路径字符串
    # 1. Real Data (待修正的数据) -> 使用 data_tag
    real_data_arg = f"../data/{data_tag}/reco/{data_tag}_%d_ana_final.root,{file_range}"
    
    # 2. MC Rec (效率分子) -> 使用 mc_tag
    mc_rec_arg = f"../data/{mc_tag}/reco/{mc_tag}_%d_ana_final.root,{MC_REF_RANGE}"
    
    # 3. MC Gen (效率分母) -> 使用 mc_tag
    mc_gen_arg = f"../data/{mc_tag}/reco/{mc_tag}_%d_sim.root,{MC_REF_RANGE}"
    
    # 4. Real Data Gen (用于验证) -> 使用 data_tag
    real_data_gen_arg = f"../data/{data_tag}/reco/{data_tag}_%d_sim.root,{file_range}"
    
    # 构造完整的 ROOT 命令
    # 注意：参数必须用双引号包裹，并在 shell 中正确转义
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
    
    # 执行命令
    # result = subprocess.run(root_cmd, shell=True, capture_output=True, text=True) # 如果想捕获输出
    result = subprocess.run(root_cmd, shell=True)
    
    duration = time.time() - start_time
    
    if result.returncode == 0:
        print(f"✅ Finished {output_name} in {duration:.2f}s")
    else:
        print(f"❌ Failed {output_name} with code {result.returncode}")
        
    return result.returncode

def main():
    print(f"--- Starting Batch Analysis for {len(DATASETS)} datasets with {MAX_WORKERS} workers ---")
    
    # 使用进程池并行运行
    with multiprocessing.Pool(processes=MAX_WORKERS) as pool:
        pool.map(run_task, DATASETS)
        
    print("--- All Tasks Completed ---")

if __name__ == "__main__":
    main()
