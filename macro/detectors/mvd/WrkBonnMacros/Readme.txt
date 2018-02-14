Set of macros and scripts to run full simulations, digitization, reconstruction, tracking and PID within pandaroot.

###########
# macros: #
###########

runSim.C : it runs a sim with Mvd-FullMaterial, Stt, Gem, Emc, Muon Detector, Dirc, Dsk and the default beam pipe. EvtGen is used to simulate our channels.

runDigiReco.C: this macro performs digitization and reconstruction (for the detectors not needing pattern recognition)

runTrackingPid.C: new Mvd+Stt pattern recognition (NOT LHE), Kalman filter, Pid Correlator and ideal Pid Associator

anaDMesonsCharged.C: example of an analysis macro

############
# scripts: #
############

go.sh: The script is running Sim, DigiReco and TrackingPid checking the output files. If there is an error in one of the phases, first the script tries to re-run this step. Then, if this doesn't work it's re-running the all chain starting from the simulation and using a new seed number which is set to be 1000000+original_seed. This allows to identify easily files coming from this second iteration.

Usage: 

$1 =  (data directory)
$2 =  (run prefix)
$3 =  (momentum)
$4 =  (nEvents)
$5 =  (ranSeed)
$6 =  (Process number) [This is useful when working on the cluster]
Usage: go.sh wiht 6 argument(s)

condor.cmd: condor script to run go.sh on the cluster (in Bonn@CBcluster). You just have to modify the line setting the arguments to be passed to the script. The last line ("queue") sets the number of jobs you want to run in parallel. "output", "error" and "log" specify the path where you want to have the log files produced by the jobs.

TIP: as you can see I used "LOG" as a path for the log files and "DATA" for the storage of produced files. This is a convenient way of doing it since you can have two symbolic links called DATA and LOG in your folder which are pointing to the final destinations. 