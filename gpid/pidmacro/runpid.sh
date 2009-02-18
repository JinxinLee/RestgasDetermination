cp $PANDAROOTHOME/PndTools/mpiTools/macros/emc/rootlogon.C .
#
# Run the scripts
#
ls
echo $1
echo $2
echo $3
echo $4
echo $5

# $2 no of events
# $3
root -l -b -q "run_sim_alldet.C($2,\"$3\",$4,$5,$6,$7,$8,$9,$1)" >> logfile 2>&1

root -l -b -q "run_rec_alldet.C" >> logfile 2>&1


echo "hi here is a echo%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
ls
exit 0

##############-----This part is for extended simfiles to be analysed by full_emc #######
cnt=1
for FILENAME in "`find . -name "points_sttcombi.root_*" -print`" ; do
 if [ -n "$FILENAME" ]; then
    root -l -b -q "full_emc.C(\"$FILENAME\",\"params_sttcombi.root\",\"reco_sttcombi.root_$cnt\")" >> logfile 2>&1
    let cnt=cnt+1
 fi
done

#
# Validate the output and return the appropiate value
#

for ofile in "logfile" "run_sim_alldet.C" "run_rec_alldet.C" "rootlogon.C" "points_sttcombi.root" "params_sttcombi.root" "reco_sttcombi.root" ; do
  [ -f $ofile ]  || error="$error $ofile doesn't exist,";
done

for message in "Segmentation violation" "Segmentation fault" "Abort" "Bus error" "Floating point exception" "root: command not found" "cp: cannot stat" "Error opening Input file" ; do
 grep -i "$message" logfile &&  error="$error and $message"
done
#
# Success
#


if [ -z "$error"  ] ; then
  exit 0
fi
#
# Failure
#
cat logfile
echo "<E> Validation failed with $error"
exit -1
                                                                                                                                           94,1          Bot


