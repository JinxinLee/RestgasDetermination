# Filter for pi0pi0 
# Aaron Roodman Jan 10,2003


talkto RandomControl {
    maxRandomsPerEvent set 120000000
}
talkto GfiEvtGen {
 maxNumFilterTrials set 1000000
}


mod talk GefSelectFilter 

# photons inside the detector (CM frame!)  Lab: -0.78 to 0.96 >> CM: around -0.92 to 0.90
BooObjects GefKinematic define PhotonAcc = gamma 0.0 10.0 -100.0 100.0 -0.92 0.92 

# pi0 from the photons
BooObjects GefKinematic combine Pi0Acc = PhotonAcc PhotonAcc 0.10 0.16 1.0 10.0

# combine 2 pi0 into B0
BooObjects GefKinematic combine B0Pi0Pi0 = Pi0Acc Pi0Acc -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.175  -0.6 0.6

# require a B
BooObjects GefKinematic require B0Pi0Pi0

# Filter in CM
beforeFilter  set GefKinematic

exit
