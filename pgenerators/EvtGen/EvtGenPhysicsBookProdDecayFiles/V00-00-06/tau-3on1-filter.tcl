#
# This tcl uses Koralb for generation of generic tau decays, applying
# a filter to select events with 4 tracks.
#
disableGenerators 0
module enable GfiKoralb

module talk GefSelectFilter  
  BooObjects GefKinematic lump t+ = e+ mu+ pi+ K+
  BooObjects GefKinematic lump t- = e- mu- pi- K-
  BooObjects GefKinematic define t+ = t+ -100 100 -100 100 -100 100
  BooObjects GefKinematic define t- = t- -100 100 -100 100 -100 100
  BooObjects GefKinematic combine t+- = t+ t-  -100 100 -100 100 -100 100 -100 100
  BooObjects GefKinematic combine 3on1 = t+- t+-  -100 100 -100 100 -100 100 -100 100
  BooObjects GefKinematic require 3on1

  beforeFilter set GefKinematic
exit

