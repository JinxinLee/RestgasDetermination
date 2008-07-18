#
# This tcl uses KORALB for generation of tau+ tau-
# tau+ -> generic decays , tau-> mu+ mu- mu+
#
disableGenerators 0
module enable GfiKoralb

module talk GfiKoralb
  npar set "4:0 5:26 3:1 7:1"
  xpar set "17:1 18:6 19:0"
exit
