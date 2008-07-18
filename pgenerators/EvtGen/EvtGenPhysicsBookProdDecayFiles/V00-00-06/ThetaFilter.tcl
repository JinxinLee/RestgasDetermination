#
# modified the example from:
# 
#http://www.slac.stanford.edu/BFROOT/www/Physics/Tools/generators/GenFwkInt/GenFilters.html
#
# Selects Deltas from continuum for PentaQuark Search
module talk GefSelectFilter 

#
# Copy "GefPdtList" into "Pdt", and tell "Pdt" to require Delta+ or anti-Delta-
# (can be done in either order):
#

BooNew  Pdt  = GefPdtList 

BooObjects Pdt or Delta+ anti-Delta-

#
# Select Pdt as the filter to use before the event is transformed to the lab:
#

beforeFilter set Pdt 


exit

