########################################################################
##  Generator level filter for inclusive semileptonic production
##    with a very hard lepton : e or mu with p*>2.2 GeV,
##
##  L.H. Wilden, 6 November 2001
##  Based on filterInclusiveHardLepton.tcl by T.I. Meyer
##
##  Should select approx. 0.33% of BB generic
##
#########################################################################
mod talk GefSelectFilter

# select e or mu with cuts on p* in CM

  BooNew     muCMFilter = GefPdtList
  BooObjects muCMFilter or mu+ mu- 
  BooObjects muCMFilter p 2.2 5.5

  BooNew     eCMFilter = GefPdtList
  BooObjects eCMFilter or e+ e-
  BooObjects eCMFilter p 2.2 5.5

  BooCompose leptonCMFilter = or muCMFilter eCMFilter
  BooObjects leptonCMFilter infoAll

  beforeFilter set leptonCMFilter

exit

