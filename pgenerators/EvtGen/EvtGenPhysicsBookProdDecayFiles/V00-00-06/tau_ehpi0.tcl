#
# This tcl uses Koralb for generation of generic tau decays
#
disableGenerators 0
module enable GfiKoralb
module enable GefSelectFilter
 
module talk GefSelectFilter 
  BooNew Pdt = GefPdtList
  BooObjects Pdt or Pi0 
  beforeFilter set Pdt
exit

module talk GfiKoralb
  npar set "4:1 5:4"
exit
