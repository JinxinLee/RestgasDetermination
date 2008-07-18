#
# Generate single photons 4000 MeV (lab)
# theta uniform 13-142deg (lab)
#
# Bill Lockman
# lockman@slac.stanford.edu
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "gamma"
      Pmin set 4.0
      Pmax set 4.0
      usePt set false
      CosThetamin set -0.79
      CosThetamax set 0.975
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


