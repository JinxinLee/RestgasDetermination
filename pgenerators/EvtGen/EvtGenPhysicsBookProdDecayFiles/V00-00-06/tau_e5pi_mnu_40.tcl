#
# This tcl uses Koralb for generation of tau pair to e- 5pi 
# with the tau neutrino mass set to 40 MeV
#
disableGenerators 0
module enable GfiKoralb

module talk GfiKoralb
  npar set "4:11  5:1"
  xpar set "7:0.04"
exit

