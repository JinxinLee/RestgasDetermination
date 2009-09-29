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
  : famp(0), ft(0), fpadID(0), ftlength(0)
{}

PndTpcDigi::PndTpcDigi(const PndTpcDigi& d)
  : famp(d.famp), ft(d.ft), fpadID(d.fpadID), ftlength(d.ftlength),
    fmcid(d.fmcid)
{}



PndTpcDigi::PndTpcDigi(const double Amp,
				 const double t,
				 const unsigned int PadID,
				 const McIdCollection& mcid)
  : famp(Amp), ft(t), fpadID(PadID), fmcid(mcid), ftlength(0)
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
