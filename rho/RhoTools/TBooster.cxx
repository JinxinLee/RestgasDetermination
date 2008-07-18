//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TBooster.cxx,v 1.4 2002-02-01 23:00:05 marcel Exp $
//
// Description:
//	Class TBooster
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//
// History (add to end):
//      Gautier   Sept, 1997  - creation
//
// Copyright Information:
//	Copyright (C) 1997		Lawrence Berkeley Laboratory
//	Copyright (C) 1997	       CEA - Centre d'Etude de Saclay
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TBooster.h"
#include "RhoMath/TLorentzVectorErr.h"
#include "TRotation.h"
#include "TLorentzVector.h"
#include "TLorentzRotation.h"
#include "RhoBase/TFactory.h"
#include "RhoBase/TFitParams.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"

ClassImp(TBooster)

TBuffer &operator>>(TBuffer &buf, TBooster *&obj)
{
   obj = (TBooster *) buf.ReadObject(TBooster::Class());
   return buf;
}

#include <iostream>
using namespace std;

//
// Constructors
//

TBooster::TBooster( const TCandidate* referenceCand, Bool_t saveCovMatrix )
: fParticle(0),
fLorentzVector(0), fRotateAndBoostTo(0), fRotateAndBoostFrom(0),
fSaveCovMatrix(saveCovMatrix)
{
    if( referenceCand ) SetParticle( *referenceCand );
}

TBooster::TBooster( const TCandidate& referenceCand, Bool_t saveCovMatrix )
: fParticle(0),
fLorentzVector(0), fRotateAndBoostTo(0), fRotateAndBoostFrom(0),
fSaveCovMatrix(saveCovMatrix)
{
    SetParticle( referenceCand );
}

TBooster::TBooster( const TLorentzVector& lorentzVector, Bool_t saveCovMatrix )
: fParticle(0),
fLorentzVector(0), fRotateAndBoostTo(0), fRotateAndBoostFrom(0),
fSaveCovMatrix(saveCovMatrix)
{
    SetLorentzVector( lorentzVector );
}

void
TBooster::SetParticle( const TCandidate& referenceCand )
{
    clean();
    fParticle = TFactory::Instance()->NewCandidate( referenceCand );
}

void
TBooster::SetLorentzVector( const TLorentzVector& lorentzVector )
{
    clean();
    fLorentzVector = new TLorentzVector( lorentzVector );
}

//--------------
// Destructor --
//--------------

TBooster::~TBooster() 
{
    clean();
}


TCandidate&
TBooster::BoostTo( const TCandidate& cand) const
{
    return Boost( cand, To);
}

TCandidate&
TBooster::BoostFrom( const TCandidate& cand) const
{
    return Boost( cand, From);
}

void
TBooster::BoostTo( TCandList& initialList, 
		  TCandList& boostedList,
		  Bool_t cleanList) const
{
    if( cleanList ) {
	boostedList.Cleanup();
    }
    TCandListIterator iter( initialList );
    TCandidate* cand=0;
    while( cand=iter.Next() )
    {
	boostedList.Add(Boost( *cand, To ));
    }
}

void
TBooster::BoostFrom( TCandList& initialList, 
		    TCandList& boostedList,
		    Bool_t cleanList) const
{
    if( cleanList ) {
	boostedList.Cleanup();
    }
    TCandListIterator iter( initialList );
    TCandidate* cand=0;
    while( cand=iter.Next() )
    {
	boostedList.Add(Boost( *cand, From ));
    }
}


//		-----------------------------------------
// 		-- Private Function Member Definitions --
//		-----------------------------------------

