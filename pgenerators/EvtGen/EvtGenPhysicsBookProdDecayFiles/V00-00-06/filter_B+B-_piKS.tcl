###################
##  Generator level filter 
##
##  Select background for h+KS  ( h = pi,K)
##   
##
##  G.Cavoto  May,20th 2000
##  cavoto@slac.stanford.edu
##
##################
mod talk GefSelectFilter


### select pi+ KS 
BooObjects GefKinematic lump t- = e- mu- pi- K- anti-p-
BooObjects GefKinematic define  t- =  t-  -100. 100. -100. 100. -100. 100. 0.139 
BooObjects GefKinematic lump t+ = e+ mu+ pi+ K+ p+
BooObjects GefKinematic define  t+ =  t+  -100. 100. -100. 100. -100. 100. 0.139 

BooObjects GefKinematic combine Kshort = t- t+   0.42 0.58  1.9 3.0 -100 100 -100 100

BooObjects GefKinematic lump  thepi- = e- mu- pi- K- anti-p- 
BooObjects GefKinematic define   thepi- = thepi-  1.9 3. -100 100 -100 100 

BooObjects GefKinematic lump  thepi+ = e+ mu+ pi+ K+ p+
BooObjects GefKinematic define   thepi+ = thepi+  1.9 3.0 -100 100 -100 100 

BooObjects GefKinematic combine BPKSminus  = thepi- Kshort   -100.  100. 0.0 1.  -100 100 -100 100
BooObjects GefKinematic combine BPKSplus   = thepi+ Kshort   -100.  100. 0.0 1.  -100 100 -100 100
# get KS-->pi0pi0 but this is just  poorer filter rejection factor 
BooObjects GefKinematic define   theKshort  =  K_S0 1.9 3.

BooObjects GefKinematic combine BminustheKS  = thepi-    theKshort     -100.  100. 0.0 1.  -100 100 -100 100
BooObjects GefKinematic combine BplustheKS   = thepi+    theKshort     -100.  100. 0.0 1.  -100 100 -100 100





BooObjects GefKinematic require  BPKSminus BPKSplus BminustheKS BplustheKS 
BooObjects GefKinematic list

#Tell the module to use this filter before the event is transformed
# to the lab:
   beforeFilter set GefKinematic
exit
