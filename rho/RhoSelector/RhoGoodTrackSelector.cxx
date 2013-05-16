//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoGoodTrackSelector                                                 //
//                                                                      //
// Selector classes for track quality selections                        //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoSelector/RhoGoodTrackSelector.h"

#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"

ClassImp( RhoGoodTrackSelector )

TBuffer& operator>> ( TBuffer& buf, RhoGoodTrackSelector *&obj )
{
  obj = ( RhoGoodTrackSelector* ) buf.ReadObject ( RhoGoodTrackSelector::Class() );
  return buf;
}

#include <iostream>
using namespace std;

RhoGoodTrackSelector::RhoGoodTrackSelector ( const char* name) :
  RhoParticleSelectorBase ( name,"pi-" ),
  fCutFit(kFALSE),
  fCutVtx(kFALSE),
  fCutMom(kFALSE),
  fCutPt(kFALSE)

{
}

RhoGoodTrackSelector::~RhoGoodTrackSelector()
{
}

Bool_t RhoGoodTrackSelector::Accept ( RhoCandidate& b )
{
  // The GoodTrackSelector does a selection based on the
  // quality of a reconstructed charged track

  if ( &b == 0 ) { return kFALSE; }
  if ( !Accept ( *(b.GetRecoCandidate()) ) ) { return kFALSE; }
  //SetTypeAndMass ( b );

  return kTRUE;
}

Bool_t RhoGoodTrackSelector::Accept ( FairRecoCandidate& cand )
{
  // The GoodTrackSelector does a selection based on the
  // quality of a reconstructed charged track
  if ( &cand == 0 ) { return kFALSE; }
  if ( cand.GetCharge() == 0 ) { return kFALSE; }
//  // Cut on number of hits and distance from inner to outer layer
//   if (fCutHits) {
//     Int_t nhits = cand.GetNHits();     // Get the number of hits
//     if ( nhits <= nHitsMin ) return kFALSE;
//     if ( nhits >  nHitsMax ) return kFALSE;
//   }
  // Cut on momenta
  TVector3 p = cand.GetMomentum();
  if (fCutMom){
    if ( p.Mag()  <  fPMin ) { return kFALSE; }
    if ( p.Mag()  >  fPMax ) { return kFALSE; }
  }

  if (fCutPt){
    if ( p.Perp() <  fPtMin ) { return kFALSE; }
    if ( p.Perp() >  fPtMax ) { return kFALSE; }
  }


  // Cut on quality of fit
  if (fCutFit){
    Float_t chiq = cand.GetChiSquared();
    if ( chiq  > fChisqMax ) { return kFALSE; }
    Float_t ndf = cand.GetDegreesOfFreedom();
    if ( TMath::Prob(chiq,ndf) < fChisqProbMin ) { return kFALSE; }
  }

  // Cut on primary vertex 
  if (fCutVtx) {
    TVector3 v = cand.GetPosition();  // Get the starting point (spot corrected)
    if ( v.Perp() > fDocaMax ) { return kFALSE; }
    if ( v.Z()  > fZMax ) { return kFALSE; }
    if ( v.Z()  < fZMin ) { return kFALSE; }
  }

  return kTRUE;
}

void RhoGoodTrackSelector::PrintOn ( std::ostream& o ) const
{
  o << GetName() << endl;
//  if ( fNHitsMin > 0 ) { o << "Number of Hits       = " << fNHitsMin << " ... " << fNHitsMax << endl; }
  if ( fPMin > 0 ) { o << "Momentum range       = " << fPMin << " ... " << fPMax << " GeV" << endl; }
  if ( fPtMin > 0 ) { o << "Momentum range (Pt)  = " << fPtMin << " ... " << fPtMax << " GeV" << endl; }
  if ( fChisqProbMin > 0 ) { o << "Fit probability/chi2 = " << fChisqProbMin << '/' << fChisqMax << endl; }
  if ( fZMin>-1.E8 || fZMax < 1.E8 ) { o << "z position           = " << fZMin << " ... " << fZMax << endl; }
  if ( fDocaMax < 1.E8 ) { o << "doca                 = " << fDocaMax << endl; }
  o << endl;
}

std::ostream&  operator << ( std::ostream& o, const RhoGoodTrackSelector& a )
{
  a.PrintOn ( o );
  return o;
}


