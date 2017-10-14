#include "KFParticleDatabase.h"

KFParticleDatabase* KFParticleDatabase::fDatabase = 0;

KFParticleDatabase::KFParticleDatabase()
{
  fMass[11]    = 0.000510999;
  fMass[-11]   = 0.000510999;
  fMass[211]   = 0.13957;
  fMass[-211]  = 0.13957;
  fMass[2212]  = 0.9382723;
  fMass[-2212] = 0.9382723;
  fMass[321]   = 0.493667;
  fMass[-321]  = 0.493667;
  fMass[13]    = 0.105658;
  fMass[-13]    = 0.105658;

  fDatabase = this;
}

KFParticleDatabase kfPartDatabase; //create instance
