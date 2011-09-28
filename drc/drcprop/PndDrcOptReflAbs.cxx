// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiPerfect.h"
#include "PndDrcOptReflAbs.h"

#include "PndDrcOptReflAbs.h"
#include "PndDrcOptDev.h"

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
#include <iostream>
using std::cout;
//using std::cerr;
//using std::cin;
using std::endl;
//
//#include <valarray>
//using std::valarray;
//
//#include <fstream>
//using std::fstream;
//
//#include <string>
//using std::string;
//
//#include <list>
//using std::list;

//----------------------------------------------------------------------
PndDrcOptReflAbs::PndDrcOptReflAbs()
{
  fVerbosity      = 0;
  fPolDir         = Drc::PolDirBoth;
}
//----------------------------------------------------------------------
PndDrcOptReflAbs::~PndDrcOptReflAbs()
{
  
}
//----------------------------------------------------------------------
void PndDrcOptReflAbs::Copy(const PndDrcOptReflAbs& s)
{
  fVerbosity   = s.fVerbosity;
  fPolDir      = s.fPolDir;
  
}
//----------------------------------------------------------------------
PndDrcOptReflAbs::PndDrcOptReflAbs(const PndDrcOptReflAbs& s)
{
  Copy(s);
}
//----------------------------------------------------------------------
PndDrcOptReflAbs& PndDrcOptReflAbs::operator=(const PndDrcOptReflAbs& s)
{
  if (&s != this) Copy(s);
  return *this;
}
