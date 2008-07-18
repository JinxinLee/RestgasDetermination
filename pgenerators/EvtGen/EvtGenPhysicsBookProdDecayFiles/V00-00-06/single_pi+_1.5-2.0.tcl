#
# Generate single pi+
# P uniform 1.5-2.0 GeV (lab)
# theta uniform 13-142deg (lab)
#
# Christos Touramanis 
# christos@slac.stanford.edu
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "pi+"
      Pmin set 1.5
      Pmax set 2.0
      usePt set false
      CosThetamin set -0.79
      CosThetamax set 0.975
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


