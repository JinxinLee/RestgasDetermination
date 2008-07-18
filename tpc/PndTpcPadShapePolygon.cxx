//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPadShapePolygon
//      see PndTpcPadShapePolygon.hh for details
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
#include "PndTpcPadShapePolygon.h"

// C/C++ Headers ----------------------
#include <cmath>

// Collaborating Class Headers --------
#include "TError.h"
#include "PndTpcPRLookupTable.h"

// Class Member definitions -----------

PndTpcPadShapePolygon::PndTpcPadShapePolygon(const std::vector<double>& x,
				       const std::vector<double>& y,
				       const unsigned int _ID)
  :PndTpcAbsPadShape(_ID)
{
  if (x.size() != y.size())
    Fatal("PndTpcPadShapePolygon::PndTpcPadShapePolygon","The size of x coordinates is different from the size of y coordinates. Constructing a PndTpcPadShapePolygon is impossible.");
  if (x.size() < 3)
    Fatal("PndTpcPadShapePolygon::PndTpcPadShapePolygon:","It is not possible to build a polygon with only %i points.",x.size());
  xCoord = x;
  yCoord = y;
  xCoord.push_back(x[0]);
  yCoord.push_back(y[0]);
  if (!Contains(0,0))
    Fatal("PndTpcPadShapePolygon::PndTpcPadShapePolygon","The origin is outside the polygon. (forbidden)");
  lookupTable = new PndTpcPRLookupTable();
}

PndTpcPadShapePolygon::~PndTpcPadShapePolygon()
{
  //lookupTable will automaticly be deleted by ~PndTpcAbsPadShape()
}

void 
PndTpcPadShapePolygon::EvalBoundingRect(double& width, double& height,
				     const double angle) const
{
  double xmax=0;
  double ymax=0;
  for(int i=0; i<xCoord.size()-1;i++)
  {
    double xSh = std::fabs( xCoord[i]*std::cos(angle) -
			    yCoord[i]*std::sin(angle) );
    double ySh = std::fabs( xCoord[i]*std::sin(angle) +
			    yCoord[i]*std::cos(angle) );
    if (xSh > xmax)
      xmax = xSh;
    if (ySh > ymax)
      ymax = ySh;
  }
  width=2*xmax;
  height=2*ymax;
}
bool
PndTpcPadShapePolygon::Contains(const double xp, const double yp) const
{
  //Is (xp, yp) inside the Polygon?
  double xleft;
  int left = 0;
  for (int i=0;i<xCoord.size()-1;i++)
  {
    if (yCoord[i] == yCoord[i+1])
    {
      if (std::fabs(yp-yCoord[i])<1E-5 &&
	  ( (xp-xCoord[i]>-1E-5 && xp-xCoord[i+1]<1E-5) ||
	    (xp-xCoord[i]<1E-5 && xp-xCoord[i+1]>-1E-5) ))
	return(true);//on the boundary
      continue;
    }
    if (yp <= yCoord[i] && yp <= yCoord[i+1]) continue;
    if (yCoord[i] < yp && yCoord[i+1] < yp) continue;
    xleft = xCoord[i] + (yp-yCoord[i])*
      (xCoord[i+1]-xCoord[i])/(yCoord[i+1]-yCoord[i]);
    if (std::fabs(xleft-xp) < 1E-5) return(true);//on the boundary
    if (xp < xleft) left++;
  }
  if (left%2) return true;
  return false;
}

void
PndTpcPadShapePolygon::GetBoundaryPoint(const int index,
				double& x, double& y) const
{
  x = xCoord.at(index);
  y = yCoord.at(index);
}

std::ostream& operator<< (std::ostream& s,
			  const PndTpcPadShapePolygon& me)
{
  s <<"PndTpcPadShapePolygon\n"
    <<"ID="<<me.ID<<"\n"
    <<"lookupTable="<<me.lookupTable<<"\n";
  for (int i=0; i<me.xCoord.size(); i ++)
  {
    s <<"("<<me.xCoord[i]<<","<<me.yCoord[i]<<")";
    if (i%3 == 2)
      s <<"\n";
  }
  s <<"\n";
  return s;
}


