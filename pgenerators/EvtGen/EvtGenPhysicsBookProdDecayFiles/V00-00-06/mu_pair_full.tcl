#
# This tcl configures the Bkqed generator to generate mu-pairs over
# the full angular range.  
# C. Hearty, Nov. 23, 1999, hearty@physics.ubc.ca
#
disableGenerators 0
module enable GfiBkqed

module talk GfiBkqed
  generate set "mu+ mu-"
  minThetaParticle1 set 0. 
  maxThetaParticle1 set 180.0
  minThetaParticle2 set 0.
  maxThetaParticle2 set 180.0
  minEnergyParticle1 set 0.1
  minEnergyParticle2 set 0.1
  maxCMSAcollinearity set 180.0
  minThetaPhoton set 0.
  maxThetaPhoton set 180.
  minEnergyPhoton set 0.001
  minNumEnergeticPhotons set 0
  maxNumPhotonsProduced set 1
  minPhotonTrackSeparation set 0.0
exit
