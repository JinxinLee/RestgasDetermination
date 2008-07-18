# 
# Set up EvtGen to produce chic 1 or 2 
#------------------------------------------------------------------#
# GfiEvtGen specifics:
sourceFoundFile GeneratorsQA/common.tcl
disableGenerators 0
module enable GfiEvtGen
module talk GfiEvtGen
  GENERATE set "Upsilon(4S)"
  UDECAY set   "B0B0bar_Inclusivetap.dec"
  maxNumFilterTrials set 100000
show
exit

module enable GefSelectFilter 
module talk GefSelectFilter 
# 
# Copy "GefPdtList" into "Pdt", and tell "Pdt" to require Ds+ and Ds- 
# (can be done in either order): 
# 
  BooNew  Pdt  = GefPdtList 
  BooObjects  Pdt  or  eta'

# 
# Select myFilter as the filter to use before the event is transformed to the lab: 
# 
  beforeFilter  set Pdt

exit 


