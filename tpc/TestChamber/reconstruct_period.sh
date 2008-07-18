#!/bin/bash

#for i in /afs/e18/panda/TPC/Test_Chamber_Data/produced_aug07/H*; do sh reconstruct_period.sh $i chr_time1_071016_1 config/chr_time1_071012.conf; done

LOGFILE=/afs/e18/panda/TPC/Test_Chamber_Data/produced_aug07/settings.log
touch $LOGFILE
OUTDIR_PREFIX=reco_trees
DATESTRING=`date +%F_%H-%M-%S`


OLD_SETTINGS=`cat $LOGFILE | grep "^Settings ID" | awk '{print$3;}'`


if [ -z $1 ]; then
	echo "Usage:"
	echo "sh reconstruct_period.sh PERIOD_DIR SETTINGSID CONFIGFILE"
	echo "or"
	echo "sh reconstruct_period.sh new NEWSETTINGSID CONFIGFILE"
	exit
fi

if [ -z $2 ]; then
	echo "Usage:"
	echo "sh reconstruct_period.sh PERIOD_DIR SETTINGSID CONFIGFILE"
	echo "or"
	echo "sh reconstruct_period.sh new NEWSETTINGSID CONFIGFILE"
	exit
fi

if [ -z $3 ]; then
	echo "Usage:"
	echo "sh reconstruct_period.sh PERIOD_DIR SETTINGSID CONFIGFILE"
	echo "or"
	echo "sh reconstruct_period.sh new NEWSETTINGSID CONFIGFILE"
	exit
fi

CONFFILE=$3

if [ $1 = "new" ]; then
	NEWSETTINGS=$2

	if [ -z $NEWSETTINGS ]; then
		echo "argument #2 was 'new' but no arg #3 is given for new ID ->abort"
		exit
	fi
	for i in $OLD_SETTINGS; do
		if [ $i = $NEWSETTINGS ]; then
			echo "The new setting ID $NEWSETTINGS already exists ->abort"
			exit
		fi
	done
	echo "" >> $LOGFILE
	echo "===================================================" >> $LOGFILE
	echo "Settings ID $NEWSETTINGS :" >> $LOGFILE
	echo "Date of creation:" >> $LOGFILE
	echo $DATESTRING >> $LOGFILE
	echo "PandaROOT repository revision:" >> $LOGFILE
	TEMPDIR=`pwd`
	cd $MYPANDA/tpc/TestChamber
	svn status -uv | tail -1 | awk '{print $4;}' >> $LOGFILE
	cd $TEMPDIR
	echo "config file $CONFFILE:" >> $LOGFILE
	echo "-----------------------" >> $LOGFILE
	cat $CONFFILE  >> $LOGFILE
	echo "//End of config file" >> $LOGFILE
	echo "-----------------------" >> $LOGFILE
	echo "---------------------------------------------------" >> $LOGFILE
	echo "Created new setting entry in logfile"
	exit
else
	PERIOD_DIR=$1
	SETTINGS=$2
	FLAG="0"
	for i in $OLD_SETTINGS; do
		if [ $i = $SETTINGS ]; then
			FLAG="1"
		fi
	done
	if [ $FLAG = "0" ]; then
		echo "The setting ID $SETTINGS does not exist, you would have to use the new option ->abort"
		exit
	fi
fi

OUTDIR=$PERIOD_DIR/$OUTDIR_PREFIX\_$SETTINGS\_$DATESTRING
mkdir $OUTDIR
cd $PERIOD_DIR
for i in multi*.root; do
	cd $MYPANDA/tpc/TestChamber
	if [ -e $PERIOD_DIR/$i ]; then
		./testChamberReco $PERIOD_DIR/$i $OUTDIR/reco_$i $CONFFILE 2>&1 | tee $OUTDIR/reco_$i.log
	else
		echo "No multi*.root file in PERIOD_DIR"
		rmdir $OUTDIR
		exit
	fi
	cd $PERIOD_DIR
done

