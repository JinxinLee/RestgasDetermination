#
# Decay file for generating eeee events with diag36
#
# To generate multiple files, set the seed with 
#
#	module talk RacTestInput
#	  run set <some number>
#	exit
#
# Jon Fullwood     Mon Nov 29 14:55:52 PST 1999
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
  process set 5
  ptCut set 0.35
  backCosThetaCut set -0.80
  forCosThetaCut set 0.96
  maxFtWeight set 2.0
exit



