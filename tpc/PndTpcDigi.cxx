//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcDigi
//      see PndTpcDigi.hh for details
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
#include "PndTpcDigi.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------

PndTpcDigi::PndTpcDigi()
  : _amp(0), _t(0), _padID(0), _tlength(0)
{}


PndTpcDigi::PndTpcDigi(const double Amp,
				 const double t,
				 const unsigned int PadID,
				 const McIdCollection& mcid)
  : _amp(Amp), _t(t), _padID(PadID), _mcid(mcid), _tlength(0)
{}

bool 
operator==(const PndTpcDigi& lhs, const PndTpcDigi& rhs)
{
  return lhs._amp==rhs._amp && lhs._t==rhs._t && lhs._padID==rhs._padID;
}

bool
operator<(const PndTpcDigi& lhs, const PndTpcDigi& rhs)
{
  return lhs._t<rhs._t;
}

std::ostream& operator<< (std::ostream& s, const PndTpcDigi& me){
  s << "PndTpcDigi:\n"
    << "    Amp   ="<<me._amp<<"\n"
    << "    t     ="<<me._t<<"\n"
    << "    PadID ="<<me._padID<<"\n";
  return s;
}


ClassImp(PndTpcDigi)
