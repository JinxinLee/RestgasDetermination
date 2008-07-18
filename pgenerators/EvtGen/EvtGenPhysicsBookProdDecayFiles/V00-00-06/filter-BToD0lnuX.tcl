###################
##  Generator level filter for B->D0(K-pi+)l-nuX
##  e and mu with p*>1.0 GeV,
##  a D0-l combination of the correct sign is also required.
##
##  R. Kowalewski  Apr 26, 2001
##  kowalews@uvic.ca
##
##################
mod talk GefSelectFilter

# lepton and d0l selectors in CM
#
  BooNew     d0epFilter = GefPdtList
  BooObjects d0epFilter and anti-D0 e+
  BooNew     d0emFilter = GefPdtList
  BooObjects d0emFilter and D0 e-
  BooNew     d0mpFilter = GefPdtList
  BooObjects d0mpFilter and anti-D0 mu+
  BooNew     d0mmFilter = GefPdtList
  BooObjects d0mmFilter and D0 mu-
  BooCompose d0eFilter = or d0emFilter d0epFilter
  BooCompose d0mFilter = or d0mmFilter d0mpFilter
  BooCompose d0lFilter = or d0eFilter d0mFilter
  BooNew     muCMFilter = GefPdtList
  BooObjects muCMFilter or mu+ mu- 
  BooObjects muCMFilter p 1.0 5.5
  BooNew     eCMFilter = GefPdtList
  BooObjects eCMFilter or e+ e-
  BooObjects eCMFilter p 1.0 5.5
  BooCompose leptonCMFilter = or muCMFilter eCMFilter
  BooCompose CMFilter = and leptonCMFilter d0lFilter
  BooObjects CMFilter infoAll
  beforeFilter set CMFilter
#
exit
