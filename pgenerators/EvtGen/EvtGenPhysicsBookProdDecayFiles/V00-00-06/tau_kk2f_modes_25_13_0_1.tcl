#
# This tcl uses Kk2f for generation of tau+ tau-
# tau- -> generic decays , tau+  -> e+ rho
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:25 2001:0 3003:13 3004:1 3005:0"
exit
