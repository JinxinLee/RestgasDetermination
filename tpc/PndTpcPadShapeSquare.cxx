//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPadShapeSquare
//      see PndTpcPadShapeSquare.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//
//
//-----------------------------------------------------------
// This Class' Header ------------------
#include "PndTpcPadShapeSquare.h"

// C/C++ Headers ----------------------
#include <cmath>
#include <exception>

// Collaborating Class Headers --------
#include "PndTpcPRLookupTable.h"

// Class Member definitions -----------

PndTpcPadShapeSquare::PndTpcPadShapeSquare(const double fd,
				     const unsigned int fID)
  :PndTpcAbsPadShape(fID),d(fd)
{
  lookupTable = new PndTpcPRLookupTable();
}

PndTpcPadShapeSquare::~PndTpcPadShapeSquare()
{
  //lookupTable will automaticly be deleted by ~PndTpcAbsPadShape()
}

void 
PndTpcPadShapeSquare::EvalBoundingRect(double& width, double& height,
				    const double angle) const
{
  width = std::fabs( d*std::cos(angle) - d*std::sin(angle));
  height = std::fabs( d*std::cos(angle) + d*std::sin(angle));
  if (width>height)
    height=width;
  else
    width=height;
}

bool
PndTpcPadShapeSquare::Contains(const double xp, const double yp) const
{
  return( std::fabs(xp)<=d &&
	  std::fabs(yp)<=d);
}

void
PndTpcPadShapeSquare::GetBoundaryPoint(const int index,
				double& x, double& y) const
{
  switch(index)
  {
  case 0: x=  (y= d/2); break;
  case 1: x= -(y= -d/2); break;
  case 2: x=  (y=-d/2); break;
  case 3: x= -(y=d/2); break;
  default: throw "index out of range";
  }
}

std::ostream& operator<< (std::ostream& s,
			 const PndTpcPadShapeSquare& me)
{
  return s <<"PndTpcPadShapeSquare\n"
           <<"ID="<<me.ID<<"\n"
           <<"lookupTable="<<me.lookupTable<<"\n"
           <<"d="<<me.d<<"\n";
  
}
