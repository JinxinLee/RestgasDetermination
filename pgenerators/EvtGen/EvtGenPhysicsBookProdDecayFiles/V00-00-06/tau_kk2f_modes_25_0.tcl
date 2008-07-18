#
# This tcl uses Kk2f for generation of generic tau+ decays and tau- decays to e- gamma
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:25 2001:0"
exit
