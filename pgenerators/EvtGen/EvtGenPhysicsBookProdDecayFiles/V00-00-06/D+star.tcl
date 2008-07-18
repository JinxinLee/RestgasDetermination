# 
# Filter requiring a D*+ in a continuum event
#
# Best to use with a UDECAY file which forces the VPHO to ccbar
# 
# Author: Joe Izen, University of Texas at Dallas
# 
#------------------------------------------------------------------#
# GfiEvtGen specifics:

disableGenerators 0
module enable GfiEvtGen
module talk GfiEvtGen
  maxNumFilterTrials set 100000
  GENERATE set "continuum"
exit

module enable GefSelectFilter 
module talk GefSelectFilter 

# 
# Copy "GefPdtList" into "Pdt", and tell "Pdt" to require D*+
# (can be done in either order): 
# 
#  BooNew  Pdt  = GefPdtList 
  BooObjects  GefPdtList  and  D*+

# 
# Select GefPdtList as the filter to use before the event is transformed to the lab: 
# 
  beforeFilter  set  GefPdtList
exit 
