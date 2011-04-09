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
  : famp(0), ft(0), fpadID(0), ftlength(0), findex(0)
{}


PndTpcDigi::PndTpcDigi(const double Amp,
				 const double T,
				 const unsigned int PadID,
				 const McIdCollection& mcid, const unsigned int caindex)
  : famp(Amp), ft(T), fpadID(PadID), fmcid(mcid), ftlength(0), findex(caindex)
{}

bool 
operator==(const PndTpcDigi& lhs, const PndTpcDigi& rhs)
{
  return lhs.famp==rhs.famp && lhs.ft==rhs.ft && lhs.fpadID==rhs.fpadID;
}

bool
operator<(const PndTpcDigi& lhs, const PndTpcDigi& rhs)
{
  return lhs.ft<rhs.ft;
}

std::ostream& operator<< (std::ostream& s, const PndTpcDigi& me){
  s << "PndTpcDigi:\n"
    << "    Amp   ="<<me.famp<<"\n"
    << "    t     ="<<me.ft<<"\n"
    << "    PadID ="<<me.fpadID<<"\n";
  return s;
}


ClassImp(PndTpcDigi)
