// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptReflSurf.h"

//#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//
//#include "TObject.h"
//#include "TVector3.h"
//#include "TRandom.h"
//
//#include "Math/Vector3D.h"
//using ROOT::Math::XYZVector;
//
//#include "Math/Point3D.h"
//using ROOT::Math::XYZPoint;
//
//#include "Math/Transform3D.h"
//using ROOT::Math::Transform3D;
//
//#include "Math/RotationX.h"
//using ROOT::Math::RotationX;
//#include "Math/RotationY.h"
//using ROOT::Math::RotationY;
//#include "Math/RotationZ.h"
//using ROOT::Math::RotationZ;
//#include "Math/Rotation3D.h"
//using ROOT::Math::Rotation3D;
//
//#include <iostream>
//using std::cout;
//using std::cerr;
//using std::cin;
//using std::endl;
//
//#include <cmath>


//----------------------------------------------------------------------
PndDrcOptReflSurf::PndDrcOptReflSurf() 
{
  fProb = 1;
}
//----------------------------------------------------------------------
PndDrcOptReflSurf* PndDrcOptReflSurf::Clone() const
{
  return new PndDrcOptReflSurf(*this);
}
//----------------------------------------------------------------------
const Drc::Reflectivity PndDrcOptReflSurf::Query(const PndDrcPhoton&    ph,
						 const XYZVector        normal,
						 const double           n_next,
						 const Drc::ReflDir     direction) 
  const
{
  double ran = fRan.Uniform(0.0,1.0);
 

  if (ran<fProb)
    {
      return Drc::ReflRefracted;
    }
  else
    {
      return Drc::ReflAbsorbed;
    }
}
