//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class McId
//      see McId.h for details
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
#include "McId.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------

ClassImp(McId);

McId::McId()
  : fmceventID(0), fmctrackID(0), fweight(1)
{;}

McId::McId(const unsigned int eventid,
	   const unsigned int trackid,
	   const double weight)
  : fmceventID(eventid), fmctrackID(trackid), fweight(weight)
{;}

McId::~McId(){}

bool 
operator== (const McId& lhs, const McId& rhs){
  return lhs.fmceventID==rhs.fmceventID &&
    lhs.fmctrackID==rhs.fmctrackID;
}

bool 
operator< (const McId& lhs, const McId& rhs){
  return lhs.fmceventID<rhs.fmceventID ||
    (lhs.fmceventID==rhs.fmceventID && lhs.fmctrackID<rhs.fmctrackID);
}



std::ostream& operator<< (std::ostream& s, const McId& me){
  s << "McID: \n"
    << "  Event "<<me.fmceventID<<"\n"
    << "  Track "<<me.fmctrackID<<"\n"
    << "  weight "<<me.fweight;
  return s;
}
