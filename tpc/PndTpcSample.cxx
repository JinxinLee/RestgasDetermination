//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSample
//      see PndTpcSample.hh for details
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
#include "PndTpcSample.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------

ClassImp(PndTpcSample)


PndTpcSample::PndTpcSample()
  : famp(0),ft(0),fpadId(0)
{;}

PndTpcSample::PndTpcSample( const int t, 
		      const int Amp, 
		      const unsigned int PadID,
		      const McIdCollection& mcid)
  : famp(Amp),ft(t),fpadId(PadID),fmcId(mcid)
{;}

PndTpcSample::~PndTpcSample()
{
}


bool 
operator== (const PndTpcSample& lhs, const PndTpcSample& rhs){
  return lhs.ft==rhs.ft && lhs.famp==rhs.famp && lhs.fpadId==rhs.fpadId;
}

bool 
operator< (const PndTpcSample& lhs, const PndTpcSample& rhs){
  return lhs.ft<rhs.ft;
}

std::ostream& operator<< (std::ostream& s, const PndTpcSample& me){
  s << "PndTpcSample:\n"
    << "   t    ="<<me.ft<<"\n"
    << "   Amp  ="<<me.famp<<"\n"
    << "   PadID="<<me.fpadId<<"\n";
  return s;
}
