# Filter to be used with ccbar_Dsstar+_Dspi0.dec

disableGenerators 0
module enable GfiEvtGen
module talk GfiEvtGen
  maxNumFilterTrials set 100000
  GENERATE set "continuum"
  exit
module enable GefSelectFilter
module talk GefSelectFilter
  BooObjects GefPdtList and D_s*+
  beforeFilter set GefPdtList
  exit
