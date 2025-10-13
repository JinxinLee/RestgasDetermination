#!/usr/bin/env python3
"""
Python script to generate configuration files for the analysis chain in batch.

This script creates a matrix of configurations based on provided parameter lists
(e.g., momentum, interaction point coordinates) and saves each combination as a
separate JSON file in a structured directory hierarchy.

Usage:
  python3 generate_configs.py --type <type> --vertex <vertex> --moms <moms...> --ipxs <ipxs...> --ipys <ipys...> --ipzs <ipzs...>

Example:
  python3 generate_configs.py --type point --vertex poca --moms 4.06 --ipxs 0.0 --ipys 0.0 --ipzs 0.0 --nevts 10000 --output-dir configs
"""
import os
import json
import argparse
import itertools

def main():
    """Main function to parse arguments and generate config files."""
    parser = argparse.ArgumentParser(
        description='Generate configuration files for the analysis chain in batch.'
    )

    # --- Arguments for combinations with defaults ---
    parser.add_argument('--type', type=str, default='point', choices=['point', 'restgas'],
                        help='Simulation type: "point" for beam-target interaction, "restgas" for beam-gas.')
    parser.add_argument('--vertex', type=str, default='poca', choices=['poca', 'mc'],
                        help='Vertex determination method for back-propagation.')
    parser.add_argument('--moms', type=float, nargs='+', default=[4.06],
                        help='List of beam momentum values (e.g., 8.9 4.06).')
    parser.add_argument('--ipxs', type=float, nargs='+', default=[0.0],
                        help='List of interaction point X coordinates.')
    parser.add_argument('--ipys', type=float, nargs='+', default=[0.0],
                        help='List of interaction point Y coordinates.')
    parser.add_argument('--ipzs', type=float, nargs='+', default=[0.0],
                        help='List of interaction point Z coordinates.')

    # --- Optional, fixed-value arguments for all generated configs ---
    parser.add_argument('--nevts', type=int, default=10000,
                        help='Number of events to simulate.')
    parser.add_argument('--njobs', type=int, default=100,
                        help='Number of parallel jobs to run (for workflow runner).')
    parser.add_argument('--dec', type=str, default='DPM2',
                        help='Name of EvtGen decay file or generator type (DPM/FTF/BOX).')
    parser.add_argument('--use_mvd_hvmaps', type=str, default='false', choices=['true', 'false'],
                        help='Set to "true" to use new MVD with hvmaps.')
    parser.add_argument('--theta_min', type=float, default=22.0,
                        help='Theta min for DPM generator.')
    parser.add_argument('--theta_max', type=float, default=150.0,
                        help='Theta max for DPM generator.')
    parser.add_argument('--output-path', type=str, default='/lustre/panda/jili/oct19/macro/target/data/',
                        help='Base output path for the simulation data itself (written into the config).')
    parser.add_argument('--output-dir', type=str, default='configs',
                        help='Directory where the generated .json config files will be saved.')

    args = parser.parse_args()

    # --- Generate all combinations ---
    param_combinations = list(itertools.product(
        args.moms,
        args.ipxs,
        args.ipys,
        args.ipzs
    ))

    print(f"Found {len(param_combinations)} combinations to generate.")
    count = 0

    for mom, ipx, ipy, ipz in param_combinations:
        # --- Build paths and names ---
        # e.g., configs/point/poca
        target_dir = args.output_dir
        
        # e.g., point/restgas_poca/mc_mom_ipx_ipy_ipz
        prefix = f"{args.type}_{args.vertex}_{mom}_{ipx}_{ipy}_{ipz}"
        
        # e.g., mom_ipx_ipy_ipz.json
        filename = f"{args.type}_{args.vertex}_{mom}_{ipx}_{ipy}_{ipz}.json"

        filepath = os.path.join(args.output_dir, filename)

        # --- Create config dictionary ---
        config_data = {
            "prefix": prefix,
            "nevts": args.nevts,
            "njobs": args.njobs,
            "dec": args.dec,
            "mom": mom,
            "use_mvd_hvmaps": args.use_mvd_hvmaps,
            "ipx": ipx,
            "ipy": ipy,
            "ipz": ipz,
            "use_restgas": "true" if args.type == 'restgas' else "false",
            "theta_min": args.theta_min,
            "theta_max": args.theta_max,
            "back_prop_vertex": args.vertex,
            "output_path": args.output_path
        }

        # --- Write file ---
        os.makedirs(target_dir, exist_ok=True)
        with open(filepath, 'w') as f:
            json.dump(config_data, f, indent=4)
        
        count += 1

    print(f"Successfully generated {count} configuration files in '{args.output_dir}'.")

if __name__ == '__main__':
    main()
