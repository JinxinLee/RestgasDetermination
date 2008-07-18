#
# Generate single eta with P of 1000MeV.
#
# Sven Menke (menke@slac.stanford.edu)
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "eta"
      Pmin set 1.000
      Pmax set 1.000
      usePt set false
      CosThetamin set -1.0
      CosThetamax set 1.0
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


