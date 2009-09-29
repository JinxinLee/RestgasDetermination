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
  : fgain(0),fspread(0)
{
  fcloudShape = new TF1("CloudShape","gaus(0)",-99,99);
}

PndTpcGem::PndTpcGem(const double Gain,
		       const double Spread)
  : fgain(Gain),fspread(Spread)
{
  fcloudShape = new TF1("CloudShape","gausn(0)",-99,99);
  fcloudShape->SetParameter(0,1); // normalized gauss
  fcloudShape->SetParameter(1,0);
  fcloudShape->SetParameter(2,Spread);
}

PndTpcGem::~PndTpcGem(){}


bool operator== (const PndTpcGem& lhs, const PndTpcGem& rhs)
{
  bool bG=fabs(lhs.fgain-rhs.fgain)<1E-3;
  bool bS=fabs(lhs.fspread-rhs.fspread)<1E-8;
  return bG && bS;
}

std::ostream& operator<< (std::ostream& s, const PndTpcGem& r){
  return s << "\n---------------------------------------- \n "
	   << "PndTpcGem:\n"
           << "   Gain = "<<r.fgain<<"\n"
           << "   Spread = "<<r.fspread<<"\n"
	   << "---------------------------------------- \n ";
}
