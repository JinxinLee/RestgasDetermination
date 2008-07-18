#
# Filter events with a D'_s1-
#
# This file is a simple filter for just one particle, the "D'_s1-".
#
# Normally, simple filters like this are specified in production
# by just the particle name (rather than a specific tcl file).
# This file is included in ProdDecayFiles in order to work around 
# a problem with oracle in which filter instructions that
# include a "'" cannot be stored in the description of a 
# production decay mode.
#
# $Id
#
module talk GefSelectFilter
  BooObjects GefPdtList or "D'_s1-"
  beforeFilter set GefPdtList
exit
