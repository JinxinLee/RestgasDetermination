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
  : ft(0),famp(0),fpadId(0),fmcTrackId(0),fmcSecId(0), fmcHitId(0),fmcEventId(0),fmother(0)
{;}

PndTpcSignal::PndTpcSignal(double T, 
		     double Amp, 
		     unsigned int PadID, 
		     unsigned int EventID,
		     PndTpcAvalanche* Mo)
  : ft(T),famp(Amp),fpadId(PadID),fmcTrackId(0),fmcSecId(0),fmcHitId(0),fmcEventId(EventID),fmother(Mo)
{;}


unsigned int 
PndTpcSignal::mcTrackId() const {
  if(fmother==0) return fmcTrackId;
  return fmother->mcTrackId();
}

unsigned int 
PndTpcSignal::mcSecId() const {
  if(fmother==0) return fmcSecId;
  return fmother->mcSecId();
}

unsigned int
PndTpcSignal::mcHitId() const {
  if(fmother==0) return fmcHitId;
  return fmother->mcHitId();
}

McId
PndTpcSignal::mcId() const {
  return McId( mcEventId(),mcTrackId(),mcSecId() );
}

bool operator== (const PndTpcSignal& lhs, const PndTpcSignal& rhs) {
  return fabs(lhs.ft-rhs.ft)<1E-4 && lhs.fpadId==rhs.fpadId;
}

bool operator< (const PndTpcSignal& lhs, const PndTpcSignal& rhs) {
  return lhs.ft<rhs.ft;
}

std::ostream& operator<< (std::ostream& s, const PndTpcSignal& me){
  return s<<"PndTpcSignal:\n"
	  <<"     t="<<me.ft<<"\n"
	  <<"     Amp="<<me.famp<<"\n"
	  <<"     PadID="<<me.fpadId<<"\n";
}
