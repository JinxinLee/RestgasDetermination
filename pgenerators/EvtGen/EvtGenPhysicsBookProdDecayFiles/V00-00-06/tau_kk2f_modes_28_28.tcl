#
# This tcl uses Kk2f for generation of cp violating tau decays to K pi
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:28 2001:28"
exit
