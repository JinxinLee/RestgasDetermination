//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoGoodPhotonSelector                                                //
//                                                                      //
// Selector classes for photon quality selections                       //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoSelector/RhoGoodPhotonSelector.h"

//#include "RhoBase/TRho.h"
#include "RhoBase/RhoCandidate.h"

ClassImp ( RhoGoodPhotonSelector )

TBuffer& operator>> ( TBuffer& buf, RhoGoodPhotonSelector *&obj )
{
  obj = ( RhoGoodPhotonSelector* ) buf.ReadObject ( RhoGoodPhotonSelector::Class() );
  return buf;
}

#include <iostream>
using namespace std;

RhoGoodPhotonSelector::RhoGoodPhotonSelector ( const char* name, Double_t emc, Int_t nCrys, Double_t lat, Bool_t qc ) :
  RhoParticleSelectorBase ( name,"gamma" )
{
  SetShowerEnergy ( emc,1.E8 );
  SetCrystals ( nCrys,100000 );
  //SetLAT ( 0.0,lat );
  //SetPhi ( 0, 2.*3.14159265358979323846 );
  SetTheta ( 0, 2.*3.14159265358979323846 );
}

void RhoGoodPhotonSelector::SetCriterion ( const char* c )
{
  // Set criteria accroding to
  // http://babar-hn.slac.stanford.edu:5090/HyperNews/get/AnalTools/273.html
  TString crit ( c );

  if ( crit=="veryLoose" || crit=="loose" ) {
    SetShowerEnergy ( 0.03,1.E8 );
    SetCrystals ( 0,100000 );
    //SetLAT ( 0.0,0.8 );
    //SetPhi ( 0, 2.*3.14159265358979323846 );
    SetTheta ( 0, 2.*3.14159265358979323846 );
  } else {
    SetShowerEnergy ( 0.1,1.E8 );
    SetCrystals ( 0,100000 );
    //SetLAT ( 0.0,0.8 );
    //SetPhi ( 0, 2.*3.14159265358979323846 );
    SetTheta ( 0, 2.*3.14159265358979323846 );
  }

  RhoParticleSelectorBase::SetCriterion ( c );
}

Bool_t RhoGoodPhotonSelector::Accept ( RhoCandidate& b )
{
  // The RhoGoodPhotonSelector does a selection based on the
  // quality of a reconstructed charged track

  if ( &b == 0 ) { return kFALSE; }
  FairRecoCandidate* tmppoint = b.GetRecoCandidate();
  PndPidCandidate* tmp2 = ( PndPidCandidate* ) tmppoint;
  if ( !Accept ( *tmp2 ) ) { return kFALSE; }
  SetTypeAndMass ( b );
  return kTRUE;
}

Bool_t RhoGoodPhotonSelector::Accept ( PndPidCandidate& cand )
{
  // The RhoGoodPhotonSelector does a selection based on the
  // quality of a reconstructed charged track

  if ( &cand == 0 ) { return kFALSE; }

  if ( fabs ( cand.GetCharge() ) >0.001 ) { return kFALSE; }

  // Check EMC energy deposit
  Float_t emc  = cand.GetEmcCalEnergy();
  if ( emc <  fEmcMin ) { return kFALSE; }
  if ( emc >  fEmcMax ) { return kFALSE; }

  // Cut on number of crystals

  Int_t nc = cand.GetEmcNumberOfCrystals();
  if ( nc <  fNcMin ) { return kFALSE; }
  if ( nc >  fNcMax ) { return kFALSE; }


  // Check LAT
  // ************* at the moment no lateral shape is provided in PandaROOT
  /*
  Float_t lat  = cand.GetEmcLateralShape();
  if (lat <= fLatMin) return kFALSE;
  if (lat >  fLatMax) return kFALSE;
  */


  // Check angular acceptance

  TVector3 p = cand.GetMomentum();
  if ( p.Theta() <  fThetaMin ) { return kFALSE; }
  if ( p.Theta() >  fThetaMax ) { return kFALSE; }

  // ************* removed phi cut for the time being
  // ***** K.G. 25. 3. 2010

  //if (p.Phi() + 3.14159265358979323846  <  fPhiMin)   return kFALSE;
  //if (p.Phi() + 3.14159265358979323846  >  fPhiMax)   return kFALSE;

  return kTRUE;
}

void RhoGoodPhotonSelector::PrintOn ( std::ostream& o ) const
{
  o << GetName() << endl;
  if ( fEmcMin > 0 ) { o << "EMC energy          = " << fEmcMin << " ... " << fEmcMax << " GeV" << endl; }
  if ( fNcMin > 0 ) { o << "Number of Crystals  = " << fNcMin << " ... " << fNcMax << endl; }
  //if (latMin > 0)        o << "LAT                 = " << fLatMin << " ... " << fLatMax << endl;
  if ( fThetaMin > 0 ) { o << "Theta [Rad]         = " << fThetaMin << " ... " << fThetaMax << endl; }
  //if ( fPhiMin > 0 )        o << "Phi   [Rad]         = " << fPhiMin << " ... " << fPhiMax << endl;
  o << endl;
}

std::ostream&  operator << ( std::ostream& o, const RhoGoodPhotonSelector& a )
{
  a.PrintOn ( o );
  return o;
}

