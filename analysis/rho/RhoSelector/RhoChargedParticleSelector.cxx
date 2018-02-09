//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Selector classes for charged particle selection                      //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "TDatabasePDG.h"

#include "RhoSelector/RhoChargedParticleSelector.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"


ClassImp ( RhoChargedParticleSelector )

TBuffer& operator>> ( TBuffer& buf, RhoChargedParticleSelector *&obj )
{
  obj = ( RhoChargedParticleSelector* ) buf.ReadObject ( RhoChargedParticleSelector::Class() );
  return buf;
}

RhoChargedParticleSelector::RhoChargedParticleSelector ( const char* name, const char* type ) :
  RhoParticleSelectorBase ( name,type )
{}

Bool_t RhoChargedParticleSelector::Accept ( RhoCandidate& b )
{
  if ( &b == 0 ) { return kFALSE; }
  if ( b.GetCharge() ==0 ) { return kFALSE; }
  //SetTypeAndMass(b);

  return kTRUE;
}

Bool_t RhoChargedParticleSelector::Accept ( FairRecoCandidate& b )
{
  Bool_t decision = kFALSE;
  if ( &b == 0 ) { return kFALSE; }
  if ( b.GetCharge() !=0 ) { decision = kTRUE; }
  return decision;
}



