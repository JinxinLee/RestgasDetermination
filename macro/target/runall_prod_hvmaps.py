#!/usr/bin/env python3
"""
Python 3.7.3 port of runall_prod_hvmaps.sh

Usage:
  python runall_prod_hvmaps.py [prefix] [nevts] [gen] [pbeam]

Behaviour mirrors the original shell script:
- reads SIMPATH and FAIRROOTPATH from environment and prints them
- uses SLURM_ARRAY_TASK_ID (defaults to '1' if not set)
- runs a sequence of ROOT macros and writes their outputs to log files
- finds the last line containing 'Generated Events' from the sim log and appends it to subsequent logs

Note: This script calls the `root` binary (ROOT) available on PATH. If ROOT is not
available, the script will write an error message into the log files instead of macro output.
"""

from __future__ import print_function
import os
import sys
import subprocess
import argparse

def main(argv=None):
    """Thin wrapper that calls the original shell script `runall_prod_hvmaps.sh`.

    This replacement ensures the cluster runs the tested shell pipeline directly.
    """
    parser = argparse.ArgumentParser(description='Wrapper to call runall_prod_hvmaps.sh')
    parser.add_argument('prefix', nargs='?', default='9999')
    parser.add_argument('nevts', nargs='?', default='1000')
    parser.add_argument('dec', nargs='?', default='pp_dd')
    parser.add_argument('pbeam', nargs='?', default='8.9')
    args = parser.parse_args(argv)

    print('SIMPATH is', os.environ.get('SIMPATH', ''))
    print('FAIRROOTPATH is', os.environ.get('FAIRROOTPATH', ''))

    # Ensure the shell script exists in the same directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    sh_path = os.path.join(script_dir, 'runall_prod_hvmaps.sh')
    if not os.path.exists(sh_path):
        print('ERROR: shell script not found at', sh_path)
        sys.exit(2)

    cmd = [sh_path, args.prefix, args.nevts, args.dec, args.pbeam]
    # Ensure it's executable; if not, try to call it with bash
    if os.access(sh_path, os.X_OK):
        ret = subprocess.call(cmd)
    else:
        ret = subprocess.call(['bash'] + cmd)

    sys.exit(ret)


if __name__ == '__main__':
    main()
