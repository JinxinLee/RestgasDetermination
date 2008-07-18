#
###########################################
# This tcl file filters D0bar from D*-    #
# with CMS-momentum p*(D0bar).gt.2.4      #
#                                         #
# 02/06/02 - pompili@slac.stanford.edu    #
#                                         #
###########################################
#
module talk GefSelectFilter
# Select the filter GefPdtList which is a BooObject and copy into "Pdt"
  BooNew Pdt = GefPdtList
# Ask a D0bar in each event
  BooObjects Pdt or anti-D0
# Set the CMS-momentum range:
  BooObjects Pdt p 2.4 11.0
# Define the ancestor (i.e. the D*- mother)
  BooObjects Pdt ancestors D*-
# Use this filter before event is transformed to the lab:
  beforeFilter set Pdt
exit
