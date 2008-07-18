#
# Generate single pi0 with P of 200MeV.
#
# Sven Menke (menke@slac.stanford.edu)
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "pi0"
      Pmin set 0.200
      Pmax set 0.200
      usePt set false
      CosThetamin set -1.0
      CosThetamax set 1.0
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


