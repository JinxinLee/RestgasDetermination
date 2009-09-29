//-----------------------------------------------------------
//
// Description:
//      Implementation of class PndTpcDevmapCyl
//      see PndTpcDevmapCyl.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM         (original author)
//      Felix Boehmer           TUM
//
//-----------------------------------------------------------

#include <iostream>
#include "TMath.h"
// This Class' Header ------------------
#include "PndTpcDevmapCyl.h"


// Class Member definitions -----------


PndTpcDevmapCyl::PndTpcDevmapCyl(const char* const fileName, double vDrift)
  :PndTpcFieldCylGrid<TVector3>(TVector3(0.,0.,0.),
			     0.,0.,0.,0.), floaded(false)
{

  //TODO: redesign, get vDrift from param management
  fvDrift = vDrift;
  loader = new PndTpcDevmapCylLoader(this, fileName);
  if(loader->load()==0)floaded=true;
  if (!floaded)
    Error("PndTpcDevmapCyl::PndTpcDevmapCyl()",
	  "Failed to load field data - check deviation-map file!");
  evalMaxPoint();
}

PndTpcDevmapCyl::~PndTpcDevmapCyl()
{delete loader;}

void
PndTpcDevmapCyl::print(std::ostream& s) const
{
  s << "PndTpcDevmapCyl\n";
  PndTpcFieldCylGrid<TVector3>::print(s); 
}

TVector3
PndTpcDevmapCyl::value(const TVector3& point) const
{
  if(!floaded)return TVector3(0,0,0);
  //std::cout<<"PndTpcDevmapCyl::value("<<point.X()<<","<<point.Y()<<","
  //	   <<point.Z()<<")"<<std::endl;
  if (pointOk(point) == true)
  {
    return PndTpcFieldCylGrid<TVector3>::value(point);    
  }
  else //return value for boundary point next to point 
  {
    //Error("PndTpcDevmapCyl::value()", "Point outside of tpc volume!");
    TVector3 boundPoint = point - frelPosition;
    double r2 = boundPoint.X()*boundPoint.X()+boundPoint.Y()*boundPoint.Y();
    if (r2 >= maxR()*maxR())
    {
      boundPoint.SetX(boundPoint.X()*maxR()/TMath::Sqrt(r2+1e-10));//1e-10 guarantees, that the point is inside volume
      boundPoint.SetY(boundPoint.Y()*maxR()/TMath::Sqrt(r2+1e-10));
    }
    else if(r2 < minR()*minR())
    {
      boundPoint.SetX(boundPoint.X()*minR()/sqrt(r2-1e-10));
      boundPoint.SetY(boundPoint.Y()*minR()/sqrt(r2-1e-10));
    }
    if (boundPoint.Z() >= maxZ())
      boundPoint.SetZ(maxZ()-1e-10);
    else if (boundPoint.Z() < minZ())
      boundPoint.SetZ(minZ());
    //recursive: guarantees, that the point is inside volume
    return value(boundPoint + frelPosition);
  }
}
