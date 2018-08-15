echo sim
root  -l -q sim_complete.C\(100,\"TGeant4\",10\) >& sim.log
echo digi
root  -l -q digi_complete.C >& digi.log
echo reco
root  -l -q reco_complete.C >& reco.log
echo pid
root  -l -q pid_complete.C >& pid.log 
#echo ana
#root  -l -q ana_complete.C >> ana.log 2>&1
echo END