TCandidate&
TBooster::Boost( const TCandidate& cand, 
		BoostSign sign) const
{
    TCandidate *bc = TFactory::Instance()->NewCandidate(cand);
    TVector3 boostVector = LorentzVector().BoostVector();
    if (sign == To)
	bc->Boost(-boostVector);
    else
	bc->Boost(boostVector);
    return *bc;

    //    // recursive implementation
    //    // the function returns a poInt_ter to the current mother
    //    // in order to establish mother-daughter links
    //    //
    //    // 1) get the boost vector beta=p/E of the moving frame.
    //    TVector3 beta( theLorentzVector().boostVector() );
    //    //      boostTo   : -beta
    //    //      boostFrom :  beta
    //    if( sign==To ) beta *= -1;
    //    // 2) now apply the boost to the four-vector
    //    TLorentzVector the4Vector( cand.p4() ); // four-vector of the candidate
    //    the4Vector.boost( beta );                 // boost it along the boost vector
    
    // Create a instance of a TCandidate with the boosted 4-vector
    //  the problem is that we loose all connection with the original...
    //  the solution would be to copy the TCandidate and then
    //  to boost it !!!  We are missing a boost method in TCandidate,
    //  or in TCandBase.
    
    if (fSaveCovMatrix == kTRUE) {
	
	const TLorentzVector &boosted4Mtm = BoostedP4(cand, sign);
	
	// Boost 4-momentum error (covariance) matrix
	const TLorentzVectorErr &boostedCovError = BoostedError(cand, sign);
	
	// Don't boost decay vertex - this will have a different meaning in the boosted
	// frame - a new re-fit of the boosted candidates would need to be done to get the
	// correct vertex in the boosted frame. Also, can't change the values
	// of the vertex in the abstract class, TAbsVertex, unless we know what vertexer was
	// used. This would complicate the TBooster Interface.
	
	// Set the trajectory for the boosted TCandidate 
	// (really, set it's new covariance matrix).
	
	Double_t charge = cand.Charge(); // charge is Double_t!
	
	TCandidate *boostedCand = TFactory::Instance()->NewCandidate(TCandidate(boosted4Mtm, charge));    
	
	boostedCand->SetTrajectory(boosted4Mtm, boostedCovError.CovMatrix(),(int) charge, cand.PdtEntry(), 0);    
	
	// now loop on daughters
	TCandListIterator iterDau = cand.DaughterIterator();
	TCandidate* daug=0;
	while ( daug=iterDau.Next() )
	{
	    
	    const TLorentzVector &boostedDaug4Mtm = BoostedP4(*daug, sign);
	    
	    // Boost 4-momentum error (covariance) matrix
	    const TLorentzVectorErr &boostedDaugCovError = BoostedError(*daug, sign);
	    
	    // Set the trajectory for the boosted TCandidate 
	    // (really, set it's new covariance matrix).
	    
	    Double_t daugCharge = daug->Charge(); // charge is Double_t!
	    
	    TCandidate* boostedDaugCand = TFactory::Instance()->NewCandidate(TCandidate(boostedDaug4Mtm, daugCharge));
	    
	    boostedDaugCand->SetTrajectory(boostedDaug4Mtm, boostedDaugCovError.CovMatrix(), (int)daugCharge, 
		daug->PdtEntry(), 0);
	    
	    AddDaughterLink( *boostedCand, boostedDaugCand );
	}
	
	// Set mass here when the daughters are defined.
	boostedCand->SetMass( cand.Mass() );
	// have to cast off the const because of the TCandidate Int_terface !!!!!  
	if ( cand.PdtEntry() ) boostedCand->SetType(cand.PdtEntry() );
	return *boostedCand;
	
    }
    
    TCandidate *boostedCand = TFactory::Instance()->NewCandidate(TCandidate( BoostedP4( cand, sign ), cand.Charge() ));
    //
    // now loop on daughters
    TCandListIterator iterDau = cand.DaughterIterator();
    TCandidate* dau=0;
    while ( dau=iterDau.Next() )
    {
	TCandidate boostedDau = Boost( *dau, sign );
	AddDaughterLink( *boostedCand, &boostedDau );
    }
    
    // Set mass here when the daughters are defined.
    boostedCand->SetMass( cand.Mass() );
    if ( cand.PdtEntry() ) boostedCand->SetType( cand.PdtEntry() );
    
    // set marker (for overlap checks)
    boostedCand->SetMarker(cand.GetMarker(0),0);
    boostedCand->SetMarker(cand.GetMarker(1),1);
    boostedCand->SetMarker(cand.GetMarker(2),2);
    boostedCand->SetMarker(cand.GetMarker(3),3);
    
    // set link to AOD Information
    if ( &cand.GetMicroCandidate() ) {
	boostedCand->SetMicroCandidate(cand.GetMicroCandidate());
    }
    return *boostedCand;
    
}

TLorentzVector 
TBooster::BoostedP4( const TCandidate& cand, BoostSign sign ) const
{
    // new implemetation using Chris Hearty's rotateAndBoost
    return RotateAndBoost( sign ) * cand.P4();
}

TLorentzVectorErr
TBooster::BoostedError(const TCandidate& cand, BoostSign sign) const 
{
    
    // Get combined rotation and Lorentz Boost
    const TLorentzRotation &rotation = RotateAndBoost( sign );
    // Form BbrLorentzVectorErr object for 4-momentum
    // BbrLorentzVectorErr(4-momentum, covariance Matrix)
    
    TLorentzVectorErr theError(cand.P4(), cand.P4Err());
    
    // Now transform this error using the rotation and boost.
    // The covariance matrix is changed using a similarity transformation
    
    return theError.Transform(rotation);
}

