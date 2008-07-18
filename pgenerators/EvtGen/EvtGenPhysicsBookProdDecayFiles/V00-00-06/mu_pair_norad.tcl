#
# This tcl configures the Bkqed generator to generate mu-pairs over
# a solid angle region 13 to 160 degrees from the beam-pipe in the
# cms frame.
#
# Both ISR and FSR are switched off. However, there will still
# be some CM energy spread unless it is explpicitly hardcoded to 0 in
# PepEnvData/PepBeams.cc. -- Bill Lockman 2001-Sep-25
#
disableGenerators 0
module enable GfiBkqed

mod talk GfiBkqed
  generate set "mu+ mu-"
  minThetaParticle1 set 13.0 
  maxThetaParticle1 set 160.0
  minThetaParticle2 set 13.0
  maxThetaParticle2 set 160.0
#  minEnergyParticle1 set 0.1
#  minEnergyParticle2 set 0.1
#  maxCMSAcollinearity set 180.0
#  minThetaPhoton set 0.
#  maxThetaPhoton set 180.
#  minEnergyPhoton set 0.001
#  minNumEnergeticPhotons set 0
  maxNumPhotonsProduced set 0
  dkmin set 0
  dkmax set 0.99
#  minPhotonTrackSeparation set 0.0
exit
