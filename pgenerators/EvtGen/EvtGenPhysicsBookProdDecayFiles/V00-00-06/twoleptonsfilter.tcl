module talk GefSelectFilter
 BooObjects GefKinematic lump t = e+ e- mu+ mu-

  # define t for use in the filter, with a 1 GeV cut:
  BooObjects GefKinematic define t = t 1

  # combine to such leptons, requiring the total mass to be more 
  # than the muon mass, to ensure the filter doesn't pick up the same
  # lepton twice:
  BooObjects GefKinematic combine tt = t t 0.106

  # require a combination like that in each event:
  BooObjects GefKinematic require tt

  # make their requirements before boosting to the lab frame:
  beforeFilter set GefKinematic
exit


