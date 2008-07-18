###################
##  Generator level filter 
##
##  Select background for h+h-  ( h = pi,K)
##   
##
##  G.Cavoto  Mar,13th 2000
##  cavoto@slac.stanford.edu
##
##################
mod talk GefSelectFilter

#### select h+h- 

BooObjects GefKinematic lump tPPPM- = e- mu- pi- K- anti-p-
BooObjects GefKinematic define  tPPPM- =  tPPPM-    0.  6.   -100.  100. -100. 100.
BooObjects GefKinematic lump tPPPM+ = e+ mu+ pi+ K+      p+
BooObjects GefKinematic define  tPPPM+ =  tPPPM+    0. 6.    -100.  100. -100. 100.
BooObjects GefKinematic combine BPPPM   =  tPPPM+ tPPPM-  4.2   6.0  0.0 1.5 -100 100 -100 100
BooObjects GefKinematic require BPPPM 
BooObjects GefKinematic list

#Tell the module to use this filter before the event is transformed
# to the lab:
   beforeFilter set GefKinematic
exit
