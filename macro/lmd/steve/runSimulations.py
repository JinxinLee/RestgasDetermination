import os, sys, re
from subprocess import call

lib_path = os.path.abspath('argparse-1.2.1/build/lib')
sys.path.append(lib_path)

import argparse

parser = argparse.ArgumentParser(description='Script for full simulation of PANDA Luminosity Detector via externally generated MC data.', formatter_class=argparse.RawTextHelpFormatter)

parser.add_argument('num_events', metavar='num_events', type=int, nargs=1, help='number of events to simulate')
parser.add_argument('lab_momentum', metavar='lab_momentum', type=float, nargs=1, help='lab momentum of incoming beam antiprotons\n(required to set correct magnetic field maps etc)')
parser.add_argument('gen_data_dirname', metavar='gen_data_dirname', type=str, nargs=1, help='Name of directory containing the generator data that is used as input. Note that this is only the name of the directory and NOT the full path. The base path of the directory should be specified with the --gen_data_dir flag.')

parser.add_argument('--low_index', metavar='low_index', type=int, default=-1,
                   help='Lowest index of generator file which is supposed to be used in the simulation. Default setting is -1 which will take the lowest found index.')
parser.add_argument('--high_index', metavar='high_index', type=int, default=-1,
                   help='Highest index of generator file which is supposed to be used in the simulation. Default setting is -1 which will take the highest found index.')

parser.add_argument('--gen_data_dir', metavar='gen_data_dir', type=str, default=os.getenv('GEN_DATA'),
                   help='Base directory to input files created by external generator. By default the environment variable $GEN_DATA will be used!')

parser.add_argument('--genfile_index_range', metavar=('genfile_index_lower_range' 'genfile_index_lower_range'), type=int, nargs=2, default=[min, max], help='Index range of generator files to be processed. By default all available files will be processed!')

parser.add_argument('--use_beam_offset', metavar=("beam_offset_x", "beam_offset_y", "beam_spread_x", "beam_spread_y"), type=float, nargs=4, default=[0.0, 0.0, 0.0, 0.0],
                   help="beam_offset_x: interaction vertex mean X position (in cm)\n"
			"beam_offset_y: interaction vertex mean Y position (in cm)\n"
			"beam_spread_x: interaction vertex X position distribution width (in cm)\n"
			"beam_spread_y: interaction vertex Y position distribution width (in cm)")

parser.add_argument('--use_beam_gradient', metavar=("beam_gradient_x", "beam_gradient_y", "beam_emittance_x", "beam_emittance_y"), type=float, nargs=4, default=[0.0, 0.0, 0.0, 0.0],
                   help="beam_gradient_x: mean beam inclination on target in x direction dPx/dPz (in mrad)\n"
			"beam_gradient_y: mean beam inclination on target in y direction dPy/dPz (in mrad)\n"
			"beam_emittance_x: beam emittance in x direction (in mrad)\n"
			"beam_emittance_y: beam emittance in y direction (in mrad)")

'''with this line the user can be prompted for input '''

args = parser.parse_args()

# generate output directory name from specified beam parameters
filename_base = re.sub('\.', 'o', args.gen_data_dirname[0])

dirname = args.gen_data_dirname[0] + '_pixel_beam_offsetXYDXDY'
for val in args.use_beam_offset:
	dirname = dirname + '_' + str(val)  
dirname += '_gradXYDXDY'
for val in args.use_beam_gradient:
	dirname = dirname + '_' + str(val)

dirname_full = os.getenv('DATA_DIR') + '/' + dirname

path=args.gen_data_dir
while not os.path.isdir(path):
	path=raw_input('Please enter valid generator base path: ')


# check for the index range in the specified generator folder
first = 1
lowest_index = -1;
highest_index = -1;

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

low_index_used=lowest_index
high_index_used=highest_index

if args.low_index > lowest_index and args.low_index <= highest_index:
	low_index_used = args.low_index
if args.high_index < highest_index and args.high_index >= lowest_index:
	high_index_used = args.high_index

print 'preparing simulations in index range ' + str(low_index_used) + ' - ' + str(high_index_used)


# now chop all jobs into bunches of 100 which is max job array size on himster atm
max_jobarray_size = 100

for job_index in range(low_index_used, high_index_used, max_jobarray_size):
	bashcommand = 'qsub -t ' + str(job_index) + '-' + str(min(job_index+max_jobarray_size-1, high_index_used)) + ' -N reconstructMCData_'+dirname+' -l nodes=1:ppn=1,walltime=20:00:00 -j oe -o /home/pflueger/himster_output_logs/'+dirname+' -v var1="'+str(args.num_events[0])+'",var2="'+str(args.lab_momentum[0])+'",var3="'+args.gen_data_dir+'/'+args.gen_data_dirname[0] + '/' + filename_base+'",var4="'+dirname+'",var5="'+dirname_full+'",var7="'+str(args.use_beam_offset[0])+'",var8="'+str(args.use_beam_offset[1])+'",var9="'+str(args.use_beam_offset[2])+'",var10="'+str(args.use_beam_offset[3])+'",var11="'+str(args.use_beam_gradient[0])+'",var12="'+str(args.use_beam_gradient[1])+'",var13="'+str(args.use_beam_gradient[2])+'",var14="'+str(args.use_beam_gradient[3])+'" -V ./runLumiFullSimPixel.sh'
	call(bashcommand.split())
