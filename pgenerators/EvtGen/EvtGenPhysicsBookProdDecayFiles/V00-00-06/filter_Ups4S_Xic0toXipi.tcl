########################################################################
#                                                                      #
# PURPOSE:  To set up a filter that  selects the decay chains:         #
#  Xi_c0 -> Xi-, Xi- ->Lambda0, Lambda0 -> p                           #
#                                                                      #      
# This filter is made to run using the Ups4S_Xic0toXipi.dec that       #
# produces only Xi- pi+ decays of Xi_c0, Lambda0 pi- decays of         #
# Xi-, and p pi- decays of Lambda0, and CC modes.  This file can       #
# then be run on Upsilon(4S) events, getting events with Xi_c0 from B  #
# decays.                                                              #  
#                                                                      #
# created: V. Ziegler Thu Jun 24 2004                                  #
#          vziegler@slac.stanford.edu                                  #
#                                                                      #
########################################################################

module talk GefSelectFilter 
  
#-- Select Xi- from Xi_c0 and anti-Xi+ from anti-Xi_c0
  BooNew     FXic0     = GefPdtList
  BooObjects FXic0       or Xi-
  BooObjects FXic0       ancestors Xi_c0
  BooNew     FXic0bar  = GefPdtList
  BooObjects FXic0bar    or anti-Xi+
  BooObjects FXic0bar    ancestors anti-Xi_c0

#-- Select Lambda0 from Xi- and anti-Lambda0 from anti-Xi+
  BooNew     FXiminus  = GefPdtList
  BooObjects FXiminus    or Lambda0
  BooObjects FXiminus    ancestors Xi-
  BooNew     FXiplus   = GefPdtList
  BooObjects FXiplus     or anti-Lambda0
  BooObjects FXiplus     ancestors anti-Xi+

#-- Select p+ from Lambda0 and anti-p- from anti-Lambda0
  BooNew     FLambda    = GefPdtList
  BooObjects FLambda      or p+
  BooObjects FLambda      ancestors Lambda0
  BooNew     FLambdaBar = GefPdtList
  BooObjects FLambdaBar   or anti-p-
  BooObjects FLambdaBar   ancestors anti-Lambda0

#-- Xi_c0 -> Xi- ,Xi- ->Lambda0, Lambda0 -> p 
  BooCompose FMode = and FXic0 FXiminus FLambda

#-- anti-Xi_c0 -> anti-Xi+ ,anti-Xi+ ->anti-Lambda0, anti-Lambda0 -> anti-p- pi+
  BooCompose FAntiMode = and FXic0bar FXiplus FLambdaBar

  BooCompose FXicDecays = or FMode FAntiMode
#----------------------------------------------------------------------
  beforeFilter  set  FXicDecays
exit
