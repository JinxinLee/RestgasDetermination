import os, sys, re, errno, glob, time, glob
import subprocess
import multiprocessing

cpu_cores = multiprocessing.cpu_count()

lib_path = os.path.abspath('argparse-1.2.1/build/lib')
sys.path.append(lib_path)

import argparse

dirs = []

glob_pattern = ''
dir_pattern = ''

def getListOfDirectories(path):
  if os.path.isdir(path):
    for dir in os.listdir(path):
      bunch_dirs = glob.glob(path + '/bunches_*')
      if bunch_dirs:
        for bunch_dir in bunch_dirs:
          filelists = glob.glob(bunch_dir + '/' + glob_pattern)
          if filelists:
            m = re.search(dir_pattern, bunch_dir)
            if m:
              dirs.append(bunch_dir)
        return
      else:
        if glob.glob(path + '/Lumi_MC_*.root'):
          return
      dirpath = path + '/' + dir
      if os.path.isdir(dirpath):
        getListOfDirectories(dirpath)


parser = argparse.ArgumentParser(description='Script for going through whole directory trees and looking for bunches directories with filelists in them creating lmd data objects.', formatter_class=argparse.RawTextHelpFormatter)

parser.add_argument('type', metavar='type', type=int, nargs=1,
                    help='type of data to create (0 = angular, 1 = efficiency, 2 = resolution, 3 = vertex/ip')
parser.add_argument('dirname', metavar='dirname_to_scan', type=str, nargs=1,
                    help='Name of directory to scan recursively for lmd data files and call merge!')
parser.add_argument('--dir_pattern', metavar='path name pattern', type=str, default='.*', help='')

args = parser.parse_args()

dir_pattern = args.dir_pattern

pattern = ''
if args.type[0] == 0:
  pattern = ' -f lmd_data_\\d*.root'
  glob_pattern = 'lmd_data_*.root'
elif args.type[0] == 1:
  pattern = ' -f lmd_acc_data_\\d*.root'
  glob_pattern = 'lmd_acc_data_*.root'
elif args.type[0] == 2:
  pattern = ' -f lmd_res_data_\\d*.root'
  glob_pattern = 'lmd_res_data_*.root'
elif args.type[0] == 3:
  pattern = ' -f lmd_vertex_data_\\d*.root'
  glob_pattern = 'lmd_vertex_data_*.root'

getListOfDirectories(args.dirname[0])

for dir in dirs:
  bashcommand = default=os.getenv('VMCWORKDIR') + '/build/bin/mergeLmdData -p ' + dir + ' -t ' + str(args.type[0]) + pattern 
                    
  returnvalue = subprocess.call(bashcommand.split())
