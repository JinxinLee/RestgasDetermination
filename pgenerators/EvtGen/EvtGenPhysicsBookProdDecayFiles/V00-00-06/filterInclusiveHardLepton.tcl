########################################################################
##  Generator level filter for inclusive semileptonic production
##    with a hard lepton : e or mu with p*>1.0 GeV,
##
##  T.I. Meyer, 30 April 2001
##  Based on filterISL+Dilep.tcl by R. Kowalewski
##
##  Should select 31% of B0B0bar generic, 8.2% of ccbar continuum at p*>1.0
##
#########################################################################
mod talk GefSelectFilter

# select e or mu with cuts on p* in CM

  BooNew     muCMFilter = GefPdtList
  BooObjects muCMFilter or mu+ mu- 
  BooObjects muCMFilter p 1.0 5.5

  BooNew     eCMFilter = GefPdtList
  BooObjects eCMFilter or e+ e-
  BooObjects eCMFilter p 1.0 5.5

  BooCompose leptonCMFilter = or muCMFilter eCMFilter
  BooObjects leptonCMFilter infoAll

  beforeFilter set leptonCMFilter

exit

