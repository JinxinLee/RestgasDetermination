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
  : _mceventID(0), _mctrackID(0), _weight(1)
{;}

McId::McId(const unsigned int eventid,
	   const unsigned int trackid,
	   const double weight)
  : _mceventID(eventid), _mctrackID(trackid), _weight(weight)
{;}

McId::~McId(){}

bool 
operator== (const McId& lhs, const McId& rhs){
  return lhs._mceventID==rhs._mceventID &&
    lhs._mctrackID==rhs._mctrackID;
}

bool 
operator< (const McId& lhs, const McId& rhs){
  return lhs._mceventID<rhs._mceventID ||
    (lhs._mceventID==rhs._mceventID && lhs._mctrackID<rhs._mctrackID);
}



std::ostream& operator<< (std::ostream& s, const McId& me){
  s << "McID: \n"
    << "  Event "<<me._mceventID<<"\n"
    << "  Track "<<me._mctrackID<<"\n"
    << "  weight "<<me._weight;
  return s;
}
