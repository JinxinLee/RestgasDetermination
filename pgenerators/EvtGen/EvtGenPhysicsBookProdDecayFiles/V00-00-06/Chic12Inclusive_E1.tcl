# FILE: EvtGen.tcl
# PURPOSE: run GeneratorsQA using the EvtGen generator
# $Id: Chic12Inclusive_E1.tcl,v 1.1.1.1 2005/03/29 17:12:20 steinke Exp $
#  Choose inclusive chc1 or 2 
# Y.karyotakis 18 Feb 20000

#------------------------------------------------------------------#

# common stuff:

sourceFoundFile GeneratorsQA/common.tcl

#------------------------------------------------------------------#
# GfiEvtGen specifics:

disableGenerators 0
module enable GfiEvtGen

module talk GfiEvtGen
  GENERATE set "Upsilon(4S)"
  UDECAY set "Chic12Inclusive_E1.dec"
  maxNumFilterTrials set 100000
exit

module enable GefSelectFilter 
module talk GefSelectFilter 
# 
# Copy "GefPdtList" into "Pdt", and tell "Pdt" to require Ds+ and Ds- 
# (can be done in either order): 
# 
  BooNew  Pdt  = GefPdtList 
  BooObjects  Pdt  or  chi_c1 chi_c2

# 
# Select myFilter as the filter to use before the event is transformed to the lab: 
# 
  beforeFilter  set Pdt

exit 
  

