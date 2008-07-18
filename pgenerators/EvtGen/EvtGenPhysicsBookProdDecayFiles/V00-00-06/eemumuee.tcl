#
# Decay file for generating ee->mumuee events with diag36
#
# To generate multiple files, set the seed with 
#
#	module talk RacTestInput
#	  run set <some number>
#	exit
#
# Dennis Wright   5 March 2002
#

# GfiDiag36 specifics:

#
# Disable all generators
#
disableGenerators 0

#
# Enable diag36
#
module enable GfiDiag36

#
# Require a high pt track in the angular acceptance of the EMC 
#
mod talk GfiDiag36
  process set 3
  ptCut set 0.15
  backCosThetaCut set -0.80
  forCosThetaCut set 0.96
exit



