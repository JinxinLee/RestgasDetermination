#!/usr/bin/python

import os, sys, re, errno, glob
import subprocess
import multiprocessing

cpu_cores = multiprocessing.cpu_count()

lib_path = os.path.abspath('argparse-1.2.1/build/lib')
sys.path.append(lib_path)

import argparse
import ConfigParser

# define a function that relinks the field maps (temp workaround for the hc solenoid field map problem)
def relinkSolenoidFieldMaps(mom):
    if mom > 3.0:
        fieldmap_path = os.getenv('VMCWORKDIR') + '/macro/lmd/Anastasia/solenoid_fc';
    else:
        fieldmap_path = os.getenv('VMCWORKDIR') + '/macro/lmd/Anastasia/solenoid_hc';
    
    fieldmaps = os.listdir(fieldmap_path);
    for fieldmap in fieldmaps:
        if os.path.splitext(fieldmap)[1] == '.root':
            bashcommand = 'ln -sf ' + fieldmap_path + '/' + fieldmap + ' ' + os.getenv('VMCWORKDIR') + '/input/.'
            subprocess.call(bashcommand.split())
    
    return;

# define a function that generates the simulation config file
def generateLmdSimConfig(path, arguments):
    config = ConfigParser.RawConfigParser()

    config.optionxform = str

    config.add_section('Basics')
    config.set('Basics', 'beam_momentum', str(arguments.lab_momentum[0]))
    config.set('Basics', 'num_events', str(arguments.num_events[0]))
    config.set('Basics', 'generator_filename_template', arguments.gen_data_dirname[0])
    
    config.add_section('Reco')
    config.set('Reco', 'use_missing_plane', str(int(arguments.use_missing_plane)))
    config.set('Reco', 'use_track_cuts', str(int(arguments.use_track_cuts)))
    config.set('Reco', 'merge_hits', str(int(arguments.merge_hits)))
    
    xcut = '1'
    ycut = '1'
    boxcut = '0'
    
    if arguments.track_filter == 'NONE':
        xcut = '0'
        ycut = '0'
    elif arguments.track_filter == 'BOX':
        xcut = '0'
        ycut = '0'
        boxcut = '1'
    elif arguments.track_filter == 'X':
        ycut = '0'
    elif arguments.track_filter == 'Y':
        xcut = '0'
                    
    config.set('Reco', 'use_x_theta_cut', xcut)
    config.set('Reco', 'use_y_phi_cut', ycut)
    config.set('Reco', 'use_box_cut', boxcut)

    config.set('Reco', 'clean_backpropagated_data', str(int(arguments.clean_backpropagated_data)))
    config.set('Reco', 'write_all_MC_info', str(int(arguments.write_all_MC_info)))
    
    config.add_section('IP-Beam')
    config.set('IP-Beam', 'ip_offset_x', str(arguments.use_ip_offset[0]))
    config.set('IP-Beam', 'ip_offset_y', str(arguments.use_ip_offset[1]))
    config.set('IP-Beam', 'ip_offset_z', str(arguments.use_ip_offset[2]))
    config.set('IP-Beam', 'ip_spread_x', str(arguments.use_ip_offset[3]))
    config.set('IP-Beam', 'ip_spread_y', str(arguments.use_ip_offset[4]))
    config.set('IP-Beam', 'ip_spread_z', str(arguments.use_ip_offset[5]))
    config.set('IP-Beam', 'beam_gradient_x', str(arguments.use_beam_gradient[0]))
    config.set('IP-Beam', 'beam_gradient_y', str(arguments.use_beam_gradient[1]))
    config.set('IP-Beam', 'beam_divergence_x', str(arguments.use_beam_gradient[2]))
    config.set('IP-Beam', 'beam_divergence_y', str(arguments.use_beam_gradient[3]))

    # Writing our configuration file to 'example.cfg'
    with open(path + '/sim_prop.cfg', 'wb') as configfile:
        config.write(configfile)


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

parser.add_argument('--force_directory', action='store_true', help='Force the usage of the specified directory directly instead of adding beam offset infos etc.')


parser.add_argument('--no-missing_plane', dest='use_missing_plane', action='store_false',
                    help='Do NOT missing plane algorithm.')
