#
# Generate single pi- 
# P 1.0 GeV (lab)
# theta 45deg (lab)
#
# Christos Touramanis 
# christos@slac.stanford.edu
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "pi-"
      Pmin set 1.0
      Pmax set 1.0
      usePt set false
      CosThetamin set 0.707
      CosThetamax set 0.707
      ipX set 0.0
      ipY set 0.0
      ipZ set 0.0
      ipT set 0.0
exit


