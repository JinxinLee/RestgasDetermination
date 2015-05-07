//--------------------------------------------------------------------------
// File and Version Information:
//  $Id: RhoBooster.cxx,v 1.4 2002-02-01 23:00:05 marcel Exp $
//
// Description:
//  Class RhoBooster
//
// Environment:
//  Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//  Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//
// History (add to end):
//      Gautier   Sept, 1997  - creation
//
// Copyright Information:
//  Copyright (C) 1997    Lawrence Berkeley Laboratory
//  Copyright (C) 1997         CEA - Centre d'Etude de Saclay
//
// ROOT Version by Marcel Kunze, RUB
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)
//------------------------------------------------------------------------

#include "RhoTools/RhoBooster.h"

ClassImp ( RhoBooster )

TBuffer& operator>> ( TBuffer& buf, RhoBooster *&obj )
{
  obj = ( RhoBooster* ) buf.ReadObject ( RhoBooster::Class() );
  return buf;
}

#include <iostream>
using namespace std;

//
// Constructors
//

RhoBooster::RhoBooster ( const RhoCandidate* referenceCand, Bool_t saveCovMatrix )
  : fBoostVector ( 0.,0.,0. ),
//    fRotateAndBoostTo(0),
//    fRotateAndBoostFrom(0),
    fSaveCovMatrix ( saveCovMatrix )
{
  if ( referenceCand ) SetBoostVector(referenceCand);
}

RhoBooster::RhoBooster ( const RhoCandidate& referenceCand, Bool_t saveCovMatrix )
  : fBoostVector ( 0.,0.,0. ),
//    fRotateAndBoostTo(0),
//    fRotateAndBoostFrom(0),
    fSaveCovMatrix ( saveCovMatrix )
{
  SetBoostVector(referenceCand);
}

RhoBooster::RhoBooster ( const TLorentzVector& lorentzVector, Bool_t saveCovMatrix )
  : fBoostVector ( 0.,0.,0. ),
//    fRotateAndBoostTo(0),
//    fRotateAndBoostFrom(0),
    fSaveCovMatrix ( saveCovMatrix )
{
  SetBoostVector(lorentzVector);
}

RhoBooster::RhoBooster ( const TVector3& boostVector, Bool_t saveCovMatrix )
  : fBoostVector ( 0.,0.,0. ),
//    fRotateAndBoostTo(0),
//    fRotateAndBoostFrom(0),
    fSaveCovMatrix ( saveCovMatrix )
{
  SetBoostVector(boostVector);
}


//--------------
// Destructor --
//--------------

RhoBooster::~RhoBooster()
{
  //clean();
}

// void
// RhoBooster::clean()
// {
//   if(fRotateAndBoostTo) delete fRotateAndBoostTo;
//   fRotateAndBoostTo=0;
//   if(fRotateAndBoostFrom) delete fRotateAndBoostFrom;
//   fRotateAndBoostFrom=0;
// }

RhoCandidate&
RhoBooster::BoostTo ( const RhoCandidate& cand ) const
{
  return Boost ( cand, To );
}

RhoCandidate&
RhoBooster::BoostFrom ( const RhoCandidate& cand ) const
{
  return Boost ( cand, From );
}

void
RhoBooster::BoostTo ( RhoCandList& initialList,
                      RhoCandList& boostedList,
                      Bool_t cleanList ) const
{
  if ( cleanList ) {
    boostedList.Cleanup();
  }
  RhoCandListIterator iter ( initialList );
  RhoCandidate* cand=0;
  while ( (cand=iter.Next()) ) {
    boostedList.Add ( &(Boost ( *cand, To )) );
  }
}

void
RhoBooster::BoostFrom ( RhoCandList& initialList,
                        RhoCandList& boostedList,
                        Bool_t cleanList ) const
{
  if ( cleanList ) {
    boostedList.Cleanup();
  }
  RhoCandListIterator iter ( initialList );
  RhoCandidate* cand=0;
  while ( (cand=iter.Next()) ) {
    boostedList.Add ( &(Boost ( *cand, From )) );
  }
}