parser.add_argument('--no-track_cuts', dest='use_track_cuts', action='store_false',
                    help='Do NOT cut on tracks during search.')
parser.add_argument('--no-hit-merging', dest='merge_hits', action='store_false',
                    help='Do NOT merge hits on front and back of a plane.')
parser.add_argument('track_filter', nargs='?', choices=['NONE', 'BOX', 'X', 'Y', 'XY'], default='XY',
                    help='Choice between different track filtering types:'
                    'BOX: some box cut i dont know ask Anastasia...'
                    'X: X & theta cut'
                    'Y: Y & phi cut'
                    'XY: cut both X & theta and Y & phi')

parser.add_argument('--no-data_cleaning', dest='clean_backpropagated_data', action='store_false',
                    help='Do Not perform Multi Variable Analysis (mainly momentum cut) on back-propagated data.')
parser.add_argument('--no-write_all_MC_info', dest='write_all_MC_info', action='store_false',
                    help='Do NOT write MC info which is not reference to any hit (required for acceptances).')

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

'''with this line the user can be prompted for input '''

args = parser.parse_args()


# generate the output directory name

filename_base = args.gen_data_dirname[0]
if not args.force_directory:
  # generate output directory name from specified beam parameters
  filename_base = re.sub('\.', 'o', args.gen_data_dirname[0])

  dirname = args.gen_data_dirname[0] + '_pixel_ip_offsetXYZDXDYDZ'
  for val in args.use_ip_offset:
	dirname = dirname + '_' + str(val)  
  dirname += '_gradXYDXDY'
  for val in args.use_beam_gradient:
	dirname = dirname + '_' + str(val)
else:
  dirname = args.gen_data_dirname[0]

dirname_full = os.getenv('DATA_DIR') + '/' + dirname

path = args.gen_data_dir
while not os.path.isdir(path):
	path = raw_input('Please enter valid generator base path: ')


# check for the index range in the specified generator folder
first = 1
lowest_index = -1;
highest_index = -1;

if not args.force_directory:
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
else:
  low_index_used = args.low_index
  high_index_used = args.high_index

print 'preparing simulations in index range ' + str(low_index_used) + ' - ' + str(high_index_used)

try:
	os.makedirs(dirname_full)
except OSError as exception:
        if exception.errno != errno.EEXIST:
		print 'error: thought dir does not exists but it does...'

generateLmdSimConfig(dirname_full, args)

exit(0)

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


if is_cluster:
  print 'This is a cluster environment... submitting jobs to cluster!'
  relinkSolenoidFieldMaps(args.lab_momentum[0]);
  
  for job_index in range(low_index_used, high_index_used + 1, max_jobarray_size):
	bashcommand = 'qsub -t ' + str(job_index) + '-' + str(min(job_index + max_jobarray_size - 1, high_index_used)) + ' -N lmd_fullsim_' + dirname + ' -l nodes=1:ppn=1,walltime=20:00:00 -j oe -o ' + dirname_full + '/sim.log -v num_evts="' + str(args.num_events[0]) + '",mom="' + str(args.lab_momentum[0]) + '",gen_input_file_stripped="' + args.gen_data_dir + '/' + args.gen_data_dirname[0] + '/' + filename_base + '",dirname="' + dirname + '",pathname="' + dirname_full + '",beamX0="' + str(args.use_ip_offset[0]) + '",beamY0="' + str(args.use_ip_offset[1]) + '",targetZ0="' + str(args.use_ip_offset[2]) + '",beam_widthX="' + str(args.use_ip_offset[3]) + '",beam_widthY="' + str(args.use_ip_offset[4]) + '",target_widthZ="' + str(args.use_ip_offset[5]) + '",beam_gradX="' + str(args.use_beam_gradient[0]) + '",beam_gradY="' + str(args.use_beam_gradient[1]) + '",beam_grad_sigmaX="' + str(args.use_beam_gradient[2]) + '",beam_grad_sigmaY="' + str(args.use_beam_gradient[3]) + '" -V ./runLumiFullSimPixel.sh'
	subprocess.call(bashcommand.split())

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
