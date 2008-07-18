#
# This tcl uses Kk2f for generation of generic tau+ decays and decay of tau- to  4PI- 3PI+
#
disableGenerators 0
module enable GfiKk2f

module talk GfiKk2f
generate set "tau+ tau-"
show
exit

mod talk GefSelectFilter
BooObjects GefIsr sqrtseff_multigam 0 7.5 0.87
beforeFilter set GefIsr
show
exit
