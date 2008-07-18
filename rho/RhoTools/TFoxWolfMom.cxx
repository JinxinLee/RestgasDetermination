//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TFoxWolfMom.cc
//
// Description:
//
//      FoxWolfMom        calculate Fox Wolfram Moments to a given order
//                        (default order is 4 since performance has not 
//                        been checked for higher orders)
//                          
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Theresa Champion                      Original author
//
//      Gautier  8/26/00  add protection against null energy
//
// Copyright Information:
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------
#include <assert.h>

//-----------------------
// This Class's Header --
//-----------------------
#include "RhoTools/TFoxWolfMom.h"  

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "RhoBase/TRho.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "TVector.h"   
#include "TLorentzVector.h"   
#include "TIterator.h"

#include "RhoTools/TBooster.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TEventInfo.h"

ClassImp(TFoxWolfMom)

TBuffer &operator>>(TBuffer &buf, TFoxWolfMom *&obj)
{
   obj = (TFoxWolfMom *) buf.ReadObject(TFoxWolfMom::Class());
   return buf;
}

#include <iostream>
using namespace std;

struct TFoxWolfMomData{
  int maxorder;
  TVector* Xarray;
  TVector* FWarray;
  TVector* FWnorm;
  TVector* sumarray;

  TCandList *boostedList;

  // "new" variables
  TVector *xArray;
  TVector *fwArray;
  TVector *sumArray;
  double fwZero;
};

//----------------
// Constructors --
//----------------

TFoxWolfMom::TFoxWolfMom( TCandList* tracklist, 
			      const TEventInfo* evtinfo, int maxorder)
{

  if (tracklist->GetNumberOfCandidates()<1) {
    cerr << "TFoxWolfMom asked to be calculated from a zero length list"
		  << endl;
  }
  _privateData = new TFoxWolfMomData;
  _privateData->boostedList= 0;

  _privateData->maxorder = maxorder;
  _privateData->Xarray = new TVector(maxorder+1);
  _privateData->FWarray = new TVector(maxorder+1);
  _privateData->FWnorm = new TVector(maxorder+1);
  _privateData->sumarray = new TVector(maxorder+1);

  TCandidate theUps( evtinfo->CmFrame(), 0.0 );
  TBooster theBooster( &theUps );

  double dummy(0.0);
  _privateData->xArray = new TVector(maxorder+1);
  _privateData->fwArray = new TVector(maxorder+1);
  _privateData->sumArray = new TVector(maxorder+1);


  this->BoostTracks(*tracklist, theBooster);
  this->Compute(maxorder);

}

//--------------
// Destructor --
//--------------

TFoxWolfMom::~TFoxWolfMom()
{

  delete _privateData->boostedList;

  delete _privateData->xArray;
  delete _privateData->fwArray;
  delete _privateData->sumArray;

  delete _privateData->Xarray;
  delete _privateData->FWarray;
  delete _privateData->FWnorm;
  delete _privateData->sumarray;
  delete _privateData;

}

void 
TFoxWolfMom::BoostTracks(TCandList &list, 
			   const TBooster &booster){
  
  _privateData->boostedList = new TCandList;  
  booster.BoostTo( list, *_privateData->boostedList );

}

void 
TFoxWolfMom::Compute(int maxorder){
  if (maxorder>0){
      
    const TCandidate* track1;
    const TCandidate* track2;
    double sumEnergy = 0.;
    double pmag = 0.;
    double p1Mag;
    double p2Mag;
    assert(_privateData->maxorder+1 == _privateData->xArray->GetNoElements());

    TCandListIterator iter1(*_privateData->boostedList);
    while ( track1 = iter1.Next() ) {

      sumEnergy += track1->Energy();
      p1Mag = track1->P();
	
      // loop over track2
      TCandListIterator iter2(iter1);
      while ( track2 = iter2.Next() ) {
	p2Mag = track2->P();
	pmag =  p1Mag*p2Mag;
	double x =  track1->P3().Dot(track2->P3())/pmag;
	//  was: cos ( track1cms.angle(Hep3Vector(track2cms)) );
	  
	// compute xArray values for Legendre polynomials
	(*_privateData->xArray)(0) = 1.0;
	(*_privateData->xArray)(1) = x;

	// Use () for safe array access.
	// Use [] when array index is calculated.
	for(int i=2; i<=_privateData->maxorder; i++){
	  (*_privateData->xArray)(i) = ( (2*(i+1)-3)*x* 
					 (*_privateData->xArray)(i-1) - 
					 (i-1)*(*_privateData->xArray)(i-2) )/i;
	}
	  
	// add contribution to Fox Wolfram Moments for this pair of tracks
	for(int j=0; j<=_privateData->maxorder; j++){
	  (*_privateData->sumArray)(j) += 2.0 * pmag * (*_privateData->xArray)(j) ;
	}
	  
      } // end loop over track2

      // Add contribution from the diagonal.  
      // Note that P_l(1) is 1.
      for(int j=0; j<=_privateData->maxorder; j++){
	(*_privateData->sumArray)(j) += p1Mag *p1Mag;
      }

    } // end loop over track1
      
    // compute total Fox Wolfram Moments
    double sumESquared = sumEnergy*sumEnergy;
    for(int i=0; i<=_privateData->maxorder; i++){
      if( sumESquared==0. ) 
	{  
	  (*_privateData->fwArray)(i) = 1.;
	} else {
	  (*_privateData->fwArray)(i) = (*_privateData->sumArray)(i)/sumESquared;
	}
    }
    _privateData->fwZero =(*_privateData->fwArray)(0);
    
    // Set up old (deprecated) variables
    for (int n=0; n<=_privateData->maxorder; n++){
      (*_privateData->Xarray)(n) = (*_privateData->xArray)(n);
      (*_privateData->FWarray)(n) = (*_privateData->fwArray)(n);
      (*_privateData->sumarray)(n) = (*_privateData->sumArray)(n);
    }

  } else {
    cerr << "Cannot compute FW moments for order " << maxorder
		  << endl;
  }

  
}

const TVector &
TFoxWolfMom::SumArray() const{
  return *(_privateData->sumArray);

}

const TVector & 
TFoxWolfMom::FWArray() const{
  return *_privateData->FWarray;
}

const TVector & 
TFoxWolfMom::Legendre() const{
  return *_privateData->Xarray;
}

double
TFoxWolfMom::Legendre(int order) const{
  return (*_privateData->xArray)(order);
}


double
TFoxWolfMom::FWzero() const{
  return _privateData->fwZero;
}

double
TFoxWolfMom::FWnorm(int order) const{
  if(order <= _privateData->maxorder){ 
    if ((*_privateData->fwArray)(0)!=0.)
      return ((*_privateData->fwArray)(order)/(*_privateData->fwArray)(0));
    else {
      cerr << "Zeroth FW moment was zero.  Returning zero for "
		    << order << "th moment" << endl;
      return 0;
    }
  }
  cerr << "FW moments were constructed up to order " 
		<< _privateData->maxorder << " but order " 
		<< order << " has been requested."
		<< endl << "Returning 0." << endl;
  return 0;
  
}


//----------------
// Accessor     --
//----------------

