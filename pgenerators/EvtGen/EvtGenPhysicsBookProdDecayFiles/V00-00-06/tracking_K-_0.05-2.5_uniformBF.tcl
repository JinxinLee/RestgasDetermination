#
# Generate single K- with P uniform in the range
# from 50MeV to 2.5GeV in the labframe with uniform 
# B-field.
#
# Contact Anders Ryd (ryd@slac.stanford.edu)
#
disableGenerators 0
module enable GfiSingleParticle

module talk BbsModule
#make the field uniform in tracking volume
  dbinCmd -i run fld_flg 2
exit

module talk GfiSingleParticle
      GENERATE set "K-"
      Pmin set 0.05
      Pmax set 2.5
      usePt set false
      CosThetamin set -1.0
      CosThetamax set 1.0
      ipX set 0.0
      ipY set 0.37
      ipZ set 0.0
      ipT set 0.0
exit


