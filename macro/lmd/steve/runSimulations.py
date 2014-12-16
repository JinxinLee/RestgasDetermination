#!/usr/bin/python

import os, sys, re, errno, glob, time, glob
import subprocess
import multiprocessing

cpu_cores = multiprocessing.cpu_count()

lib_path = os.path.abspath('argparse-1.2.1/build/lib')
sys.path.append(lib_path)

import argparse

himster_total_job_threshold = 1600

# define a function that relinks the field maps (temp workaround for the hc solenoid field map problem)
# def relinkSolenoidFieldMaps(mom):
#     if mom > 3.0:
#         fieldmap_path = os.getenv('VMCWORKDIR') + '/macro/lmd/Anastasia/solenoid_fc';
#     else:
#         fieldmap_path = os.getenv('VMCWORKDIR') + '/macro/lmd/Anastasia/solenoid_hc';
#     
#     fieldmaps = os.listdir(fieldmap_path);
#     for fieldmap in fieldmaps:
#         if os.path.splitext(fieldmap)[1] == '.root':
#             bashcommand = 'ln -sf ' + fieldmap_path + '/' + fieldmap + ' ' + os.getenv('VMCWORKDIR') + '/input/.'
#             subprocess.call(bashcommand.split())
#     
#     return;


# check number of jobs currently running or in queue on himster from my side
def getNumJobsOnHimster():    
    bashcommand = 'qstat -t | wc -l'
    returnvalue = subprocess.Popen(bashcommand, shell=True, stdout=subprocess.PIPE)
    out, err = returnvalue.communicate()
    return int(out)


def getListOfMCFiles(pathname):
    return glob.glob(pathname + '/Lumi_MC_*.root')

