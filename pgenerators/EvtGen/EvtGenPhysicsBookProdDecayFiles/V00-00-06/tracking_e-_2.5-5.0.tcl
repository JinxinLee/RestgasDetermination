#
# Generate single e- with P uniform in the range
# from 2.5MeV to 5.0GeV in the labframe.
#
#
# Contact Giampi (Giampi@slac.stanford.edu)
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "e-"
      Pmin set 2.5
      Pmax set 5.0
      usePt set false
      CosThetamin set -1.0
      CosThetamax set 1.0
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


