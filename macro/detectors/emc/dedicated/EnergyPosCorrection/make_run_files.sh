#!/bin/bash
# This script produce many bash scripts to run data production for emc correction
# Each of the scripts will run simulation nruns (50 by default) for nevents (10k by default) 
# for each option set {"TS", "shashlyk"} {"low","high"} energy range
# "TS" - target spectrometer
# "low" (0-1.5 Gev), "high" (1-10 GeV)
fullPath="/lustre/panda/dmelnych/emc_correction/"
engine="TGeant3"
particle="gamma"
# number of runs for each option set
nruns1=50
nruns2=50
nruns3=50
nruns4=50
# number of events in run
nevents=10000

# Prepare template files for each option
# "TS", "low"
filename1=run_data_production_1000.sh
# "TS", "high"
filename2=run_data_production_2000.sh
# "shashlyk", "low"
filename3=run_data_production_3000.sh
# "shashlyk", "high"
filename4=run_data_production_4000.sh

if [ -e $filename1 ] ; then 
	printf 'Template  already exists. Removed.\n'
	rm $filename1
fi
if [ -e $filename2 ] ; then 
	printf 'Template  already exists. Removed.\n'
	rm $filename2
fi
if [ -e $filename3 ] ; then 
	printf 'Template  already exists. Removed.\n'
	rm $filename3
fi
if [ -e $filename4 ] ; then 
	printf 'Template  already exists. Removed.\n'
	rm $filename4
fi
printf '#$ -j y \n' | tee -a $filename1 $filename2 $filename3 $filename4
printf '#$ -wd %s \n' "$fullPath" | tee -a $filename1 $filename2 $filename3 $filename4
printf '. %sinit.sh; \n' "$fullPath"| tee -a $filename1 $filename2 $filename3 $filename4
printf '. /lustre/panda/dmelnych/pandaroot/build/config.sh;\n'| tee -a $filename1 $filename2 $filename3 $filename4
printf 'echo $SIMPATH; \n'| tee -a $filename1 $filename2 $filename3 $filename4
printf 'echo $ROOTSYS; \n'| tee -a $filename1 $filename2 $filename3 $filename4
printf 'root -l -b -q "emc_correction_data_production.C(%s,\\"gamma\\",\\"TGeant3\\",true,\\"TS\\",\\"low\\",1000)" \n' "$nevents">> $filename1
printf 'root -l -b -q "emc_correction_data_production.C(%s,\\"gamma\\",\\"TGeant3\\",true,\\"TS\\",\\"high\\",2000)" \n' "$nevents">> $filename2
printf 'root -l -b -q "emc_correction_data_production.C(%s,\\"gamma\\",\\"TGeant3\\",true,\\"shashlyk\\",\\"low\\",3000)" \n' "$nevents">> $filename3
printf 'root -l -b -q "emc_correction_data_production.C(%s,\\"gamma\\",\\"TGeant3\\",true,\\"shashlyk\\",\\"high\\",4000)" \n' "$nevents">> $filename4

chmod +x $filename1
chmod +x $filename2
chmod +x $filename3
chmod +x $filename4

for ((i=1001;i<=(1000+nruns1);i++))
do
in1="1000)"
out1="$i)"
cp run_data_production_1000.sh run_data_production_"$i".sh
sed -i 's/'$in1'/'$out1'/g' run_data_production_"$i".sh
done

for ((i=2001;i<=(2000+nruns2);i++))
do
in1="2000)"
out1="$i)"
cp run_data_production_2000.sh run_data_production_"$i".sh
sed -i 's/'$in1'/'$out1'/g' run_data_production_"$i".sh
done

for ((i=3001;i<=(3000+nruns3);i++))
do
in1="3000)"
out1="$i)"
cp run_data_production_3000.sh run_data_production_"$i".sh
sed -i 's/'$in1'/'$out1'/g' run_data_production_"$i".sh
done

for ((i=4001;i<=(4000+nruns4);i++))
do
in1="4000)"
out1="$i)"
cp run_data_production_4000.sh run_data_production_"$i".sh
sed -i 's/'$in1'/'$out1'/g' run_data_production_"$i".sh
done
