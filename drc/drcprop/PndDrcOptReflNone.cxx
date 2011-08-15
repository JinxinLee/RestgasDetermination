// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptReflNone.h"

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
PndDrcOptReflNone* PndDrcOptReflNone::Clone() const
{
  return new PndDrcOptReflNone(*this);
}
//----------------------------------------------------------------------
const Drc::Reflectivity PndDrcOptReflNone::Query(const PndDrcPhoton&    ph,
						 const XYZVector        normal,
						 const double           n_next,
						 const Drc::ReflDir     direction) const
{
      return Drc::ReflAbsorbed;
}
