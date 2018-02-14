
cat >job_ModulesStat.sh <<EOF
#!/bin/bash
#
#PBS -N lmdAlignStat
#PBS -j oe
#PBS -o LOGlmdAlignStat
#PBS -V
#PBS -l nodes=1:ppn=1,walltime=23:59:00,vmem=2GB
export PATH=\$PBS_O_PATH
source /home/karavdin/pandaRoot12/build/config.sh
./ModulesStat >> OUTlmdAlignStat
exit 0
EOF

### submit job to batch system
    	    iter=0
            while [ $iter -lt 1 ];do 
		qsub job_ModulesStat.sh
		done_test=$(echo $?)
		if [ "$done_test" -eq "0" ];then
		    echo "Job submitted!"
		    iter=1
		else
		    echo "qsub did not work sleep for 60 seconds and then try again." 
		    sleep 60
		fi
	    done
	    