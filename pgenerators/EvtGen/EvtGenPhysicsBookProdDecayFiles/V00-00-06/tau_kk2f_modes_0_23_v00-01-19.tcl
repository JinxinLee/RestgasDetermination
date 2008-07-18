#
# This tcl uses Kk2f for generation of generic tau+ decays and decay of tau- to  4PI- 3PI+
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:0 2001:23"
exit