//    -----------------------------------------
//    -- Private Function Member Definitions --
//    -----------------------------------------

RhoCandidate&
RhoBooster::Boost ( const RhoCandidate& cand,
                    BoostSign sign ) const
{
  RhoCandidate* bc = RhoFactory::Instance()->NewCandidate ( cand );
  if ( sign == To ) {
    bc->Boost ( -fBoostVector );
  } else {
    bc->Boost ( fBoostVector );
  }
  return *bc;

  //    // recursive implementation
  //    // the function returns a poInt_ter to the current mother
  //    // in order to establish mother-daughter links
  //    //
  //    // 1) get the boost vector beta=p/E of the moving frame.
  //    TVector3 beta( theLorentzVector().fBoostVector() );
  //    //      boostTo   : -beta
  //    //      boostFrom :  beta
  //    if( sign==To ) beta *= -1;
  //    // 2) now apply the boost to the four-vector
  //    TLorentzVector the4Vector( cand.p4() ); // four-vector of the candidate
  //    the4Vector.boost( beta );                 // boost it along the boost vector

  // Create a instance of a RhoCandidate with the boosted 4-vector
  //  the problem is that we loose all connection with the original...
  //  the solution would be to copy the RhoCandidate and then
  //  to boost it !!!  We are missing a boost method in RhoCandidate,
  //  or in TCandBase.

//   if ( fSaveCovMatrix == kTRUE ) {
//     const TLorentzVector& boosted4Mtm = BoostedP4 ( cand, sign );
//     // Boost 4-momentum error (covariance) matrix
//     const RhoLorentzVectorErr& boostedCovError = BoostedError ( cand, sign );
// 
//     // Don't boost decay vertex - this will have a different meaning in the boosted
//     // frame - a new re-fit of the boosted candidates would need to be done to get the
//     // correct vertex in the boosted frame. Also, can't change the values
//     // of the vertex in the abstract class, TAbsVertex, unless we know what vertexer was
//     // used. This would complicate the TBooster Interface.
// 
//     // Set the trajectory for the boosted RhoCandidate
//     // (really, set it's new covariance matrix).
//     Double_t charge = cand.Charge(); // charge is Double_t!
//     RhoCandidate* boostedCand = RhoFactory::Instance()->NewCandidate ( RhoCandidate ( boosted4Mtm, charge ) );
//     boostedCand->SetTrajectory ( boosted4Mtm, boostedCovError.CovMatrix(), ( int ) charge, cand.PdtEntry(), 0 );
//     // now loop on daughters
//     RhoCandListIterator iterDau = cand.DaughterIterator();
//     RhoCandidate* daug=0;
//     while ( daug=iterDau.Next() ) {
//       const TLorentzVector& boostedDaug4Mtm = BoostedP4 ( *daug, sign );
//       // Boost 4-momentum error (covariance) matrix
//       const RhoLorentzVectorErr& boostedDaugCovError = BoostedError ( *daug, sign );
//       // Set the trajectory for the boosted RhoCandidate
//       // (really, set it's new covariance matrix).
//       Double_t daugCharge = daug->Charge(); // charge is Double_t!
//       RhoCandidate* boostedDaugCand = RhoFactory::Instance()->NewCandidate ( RhoCandidate ( boostedDaug4Mtm, daugCharge ) );
//       boostedDaugCand->SetTrajectory ( boostedDaug4Mtm, boostedDaugCovError.CovMatrix(), ( int ) daugCharge,
//                                        daug->PdtEntry(), 0 );
//       boostedCand->AddDaughterLink ( boostedDaugCand );
//     }
//     // Set mass here when the daughters are defined.
//     boostedCand->SetMass ( cand.Mass() );
//     // have to cast off the const because of the RhoCandidate Int_terface !!!!!
//     if ( cand.PdtEntry() ) { boostedCand->SetType ( cand.PdtEntry() ); }
//     return *boostedCand;
//   }
// 
//   RhoCandidate* boostedCand = RhoFactory::Instance()->NewCandidate ( RhoCandidate ( BoostedP4 ( cand, sign ), cand.Charge() ) );
//   //
//   // now loop on daughters
//   RhoCandListIterator iterDau = cand.DaughterIterator();
//   RhoCandidate* dau=0;
//   while ( dau=iterDau.Next() ) {
//     RhoCandidate boostedDau = Boost ( *dau, sign );
//     boostedCand->AddDaughterLink ( &boostedDau );
//   }
// 
//   // Set mass here when the daughters are defined.
//   boostedCand->SetMass ( cand.Mass() );
//   if ( cand.PdtEntry() ) { boostedCand->SetType ( cand.PdtEntry() ); }
// 
//   // set marker (for overlap checks)
//   boostedCand->SetMarker ( cand.GetMarker ( 0 ),0 );
//   boostedCand->SetMarker ( cand.GetMarker ( 1 ),1 );
//   boostedCand->SetMarker ( cand.GetMarker ( 2 ),2 );
//   boostedCand->SetMarker ( cand.GetMarker ( 3 ),3 );
// 
//   // set link to AOD Information
//   if ( &cand.GetRecoCandidate() ) {
//     boostedCand->SetRecoCandidate ( cand.GetRecoCandidate() );
//   }
//   return *boostedCand;

}

