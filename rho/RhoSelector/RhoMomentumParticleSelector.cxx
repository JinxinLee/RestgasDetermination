//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoMomentumParticleSelector                                          //
//                                                                      //
// Selector classes for particle selection                              //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "TDatabasePDG.h"

#include "RhoSelector/RhoMomentumParticleSelector.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"



ClassImp ( RhoMomentumParticleSelector )

TBuffer& operator>> ( TBuffer& buf, RhoMomentumParticleSelector *&obj )
{
  obj = ( RhoMomentumParticleSelector* ) buf.ReadObject ( RhoMomentumParticleSelector::Class() );
  return buf;
}

RhoMomentumParticleSelector::RhoMomentumParticleSelector ( const char* name, Double_t cv,Double_t w,const char* type ) :
  RhoParticleSelectorBase ( name,type ), fCentralValue ( cv ), fWindow ( 0.5*w )
{}

Bool_t RhoMomentumParticleSelector::Accept ( RhoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }
  SetTypeAndMass ( b );
  return ( fabs ( b->P()-fCentralValue ) <fWindow );
}

Bool_t RhoMomentumParticleSelector::Accept ( FairRecoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }
  return ( fabs ( b->GetMomentum().Mag()-fCentralValue ) <fWindow );
}



