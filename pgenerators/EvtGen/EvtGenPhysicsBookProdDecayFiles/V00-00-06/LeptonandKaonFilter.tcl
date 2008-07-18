module talk GefSelectFilter


  BooObjects GefKinematic lump t = e+ e- mu+ mu-
  BooObjects GefKinematic lump q =D*+ D*-
  BooObjects GefKinematic lump y =pi+ pi-
  BooObjects GefKinematic lump d = K+ K-
  BooObjects GefKinematic define t = t 1
  BooObjects GefKinematic define d = d 0.1
  BooObjects GefKinematic define q = t 1
  BooObjects GefKinematic define y = d 0.
  BooObjects GefKinematic combine dq = q d 0.001 100 0 100 0 100 -1 1 -1. 0.  
  BooObjects GefKinematic combine qy = q y 0.001 100 0 100 0 100 -1 1 0.9 1.  
  BooObjects GefKinematic combine qt = q t 0.001 100 0 100 0 100 -1 1 0.6 1.  
  BooObjects GefKinematic require qy
  BooObjects GefKinematic require dq
  BooObjects GefKinematic require qt
  beforeFilter set GefKinematic
exit


