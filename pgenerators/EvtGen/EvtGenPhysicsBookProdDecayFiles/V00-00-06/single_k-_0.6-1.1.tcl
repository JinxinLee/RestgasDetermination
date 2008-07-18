#
# Generate single pi+
# P uniform 0.1-0.6 GeV   (lab)
# theta uniform 13-142deg (lab)
#
# Stefan Spanier 
# spanier@slac.stanford.edu
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "k-"
      Pmin set 0.6
      Pmax set 1.1
      usePt set false
      CosThetamin set -0.790
      CosThetamax set  0.975
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


