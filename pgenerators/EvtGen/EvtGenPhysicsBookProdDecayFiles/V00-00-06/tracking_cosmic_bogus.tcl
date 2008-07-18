#
# $Id: tracking_cosmic_bogus.tcl,v 1.1.1.1 2005/03/29 17:12:44 steinke Exp $
#
# tracking_cosmic_bogus.tcl
#
# Run the HemiCosm generator with a small target cylinder and
# use a special feature of Bogus to extrapolate the input to
# the exterior of BaBar
#
# Improvements for the future: add Bogus tcl parameter to
# control size of cylinder that the cosmic is extrapolated
# to. Right now the size is fixed at 3x3 meters.
#
# Contact David C. Williams (davidw@scipp.ucsc.edu)
#
disableGenerators 0
module enable GfiHemiCosm
module talk GfiHemiCosm
  detectorRadius set 13 
  detectorLength set 20 
exit

module talk Bogus
  primariesSource set HemiCosmKluge
exit
