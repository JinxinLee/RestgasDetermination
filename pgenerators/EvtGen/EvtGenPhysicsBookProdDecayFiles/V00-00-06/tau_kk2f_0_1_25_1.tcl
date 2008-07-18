#
# This tcl uses Kk2f for generation of tau+ -> generic tau- -> e-gamma
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:0 2001:25 3003:1 3004:1"
exit

