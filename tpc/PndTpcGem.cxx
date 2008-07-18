//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcGem
//      see PndTpcGem.h for details
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
#include "PndTpcGem.h"

// C/C++ Headers ----------------------
#include <cmath>

// Collaborating Class Headers --------


// Class Member definitions -----------

PndTpcGem::PndTpcGem()
  : _gain(0),_spread(0),_cloudShape("CloudShape","gaus(0)",-99,99)
{;}

PndTpcGem::PndTpcGem(const double Gain,
		       const double Spread)
  : _gain(Gain),_spread(Spread),_cloudShape("CloudShape","gausn(0)",-99,99)
{
  _cloudShape.SetParameter(0,1); // normalized gauss
  _cloudShape.SetParameter(1,0);
  _cloudShape.SetParameter(2,Spread);
}

PndTpcGem::~PndTpcGem(){}


bool operator== (const PndTpcGem& lhs, const PndTpcGem& rhs)
{
  bool bG=fabs(lhs._gain-rhs._gain)<1E-3;
  bool bS=fabs(lhs._spread-rhs._spread)<1E-8;
  return bG && bS;
}

std::ostream& operator<< (std::ostream& s, const PndTpcGem& r){
  return s << "\n---------------------------------------- \n "
	   << "PndTpcGem:\n"
           << "   Gain = "<<r._gain<<"\n"
           << "   Spread = "<<r._spread<<"\n"
	   << "---------------------------------------- \n ";
}
