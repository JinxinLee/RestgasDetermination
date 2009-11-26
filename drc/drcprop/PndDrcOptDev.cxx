// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptDev.h"

//#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
#include "PndDrcSurfAbs.h"
//#include "PndDrcOptMatAbs.h"
//
//#include "TObject.h"
//#include "TVector3.h"
//#include "TRandom.h"
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
//
//#include <cmath>

//----------------------------------------------------------------------
PndDrcOptDev::PndDrcOptDev()
{
  fCopyNumber  = 0;
  fName        = "unnamed PndDrcOptDev";
  fPhotonTrace = false;
  fVerbosity   = 0;
}
//----------------------------------------------------------------------
void PndDrcOptDev::Copy(const PndDrcOptDev& d)
{
  fCopyNumber        = d.fCopyNumber;
  fName              = d.fName;
  fVerbosity         = d.fVerbosity;
  fPhotonTrace       = d.fPhotonTrace;
  fPhotonTraceStream = d.fPhotonTraceStream;

  fListSurf.clear();
  list<PndDrcSurfAbs*>::const_iterator kSurf;
  for(kSurf=d.fListSurf.begin(); kSurf != d.fListSurf.end(); ++kSurf) 
    {
      const PndDrcSurfAbs* tmp = (*kSurf);
      PndDrcSurfAbs* tmp1 = tmp->Clone();
      fListSurf.push_back(tmp1);
    }
}
//----------------------------------------------------------------------
PndDrcOptDev::PndDrcOptDev(const PndDrcOptDev& d)
{
  if (d.fVerbosity>=1) cout<<"  PndDrcOptDev::PndDrcOptDev"
			    <<"(const PndDrcOptDev&) name,copy: "
			    <<d.fName<<" "<<d.fCopyNumber<<endl;
  Copy(d);
}
//----------------------------------------------------------------------
PndDrcOptDev& PndDrcOptDev::operator=(const PndDrcOptDev& d)
{
  if (d.fVerbosity>=1) cout<<"  PndDrcOptDev::operator="
			    <<"(const PndDrcOptDev&) name,copy: "
			    <<d.fName<<" "<<d.fCopyNumber<<endl;
  if (&d != this) Copy(d);
  return *this;

}
//----------------------------------------------------------------------
PndDrcOptDev::~PndDrcOptDev()
{
  list<PndDrcSurfAbs*>::const_iterator kSurf;

  for(kSurf=fListSurf.begin(); kSurf != fListSurf.end(); ++kSurf) 
    {
      delete (*kSurf);
    }
}
//----------------------------------------------------------------------
void PndDrcOptDev::SetCopyNumber(int i)
{
  fCopyNumber = i;

  list<PndDrcSurfAbs*>::const_iterator kSurf;

  for(kSurf=fListSurf.begin(); kSurf != fListSurf.end(); ++kSurf) 
    {
      (*kSurf)->SetCopyNumber(i);
   }


}
//----------------------------------------------------------------------
void PndDrcOptDev::AddSurface(const PndDrcSurfAbs& surf)
{
  if (fVerbosity>=1) cout<<"  PndDrcOptDev::addSurface to "<<Name()<<":"
			    <<surf.Name()<<" "<<surf.CopyNumber()<<endl;
  PndDrcSurfAbs* tmp = surf.Clone();

  fListSurf.push_back(tmp);
}
//----------------------------------------------------------------------
void PndDrcOptDev::Print(fstream& stream) const
{
  list<PndDrcSurfAbs*>::const_iterator kSurf;

  for(kSurf=fListSurf.begin(); kSurf != fListSurf.end(); ++kSurf) 
    {
      if (fVerbosity>=3) cout<<" PndDrcOptDev::print: set print flag for "
			      <<(*kSurf)->Name()<<" "<<(*kSurf)->CopyNumber()<<endl;
      (*kSurf)->Print(stream);
   }

  // prepare tracing of photons
  fPhotonTrace = true;
  fPhotonTraceStream = &stream;
}
//----------------------------------------------------------------------
PndDrcOptMatAbs& PndDrcOptDev::OptMaterial() const
{
  static PndDrcOptMatAbs* dummy = 0;
  //  //cerr<<" *** PndDrcOptDev::optMaterial: This function must be overloaded when\n";
  //cerr<<"                             using volumes with refraction. Abort.\n";
  //exit(EXIT_FAILURE);
  return *dummy;
}
//----------------------------------------------------------------------
void PndDrcOptDev::AddTransform(const Transform3D& trans)
{
  list<PndDrcSurfAbs*>::const_iterator kSurf;
	      
  for (kSurf=fListSurf.begin(); 
       kSurf != fListSurf.end(); 
       ++kSurf)
    {
      const_cast<PndDrcSurfAbs*>(*kSurf)->AddTransform(trans);
    }

}

//----------------------------------------------------------------------
void PndDrcOptDev::Print()
{
  list<PndDrcSurfAbs*>::const_iterator kSurf;
	    
  for (kSurf=fListSurf.begin(); 
       kSurf != fListSurf.end(); 
       ++kSurf)
    {
      cout<<"\t\t\t"<<std::hex<<(long int)(*kSurf)<<std::dec<<" "<<(*kSurf)->Name()<<endl;
    }
}
//----------------------------------------------------------------------
void PndDrcOptDev::SetPrintColor(int col)
{
  list<PndDrcSurfAbs*>::const_iterator kSurf;
	    
  for (kSurf=fListSurf.begin(); 
       kSurf != fListSurf.end(); 
       ++kSurf)
    {
      (*kSurf)->SetPrintColor(col);
    }

}
//----------------------------------------------------------------------
void PndDrcOptDev::SetFresnel(bool flag)
{
  list<PndDrcSurfAbs*>::const_iterator kSurf;
	    
  for (kSurf=fListSurf.begin(); 
       kSurf != fListSurf.end(); 
       ++kSurf)
    {
      (*kSurf)->SetFresnel(flag);
    }

}
