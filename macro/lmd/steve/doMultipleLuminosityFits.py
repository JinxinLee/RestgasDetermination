import os, sys, re, errno, glob, time, glob
import subprocess
import multiprocessing

cpu_cores = multiprocessing.cpu_count()

lib_path = os.path.abspath('argparse-1.2.1/build/lib')
sys.path.append(lib_path)

import argparse

dirs = []
box_dirs = []

dpm_glob_pattern = 'lmd_data.root'
dir_pattern = ''
tail_dir_pattern = ''

def getListOfDPMDirectories(path):
  if os.path.isdir(path):
    for dir in os.listdir(path):
      bunch_dirs = glob.glob(path + '/bunches_*/' + tail_dir_pattern)
      if bunch_dirs:
        for bunch_dir in bunch_dirs:
          filelists = glob.glob(bunch_dir + '/' + dpm_glob_pattern)
          if filelists:
            dirs.append(bunch_dir)
        return
      else:
        if glob.glob(path + '/Lumi_MC_*.root'):
          return
      dirpath = path + '/' + dir
      if os.path.isdir(dirpath):
        getListOfDPMDirectories(dirpath)

box_res_glob_pattern = 'resolution_params_1.root'
box_acc_glob_pattern = 'lmd_acc_data.root'
      
def getListOfBoxDirectories(path):
  if os.path.isdir(path):
    for dir in os.listdir(path):
      bunch_dirs = glob.glob(path + '/bunches_*/' + tail_dir_pattern)
      if bunch_dirs:
        for bunch_dir in bunch_dirs:
          filelists = glob.glob(bunch_dir + '/' + box_acc_glob_pattern)
          if filelists:
            filelists = glob.glob(bunch_dir + '/' + box_res_glob_pattern)
            if filelists:
              box_dirs.append(bunch_dir)
        return
      else:
        if glob.glob(path + '/Lumi_MC_*.root'):
          return
      dirpath = path + '/' + dir
      if os.path.isdir(dirpath):
        getListOfBoxDirectories(dirpath)

def findMatchingDirs():
  matching_dir_pairs = []
  for dpm_dir in dirs:
    m = re.search(dir_pattern, dpm_dir)
    if m:
      match = re.search('^(.*/)dpm_.*?(/.*/)\d*-\d*x\d*_(.*cut)/.*(/.*?)$', dpm_dir)
      pattern = '^' + match.group(1) + 'box_.*?' + match.group(2) + '.*' + match.group(3) + '/.*' + match.group(4) + '$'
      print pattern
      for box_dir in box_dirs:
        print box_dir
        box_match = re.search(pattern, box_dir)
        if box_match:
          matching_dir_pairs.append([dpm_dir, box_dir])
          break
  return matching_dir_pairs
      
      

parser = argparse.ArgumentParser(description='Script for going through whole directory trees and looking for bunches directories with filelists in them creating lmd data objects.', formatter_class=argparse.RawTextHelpFormatter)

parser.add_argument('dirname', metavar='dirname_to_scan', type=str, nargs=1,
                    help='Name of directory to scan recursively for lmd data files and call merge!')

parser.add_argument('dirname_pattern', metavar='directory pattern', type=str, nargs=1,
                    help='Only found directories with this pattern are used!')

parser.add_argument('--tail_dir_pattern', metavar='tail directory pattern', type=str, default='merge_data',
                    help='Only found directories with this pattern are used!')

parser.add_argument('--ref_box_gen_data', metavar='ref box gen data', type=str, default='',
                    help='If specified then this path will be used for all fits as the reference box gen data.')

args = parser.parse_args()

dir_pattern = args.dirname_pattern[0]

tail_dir_pattern = args.tail_dir_pattern

getListOfDPMDirectories(args.dirname[0])
getListOfBoxDirectories(args.dirname[0])

print dirs
print box_dirs

matches = findMatchingDirs()

command_suffix = '';
if args.ref_box_gen_data != '':
  command_suffix = ' -r '+ args.ref_box_gen_data

for match in matches:
  bashcommand = default=os.getenv('VMCWORKDIR') + '/build/bin/runLumiFit -d ' + match[0] + ' -a ' + match[1] + command_suffix
  returnvalue = subprocess.call(bashcommand.split())