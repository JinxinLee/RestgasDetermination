import os, sys, re, errno, glob, time, glob
import subprocess
import multiprocessing

cpu_cores = multiprocessing.cpu_count()

lib_path = os.path.abspath('argparse-1.2.1/build/lib')
sys.path.append(lib_path)

import argparse

himster_total_job_threshold = 100 # job threshold of this type (too many jobs could generate to much io load as quite a lot of data is read in from the storage...)

# check number of jobs currently running or in queue on himster from my side
def getNumJobsOnHimster():    
    bashcommand = 'qstat -t | grep miFitData | wc -l'
    returnvalue = subprocess.Popen(bashcommand, shell=True, stdout=subprocess.PIPE)
    out, err = returnvalue.communicate()
    return int(out)

dirs = []

def getListOfDirectories(path, force):
  if os.path.isdir(path):
    for dir in os.listdir(path):
      bunch_dirs = glob.glob(path + '/bunches_*')
      if bunch_dirs:
        for bunch_dir in bunch_dirs:
          if not force:
            filelists = glob.glob(bunch_dir + '/lmd_*data_*.root')
          else:
            filelists = []
          if not filelists:
            filelists = glob.glob(bunch_dir + '/filelist_*.txt')
            if filelists:
              dirs.append(bunch_dir)
        return
      else:
        if glob.glob(path + '/Lumi_MC_*.root'):
          return
      dirpath = path + '/' + dir
      if os.path.isdir(dirpath):
        getListOfDirectories(dirpath, force)


parser = argparse.ArgumentParser(description='Script for going through whole directory trees and looking for bunches directories with filelists in them creating lmd data objects.', formatter_class=argparse.RawTextHelpFormatter)


parser.add_argument('lab_momentum', metavar='lab_momentum', type=float, nargs=1, help='lab momentum of incoming beam antiprotons\n(required to set correct magnetic field maps etc)')
parser.add_argument('type', metavar='type', type=str, nargs=1,
                    help='type of data to create (a = angular, e = efficiency, r = resolution, v = vertex/ip')
parser.add_argument('dirname', metavar='dirname_to_scan', type=str, nargs=1,
                    help='Name of directory to scan recursively for qa files and create bunches')

parser.add_argument('--force', action='store_true', help='number of events to use')
parser.add_argument('--num_events', metavar='num_events', type=int, default=0, help='number of events to use')
parser.add_argument('--elastic_cross_section', metavar='elastic_cross_section', type=float, default=1.0, help='Total elastic cross section. Relevant for luminosity extraction performance tests!')

args = parser.parse_args()

failed_submit_commands = []

getListOfDirectories(args.dirname[0], args.force)


max_jobarray_size = 100

for dir in dirs:
  num_filelists = len(glob.glob(dir + '/filelist_*.txt'))

  input_path = os.path.split(dir)[0]
  filelist_path = dir
  output_path = filelist_path

  for job_index in range(1, num_filelists, max_jobarray_size):
    bashcommand = 'qsub -t ' + str(job_index) + '-' + str(min(job_index + max_jobarray_size - 1, num_filelists)) + ' -N createLumiFitData' \
                    + ' -l nodes=1:ppn=1,walltime=02:00:00,mem=100mb,vmem=700mb -j oe -o ' + output_path + '/createLumiFitData_pbs.log ' \
                    + '-v numEv="' + str(args.num_events) + '",pbeam="' + str(args.lab_momentum[0]) \
                    + '",input_path="' + input_path + '",filelist_path="' + filelist_path + '",output_path="' + output_path \
                    + '",type="' + args.type[0] + '",elastic_cross_section="' + str(args.elastic_cross_section) + '" -V ./createLumiFitData.sh'
    
    jobs_on_himster = getNumJobsOnHimster()
    print str(jobs_on_himster) + " < " + str(himster_total_job_threshold) + " ?"
    if getNumJobsOnHimster() < himster_total_job_threshold:
      print "yes"
      returnvalue = subprocess.call(bashcommand.split())
      if returnvalue > 0:
        failed_submit_commands.append(bashcommand)
      else:
          time.sleep(5) # sleep 5 sec to make the queue changes active
    else:
      failed_submit_commands.append(bashcommand)


while failed_submit_commands:
  print 'we have ' + str(len(failed_submit_commands)) + ' jobs arrays that were not accepted!'
  
  bashcommand = failed_submit_commands.pop(0)
  print 'trying to resubmit ' + bashcommand
  
  if getNumJobsOnHimster() > himster_total_job_threshold:
    returnvalue = 1
  else:
    returnvalue = subprocess.call(bashcommand.split())
  # if we failed to submit it again
  if returnvalue > 0:
    # put the command back into the list 
    failed_submit_commands.insert(0, bashcommand)
    # and sleep for 30 min
    print 'waiting for 30 min and then try a resubmit...'
    time.sleep(180) #sleep for 30min
