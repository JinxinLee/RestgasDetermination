//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoPlusParticleSelector                                              //
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

#include "RhoSelector/RhoPlusParticleSelector.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"



ClassImp ( RhoPlusParticleSelector )

TBuffer& operator>> ( TBuffer& buf, RhoPlusParticleSelector *&obj )
{
  obj = ( RhoPlusParticleSelector* ) buf.ReadObject ( RhoPlusParticleSelector::Class() );
  return buf;
}

RhoPlusParticleSelector::RhoPlusParticleSelector ( const char* name, const char* type ) :
  RhoParticleSelectorBase ( name,type )
{}

Bool_t RhoPlusParticleSelector::Accept ( RhoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }
  if ( b->GetCharge() <0 ) { return kFALSE; }
  SetTypeAndMass ( b );

  return kTRUE;
}

Bool_t RhoPlusParticleSelector::Accept ( FairRecoCandidate* b )
{
  Bool_t decision = kFALSE;
  if ( b == 0 ) { return kFALSE; }
  if ( b->GetCharge() ==1 ) { decision = kTRUE; }
  return decision;
}


