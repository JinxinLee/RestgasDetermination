// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptReflSilver.h"

#include "PndDrcSurfAbs.h"
#include "PndDrcPhoton.h"
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
//
//#include <iostream>
//using std::cout;
//using std::cerr;
//using std::cin;
//using std::endl;
//
//#include <cmath>



//----------------------------------------------------------------------
PndDrcOptReflSilver* PndDrcOptReflSilver::Clone() const
{
  return new PndDrcOptReflSilver(*this);
}
//----------------------------------------------------------------------
const Drc::Reflectivity PndDrcOptReflSilver::Query(const PndDrcPhoton&    ph,
						   const XYZVector        normal,
						   const double           n_next,
						   const Drc::ReflDir     direction) 
const
{
  double lambda = ph.Wavelength();

  // assume reflectivity of 0.96 at 400 to 0.98 at 1000nm

  double ref_prob = 0.96 + (lambda-400.0)/600.0 * 0.02;
  if (ref_prob>0.98) ref_prob=0.98;

  double ran = gRandom->Uniform(0.0,1.0);

  if (ran>ref_prob || lambda<400)
    {
      return Drc::ReflAbsorbed;
    }
  else
    {
      return Drc::ReflReflected;
    }

}
