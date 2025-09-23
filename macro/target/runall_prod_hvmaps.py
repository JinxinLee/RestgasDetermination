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

# Compatibility: some older Python builds (or non-Python3 interpreters) may not have
# FileNotFoundError defined. Ensure a fallback to OSError so except FileNotFoundError
# works safely.
try:
    FileNotFoundError  # noqa: F821
except NameError:
    FileNotFoundError = OSError


def run_root(invocation, logfile):
    """Run a ROOT macro invocation and write combined stdout/stderr to logfile.

    invocation: string like 'prod_sim_hvmaps.C("outprefix",1000,"pp_dd",8.9)'
    logfile: path to write the captured output
    returns: captured output (string)
    """
    cmd = ["root", "-l", "-q", "-b", invocation]
    try:
        # universal_newlines is widely supported and returns str output
        proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
        out = proc.stdout
    except FileNotFoundError:
        out = "ERROR: 'root' executable not found in PATH.\n"
    except Exception as e:
        out = "ERROR running root: {}\n".format(e)

    # Ensure directory exists for logfile
    logdir = os.path.dirname(logfile)
    if logdir and not os.path.exists(logdir):
        os.makedirs(logdir)

    with open(logfile, 'w', encoding='utf-8') as f:
        f.write(out)
    return out


def append_line_to_log(logfile, line):
    with open(logfile, 'a', encoding='utf-8') as f:
        f.write(line.rstrip('\n') + '\n')


def find_generated_events(output):
    """Return the last line containing 'Generated Events' from output, or empty string."""
    if not output:
        return ''
    lines = [l for l in output.splitlines() if 'Generated Events' in l]
    return lines[-1] if lines else ''


def main(argv=None):
    parser = argparse.ArgumentParser(description='Port of runall_prod_hvmaps.sh to Python 3.7.3')
    parser.add_argument('prefix', nargs='?', default='9999', help='Prefix of output files')
    parser.add_argument('nevts', nargs='?', default='1000', help='Number of events to simulate')
    parser.add_argument('dec', nargs='?', default='pp_dd', help='Decay/gen string (e.g. pp_dd or DPM/FTF/BOX)')
    parser.add_argument('pbeam', nargs='?', default='8.9', help='Momentum of pbar-beam')
    args = parser.parse_args(argv)

    prefix = args.prefix
    nEvts = args.nevts
    dec = args.dec
    mom = args.pbeam

    slurm_id = os.environ.get('SLURM_ARRAY_TASK_ID', '1')

    print('SIMPATH is', os.environ.get('SIMPATH', ''))
    print('FAIRROOTPATH is', os.environ.get('FAIRROOTPATH', ''))

    outprefix = os.path.join('data', 'dpm', '{0}_{1}'.format(prefix, slurm_id))

    # Ensure parent directory exists
    parent = os.path.dirname(outprefix)
    if parent and not os.path.exists(parent):
        os.makedirs(parent)

    # 1) prod_sim_hvmaps.C(outprefix, nEvts, dec, mom)
    sim_log = outprefix + '_sim.log'
    invocation = 'prod_sim_hvmaps.C("{op}",{nev},"{dec}",{mom})'.format(op=outprefix, nev=nEvts, dec=dec, mom=mom)
    sim_out = run_root(invocation, sim_log)

    # extract Generated Events line
    numev_line = find_generated_events(sim_out)

    # 2) prod_aod_hvmaps.C(outprefix)
    digi_log = outprefix + '_digi.log'
    invocation = 'prod_aod_hvmaps.C("{op}")'.format(op=outprefix)
    digi_out = run_root(invocation, digi_log)
    if numev_line:
        append_line_to_log(digi_log, numev_line)

    # 3) reco_complete.C(nEvts, outprefix)
    reco_log = outprefix + '_reco.log'
    invocation = 'reco_complete.C({nev},"{op}")'.format(nev=nEvts, op=outprefix)
    reco_out = run_root(invocation, reco_log)
    if numev_line:
        append_line_to_log(reco_log, numev_line)

    # 4) pid_complete.C(nEvts, outprefix)
    pid_log = outprefix + '_pid.log'
    invocation = 'pid_complete.C({nev},"{op}")'.format(nev=nEvts, op=outprefix)
    pid_out = run_root(invocation, pid_log)
    if numev_line:
        append_line_to_log(pid_log, numev_line)

    # 5) ana_dpm.C(nEvts, outprefix)
    ana_log = outprefix + '_ana.log'
    invocation = 'ana_dpm.C({nev},"{op}")'.format(nev=nEvts, op=outprefix)
    ana_out = run_root(invocation, ana_log)
    if numev_line:
        append_line_to_log(ana_log, numev_line)

    print('\nFinished running macros. Logs written to:')
    for p in (sim_log, digi_log, reco_log, pid_log, ana_log):
        print(' -', p)
    if numev_line:
        print('\nAppended Generated Events line to each log:')
        print('  ', numev_line)
    else:
        print('\nNo "Generated Events" line found in sim log.')


if __name__ == '__main__':
    main()
