//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//      
//	Class TConeMomFlow - a simple object to calculate the momentum flow
//        of an event in polar angle intervals around a given axis (a la CLEO)
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Georges Vasseur
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
#include "RhoTools/TConeMomFlow.h"
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

ClassImp(TConeMomFlow)

TBuffer &operator>>(TBuffer &buf, TConeMomFlow *&obj)
{
   obj = (TConeMomFlow *) buf.ReadObject(TConeMomFlow::Class());
   return buf;
}

#include <iostream>
using namespace std;

TConeMomFlow::TConeMomFlow( int nb_intervals )
  : _ninter( nb_intervals )
  , _flowarray( _ninter )
  , _momsum( 0. )
{
}

void
TConeMomFlow::BoostAndCompute( TCandList& list,
			const TVector3& axis, TBooster* booster )
{
  assert( booster!=0 );
  TCandList boostedList;
  booster->BoostTo( list, boostedList );
  Compute( boostedList, axis );
  boostedList.Cleanup();
}

void
TConeMomFlow::Compute( TCandList& list,
                         const TVector3& axis)
{
  const double piover2 = 0.5*TMath::Pi();
  const double angwidth = piover2/_ninter;

  // initialize
  Reset();
  
  if( list.GetNumberOfCandidates()==0 ) return;

  TCandidate* c;
  TCandListIterator iter(list);
  int l;

  // start a loop over the all candidates
  while ( c = iter.Next() ) 
    {
      // this candidate's 3-momentum
      TVector3 p(c->P3());
      double pmag = p.Mag();

      // the angle with the event axis
      double ang =  axis.Angle(p);
      if ( ang > piover2 ) ang = TMath::Pi() - ang;
      l = (int)(ang/angwidth);
      if ( l < 0 ) {
        l = 0;
        cerr << "TConeMomFlow: negative cone number " << l
          << endl;
      }
      else if ( l >= _ninter ) {
        l = _ninter-1;
        cerr << "TConeMomFlow: too big cone number " << l
          << endl;
      }
      
      // contribution of this track
      _flowarray(l) += pmag;
      
      // total momentum
      _momsum += pmag;
      
    }

}

void
TConeMomFlow::Reset() 
{
  for ( int i=0; i<_ninter; i++) 
    { 
      _flowarray(i) = 0.;
      _momsum = 0.;
    }
}

double
TConeMomFlow::NormFlow( int order ) const
{
  if( _momsum>0. ) {
    if( order < _ninter ){ 
      return ( Flow(order)/_momsum );
    }
  }
  return 0.;
}

TConeMomFlow::~TConeMomFlow() {}

