#
# This tcl uses Kk2f for generation of generic tau+ decays and tau- decays to 4PI- 3PI+ PI0
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
xpar set "2000:24 2001:0"
exit
