//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPad
//      see PndTpcPad.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Cristoforo Simonetto TUM
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcPad.h"

// C/C++ Headers ----------------------
#include <cmath>

// Collaborating Class Headers --------
#include "PndTpcAbsPadShape.h"
#include "TError.h"

// Class Member definitions -----------

PndTpcPad::PndTpcPad()
  : fx(0),fy(0),fangle(0),fwidth(0),fheight(0),fshape(0),fsectorId(0),fid(0)
{;}

PndTpcPad::PndTpcPad(const double X,
	       const double Y,
	       const double Angle,
	       PndTpcAbsPadShape* const Shape,
	       const unsigned int sectorID,
	       const unsigned int ID)
  : fx(X),fy(Y),fangle(Angle),fshape(Shape),fsectorId(sectorID),fid(ID)
{
  EvalBoundingRect();
}

bool
PndTpcPad::Contains(const double X, const double Y) const
{
  if (fshape == 0)
    Fatal("PndTpcPad::Contains","No AbsPadShapePolygon assigned.");
  double xSh = X;
  double ySh = Y;
  ToShapeCoord(xSh, ySh);
  return(fshape->Contains(xSh, ySh));
}

bool
PndTpcPad::CircleIntersection(const double X, const double Y,
			   const double r) const
{
  double dx=std::fabs(fx-X); double dy=std::fabs(fy-Y);
  double hw = 0.5*fwidth; double hh = 0.5*fheight;
  //most Pads are far away
  if (dx > hw+r || dy > hh+r )
    return(false);
  return ( (dx < hw+r && dy < hh) || (dx < hw && dy < hh+r) ||
	   (dx-hw)*(dx-hw)+(dy-hh)*(dy-hh) < r*r );
}

double
PndTpcPad::GetValue(const double X, const double Y) const
{
  if (fshape == 0)
    Fatal("PndTpcPad::GetValue","No AbsPadShape assigned.");
  double xSh = X;
  double ySh = Y;
  ToShapeCoord(xSh, ySh);
  return(fshape->GetValue(xSh, ySh));
}

int
PndTpcPad::GetNBoundaryPoints() const
{
  if (fshape == 0)
    Fatal("PndTpcPad::GetNBoundaryPoints","No AbsPadShape assigned.");
  return(fshape->GetNBoundaryPoints());
}

void
PndTpcPad::GetBoundaryPoint(const int index, double& X, double& Y) const
{
  if (fshape == 0)
    Fatal("PndTpcPad::GetBoundaryPoint","No AbsPadShape assigned.");
   fshape->GetBoundaryPoint(index, X, Y);
   ToPlaneCoord(X, Y);
}

bool operator== (const PndTpcPad& lhs, const PndTpcPad& rhs)
{
  //true if same position, angle and shape
  double dx=lhs.fx-rhs.fx;
  double dy=lhs.fy-rhs.fy;
  double da=lhs.fangle-rhs.fangle;
  return lhs.fshape==rhs.fshape &&
    (dx*dx<1E-10) && (dy*dy<1E-10) && (da*da<1E-10);
}

bool operator< (const PndTpcPad& lhs, const PndTpcPad& rhs){
  //true if lhs below (y) or left (x) of rhs
  double dy=lhs.fy-rhs.fy;
  double dx=lhs.fx-rhs.fx;
  bool flag=(dy*dy<1E-10) && (dx<-1E-5);
    return (dy<-1E-5) || flag;
}

std::ostream& operator<< (std::ostream& s, const PndTpcPad& me){
  return s << "PndTpcPad\n"
           << "sectorID="<<me.fsectorId<<"\n"
	   << "ID="<<me.fid<<"\n"
	   << "x="<<me.fx<<"    y="<<me.fy<<"    angle="<<me.fangle<<"\n"
           << "shape="<<me.fshape<<"\n";
}

void 
PndTpcPad::ToShapeCoord(double& xP, double& yP) const
{
  double xori=xP-fx;
  double yori=yP-fy;
  xP = xori*std::cos(fangle) + yori*std::sin(fangle);
  yP = -xori*std::sin(fangle) + yori*std::cos(fangle);
}

void 
PndTpcPad::ToPlaneCoord(double& xP, double& yP) const
{
  double xori = xP*std::cos(-fangle) + yP*std::sin(-fangle);
  yP = -xori*std::sin(-fangle) + yP*std::cos(-fangle)+fy;
  xP = xori+fx;
}

void
PndTpcPad::EvalBoundingRect()
{
  if (fshape == 0)
  {
    fwidth = 0;
    fheight =0;
    return;
  }
  fshape->EvalBoundingRect(fwidth, fheight, fangle);
}

void
PndTpcPad::Draw(int c) const
{
  fshape->Draw(fx,fy,fangle, c);
}
