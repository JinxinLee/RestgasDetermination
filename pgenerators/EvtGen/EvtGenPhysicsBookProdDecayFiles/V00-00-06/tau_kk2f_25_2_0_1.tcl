#
# This tcl uses Kk2f for generation of tau+ -> mu+gamma tau- -> generic
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:25 2001:0 3003:2 3004:1"
exit

