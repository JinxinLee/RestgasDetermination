#!/bin/bash
#Parameters:
ARGS=6
echo "\$1 = $1 (data directory)"
echo "\$2 = $2 (run prefix)"
echo "\$3 = $3 (momentum)"
echo "\$4 = $4 (nEvents)"
echo "\$5 = $5 (ranSeed)"
echo "\$6 = $6 (Process number)"
test $# -ne $ARGS && echo "Usage: `basename $0` wiht $ARGS argument(s)" && exit $ARGS
seed=$(($5+$6))
echo "seed = $seed"
filename=$1"/"$2"-"$3"GeV-"$4"-"$seed".root"
echo "filename = $filename"
simargs="(\"$filename\",$3,$4,$seed)"
args="(\"$filename\")"
argstestsim="(\"$filename\",\"sim\")"
argstestdigi="(\"$filename\",\"digi\")"
argstestpid="(\"$filename\",\"pid\")"

echo
echo root -b -q runSim.C$simargs
echo root -b -q runDigiReco.C$args
echo root -b -q runTrackingPid.C$args
echo

CHECK=1
COUNT=0

while [ $CHECK = 1 -a $COUNT -lt 2 ]; do

if [ $COUNT -gt 0 ]; then
    echo
    echo '---------------'
    echo 'second iteration'
    echo '---------------'
    echo
    let "seed += 1000000"
    filename=$1"/"$2"-"$3"GeV-"$4"-"$seed".root"
    simargs="(\"$filename\",$3,$4,$seed)"
    args="(\"$filename\")"
    argstestsim="(\"$filename\",\"sim\")"
    argstestdigi="(\"$filename\",\"digi\")"
    argstestpid="(\"$filename\",\"pid\")"
 
else
    echo
    echo '---------------'
    echo 'First iteration'
    echo '---------------'
    echo
fi

# Sim

root -b -q runSim.C$simargs &> /dev/null

# Check Sim

root -b -q testFile.C$argstestsim &> /dev/null
T1=$?

if [ $T1 = 0 ]; then
    echo
    echo 'Simulation completed with success'
    echo
else
    echo
    echo 'Problems during the simulation, running again'
    echo
    root -b -q runSim.C$simargs &> /dev/null
    root -b -q testFile.C$argstestsim &> /dev/null 
    T1=$?
fi


if [ $T1 = 0 ]; then

# Digi

root -b -q runDigiReco.C$args &> /dev/null

# Check Digi

root -b -q testFile.C$argstestdigi &> /dev/null 
T2=$?

if [ $T2 = 0 ]; then
    echo
    echo 'Digitization completed with success'
    echo
else
    echo
    echo 'Problems during the digitization, running again'
    echo
    root -b -q runDigiReco.C$args &> /dev/null
    root -b -q testFile.C$argstestdigi &> /dev/null 
    T2=$?
fi

if [ "$T1 = 0" -a "$T2 = 0" ]; then

# tracking pid

root -b -q runTrackingPid.C$args &> /dev/null

# Check Pid

root -b -q testFile.C$argstestpid &> /dev/null 
T3=$?

if [ $T3 = 0 ]; then
    echo
    echo 'Tracking/PID completed with success'
    echo
else
    echo
    echo 'Problems during tracking/PID, running again'
    echo
    root -b -q runTrackingPid.C$args &> /dev/null
    root -b -q testFile.C$argstestpid &> /dev/null 
    T3=$?
fi

else
    echo
    echo 'Twice problems during the digitization, skipping PID/tracking'
    echo
fi

else
    echo
    echo 'Twice problems during the simulation, skipping the other tasks'
    echo
fi

if [ "$T1 -eq 0" -a "$T2 -eq 0" -a "$T3 -eq 0" ]; then
    CHECK=0
else
    CHECK=1
fi

let "COUNT += 1"

done

echo
echo 'End of the script'