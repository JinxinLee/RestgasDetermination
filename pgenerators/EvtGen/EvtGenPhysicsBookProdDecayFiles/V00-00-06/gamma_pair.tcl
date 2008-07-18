#
# This tcl configures the Bkqed to generate gamma pairs over
# a solid angel region 1 of 13 ro 160 degrees from the beam-pipe in the
# cms frame.
#
disableGenerators 0
module enable GfiBkqed

module talk GfiBkqed
  generate set "gamma gamma"
  minThetaParticle1 set 13.0 
  maxThetaParticle1 set 160.0
  minThetaParticle2 set 13.0
  maxThetaParticle2 set 160.0
  minEnergyParticle1 set 0.1
  minEnergyParticle2 set 0.1
  maxCMSAcollinearity set 180.0
  minThetaPhoton set 0.
  maxThetaPhoton set 180.
  minEnergyPhoton set 0.001
  minNumEnergeticPhotons set 0
  maxNumPhotonsProduced set 10
  minPhotonTrackSeparation set 0.0
exit
