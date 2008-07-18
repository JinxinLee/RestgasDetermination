#
#################################################
# This tcl file filters D+ from ccbar           #
# with the requirements:                        #
#  1) to not come from a D*+                    #
#  2) CMS-momentum p*(D0).gt.2.2                #
#                                               #
# 02/06/02 - pompili@slac.stanford.edu          #
#                                               #
#################################################
#
module talk GefSelectFilter
# Select the filter GefPdtList which is a BooObject and copy into "Pdt"
  BooNew Pdt = GefPdtList
# Ask a D+ in each event
  BooObjects Pdt or D+
# Set the CMS-momentum range:
  BooObjects Pdt p 2.2 11.0
#
# Select another filter copying "Pdt"
  BooNew PdtFromDstarplus = Pdt
# Ask to have a D*+ mother
  BooObjects PdtFromDstarplus ancestors D*+
# Create a filter with mother not a D*+
  BooNew PdtNotFromDstarplus = ! PdtFromDstarplus
# 
# Not strictly needed... anyway...
  BooCompose PdtFromCont = and Pdt PdtNotFromDstarplus
#
# Use this filter before the event is transformed to the lab:
  beforeFilter set PdtFromCont
exit
