#
# This tcl uses KORALB for generation of tau+ tau-
# tau+ -> generic decays , tau- -> mu-gamma
#
disableGenerators 0
module enable GfiKoralb

module talk GfiKoralb
  npar set "4:0 5:25"
  xpar set "17:1 18:2"
exit
