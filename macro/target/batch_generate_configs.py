#!/usr/bin/env python3
"""
Batch script to invoke generate_configs.py multiple times.

This script calls generate_configs.py based on specified IP coordinate lists,
automatically sets vertex to 'mc', and allows custom output directory.
ipx and ipy use the same list (--ips), while ipz uses a separate list (--ipzs).

Usage:
  python3 batch_generate_configs.py --ips <ips...> --ipzs <ipzs...> --output-dir <dir>

Example:
  python3 batch_generate_configs.py --ips 0.0 0.1 --ipzs -5.0 0.0 5.0 --output-dir mc_configs
"""
import os
import subprocess
import argparse

def main():
    """Main function: parse arguments and batch call generate_configs.py"""
    parser = argparse.ArgumentParser(
        description='Batch call generate_configs.py to generate configuration files'
    )

    # Required arguments
    parser.add_argument('--ips', type=float, nargs='+', required=True,
                        help='List of IP coordinate values for ipx and ipy')
    parser.add_argument('--ipzs', type=float, nargs='+', required=True,
                        help='List of IP coordinate values for ipz')
    parser.add_argument('--output-dir', type=str, required=True,
                        help='Output directory for configuration files')

    # Optional arguments (passed to generate_configs.py)
    parser.add_argument('--moms', type=float, nargs='+', default=[4.06],
                        help='List of momentum values (default: 4.06)')
    parser.add_argument('--type', type=str, default='point',
                        help='Simulation type: point or restgas (default: point)')
    parser.add_argument('--nevts', type=int, default=10000,
                        help='Number of events (default: 10000)')
    parser.add_argument('--njobs', type=int, default=100,
                        help='Number of parallel jobs (default: 100)')
    parser.add_argument('--dec', type=str, default='DPM2',
                        help='Generator type (default: DPM2)')
    parser.add_argument('--use_mvd_hvmaps', type=str, default='false',
                        help='Whether to use MVD hvmaps (default: false)')
    parser.add_argument('--theta_min', type=float, default=22.0,
                        help='Minimum theta angle (default: 22.0)')
    parser.add_argument('--theta_max', type=float, default=150.0,
                        help='Maximum theta angle (default: 150.0)')
    parser.add_argument('--output-path', type=str, 
                        default='/lustre/panda/jili/oct19/macro/target/data/',
                        help='Simulation data output path')

    args = parser.parse_args()

    # Get current script directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    generate_script = os.path.join(script_dir, 'generate_configs.py')

    # Check if generate_configs.py exists
    if not os.path.exists(generate_script):
        print(f"Error: Cannot find {generate_script}")
        return

    # Build command arguments
    cmd = [
        'python3', generate_script,
        '--vertex', 'mc',  # Fixed to mc
        '--type', args.type,
        '--moms'] + [str(m) for m in args.moms] + [
        '--ipxs'] + [str(ip) for ip in args.ips] + [
        '--ipys'] + [str(ip) for ip in args.ips] + [
        '--ipzs'] + [str(ip) for ip in args.ipzs] + [
        '--nevts', str(args.nevts),
        '--njobs', str(args.njobs),
        '--dec', args.dec,
        '--use_mvd_hvmaps', args.use_mvd_hvmaps,
        '--theta_min', str(args.theta_min),
        '--theta_max', str(args.theta_max),
        '--output-path', args.output_path,
        '--output-dir', args.output_dir
    ]

    print(f"Calling generate_configs.py...")
    print(f"Command: {' '.join(cmd)}")
    print()

    # Execute command
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        print(result.stdout)
        if result.stderr:
            print("Error output:")
            print(result.stderr)
    except subprocess.CalledProcessError as e:
        print(f"Execution failed: {e}")
        print(f"Standard output: {e.stdout}")
        print(f"Error output: {e.stderr}")
        return

    print(f"\nDone! Configuration files generated to '{args.output_dir}' directory.")

if __name__ == '__main__':
    main()
