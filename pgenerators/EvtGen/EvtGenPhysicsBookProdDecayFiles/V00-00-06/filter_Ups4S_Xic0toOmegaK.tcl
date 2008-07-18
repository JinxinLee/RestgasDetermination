########################################################################
#                                                                      #
# PURPOSE:  To set up a filter that  selects the decay chains:         #
#  Xi_c0 -> Omega- ,Omega- ->Lambda0, Lambda0 -> p                     #
#                                                                      #      
# This filter is made to run using the Ups4S_Xic0toOmegaK.dec that     #
# produces only Omega- K+ decays of Xi_c0, Lambda0 K- decays of        #
# Omega-, and p pi- decays of Lambda0, and CC modes.  This file can    #
# then be run on Upsilon(4S) events, getting events with Xi_c0 from B  #
# decays.                                                              #  
#                                                                      #
# created: V. Ziegler Thu Jun 24 2004                                  #
#          vziegler@slac.stanford.edu                                  #
#                                                                      #
########################################################################

module talk GefSelectFilter 
  
#-- Select Omega- from Xi_c0 and anti-Omega+ from anti-Xi_c0
  BooNew     FXic0          = GefPdtList
  BooObjects FXic0            or Omega-
  BooObjects FXic0            ancestors Xi_c0
  BooNew     FXic0Bar       = GefPdtList
  BooObjects FXic0Bar         or anti-Omega+
  BooObjects FXic0Bar         ancestors anti-Xi_c0

#-- Select Lambda0 from Omega- and anti-Lambda0 from anti-Omega+
  BooNew     FOmegaMinus   = GefPdtList
  BooObjects FOmegaMinus     or Lambda0
  BooObjects FOmegaMinus     ancestors Omega-
  BooNew     FOmegaPlus    = GefPdtList
  BooObjects FOmegaPlus      or anti-Lambda0
  BooObjects FOmegaPlus      ancestors anti-Omega+

#-- Select p+ from Lambda0 and anti-p- from anti-Lambda0
  BooNew     FLambda      = GefPdtList
  BooObjects FLambda        or p+
  BooObjects FLambda        ancestors Lambda0
  BooNew     FLambdaBar   = GefPdtList
  BooObjects FLambdaBar     or anti-p-
  BooObjects FLambdaBar     ancestors anti-Lambda0

#-- Xi_c0 -> Omega- ,Omega- ->Lambda0, Lambda0 -> p
  BooCompose FMode = and FXic0 FOmegaMinus FLambda

#-- anti-Xi_c0 -> anti-Omega+ ,anti-Omega+ ->anti-Lambda0, anti-Lambda0 -> anti-p- 
  BooCompose FAntiMode = and FXic0Bar FOmegaPlus FLambdaBar

  BooCompose FXicDecays = or FMode FAntiMode 
#----------------------------------------------------------------------
  beforeFilter  set  FXicDecays
exit
