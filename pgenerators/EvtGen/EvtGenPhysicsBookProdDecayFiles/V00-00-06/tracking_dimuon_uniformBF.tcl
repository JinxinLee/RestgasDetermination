#
# This tcl configures the Bkqed generator to generate mu-pairs over
# a solid angel region 13 to 160 degrees from the beam-pipe in the
# cms frame.
#
# Contact Anders Ryd (ryd@slac.stanford.edu)
#
disableGenerators 0
module enable GfiBkqed

module talk BbsModule
#make the field uniform in tracking volume
  dbinCmd -i run fld_flg 2
exit


module talk GfiBkqed
  generate set "mu+ mu-"
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
