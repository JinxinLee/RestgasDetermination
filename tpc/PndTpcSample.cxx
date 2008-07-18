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
  : _amp(0),_t(0),_padId(0)
{;}

PndTpcSample::PndTpcSample( const int t, 
		      const int Amp, 
		      const unsigned int PadID,
		      const McIdCollection& mcid)
  : _amp(Amp),_t(t),_padId(PadID),_mcId(mcid)
{;}

PndTpcSample::~PndTpcSample()
{
}


bool 
operator== (const PndTpcSample& lhs, const PndTpcSample& rhs){
  return lhs._t==rhs._t && lhs._amp==rhs._amp && lhs._padId==rhs._padId;
}

bool 
operator< (const PndTpcSample& lhs, const PndTpcSample& rhs){
  return lhs._t<rhs._t;
}

std::ostream& operator<< (std::ostream& s, const PndTpcSample& me){
  s << "PndTpcSample:\n"
    << "   t    ="<<me._t<<"\n"
    << "   Amp  ="<<me._amp<<"\n"
    << "   PadID="<<me._padId<<"\n";
  return s;
}
