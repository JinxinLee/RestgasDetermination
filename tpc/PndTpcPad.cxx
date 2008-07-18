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
  : _x(0),_y(0),_angle(0),_width(0),_height(0),_shape(0),_sectorId(0),_id(0)
{;}

PndTpcPad::PndTpcPad(const double x,
	       const double y,
	       const double angle,
	       PndTpcAbsPadShape* const shape,
	       const unsigned int sectorID,
	       const unsigned int ID)
  : _x(x),_y(y),_angle(angle),_shape(shape),_sectorId(sectorID),_id(ID)
{
  EvalBoundingRect();
}

bool
PndTpcPad::Contains(const double x, const double y) const
{
  if (_shape == 0)
    Fatal("PndTpcPad::Contains","No AbsPadShapePolygon assigned.");
  double xSh = x;
  double ySh = y;
  ToShapeCoord(xSh, ySh);
  return(_shape->Contains(xSh, ySh));
}

bool
PndTpcPad::CircleIntersection(const double x, const double y,
			   const double r) const
{
  double dx=std::fabs(_x-x); double dy=std::fabs(_y-y);
  double hw = 0.5*_width; double hh = 0.5*_height;
  //most Pads are far away
  if (dx > hw+r || dy > hh+r )
    return(false);
  return ( (dx < hw+r && dy < hh) || (dx < hw && dy < hh+r) ||
	   (dx-hw)*(dx-hw)+(dy-hh)*(dy-hh) < r*r );
}

double
PndTpcPad::GetValue(const double x, const double y) const
{
  if (_shape == 0)
    Fatal("PndTpcPad::GetValue","No AbsPadShape assigned.");
  double xSh = x;
  double ySh = y;
  ToShapeCoord(xSh, ySh);
  return(_shape->GetValue(xSh, ySh));
}

int
PndTpcPad::GetNBoundaryPoints() const
{
  if (_shape == 0)
    Fatal("PndTpcPad::GetNBoundaryPoints","No AbsPadShape assigned.");
  return(_shape->GetNBoundaryPoints());
}

void
PndTpcPad::GetBoundaryPoint(const int index, double& x, double& y) const
{
  if (_shape == 0)
    Fatal("PndTpcPad::GetBoundaryPoint","No AbsPadShape assigned.");
   _shape->GetBoundaryPoint(index, x, y);
   ToPlaneCoord(x, y);
}

bool operator== (const PndTpcPad& lhs, const PndTpcPad& rhs)
{
  //true if same position, angle and shape
  double dx=lhs._x-rhs._x;
  double dy=lhs._y-rhs._y;
  double da=lhs._angle-rhs._angle;
  return lhs._shape==rhs._shape &&
    (dx*dx<1E-10) && (dy*dy<1E-10) && (da*da<1E-10);
}

bool operator< (const PndTpcPad& lhs, const PndTpcPad& rhs){
  //true if lhs below (y) or left (x) of rhs
  double dy=lhs._y-rhs._y;
  double dx=lhs._x-rhs._x;
  bool flag=(dy*dy<1E-10) && (dx<-1E-5);
    return (dy<-1E-5) || flag;
}

std::ostream& operator<< (std::ostream& s, const PndTpcPad& me){
  return s << "PndTpcPad\n"
           << "sectorID="<<me._sectorId<<"\n"
	   << "ID="<<me._id<<"\n"
	   << "x="<<me._x<<"    y="<<me._y<<"    angle="<<me._angle<<"\n"
           << "shape="<<me._shape<<"\n";
}

void 
PndTpcPad::ToShapeCoord(double& xP, double& yP) const
{
  double xori=xP-_x;
  double yori=yP-_y;
  xP = xori*std::cos(_angle) + yori*std::sin(_angle);
  yP = -xori*std::sin(_angle) + yori*std::cos(_angle);
}

void 
PndTpcPad::ToPlaneCoord(double& xP, double& yP) const
{
  double xori = xP*std::cos(-_angle) + yP*std::sin(-_angle);
  yP = -xori*std::sin(-_angle) + yP*std::cos(-_angle)+_y;
  xP = xori+_x;
}

void
PndTpcPad::EvalBoundingRect()
{
  if (_shape == 0)
  {
    _width = 0;
    _height =0;
    return;
  }
  _shape->EvalBoundingRect(_width, _height, _angle);
}

void
PndTpcPad::Draw(int c) const
{
  _shape->Draw(_x,_y,_angle, c);
}