void
TBooster::clean()
{
    //delete fParticle; // comes from factory
    fParticle=0;
    delete fLorentzVector;
    fLorentzVector=0;
    delete fRotateAndBoostTo;
    fRotateAndBoostTo=0;
    delete fRotateAndBoostFrom;
    fRotateAndBoostFrom=0;
}

TCandidate*
TBooster::Create( const TCandidate& input )
{
    return TFactory::Instance()->NewCandidate(TCandidate( BoostTo( input ) ));
}

TBooster* 
TBooster::myself_() const
{
    // cast off the const
    return (TBooster*) this;
}

const TCandidate& 
TBooster::RestParticle() const
{
    if( fParticle==0 )
    {
	if( fLorentzVector==0 ) 
	{
	    cerr << "Frame not set " << endl;
	}
	
	// create the TCandidate
	myself_()->fParticle = TFactory::Instance()->NewCandidate(TCandidate( *fLorentzVector, 0 ));
    }
    
    return *fParticle;
}

const TLorentzVector& 
TBooster::LorentzVector() const
{
    if( fLorentzVector==0 )
    {
	if( fParticle==0 ) 
	{
	    cerr << "Reference candidate not set " << endl;
	}
	
	// create the Lorentz Vector
	myself_()->fLorentzVector = new TLorentzVector( fParticle->P4() );
	
    }
    return *fLorentzVector;
}

const TLorentzRotation& 
TBooster::RotateAndBoost( BoostSign sign ) const
{
    TLorentzRotation* rAndB=0;
    switch( sign )
    {
    case To :
	rAndB = myself_()->fRotateAndBoostTo;
	break;
    case From:
	rAndB = myself_()->fRotateAndBoostFrom;
	break;
    default:
	cerr << "Unknown case " << endl;
    }
    
    if( rAndB==0 )
    {
	// the boost vector
	TVector3 boostVector( LorentzVector().BoostVector() );
	//
	// rotation matrix and boost
	
	Double_t boost  = boostVector.Mag();  
	TVector3 boostAlongZ( 0., 0., boost );
	TLorentzRotation boostPart( boostAlongZ );  
	
	Double_t alpha( boostVector.Phi()   );
	Double_t beta(  boostVector.Theta() );  
	Double_t gamma(-boostVector.Phi()   );  
	
	TRotation euler;  
	euler.RotateZ( gamma );  
	euler.RotateY( beta  );
	euler.RotateZ( alpha );
	
	TLorentzRotation rotationPart( euler );
	TLorentzRotation product( rotationPart*boostPart );
	
	rAndB = new TLorentzRotation( product );
	
	if( sign==To ) 
	{  
	    rAndB->Invert();
	}
	// assign rAndB to the data member
	switch( sign )
	{
	case To :
	    myself_()->fRotateAndBoostTo = rAndB;
	    break;
	case From:
	    myself_()->fRotateAndBoostFrom = rAndB;
	    break;
	default:
	    cerr << "Unknown case " << endl;
	}
    }
    
    return *rAndB;
}

TCandMap*
TBooster::BoostAndSort( TCandList& list,
		       TCandList& storedList,
		       BoostSign sign)
{
    // boost particles in the Ups(4S) frame
    TCandList boostedList;
    if( sign==TBooster::To )
	BoostTo( list, boostedList);
    else
	BoostFrom( list, boostedList);
    // Now store the list in a sorted way
    TCandidate* c=0;
    TCandListIterator iterBoosted( boostedList );
    c = iterBoosted.Next();  // the first one
    if(c) {
	storedList.Add(*c); // include the first one in the list
	// loop on subsequent candidates in the list
	while( c=iterBoosted.Next() ) 
	{
	    Double_t p=c->P();
	    Bool_t stored(kFALSE);
	    for( Int_t i=0; i<storedList.GetNumberOfCandidates(); i++ )
	    {
		TCandidate* cStored = &storedList[i];
		Double_t pStored = cStored->P();
		if( p>pStored ) {
		    storedList.InsertAt(i,*c);
		    stored=kTRUE;
		    break;
		}
	    }
	    // the candidate hasn't been stored : append it 
	    if( !stored ) storedList.Add( *c );
	}
    }
    return 0;
}	





