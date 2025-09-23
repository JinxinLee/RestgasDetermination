#!/usr/bin/env python3
"""
submit_prod_hvmaps.py

Generate and submit sbatch array jobs to run prod hvmaps workflow on a SLURM cluster (e.g. GSI Virgo).

Features:
- Generates an sbatch script that calls either the new Python runner (`runall_prod_hvmaps.py`) or the original shell script.
- Supports --array range (e.g. 1-100), job name, time, and other sbatch options.
- Has a --dry-run mode that prints the generated sbatch script instead of submitting.
- When sbatch is available, submits the script and prints sbatch output (job id).

Usage examples:
  python3 submit_prod_hvmaps.py --array 1-10 --prefix 9999 --nevts 1000 --dec DPM2 --pbeam 8.9
  python3 submit_prod_hvmaps.py --dry-run --array 1-5

Note: This script expects to be run on a system with `sbatch` available when not in dry-run.
"""

from __future__ import print_function
import argparse
import os
import subprocess
import tempfile
import sys


DEFAULT_SCRIPT = 'runall_prod_hvmaps.py'


SBATCH_TEMPLATE = """#!/bin/bash
#SBATCH -J {jobname}
#SBATCH --time={time}
#SBATCH --get-user-env
#SBATCH -e {errlog}
#SBATCH -o {outlog}
{extra_directives}

prefix={prefix}
nEvts={nevts}
dec="{dec}"
mom={pbeam}

{call_line}
"""


def build_sbatch_script(call_line, prefix, nevts, dec, pbeam, jobname, time_limit, outbase, extra_directives, array_spec=None):
    errlog = os.path.join(outbase, 'slurm_%A_%a_errout.log') if array_spec else os.path.join(outbase, 'slurm_%j_errout.log')
    outlog = errlog
    content = SBATCH_TEMPLATE.format(jobname=jobname, time=time_limit, errlog=errlog, outlog=outlog,
                                    extra_directives=extra_directives, prefix=prefix, nevts=nevts, dec=dec, pbeam=pbeam, call_line=call_line)
    if array_spec:
        content = content.replace('\n' + '#SBATCH --get-user-env', '\n#SBATCH --get-user-env\n#SBATCH -a {0}'.format(array_spec))
    return content


def write_temp_script(content, keep=False):
    fd, path = tempfile.mkstemp(prefix='sbatch_prod_', suffix='.sh', text=True)
    # Avoid passing encoding for compatibility with constrained environments
    with os.fdopen(fd, 'w') as f:
        f.write(content)
    if keep:
        print('Wrote sbatch script to:', path)
    return path


def main(argv=None):
    parser = argparse.ArgumentParser(description='Submit prod hvmaps jobs to SLURM (sbatch)')
    parser.add_argument('--array', default=None, help='SLURM array spec, e.g. 1-100 or 1-10%%5')
    parser.add_argument('--prefix', default='9999')
    parser.add_argument('--nevts', default='1000')
    parser.add_argument('--dec', default='pp_dd')
    parser.add_argument('--pbeam', default='8.9')
    parser.add_argument('--jobname', default='pndsim')
    parser.add_argument('--time', default='8:00:00')
    parser.add_argument('--outbase', default='data/slurmlog')
    parser.add_argument('--script', default=DEFAULT_SCRIPT, help='Runner script to call (relative to this file)')
    parser.add_argument('--use-shell', action='store_true', help='Call original shell script instead of Python runner')
    parser.add_argument('--dry-run', action='store_true')
    parser.add_argument('--keep-script', action='store_true', help='Do not delete temporary sbatch script')
    parser.add_argument('--extra-sbatch', default='', help='Extra sbatch directives (one per line)')
    args = parser.parse_args(argv)

    # Ensure outbase exists
    if not os.path.exists(args.outbase):
        os.makedirs(args.outbase)

    # Build call line
    # Choose to call Python runner (preferred) or original shell script
    script_path = os.path.abspath(os.path.join(os.path.dirname(__file__), args.script))
    # Always call the shell script to keep the cluster invocation identical to original
    sh_name = os.path.basename(script_path).replace('.py', '.sh')
    call_line = './' + sh_name + ' {prefix} {nevts} {dec} {pbeam} &> data/dpm/${{prefix}}_${{SLURM_ARRAY_TASK_ID}}_sim.log'

    extra_directives = args.extra_sbatch

    sbatch_content = build_sbatch_script(call_line=call_line, prefix=args.prefix, nevts=args.nevts, dec=args.dec, pbeam=args.pbeam,
                                         jobname=args.jobname, time_limit=args.time, outbase=args.outbase, extra_directives=extra_directives,
                                         array_spec=args.array)

    if args.dry_run:
        print('--- DRY RUN: generated sbatch script ---')
        print(sbatch_content)
        return

    script_path = write_temp_script(sbatch_content, keep=args.keep_script)

    # submit via sbatch
    try:
        proc = subprocess.run(['sbatch', script_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if proc.returncode == 0:
            print('sbatch output:', proc.stdout.strip())
        else:
            print('sbatch failed:', proc.returncode)
            print('stdout:', proc.stdout)
            print('stderr:', proc.stderr)
    except FileNotFoundError:
        print("ERROR: 'sbatch' not found in PATH. Make sure you're on a node with SLURM client tools.")
        print('Generated script was written to:', script_path)


if __name__ == '__main__':
    main()
