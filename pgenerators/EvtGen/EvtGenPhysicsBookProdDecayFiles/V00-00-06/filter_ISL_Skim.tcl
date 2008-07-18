###################
##  Generator level filter for ISL Tag skim:
##  e and mu with p*>1.25 GeV,
##  -.928 < cos(thetaLab) < .976,
##  p_lab_mu > 0.95 GeV, p_lab_e > 0.45 GeV
##
##  R. Kowalewski  Feb,14th 2001
##  kowalews@uvic.ca
##
##  Should select 23.8% of BBbar generic, 5.7% of ccbar continuum
##  If only "beforeFilter" is used, should select 25% of BBbar and
##  6.2% of ccbar.
##
##################
mod talk GefSelectFilter

#### select e or mu with cuts on p* in CM and (p,cos(theta)) in lab

# lepton selectors in CM
#
  BooNew     muCMFilter = GefPdtList
  BooObjects muCMFilter or mu+ mu- 
  BooObjects muCMFilter p 1.25 5.5
  BooNew     eCMFilter = GefPdtList
  BooObjects eCMFilter or e+ e-
  BooObjects eCMFilter p 1.25 5.5
  BooCompose leptonCMFilter = or muCMFilter eCMFilter
  BooObjects leptonCMFilter infoAll
  beforeFilter set leptonCMFilter
#
# further selection in lab frame
#
  BooNew     muLabFilter = GefPdtList
  BooObjects muLabFilter or mu+ mu- 
  BooObjects muLabFilter p 0.95 10.0
  BooObjects muLabFilter cosTheta -0.928 0.976
  BooNew     eLabFilter = GefPdtList
  BooObjects eLabFilter or e+ e-
  BooObjects eLabFilter p 0.45 10.0
  BooObjects eLabFilter cosTheta -0.928 0.976
  BooCompose leptonLabFilter = or muLabFilter eLabFilter
  BooObjects leptonLabFilter infoAll
  afterFilter set leptonLabFilter
#
exit
