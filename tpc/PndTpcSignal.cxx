//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSignal
//      see PndTpcSignal.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcSignal.h"

// C/C++ Headers ----------------------
#include <math.h>

// Collaborating Class Headers --------
#include "PndTpcAvalanche.h"

// Class Member definitions -----------

ClassImp(PndTpcSignal)

PndTpcSignal::PndTpcSignal()
  : _t(0),_amp(0),_padId(0),_mcTrackId(0),_mcHitId(0),_mcEventId(0),_mother(0)
{;}

PndTpcSignal::PndTpcSignal(double t, 
		     double Amp, 
		     unsigned int PadID, 
		     unsigned int EventID,
		     PndTpcAvalanche* Mo)
  : _t(t),_amp(Amp),_padId(PadID),_mcTrackId(0),_mcHitId(0),_mcEventId(EventID),_mother(Mo)
{;}


unsigned int 
PndTpcSignal::mcTrackId() const {
  if(_mother==0) return _mcTrackId;
  return _mother->mcTrackId();
}

unsigned int 
PndTpcSignal::mcHitId() const {
  if(_mother==0) return _mcHitId;
  return _mother->mcHitId();
}

bool operator== (const PndTpcSignal& lhs, const PndTpcSignal& rhs) {
  return fabs(lhs._t-rhs._t)<1E-4 && lhs._padId==rhs._padId;
}

bool operator< (const PndTpcSignal& lhs, const PndTpcSignal& rhs) {
  return lhs._t<rhs._t;
}

std::ostream& operator<< (std::ostream& s, const PndTpcSignal& me){
  return s<<"PndTpcSignal:\n"
	  <<"     t="<<me._t<<"\n"
	  <<"     Amp="<<me._amp<<"\n"
	  <<"     PadID="<<me._padId<<"\n";
}
