echo sim
root  -l -q sim.C"(10)" >> sim.log 2>&1 
echo digi
root  -l -q digi.C >> digi.log 2>&1 
echo reco
root  -l -q reco.C >> reco.log 2>&1
echo pid
root  -l -q pid.C >> pid.log 2>&1 
echo mc
root  -l -q clonemc.C >> clonemc.log 2>&1
echo digi+some hit
root  -l -q digihit.C >> digihit.log 2>&1
echo END

