#
# This tcl uses Kk2f for generation of tau+ tau-
# tau+ -> generic decays , tau-  -> mu- pi+ K-
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:0 2001:26 3003:1 3004:12 3005:0"
exit
