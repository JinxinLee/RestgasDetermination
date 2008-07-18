# $Id: mu_pair_bgsphysicsoff.tcl,v 1.1.1.1 2005/03/29 17:12:44 steinke Exp $
#
# This tcl configures the Bkqed generator to generate mu-pairs over
# a solid angel region 13 to 160 degrees from the beam-pipe in the
# cms frame. All physics processes in G4 are turned off.
#

sourceFoundFile BgsApp/BgsGHitPhysicsOff.tcl

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
