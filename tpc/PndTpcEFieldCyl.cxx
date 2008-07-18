//-----------------------------------------------------------
// File and Version Information:
// 
//
// Description:
//      Implementation of class PndTpcEFieldCyl
//      see PndTpcEFieldCyl.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//      Felix Boehmer           TUM
//
//
//-----------------------------------------------------------



// This Class' Header ------------------
#include "PndTpcEFieldCyl.h"
#include "TMath.h"

// Class Member definitions -----------

PndTpcEFieldCyl::PndTpcEFieldCyl(const char* const fileName)
  : PndTpcFieldCylGrid<TVector3>(TVector3(0.,0.,0.),
			    0.,0.,0.,0.)
{
  loader = new PndTpcEFieldCylLoader(this, fileName);
  loader->load();
  if (_pGrid == 0)
    Fatal("PndTpcEFieldCyl::PndTpcEFieldCyl()",
	  "Failed to load field data - check E-field file!");
  evalMaxPoint();
}

PndTpcEFieldCyl::~PndTpcEFieldCyl()
{delete loader;} //PndTpcEFieldCyl has ownership of the fieldmap created by
                 //PndTpcEFieldCylLoader.


void
PndTpcEFieldCyl::print(std::ostream& s) const
{
  s << "PndTpcEFieldCyl\n";
  PndTpcFieldCylGrid<TVector3>::print(s);
}

TVector3
PndTpcEFieldCyl::value(const TVector3& point) const
{
  if (pointOk(point) == true)
    return PndTpcFieldCylGrid<TVector3>::value(point);
  else //return value for boundary point next to point 
  {
    Error("PndTpcEFieldCyl::value()", "Point outside of tpc volume!");
    TVector3 boundPoint = point - _relPosition;
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
    return value(boundPoint + _relPosition);
  }
}



