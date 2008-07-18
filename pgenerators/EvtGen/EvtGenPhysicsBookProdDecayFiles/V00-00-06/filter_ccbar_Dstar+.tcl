#
# ccbar continium production, select events with D*+
#
#       Dirk Hufnagel
#               Dec 04, 2003
#
# Mail: hufnagel@slac.stanford.edu
#

disableGenerators 0
module enable GfiEvtGen
module talk GfiEvtGen
  maxNumFilterTrials set 100000
  GENERATE set "continuum"
  exit
module enable GefSelectFilter
module talk GefSelectFilter
  BooObjects GefPdtList and D*+
  beforeFilter set GefPdtList
  exit
