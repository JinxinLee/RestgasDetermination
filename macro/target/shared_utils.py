import os
import sys
import subprocess
import json
import argparse

def run_command(command, log_file, output_file, env=None):
    """Executes a command and logs its output, skipping if the output file already exists."""
    if os.path.exists(output_file):
        print(f"Output file {output_file} already exists. Skipping command.")
        print(f"Executing: {command}")
        return True
        
    print(f"Executing: {command}")
    print(f"Logging to: {log_file}")
    try:
        current_env = os.environ.copy()
        if env:
            current_env.update(env)
            
        with open(log_file, 'w') as f:
            # Using shlex.split is problematic with complex root commands, passing string directly to shell=True
            process = subprocess.run(command, stdout=f, stderr=subprocess.STDOUT, check=True, text=True, env=current_env, shell=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {command}", file=sys.stderr)
        print(f"See log for details: {log_file}", file=sys.stderr)
        return False
    except FileNotFoundError:
        print(f"Error: 'root' command not found. Make sure ROOT is installed and in your PATH.", file=sys.stderr)
        with open(log_file, 'w') as f:
            f.write("Error: 'root' command not found. Make sure ROOT is installed and in your PATH.")
        return False

def load_config(config_file):
    """Loads configuration from a JSON file."""
    if config_file and os.path.exists(config_file):
        print(f"Loading configuration from {config_file}")
        with open(config_file, 'r') as f:
            return json.load(f)
    print(f"Warning: Configuration file '{config_file}' not found. Using defaults and command-line arguments.")
    return {}

def get_generated_events(log_file):
    """Extracts the number of generated events from a log file."""
    try:
        with open(log_file, 'r') as f:
            for line in f:
                if "Number of events generated" in line:
                    return line.strip()
    except FileNotFoundError:
        return None
    return None

def get_default_params():
    """Returns a dictionary of default parameters."""
    return {
        'prefix': 'test', 'nevts': 1000, 'dec': 'pp_dd', 'mom': 4.06,
        'use_mvd_hvmaps': 'false', 'ipx': 0.0, 'ipy': 0.0, 'ipz': 0.0,
        'use_restgas': 'false', 'theta_min': 0.0, 'theta_max': 180.0,
        'back_prop_vertex': 'poca', 'output_path': 'data',
        'njobs': 10, 'njobs_mc': 10
    }

def parse_arguments(description, is_workflow_runner=False):
    """
    Parses command-line arguments.
    
    For the main workflow runner, it handles multiple config files and job counts.
    For worker scripts, it expects a single config file.
    """
    # Base parser for all scripts
    parser = argparse.ArgumentParser(description=description)

    if is_workflow_runner:
        # Special handling for the main workflow runner
        parser.add_argument('configfiles', type=str, nargs='*', default=['config.json'],
                                 help='One or more paths to configuration JSON files or directories containing them.')
        parser.add_argument('-j', '--jobs', type=int, default=None,
                                 help='Number of parallel jobs to run (overrides config).')
        
        # Add all possible arguments to the parser for command-line overrides
        defaults = get_default_params()
        for arg, val in defaults.items():
            if isinstance(val, bool):
                 parser.add_argument(f'--{arg}', type=lambda x: (str(x).lower() == 'true'))
            else:
                 parser.add_argument(f'--{arg}', type=type(val))
        
        # Parse known args to separate workflow args from parameter overrides
        conf_args, remaining_argv = parser.parse_known_args()

        # Expand directories into a list of config files
        expanded_config_files = []
        for path in conf_args.configfiles:
            if os.path.isdir(path):
                print(f"Searching for config files in directory: {path}")
                for root, _, files in os.walk(path):
                    for file in files:
                        if file.endswith('.json'):
                            expanded_config_files.append(os.path.join(root, file))
            elif os.path.isfile(path):
                expanded_config_files.append(path)
            else:
                expanded_config_files.append(path) # Pass non-existent paths for warning later

        if not expanded_config_files:
            print(f"Warning: No config files found in paths: {conf_args.configfiles}")

        tasks = []
        for config_file in expanded_config_files:
            # 1. Start with hardcoded defaults
            params = get_default_params()
            # 2. Load from config file
            config = load_config(config_file)
            params.update(config)
            # 3. Override with command-line arguments
            parser.set_defaults(**params)
            p = parser.parse_args(remaining_argv, namespace=argparse.Namespace(**params))
            
            # Special override for --jobs if provided
            if conf_args.jobs is not None:
                p.njobs = conf_args.jobs
                p.njobs_mc = conf_args.jobs

            p.configfile = config_file # Add for reference
            tasks.append(p)
        return tasks

    else:
        # Standard parsing for worker scripts
        parser.add_argument('--config', type=str, required=True, help='Path to the configuration JSON file.')
        
        args, _ = parser.parse_known_args()
        
        # Load config to get parameters
        config_params = load_config(args.config)
        if not config_params:
            print(f"Error: Config file '{args.config}' not found or is empty.", file=sys.stderr)
            sys.exit(1)

        # Create a namespace object from the config params
        p = argparse.Namespace(**config_params)
        p.config = args.config # Keep the original path
        return p
