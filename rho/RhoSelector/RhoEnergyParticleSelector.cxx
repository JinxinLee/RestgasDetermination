//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Selector classes for energy selection                                //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "TDatabasePDG.h"

#include "RhoSelector/RhoEnergyParticleSelector.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"




ClassImp ( RhoEnergyParticleSelector )

TBuffer& operator>> ( TBuffer& buf, RhoEnergyParticleSelector *&obj )
{
  obj = ( RhoEnergyParticleSelector* ) buf.ReadObject ( RhoEnergyParticleSelector::Class() );
  return buf;
}

RhoEnergyParticleSelector::RhoEnergyParticleSelector ( const char* name, Double_t cv,Double_t w,const char* type ) :
  RhoParticleSelectorBase ( name,type ), fCentralValue ( cv ), fWindow ( 0.5*w )
{}

Bool_t RhoEnergyParticleSelector::Accept ( RhoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }
  SetTypeAndMass ( b );
  return ( fabs ( b->E()-fCentralValue ) <fWindow );
}

Bool_t RhoEnergyParticleSelector::Accept ( FairRecoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }
  return ( fabs ( b->GetEnergy()-fCentralValue ) <fWindow );
}

