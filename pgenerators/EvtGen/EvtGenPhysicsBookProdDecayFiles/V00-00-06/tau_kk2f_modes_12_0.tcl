#
# This tcl uses Kk2f for generation of tau+ -> 3pi+2pi-pi0 tau- -> generic
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:12 2001:0"
exit
