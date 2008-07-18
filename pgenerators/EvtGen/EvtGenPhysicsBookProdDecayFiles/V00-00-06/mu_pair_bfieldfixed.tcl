# $Id: mu_pair_bfieldfixed.tcl,v 1.1.1.1 2005/03/29 17:12:44 steinke Exp $
#
# This tcl configures the Bkqed generator to generate mu-pairs over
# a solid angle region 13 to 160 degrees from the beam-pipe in the
# cms frame in a fixed 1.5T BField
#

module talk TrkBuildEnv
   fieldProxy set Fixed
   bx set 0.
   by set 0.
   bz set 1.5
   NominalField set 1.5
exit

echo using fixed field \(0, 0, 1.5T\)

disableGenerators 0
module enable GfiBkqed

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

