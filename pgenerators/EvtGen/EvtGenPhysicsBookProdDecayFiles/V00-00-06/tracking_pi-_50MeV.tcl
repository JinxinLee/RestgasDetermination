#
# Generate single pi- with Pt of 50 MeC.
#
#
# Contact Anders Ryd (ryd@slac.stanford.edu)
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "pi-"
      Pmin set 0.05
      Pmax set 0.05
      usePt set true
      CosThetamin set -0.9
      CosThetamax set 0.95
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