// TLorentzVector
// RhoBooster::BoostedP4 ( const RhoCandidate& cand, BoostSign sign ) const
// {
//   // new implemetation using Chris Hearty's rotateAndBoost
//   return RotateAndBoost ( sign ) * cand.P4();
// }

// RhoLorentzVectorErr
// RhoBooster::BoostedError ( const RhoCandidate& cand, BoostSign sign ) const
// {
// 
//   // Get combined rotation and Lorentz Boost
//   const TLorentzRotation& rotation = RotateAndBoost ( sign );
//   // Form BbrLorentzVectorErr object for 4-momentum
//   // BbrLorentzVectorErr(4-momentum, covariance Matrix)
// 
//   RhoLorentzVectorErr theError ( cand.P4(), cand.P4Err() );
// 
//   // Now transform this error using the rotation and boost.
//   // The covariance matrix is changed using a similarity transformation
// 
//   return theError.Transform ( rotation );
// }


// const TLorentzRotation&
// RhoBooster::RotateAndBoost ( BoostSign sign ) const
// {
//   TLorentzRotation* rAndB=0;
//   switch ( sign ) {
//   case To :
//     rAndB = fRotateAndBoostTo;
//     break;
//   case From:
//     rAndB = fRotateAndBoostFrom;
//     break;
//   default:
//     cerr << "Unknown case " << endl;
//   }
// 
//   if ( rAndB==0 ) {
//     // rotation matrix and boost
//     Double_t boost  = fBoostVector.Mag();
//     TVector3 boostAlongZ ( 0., 0., boost );
//     TLorentzRotation boostPart ( boostAlongZ );
// 
//     Double_t alpha ( fBoostVector.Phi() );
//     Double_t beta ( fBoostVector.Theta() );
//     Double_t gamma ( -fBoostVector.Phi() );
// 
//     TRotation euler;
//     euler.RotateZ ( gamma );
//     euler.RotateY ( beta );
//     euler.RotateZ ( alpha );
// 
//     TLorentzRotation rotationPart ( euler );
//     TLorentzRotation product ( rotationPart*boostPart );
// 
//     rAndB = new TLorentzRotation ( product );
// 
//     if ( sign==To ) {
//       rAndB->Invert();
//     }
//     // assign rAndB to the data member
//     switch ( sign ) {
//     case To :
//       fRotateAndBoostTo = rAndB;
//       break;
//     case From:
//       fRotateAndBoostFrom = rAndB;
//       break;
//     default:
//       cerr << "Unknown case " << endl;
//     }
//   }
//
//  return *rAndB;
//}

