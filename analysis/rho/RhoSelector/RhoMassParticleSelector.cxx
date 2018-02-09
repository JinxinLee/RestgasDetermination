//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoMassParticleSelector                                              //
//                                                                      //
// Selector classes for mass selection                                  //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "TDatabasePDG.h"

#include "RhoSelector/RhoMassParticleSelector.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"



ClassImp ( RhoMassParticleSelector )

TBuffer& operator>> ( TBuffer& buf, RhoMassParticleSelector *&obj )
{
  obj = ( RhoMassParticleSelector* ) buf.ReadObject ( RhoMassParticleSelector::Class() );
  return buf;
}

RhoMassParticleSelector::RhoMassParticleSelector ( const char* name, Double_t cv,Double_t w,const char* type ) :
  RhoParticleSelectorBase ( name,type ), fCentralValue ( cv ), fWindow ( 0.5*w )
{}

Bool_t RhoMassParticleSelector::Accept ( RhoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }
  SetTypeAndMass ( b );
  return ( fabs ( b->GetMass()-fCentralValue ) <fWindow );
}

Bool_t RhoMassParticleSelector::Accept ( FairRecoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }
  return ( fabs ( b->GetLorentzVector().Mag()-fCentralValue ) <fWindow );
}

