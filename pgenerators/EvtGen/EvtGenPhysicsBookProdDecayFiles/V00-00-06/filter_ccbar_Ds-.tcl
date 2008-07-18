###################################################
#                                                 #
# ccbar continuum production                      #
# Make a lot of Ds-> K0s K0s pi+-  decays         #
#                               K0s -> pi+ pi-    #
#                                                 #
# created: H. Schmuecker Tue Dec 16 2000          #
#          helmut@slac.stanford.edu               #
###################################################


# module talk GfiEvtGen
#   UDECAY set "ccbar_Ds_KsKspi+-_PHSP.dec"
#   GENERATE set "continuum"
# exit

mod talk GefSelectFilter

  BooNew Pdt = GefPdtList
  BooObjects Pdt or D_s-
  BooObjects Pdt p 1.5 11.0
#Tell the module to use this filter before the event is transformed
# to the lab:
   beforeFilter set Pdt
exit


