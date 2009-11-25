// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcSurfAbs.h"

//#include "PndDrcPhoton.h"
#include "PndDrcOptReflAbs.h"
//#include "PndDrcOptMatAbs.h"
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
PndDrcSurfAbs::PndDrcSurfAbs()
{
  fName           = "unnamed PndDrcSurfAbs";
  fVerbosity      = 0;
  fCoupledFlag    = false;
  fPrintColor     = 1;//black
  fReflectivity   = 0;
  fCopyNumber     = 0;
  fPixel          = false;
  fInternal       = false;
  fFresnel        = true;
}
//----------------------------------------------------------------------
PndDrcSurfAbs::~PndDrcSurfAbs()
{
  delete fReflectivity;
}
//----------------------------------------------------------------------
void PndDrcSurfAbs::Copy(const PndDrcSurfAbs& s)
{
  fCopyNumber  = s.fCopyNumber;              
  fCoupledFlag = false;                // no coupled lists

  if (s.fVerbosity>=5) cout<<"  PndDrcSurfAbs:copy: clear couplings."<<endl;  
  fCoupledSurfaceList.clear();         // since context changes
  fCoupledDeviceList.clear();

  fName        = s.fName;
  fVerbosity   = s.fVerbosity;
  fPrintColor  = s.fPrintColor;
  fReflectivity= 0;
  if (s.fReflectivity) fReflectivity = s.fReflectivity->Clone(); 
  fPixel       = s.fPixel;
  fInternal    = s.fInternal;
  fFresnel     = s.fFresnel;
}
//----------------------------------------------------------------------
PndDrcSurfAbs::PndDrcSurfAbs(const PndDrcSurfAbs& s)
{
  if (s.fVerbosity>=1) cout<<"  PndDrcSurfAbs::PndDrcSurfAbs"
			    <<"(const PndDrcSurfAbs&) name,copy: "
			    <<s.fName<<" "<<s.fCopyNumber<<endl;
  Copy(s);
}
//----------------------------------------------------------------------
PndDrcSurfAbs& PndDrcSurfAbs::operator=(const PndDrcSurfAbs& s)
{
  if (s.fVerbosity>=1) cout<<" PndDrcSurfAbs::operator="
			    <<"(const PndDrcSurfAbs&) name,copy: "
			    <<s.fName<<" "<<s.fCopyNumber<<endl;
  if (&s != this) Copy(s);
  return *this;
}
//----------------------------------------------------------------------
void PndDrcSurfAbs::SetCoupled(PndDrcOptDev*  dev,
			    PndDrcSurfAbs* surf)
{
  if (Verbosity()>=3)
    {
      cout<<"    PndDrcSurfAbs::setCoupled"<<endl;
      {
	if (Verbosity()>=4) cout<<"     set coupling for "<<Name()<<" to dev,surf= "
				<<dev->Name()<<","
				<<surf->Name()<<endl;
      }
    }
  fCoupledDeviceList.push_back(dev);
  fCoupledSurfaceList.push_back(surf);
  fCoupledFlag = true;
}
//----------------------------------------------------------------------
void PndDrcSurfAbs::SetReflectivity(const PndDrcOptReflAbs& refl)
{
  if (Verbosity()>=3) cout<<"    PndDrcSurfAbs::setReflectivity() name="<<fName<<endl;
  if (&refl==0) return;
  delete fReflectivity;              
  fReflectivity = refl.Clone();    
}
//----------------------------------------------------------------------
void PndDrcSurfAbs::ClearReflectivity()
{
  if (Verbosity()>=3) 
    cout<<"    PndDrcSurfAbs::clearReflectivity() name="<<fName<<endl;
  if (fReflectivity) delete fReflectivity;              
  fReflectivity = 0;    
}
