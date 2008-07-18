#
###################################################
#                                                 #
# This tcl file filters D- from ccbar (no p*-cut) #
#                                                 #
# 04/20/04 - pompili@slac.stanford.edu            #
#                                                 #
###################################################
#
module talk GefSelectFilter
#
# Ask a D- in each event
  BooObjects GefPdtList or D-
#
# Use this filter before the event is transformed to the lab:
  beforeFilter set GefPdtList
#
exit


