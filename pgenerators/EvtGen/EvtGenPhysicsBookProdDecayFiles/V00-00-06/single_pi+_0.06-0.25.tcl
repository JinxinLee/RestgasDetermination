#
# Generate single pi+
# P uniform 0.06-0.25 GeV (lab)
# theta uniform 13-142deg (lab)
#
# Fergus Wilson (fwilson@slac.stanford.edu) for Gerry Lynch and SVT studies
# $Id: single_pi+_0.06-0.25.tcl,v 1.1.1.1 2005/03/29 17:12:44 steinke Exp $
#
disableGenerators 0
module enable GfiSingleParticle

module talk GfiSingleParticle
      GENERATE set "pi+"
      Pmin set 0.06
      Pmax set 0.25
      usePt set false
      CosThetamin set -0.79
      CosThetamax set 0.975
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


