#
# Generate single K- with P uniform in the range
# from 50MeV to 2.5GeV in the labframe.
#
# Use non-default FFREAD file to save more MC truth tree 
#
# Contact Dave Aston (dyaeb@slac.stanford.edu)
#
global env
 
module talk BbsModule
   ffReadFileName set "$env(DECAYFILES)/MCtruth.ffr"
exit

disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "K-"
      Pmin set 0.05
      Pmax set 2.5
      usePt set false
      CosThetamin set -1.0
      CosThetamax set 1.0
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


