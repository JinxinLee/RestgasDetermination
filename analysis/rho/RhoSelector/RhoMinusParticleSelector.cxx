//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoMinusParticleSelector                                             //
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

#include "RhoSelector/RhoMinusParticleSelector.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"




ClassImp ( RhoMinusParticleSelector )

TBuffer& operator>> ( TBuffer& buf, RhoMinusParticleSelector *&obj )
{
  obj = ( RhoMinusParticleSelector* ) buf.ReadObject ( RhoMinusParticleSelector::Class() );
  return buf;
}

RhoMinusParticleSelector::RhoMinusParticleSelector ( const char* name, const char* type ) :
  RhoParticleSelectorBase ( name,type )
{}

Bool_t RhoMinusParticleSelector::Accept ( RhoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }
  if ( b->GetCharge() >0 ) { return kFALSE; }
  SetTypeAndMass ( b );

  return kTRUE;
}

Bool_t RhoMinusParticleSelector::Accept ( FairRecoCandidate* b )
{
  Bool_t decision = kFALSE;
  if ( b == 0 ) { return kFALSE; }
  if ( b->GetCharge() ==-1 ) { decision = kTRUE; }
  return decision;
}