def linkPossibleMCFiles(pathname):
    #get list of mc files in the this directory
    mc_files_in_path = getListOfMCFiles(pathname)
    
    #get list of all directories for these setting and remove current pathname
    #so strip of the cut stuff
    m = re.match('^(.*/\d*-\d*x\d*_).*$', pathname)
    print m.group(1)
    other_dir_list = glob.glob(m.group(1) + '*')
    print other_dir_list
    other_dir_list.remove(pathname)
    dir_to_copy_from = ''
    for other_dir in other_dir_list:
      #prioritize the uncut case to copy the files from
      m = re.match('.*_uncut.*', other_dir)
      if m:
        dir_to_copy_from = other_dir
        break
    
    #if we didn't find the uncut case just copy from the first directory we found
    if dir_to_copy_from == '' and len(other_dir_list):
      dir_to_copy_from = other_dir_list[0]
      
    if dir_to_copy_from != '':
      mc_files = getListOfMCFiles(dir_to_copy_from)
      if len(mc_files) != len(mc_files_in_path):
        print 'linking mc files from ' + dir_to_copy_from + ' to ' + pathname + '!'
        currentpath = os.getcwd()
        os.chdir(pathname)
        bashcommand = 'ln -sf ../' + os.path.split(dir_to_copy_from)[1] + '/Lumi_MC_*.root .'
        print bashcommand
        returnvalue = subprocess.Popen(bashcommand, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = returnvalue.communicate()
        print out
        bashcommand = 'ln -sf ../' + os.path.split(dir_to_copy_from)[1] + '/Lumi_Params_*.root .'
        print bashcommand
        returnvalue = subprocess.Popen(bashcommand, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = returnvalue.communicate()
        print out
        os.chdir(currentpath)
        return
    return
    

parser = argparse.ArgumentParser(description='Script for full simulation of PANDA Luminosity Detector via externally generated MC data.', formatter_class=argparse.RawTextHelpFormatter)

parser.add_argument('num_events', metavar='num_events', type=int, nargs=1, help='number of events to simulate')
parser.add_argument('lab_momentum', metavar='lab_momentum', type=float, nargs=1, help='lab momentum of incoming beam antiprotons\n(required to set correct magnetic field maps etc)')
parser.add_argument('gen_data_dirname', metavar='gen_data_dirname', type=str, nargs=1,
                    help='Name of directory containing the generator data that is used as input.'
                    'Note that this is only the name of the directory and NOT the full path.'
                    'The base path of the directory should be specified with the'
                    '--gen_data_dir flag.')

parser.add_argument('--low_index', metavar='low_index', type=int, default= -1,
                   help='Lowest index of generator file which is supposed to be used in the simulation. Default setting is -1 which will take the lowest found index.')
parser.add_argument('--high_index', metavar='high_index', type=int, default= -1,
                   help='Highest index of generator file which is supposed to be used in the simulation. Default setting is -1 which will take the highest found index.')

parser.add_argument('--gen_data_dir', metavar='gen_data_dir', type=str, default=os.getenv('GEN_DATA'),
                   help='Base directory to input files created by external generator. By default the environment variable $GEN_DATA will be used!')

parser.add_argument('--output_dir', metavar='output_dir', type=str, default='', help='This directory is used for the output. Default is the generator directory as a prefix, with beam offset infos etc. added')

parser.add_argument('--use_ip_offset', metavar=("ip_offset_x", "ip_offset_y", "ip_offset_z", "ip_spread_x", "ip_spread_y", "ip_spread_z"), type=float, nargs=6, default=[0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                   help="ip_offset_x: interaction vertex mean X position (in cm)\n"
            "ip_offset_y: interaction vertex mean Y position (in cm)\n"
            "ip_offset_z: interaction vertex mean Z position (in cm)\n"
            "ip_spread_x: interaction vertex X position distribution width (in cm)\n"
            "ip_spread_y: interaction vertex Y position distribution width (in cm)\n"
            "ip_spread_z: interaction vertex Z position distribution width (in cm)")

parser.add_argument('--use_beam_gradient', metavar=("beam_gradient_x", "beam_gradient_y", "beam_emittance_x", "beam_emittance_y"), type=float, nargs=4, default=[0.0, 0.0, 0.0, 0.0],
                   help="beam_gradient_x: mean beam inclination on target in x direction dPx/dPz (in mrad)\n"
            "beam_gradient_y: mean beam inclination on target in y direction dPy/dPz (in mrad)\n"
            "beam_emittance_x: beam emittance in x direction (in mrad)\n"
            "beam_emittance_y: beam emittance in y direction (in mrad)")

parser.add_argument('--use_xy_cut', action='store_true', help='Use the x-theta & y-phi filter after the tracking stage to remove background.')
parser.add_argument('--use_m_cut', action='store_true', help='Use the tmva based momentum cut filter after the backtracking stage to remove background.')

parser.add_argument('--reco_ip_offset', metavar=("rec_ip_offset_x", "rec_ip_offset_y", "rec_ip_spread_x", "rec_ip_spread_y"), type=float, nargs=4, default=[0.0, 0.0, -1.0, -1.0],
                   help="rec_ip_offset_x: interaction vertex mean X position (in cm)\n"
            "rec_ip_offset_y: interaction vertex mean Y position (in cm)\n"
            "rec_ip_spread_x: interaction vertex X position distribution width (in cm)\n"
            "rec_ip_spread_y: interaction vertex Y position distribution width (in cm)\n")

args = parser.parse_args()



rec_ip_info = '",rec_beamX0="0.0",rec_beamY0="0.0",rec_targetZ0="0.0'
#if we are using the xy cut 
if args.use_xy_cut or args.use_m_cut:
  if args.reco_ip_offset[2] >= 0.0 and args.reco_ip_offset[3] >= 0.0:
    rec_ip_info = '",rec_beamX0="' + str(args.reco_ip_offset[0]) + '",rec_beamY0="' + str(args.reco_ip_offset[1]) + '",rec_targetZ0="0.0' 
  else:
    rec_ip_info = '",rec_beamX0="' + str(args.use_ip_offset[0]) + '",rec_beamY0="' + str(args.use_ip_offset[1]) + '",rec_targetZ0="0.0'

  
# generator file prefix
filename_base = re.sub('\.', 'o', args.gen_data_dirname[0])

path = args.gen_data_dir
while not os.path.isdir(path):
    path = raw_input('Please enter valid generator base path: ')

# check for the index range in the specified generator folder
first = 1
lowest_index = -1
highest_index = -1

dircontent = os.listdir(path + '/' + args.gen_data_dirname[0])

for file in dircontent:
  result = re.search('_(\d*).root$', file)
  if result:
    if first:
      lowest_index = int(result.group(1))
      highest_index = int(result.group(1))
      first = 0
    else:
      if int(result.group(1)) < lowest_index:
        lowest_index = int(result.group(1))
      elif int(result.group(1)) > highest_index:
        highest_index = int(result.group(1))

  low_index_used = lowest_index
  high_index_used = highest_index

if args.low_index > lowest_index and args.low_index <= highest_index:
  low_index_used = args.low_index
if args.high_index < highest_index and args.high_index >= lowest_index:
  high_index_used = args.high_index

print 'preparing simulations in index range ' + str(low_index_used) + ' - ' + str(high_index_used)

if args.output_dir == '':
  # generate output directory name
  # lets generate a folder structure based on the input
  dirname = 'plab_' + str(args.lab_momentum[0]) + 'GeV'
  
  gen_part = re.sub('_plab_.*GeV', '', args.gen_data_dirname[0]) 
  gen_part = re.sub('^\d*_', '', gen_part)
  
  dirname += '/' + gen_part
  
  dirname += '/ip_offset_XYZDXDYDZ'
  for val in args.use_ip_offset:
    dirname = dirname + '_' + str(val)
  dirname += '/beam_grad_XYDXDY'
  for val in args.use_beam_gradient:
    dirname = dirname + '_' + str(val)
  dirname += '/' + str(args.num_events[0]) 
  
  dirname_filter_suffix = str(args.low_index) + '-' + str(args.high_index) + '_'
  if args.use_xy_cut:
    dirname_filter_suffix += 'xy_'
  if args.use_m_cut:
    dirname_filter_suffix += 'm_'
  if not args.use_xy_cut and not args.use_m_cut:
    dirname_filter_suffix += 'un'
  dirname_filter_suffix += 'cut'
  if args.use_xy_cut:
    if args.reco_ip_offset[2] >= 0.0 and args.reco_ip_offset[3] >= 0.0:
      dirname_filter_suffix += '_real'
else:
  dirname = args.output_dir

pathname_base = os.getenv('DATA_DIR') + '/' + dirname
path_mc_data = pathname_base + '/mc_data'
dirname_full = dirname + '/' + dirname_filter_suffix
pathname_full = os.getenv('DATA_DIR') + '/' + dirname_full

print 'using output folder structure: ' + dirname_full

try:
    os.makedirs(pathname_full)
    os.makedirs(path_mc_data)
except OSError as exception:
    if exception.errno != errno.EEXIST:
        print 'error: thought dir does not exists but it does...'

f = open(pathname_base + '/sim_beam_prop.config', 'w')
f.write('ip_offset_x=' + str(args.use_ip_offset[0]) + '\nip_offset_y=' + str(args.use_ip_offset[1]) + '\nip_offset_z=' + str(args.use_ip_offset[2]) + '\nip_spread_x=' + str(args.use_ip_offset[3]) + '\nip_spread_y=' + str(args.use_ip_offset[4]) + '\nip_spread_z=' + str(args.use_ip_offset[5]) + '\nbeam_gradient_x=' + str(args.use_beam_gradient[0]) + '\nbeam_gradient_y=' + str(args.use_beam_gradient[1]) + '\nbeam_emittance_x=' + str(args.use_beam_gradient[2]) + '\nbeam_emittance_y=' + str(args.use_beam_gradient[3]))
f.close()

# try to search for mc files in other directories at the lowest level (of course num events and num samples have to match)
#linkPossibleMCFiles(dirname_full)

# now chop all jobs into bunches of 100 which is max job array size on himster atm
max_jobarray_size = 100

def is_exe(fpath):
  return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

program = 'qsub'
is_cluster = 0
for path in os.environ["PATH"].split(os.pathsep):
  path = path.strip('"')
  exe_file = os.path.join(path, program)
  if is_exe(exe_file):
    is_cluster = 1

program = 'parallel'
is_parallel = 0
for path in os.environ["PATH"].split(os.pathsep):
  path = path.strip('"')
  exe_file = os.path.join(path, program)
  if is_exe(exe_file):
    is_parallel = 1

failed_submit_commands = []

if is_cluster:
  print 'This is a cluster environment... submitting jobs to cluster!'
  #relinkSolenoidFieldMaps(args.lab_momentum[0]);
  
  for job_index in range(low_index_used, high_index_used + 1, max_jobarray_size):
      bashcommand = 'qsub -t ' + str(job_index) + '-' + str(min(job_index + max_jobarray_size - 1, high_index_used)) + ' -N lmd_fullsim_' + dirname \
                    + ' -l nodes=1:ppn=1,walltime=20:00:00,file=200mb -j oe -o ' + pathname_full + '/sim.log -v num_evts="' + str(args.num_events[0]) + '",mom="' + str(args.lab_momentum[0]) \
                    + '",gen_input_file_stripped="' + args.gen_data_dir + '/' + args.gen_data_dirname[0] + '/' + filename_base + '",dirname="' + dirname_full + '",pathname_base="' + path_mc_data + '",pathname="' + pathname_full \
                    + '",beamX0="' + str(args.use_ip_offset[0]) + '",beamY0="' + str(args.use_ip_offset[1]) + '",targetZ0="' + str(args.use_ip_offset[2]) \
                    + '",beam_widthX="' + str(args.use_ip_offset[3]) + '",beam_widthY="' + str(args.use_ip_offset[4]) + '",target_widthZ="' + str(args.use_ip_offset[5]) \
                    + '",beam_gradX="' + str(args.use_beam_gradient[0]) + '",beam_gradY="' + str(args.use_beam_gradient[1]) + '",beam_grad_sigmaX="' + str(args.use_beam_gradient[2]) + '",beam_grad_sigmaY="' + str(args.use_beam_gradient[3]) \
                    + '",SkipFilt="' + str(not args.use_xy_cut).lower() + '",XThetaCut="' + str(args.use_xy_cut).lower() + '",YPhiCut="' + str(args.use_xy_cut).lower() + '",CleanSig="' + str(args.use_m_cut).lower() + rec_ip_info + '" -V ./runLumiFullSimPixel.sh'
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

elif is_parallel:
  print 'This is not a cluster environment, but gnu parallel was found! Using gnu parallel!'
  relinkSolenoidFieldMaps(args.lab_momentum[0]);
  
  bashcommand = 'parallel -j' + str(cpu_cores) + ' ./runLumiFullSimPixel.sh ' + str(args.num_events[0]) + ' ' + str(args.lab_momentum[0]) + ' ' + args.gen_data_dir + '/' + args.gen_data_dirname[0] + '/' + filename_base + ' ' + dirname + ' ' + dirname_full + ' {} ' + str(args.use_ip_offset[0]) + ' ' + str(args.use_ip_offset[1]) + ' ' + str(args.use_ip_offset[2]) + ' ' + str(args.use_ip_offset[3]) + ' ' + str(args.use_ip_offset[4]) + ' ' + str(args.use_ip_offset[5]) + ' ' + str(args.use_beam_gradient[0]) + ' ' + str(args.use_beam_gradient[1]) + ' ' + str(args.use_beam_gradient[2]) + ' ' + str(args.use_beam_gradient[3])
  
  inputcommand = 'seq ' + str(low_index_used) + ' 1 ' + str(high_index_used)
  inproc = subprocess.Popen(inputcommand.split(), stdout=subprocess.PIPE)
  mainproc = subprocess.Popen(bashcommand.split(), stdin=subprocess.PIPE)
  mainproc.communicate(input=inproc.communicate()[0])

else:
  print 'This is not a cluster environment, and unable to find gnu parallel! Please install gnu parallel!' 

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
    time.sleep(1800) #sleep for 30min
