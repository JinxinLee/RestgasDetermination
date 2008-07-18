#
# This tcl uses Kk2f for generation of tau+ -> generic tau- -> 3pi-2pi+2pi0
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:0 2001:23 3003:1 3004:2"
exit

