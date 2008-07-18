#######################################################
##  Generator level filter 			     ##
##                                                   ##
##  Select background for B->Xpi0 (X = pi,K,Ks,pi0)  ##
##                                                   ##
##  Background rejection:                            ##
##        uds = 19.3                                 ##
##        tau = 21.3                                 ##
##      ccbar = 57.0                                 ##
##                                                   ##
##  May 8, 2000                                      ##
##  J. Olsen                                         ##
##  jolsen@slac.stanford.edu                         ##
##                                                   ##
#######################################################

mod talk GefSelectFilter

# The pi0 background comes from a combination of real pi0, single gamma, and
# multi-gamma combinatorics.  We must filter for all of these possibilities to
# avoid a bias in the background distribution.

# First define particles:  tracks (t+t-), photons, pi0, composite pi0, Ks.
   BooObjects GefKinematic lump t- = e- mu- pi- K- anti-p-
   BooObjects GefKinematic lump t+ = e+ mu+ pi+ K+ p+
   BooObjects GefKinematic lump t = e- e+ mu- mu+ pi- pi+ K- K+ p+ anti-p-
   BooObjects GefKinematic define t = t 1.4 3.5 -100 100 -100 100
   BooObjects GefKinematic define t- = t- -100 100 -100 100 -100 100
   BooObjects GefKinematic define t+ = t+ -100 100 -100 100 -100 100
   BooObjects GefKinematic define gammaloose = gamma -100 100 -100 100 -100 100
   BooObjects GefKinematic define Pi0 = pi0 1.1 3.5 -100 100 -100 100
   BooObjects GefKinematic define singleGam = gamma 1.5 3.5 -100 100 -100 100
   BooObjects GefKinematic combine Ks0 = t+ t- .477 .517 1.1 3.5 -100 100 -100 100
   BooObjects GefKinematic combine compPi0 = gammaloose gammaloose .085 0.185 1.1 3.5 -100 100 -100 100

# Now make all combinations of particles to create B candidates.
   BooObjects GefKinematic combine BKs0Comp = Ks0 compPi0 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine BKs0pi0 = Ks0 Pi0 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine BKs0gam = Ks0 singleGam 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine BhComp = t compPi0 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine Bhpi0 = t Pi0 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine Bhgam = t singleGam 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine BCompComp = compPi0 compPi0 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine BPi0Comp = Pi0 compPi0 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine BGamComp = singleGam compPi0 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine BPi0Pi0 = Pi0 Pi0 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine BGamGam = singleGam singleGam 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic combine BPi0Gam = Pi0 singleGam 4.5 6. 0. 1.5 -100 100 -100 100
   BooObjects GefKinematic require Bhpi0 Bhgam BhComp BKs0pi0 BKs0gam BKs0Comp BCompComp BPi0Comp BGamComp BPi0Pi0 BGamGam BPi0Gam
   BooObjects GefKinematic list
   beforeFilter set GefKinematic
exit
