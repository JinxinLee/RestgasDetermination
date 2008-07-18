#
# modified the example from:
# 
#http://www.slac.stanford.edu/BFROOT/www/Physics/Tools/generators/GenFwkInt/GenFilters.html
#
# Selects Xic0 from continuum for PentaQuark Search
module talk GefSelectFilter 

#
# Copy "GefPdtList" into "Pdt", and tell "Pdt" to require Xi_c0 anti-Xi_c0
# (can be done in either order):
#

BooNew  Pdt  = GefPdtList 

BooObjects Pdt or Xi_c0 anti-Xi_c0

#
# Select Pdt as the filter to use before the event is transformed to the lab:
#

beforeFilter set Pdt 
exit


module talk RandomControl 
    # Change the # of seeds allowed per event. Default is 3M:
    maxRandomsPerEvent  set  300000000 

    # Change the # of events allowed per run. Default is 2K:
#    maxEventsPerRun  set  1000
 exit  


mod talk GfiEvtGen
  maxNumFilterTrials set 100000000
exit



