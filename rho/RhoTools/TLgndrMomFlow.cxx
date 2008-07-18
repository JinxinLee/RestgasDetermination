//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//      
//	Class TLgndrMomFlow - a simple object to integrate the 
//      Legendre expansion of the track-vs-thrust angle weighted 
//      by the track/cluster momentum 
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Vasia Shelkov, December 2000, LBNL 
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

//-------------
// C Headers --
//-------------
#include <math.h>
#include <assert.h>
//-----------------------
// This Class's Header --
//-----------------------
#include "RhoTools/TLgndrMomFlow.h"
//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "TLorentzVector.h"
#include "RhoTools/TBooster.h"
#include "RhoBase/TCandidate.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//ClassImp(TLgndrMomFlow)

TLgndrMomFlow::TLgndrMomFlow( Int_t nb_terms )
: _nterms( nb_terms )
, _p0array( nb_terms )
, _p1array( nb_terms )
, _p2array( nb_terms )
, _trackarray( nb_terms )
{
}

void
TLgndrMomFlow::BoostAndCompute( const TCandList& list,
				 const TVector3& axis, TBooster* booster )
{
    assert( booster!=0 );
    TCandList *l = const_cast<TCandList*> (&list);
    static TCandList boostedList;
    boostedList.Cleanup();
    booster->BoostTo( *l, boostedList );
    Compute( boostedList, axis );
}

void
TLgndrMomFlow::Compute( const TCandList& list,
			 const TVector3& axis)
{
    if( list.GetLength()==0 ) return;
    // initialize
    Reset();
    //
    TCandidate* c;
    TCandList *l = const_cast<TCandList*> (&list);
    TCandListIterator iter(*l);
    iter.Rewind();
    while ( c = iter.Next() )
    { 
	// this candidate's 3-momentum
	TVector3 p(c->P3());
	Double_t pmag = p.Mag();
	// the angle with the event axis
	Double_t ang =  axis.Angle(p);
	// contribution of this track to all terms requested
	TrackTerms(_nterms, TMath::Cos(ang));
	// integrating this track contribution over all terms      
	for (Int_t l=0; l<_nterms; l++){
	    _p0array(l) += _trackarray(l); 
	    _p1array(l) += _trackarray(l)*pmag; 
	    _p2array(l) += _trackarray(l)*pmag*pmag; 
	}
    }
}

void
TLgndrMomFlow::Reset() 
{
    for ( Int_t i=0; i<_nterms; i++) 
    { 
	_p0array(i) = 0.;
	_p1array(i) = 0.;
	_p2array(i) = 0.;
	_trackarray(i) = 0.;   
    }
}

Double_t
TLgndrMomFlow::TrackTerms(Int_t n, Double_t x){
    
    switch(n) {
    case 0:
	_trackarray(0)=1.0;	
	return _trackarray(0);
    case 1:
	_trackarray(1)=x;
	return _trackarray(1);
    default:
	Int_t l = n-1;
	_trackarray(n) = (TrackTerms(l,x)*x*(2*l+1)-l*TrackTerms(l-1,x))/n;
	return _trackarray(n);	   
    }
}

TLgndrMomFlow::~TLgndrMomFlow() {}

