#
# This tcl uses Kk2f for generation of radiative 13 deg Bhabhas with s' < 7.5 GeV, |cT|<.87
#
disableGenerators 0
module enable GfiBhwide

module talk GfiBhwide
minNumEnergeticPhotons set 1

# Rounding of the theta cuts is done outward since 
# the Gen-level filter will remove the tails anyway.

minThetaPhoton set 17.5
maxThetaPhoton set 131.7

# The lab energy for a photon with E* = 2.63 GeV
# and cos T* = -0.87 is 1.7345

minEnergyPhoton set 1.70
minThetaPositron set 13
maxThetaPositron set 142
minThetaElectron set 13
maxThetaElectron set 142
show
exit

mod talk GefSelectFilter
BooObjects GefIsr sqrtseff_multigam 0 7.5 0.87
beforeFilter set GefIsr
show
exit

