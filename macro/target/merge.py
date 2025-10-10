import os
import sys
import glob
import subprocess
from shared_utils import run_command, parse_arguments, get_default_params

def get_file_list(directory, pattern):
    """Gets a list of files matching a pattern in a directory."""
    search_path = os.path.join(directory, pattern)
    files = glob.glob(search_path)
    print(f"Found {len(files)} files for pattern '{search_path}'")
    return files

def merge_files(file_list, output_file, log_file):
    """Merges a list of ROOT files using hadd."""
    if not file_list:
        print(f"No files to merge for {output_file}. Skipping.")
        return True
    
    # hadd can be sensitive to the number of files, merge in chunks if necessary
    # For now, direct merge. Add chunking if it fails for large file counts.
    command = f"hadd -f {output_file} {' '.join(file_list)}"
    
    print(f"Merging {len(file_list)} files into {output_file}...")
    return run_command(command, log_file, output_file, check_file_size=False) # Don't check size for hadd, it's variable

def run_merge(p, file_types, step_name):
    """Runs the merge process for specified file types."""
    print(f"\n--- Starting Merge Process for {step_name} ---")
    
    base_path = os.path.join(p.output_path, p.prefix)
    reco_path = os.path.join(base_path, 'reco')
    log_path = os.path.join(base_path, 'log')
    os.makedirs(log_path, exist_ok=True)

    prefix_parts = p.prefix.split('_')
    naming_prefix = f"{prefix_parts[0]}_{prefix_parts[1]}" if len(prefix_parts) >= 2 else p.prefix

    all_success = True
    for file_type in file_types:
        print(f"\n--- Merging '{file_type}' files ---")
        file_pattern = f"{naming_prefix}_*_{file_type}.root"
        files_to_merge = get_file_list(reco_path, file_pattern)
        
        if not files_to_merge:
            print(f"Warning: No files found for pattern {file_pattern}. Skipping merge for this type.")
            continue

        output_file = os.path.join(reco_path, f"{p.prefix}_{file_type}.root")
        log_file = os.path.join(log_path, f"merge_{file_type}.log")
        
        if not merge_files(files_to_merge, output_file, log_file):
            print(f"Error: Merging for {file_type} failed. See {log_file} for details.", file=sys.stderr)
            all_success = False
            # Decide if we should stop or continue
            # For now, we continue to try merging other types
    
    if all_success:
        print(f"--- Merge Process for {step_name} completed successfully. ---")
    else:
        print(f"--- Merge Process for {step_name} finished with errors. ---", file=sys.stderr)
        sys.exit(1)

def main():
    """Main execution function."""
    import argparse
    parser = argparse.ArgumentParser(description="Merge ROOT files from worker jobs.")
    parser.add_argument('--config', required=True, help="Path to the config JSON file.")
    parser.add_argument('--file-types', nargs='+', default=["sim", "digi", "reco", "pid", "par"], 
                        help="List of file types to merge (e.g., sim, digi, mc).")
    
    args = parser.parse_args()
    
    # Use a simplified parser to just get the config object
    p = parse_arguments("Merge worker files", is_workflow_runner=False)

    step_name = "POCA" if "sim" in args.file_types else "MC"
    run_merge(p, args.file_types, step_name)

if __name__ == '__main__':
    main()
