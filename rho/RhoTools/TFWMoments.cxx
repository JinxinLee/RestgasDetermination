//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//      
//	Class TFWMoments - a simple class to calculate Fox-Wolfram Moments
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author :
//      Gautier Hamel de Monchenault  
//
// From :
//      Alex Samuel, Theresa Champion, original authors
//             of Fox-Wolfram classes in BetaTools
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
#include "RhoTools/TFWMoments.h"

ClassImp(TFWMoments)

TBuffer &operator>>(TBuffer &buf, TFWMoments *&obj)
{
   obj = (TFWMoments *) buf.ReadObject(TFWMoments::Class());
   return buf;
}

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

TFWMoments::TFWMoments( int maxorder )
  : _nmom( maxorder+1 )
  , _FWarray( _nmom )
  , _sumarray( _nmom )
{
}

void
TFWMoments::BoostAndCompute( TCandList& list,
			       TBooster* booster )
{
  assert( booster!=0 );
  TCandList boostedList;
  booster->BoostTo( list, boostedList );
  Compute( boostedList );
  boostedList.Cleanup();
}

void
TFWMoments::Compute(TCandList& list )
{
  // initialize
  Reset();
  
  if( list.GetNumberOfCandidates()==0 ) return;

  TCandidate* c1;
  TCandidate* c2;
  double s = 0.;
  int l;

  // start a loop over the all candidates
  TCandListIterator iter1(list);

  while ( c1 = iter1.Next() ) 
    {
      // this candidate's 3-momentum
      TVector3 p1(c1->P3());  // use the cast operator of LorentzVector
      double pmag1 = p1.Mag();

      // loop over other candidates, starting at the next one in the list
      TCandListIterator iter2(iter1);

      while ( c2 = iter2.Next() ) 
	{
	  // this candidate's 3-momentum
	  TVector3 p2(c2->P3());
	  double pmag2 = p2.Mag();

	  // the cosine of the angle between the two candidates
	  double cosPhi =  cos ( p1.Angle(p2) );

	  // the contribution of this pair of track 
	  // (note the factor 2 : the pair enters the sum twice)
	  for( l=0; l<_nmom; l++ )
	    _sumarray(l) += 2 * pmag1 * pmag2 * Legendre( l, 0, cosPhi );
	}
      
      // contribution of this track
      for( l=0; l<_nmom; l++ )
	_sumarray(l) += pmag1 * pmag1 * Legendre( l, 0, 1. );
      
      // total energy
      s += c1->Energy();
      
    }
  
  // well ...
  if( s<=0. ) return;
  
  // normalize Fox Wolfram Moments
  for(int i=0; i<_nmom; i++)
    _FWarray(i) = _sumarray(i)/pow(s,2) ;

}

void
TFWMoments::Reset() 
{
  for ( int i=0; i<_nmom; i++) 
    { 
      _FWarray(i) = 0.;
      _sumarray(i) = 0.;
    }
}

double
TFWMoments::R( int order ) const
{
  if( H(0)>0. ) {
    if( order < _nmom ){ 
      return ( H(order)/H(0) );
    }
  }
  return 0.;
}

TFWMoments::~TFWMoments() {}


// Compute associated Legendre polynomial P_l^m(x) .
// modification of plgndr() code, from Press et. al., "Numerical Recipes in C"
// Code from Alex Samuel's BetaTools/TFoxWolfram class,
// shamelessly stollen by Gautier

double 
TFWMoments::Legendre( int l, int m, double x )
{
    assert(m >= 0.);
    assert(m <= l);
    assert(fabs(x) <= 1.);

    double pmm = 1.;

    if(m > 0)
    {
	double somx2 = sqrt((1. - x) * (1. + x));
	double fact = 1.;

	for(int i=0; i<m; i++)
	{
	    pmm *= -fact * somx2;
	    fact += 2.0;
	}
    }

    if(l == m)
	return pmm;

    else
    {
	double pmmp1 = x * (2 * m + 1) * pmm;
	if(l == m + 1)
	    return pmmp1;
	else
	{
	    for(int ll=m+2; ll<=l; ll++)
	    {
		double pll = (x * (2 * ll - 1) * pmmp1 - (ll + m - 1) * pmm) / 
		    (ll - m);
		pmm = pmmp1;
		pmmp1 = pll;
	    }

	    return pmmp1;
	}
    }
}


