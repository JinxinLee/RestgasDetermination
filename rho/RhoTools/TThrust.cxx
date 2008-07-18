//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TThrust.cc,v 1.2 1997/1/20 10:05:13 
//
// Description:
//      Class TThrust - a simple object to calculate thrust
//      There are three modes:
//              BTAllParticles (0) (default) - use both charged and neutrals
//              BTChargedOnly (1) - use only charged particles
//              BTNeutralOnly (2) - use only neutral particles
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Scott Metzler           original author
//      G. Hamel de Monchenault Implement in terms of TThrustVector
//      John Back               Re-written to be used without TSelectors.
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include <math.h>
#include "RhoTools/TThrust.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandidate.h"
#include "RhoTools/TBooster.h"
#include "RhoBase/TEventInfo.h"

ClassImp(TThrust)

TBuffer &operator>>(TBuffer &buf, TThrust *&obj)
{
   obj = (TThrust *) buf.ReadObject(TThrust::Class());
   return buf;
}

TThrust::TThrust(TCandList& list,
		     const TEventInfo& evtinfo,
		     TParticles mode, Bool_t cutInCms) 
		    : _charged( kFALSE ) {

  _cutInCms = cutInCms;
  _checkCharge = kFALSE;
  // create first the TCandidate selector
  if( mode != TThrust::BTAllParticles ) {
    if( mode == TThrust::BTChargedOnly) {
      _checkCharge = kTRUE;
      _charged = kTRUE;
    } else if( mode == TThrust::BTNeutralOnly ) {
      _checkCharge = kTRUE;
      _charged = kFALSE;
    }
  }
  init( list, evtinfo);
}

TThrust::~TThrust() 
{
}

void
TThrust::init( TCandList& list , 
		 const TEventInfo& eventInfo) {

  // create the Y(4S) candidate, and the corresponding TBooster
  TCandidate theUps( eventInfo.CmFrame(), 0.0 );
  TBooster theBooster( &theUps );

  // compute the thrust vector
  _theThrustVector.BoostAndCompute(list, &theBooster, _cutInCms, 
				   _checkCharge, _charged);

}
