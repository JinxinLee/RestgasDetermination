# ----------------------------------------------------------------------
# This filter is to select decay chains:
#
#  Xi_c0 -> Xi_- anything,
#        or                 Xi_-> Lambda0 anything, Lambda0 -> p+ pi-
#  Xi_c+ -> Xi_- anything,
#
#  Author: Sang-Joon Lee (March 19, 2002)
#
#----------------------------------------------------------------------

module talk GefSelectFilter 

#-- Searches either Xi_c0 -> Xi_- anything or Xi_c+ -> Xi_- anything
#-- However, I do not want to have events that have signal modes from both charm quarks.
  BooNew     FXi = GefPdtList
  BooObjects FXi   or  Xi-   anti-Xi+
  BooObjects FXi   ancestors Xi_c0 anti-Xi_c0 Xi_c+ anti-Xi_c-
  BooObjects FXi   ancestorsMode XOR

#-- Select p+ from Lambda0 and anti-p- from anti-Lambda0
  BooNew     FXicp  = GefPdtList
  BooObjects FXicp    or p+
  BooObjects FXicp    ancestors Lambda0
  BooNew     FaXicp = GefPdtList
  BooObjects FaXicp   or anti-p-
  BooObjects FaXicp   ancestors anti-Lambda0

#-- Xi_c0 -> Xi- any, Xi-> Lambda0 any, Lambda0 -> p+ pi- ---
  BooNew     FXic0  = GefPdtList
  BooObjects FXic0    or Xi-
  BooObjects FXic0    ancestors Xi_c0
  BooCompose FXic0t = and FXic0 FXicp

#-- anti-Xi_c0 -> anti-Xi+ any, anti-Xi+> anti-Lambda0 any, anti-Lambda0 -> anti-p- pi+ ---
  BooNew     FaXic0  = GefPdtList
  BooObjects FaXic0    or anti-Xi+
  BooObjects FaXic0    ancestors anti-Xi_c0
  BooCompose FaXic0t = and FaXic0 FaXicp

#-- Xi_c+ -> Xi- any, Xi-> Lambda0 any, Lambda0 -> p+ pi- ---
  BooNew     FXic+  = GefPdtList
  BooObjects FXic+    or Xi-
  BooObjects FXic+    ancestors Xi_c+
  BooCompose FXic+t = and FXic+ FXicp

#-- anti-Xi_c- -> anti-Xi+ any, anti-Xi+> anti-Lambda0 any, anti-Lambda0 -> anti-p- pi+ ---
  BooNew     FaXic-  = GefPdtList
  BooObjects FaXic-    or anti-Xi+
  BooObjects FaXic-    ancestors anti-Xi_c-
  BooCompose FaXic-t = and FaXic- FaXicp

#-- Signal decay chain (However, there may be events having 2 signals from the both charm quarks.)
  BooCompose FXicDecays = or FXic0t FaXic0t FXic+t FaXic-t

#-- Signal decay chain having only 1 signal mode.
  BooCompose FXicDecayChain = and FXi FXicDecays

#----------------------------------------------------------------------
  beforeFilter  set  FXicDecayChain
exit 
