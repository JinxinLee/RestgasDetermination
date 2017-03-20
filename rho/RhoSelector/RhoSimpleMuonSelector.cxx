//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoSimpleMuonSelector                                                //
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

#include "RhoSelector/RhoSimpleMuonSelector.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"


ClassImp ( RhoSimpleMuonSelector )

TBuffer& operator>> ( TBuffer& buf, RhoSimpleMuonSelector *&obj )
{
  obj = ( RhoSimpleMuonSelector* ) buf.ReadObject ( RhoSimpleMuonSelector::Class() );
  return buf;
}

RhoSimpleMuonSelector::RhoSimpleMuonSelector ( const char* name, const char* type ) :
  RhoParticleSelectorBase ( name,type )
{}

Bool_t RhoSimpleMuonSelector::Accept ( RhoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }

  SetTypeAndMass ( b );

  double Le  = b->GetPidInfo ( 0 );
  double Lmu = b->GetPidInfo ( 1 );
  double Lpi = b->GetPidInfo ( 2 );
  double Lk  = b->GetPidInfo ( 3 );
  double Lp  = b->GetPidInfo ( 4 );

  if ( fCriterion == loose ) {
    if ( Lmu<0.2 ) { return kFALSE; }
  } else if ( fCriterion == veryLoose || fCriterion == variable  || fCriterion == all ) {
    return kTRUE;
  } else if ( fCriterion == best ) {
    if ( Lmu<Le || Lmu<Lpi || Lmu<Lk || Lmu<Lp ) { return kFALSE; }
  } else if ( fCriterion == tight ) {
    if ( Lmu<0.5 ) { return kFALSE; }
  } else {
    if ( Lmu<0.9 ) { return kFALSE; }
  }

  return kTRUE;
}

Bool_t RhoSimpleMuonSelector::Accept ( FairRecoCandidate*  )
{
  return kFALSE;
}
