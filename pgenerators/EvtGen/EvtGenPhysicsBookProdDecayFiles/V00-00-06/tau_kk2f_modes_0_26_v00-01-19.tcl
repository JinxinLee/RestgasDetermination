#
# This tcl uses Kk2f for generation of generic tau+ decays and tau- decays to mu- gamma
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:0 2001:26"
exit
