#
# Generate single e- with P uniform in the range
# from 50MeV to 2.5GeV in the labframe with BFieldModel
# magnetic field.
#
#
# Contact Gerhard Raven (Gerhard.Raven@slac.stanford.edu)
#
disableGenerators 0
module enable GfiSingleParticle


module talk BbsModule
#Use BField as set by TrkBuildEnv
  dbinCmd -i run fld_flg 4
exit

module talk TrkBuildEnv
  fieldToUse set BFieldModel
exit

module talk GfiSingleParticle
      GENERATE set "e-"
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


