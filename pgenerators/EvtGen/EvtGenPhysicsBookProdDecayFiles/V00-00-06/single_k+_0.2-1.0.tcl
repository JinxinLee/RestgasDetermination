#
# Generate single K+
# P uniform 0.2-1.0 GeV   (lab)
# theta uniform 13-142deg (lab)
#
# Fergus Wilson (fwilson@slac.stanford.edu) for Gerry Lynch and SVT studies
# $Id: single_k+_0.2-1.0.tcl,v 1.1.1.1 2005/03/29 17:12:44 steinke Exp $
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "K+"
      Pmin set 0.2
      Pmax set 1.0
      usePt set false
      CosThetamin set -0.790
      CosThetamax set  0.975
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


