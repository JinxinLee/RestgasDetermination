#
# This tcl uses Kk2f for generation of tau decays to 7 charged pions + pi0
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:24 2001:24"
exit
