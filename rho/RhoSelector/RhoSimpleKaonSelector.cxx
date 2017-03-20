//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoSimpleKaonSelector                                                //
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

#include "RhoSelector/RhoSimpleKaonSelector.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"



ClassImp ( RhoSimpleKaonSelector )

TBuffer& operator>> ( TBuffer& buf,RhoSimpleKaonSelector  *&obj )
{
  obj = ( RhoSimpleKaonSelector* ) buf.ReadObject ( RhoSimpleKaonSelector::Class() );
  return buf;
}

RhoSimpleKaonSelector::RhoSimpleKaonSelector ( const char* name, const char* type ) :
  RhoParticleSelectorBase ( name,type )
{}

Bool_t RhoSimpleKaonSelector::Accept ( RhoCandidate* b )
{
  if ( b == 0 ) { return kFALSE; }

  SetTypeAndMass ( b );

  double Le  = b->GetPidInfo ( 0 );
  double Lmu = b->GetPidInfo ( 1 );
  double Lpi = b->GetPidInfo ( 2 );
  double Lk  = b->GetPidInfo ( 3 );
  double Lp  = b->GetPidInfo ( 4 );

  if ( fCriterion == loose ) {
    if ( Lk<0.2 ) { return kFALSE; }
  } else if ( fCriterion == veryLoose || fCriterion == variable  || fCriterion == all ) {
    return kTRUE;
  } else if ( fCriterion == best ) {
    if ( Lk<Le || Lk<Lmu || Lk<Lpi || Lk<Lp ) { return kFALSE; }
  } else if ( fCriterion == tight ) {
    if ( Lk<0.5 ) { return kFALSE; }
  } else {
    if ( Lk<0.9 ) { return kFALSE; }
  }

  return kTRUE;
}

Bool_t RhoSimpleKaonSelector::Accept ( FairRecoCandidate*  )
{
  return kFALSE;
}

