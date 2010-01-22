// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2010
//-----------------------------------------------------
#include "TROOT.h"
#include "TRint.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TRotation.h"
#include "TFile.h"
#include "TH1F.h"
#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;

#include "Math/Point3D.h"
using ROOT::Math::XYZPoint;

#include "Math/Transform3D.h"
using ROOT::Math::Transform3D;

#include "Math/RotationX.h"
using ROOT::Math::RotationX;
#include "Math/RotationY.h"
using ROOT::Math::RotationY;
#include "Math/RotationZ.h"
using ROOT::Math::RotationZ;
#include "Math/Rotation3D.h"
using ROOT::Math::Rotation3D;



#include "PndDrcOptBurle.h"
#include "PndDrcOptBrik.h"
//#include "PndDrcPhoton.h"
#include "PndDrcOptReflAbs.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcSurfAbs.h"
#include "PndDrcOptMatAbs.h"
#include "PndDrcOptMatLithotecQ0.h"

//#include "PndDrcOptDev.h"
//#include "PndDrcOptVol.h"
//#include "PndDrcUtil.h"
#include "PndDrcSurfPolyFlat.h"
#include <iostream>
using std::cout;
using std::cerr;
//using std::cin;
using std::endl;
//using std::numeric_limits;

PndDrcOptBurle::PndDrcOptBurle()
{
  double half_ext_x = 59.0/2;
  double half_ext_y = 59.0/2;
  double half_ext_z = 21.5/2;

  PndDrcOptReflNone refl_side;
  

  PndDrcOptBrik box(half_ext_x,half_ext_y,half_ext_z);
  box.SetName("housing");
  box.SetOptMaterial(PndDrcOptMatLithotecQ0());

  list< PndDrcSurfAbs*>  list_surf = box.SurfaceList();
  list<PndDrcSurfAbs*>::iterator ilist;
  for(ilist=list_surf.begin(); ilist != list_surf.end(); ++ilist) 
    {
      if ((*ilist)->Name()=="side6") (*ilist)->SetName("front");
      if ((*ilist)->Name()=="side1") (*ilist)->SetReflectivity(refl_side);
      if ((*ilist)->Name()=="side2") (*ilist)->SetReflectivity(refl_side);
      if ((*ilist)->Name()=="side3") (*ilist)->SetReflectivity(refl_side);
      if ((*ilist)->Name()=="side4") (*ilist)->SetReflectivity(refl_side);
      if ((*ilist)->Name()=="side5") (*ilist)->SetReflectivity(refl_side);
    }
  
  // add 64 pixels over 50x50 mm2
  double pw = 6.0;            // width mm
  double ps = (50.0-8*6.0)/7; // spacing
  
  for (int ix=0; ix<8; ix++)
    {
      for (int iy=0; iy<8; iy++)
	{
	  PndDrcSurfPolyFlat pixel;
	  //                                          y                         
	  //                                    p1---p2               
	  //                                    |     |
	  //                                 x  p4---p3
	  //                                                           
	  //                                   
	  XYZPoint p1(-25 + ix*(pw+ps),        -25.0 + iy*(pw+ps) + pw, half_ext_z-0.5);
	  XYZPoint p2(-25.0 + ix*(pw+ps) + pw, -25.0 + iy*(pw+ps) + pw, half_ext_z-0.5);
	  XYZPoint p3(-25.0 + ix*(pw+ps) + pw, -25.0 + iy*(pw+ps)     , half_ext_z-0.5);
	  XYZPoint p4(-25.0 + ix*(pw+ps),      -25.0 + iy*(pw+ps)     , half_ext_z-0.5);
	  
	  pixel.AddPoint(p1);
	  pixel.AddPoint(p2);
	  pixel.AddPoint(p3);
	  pixel.AddPoint(p4);
	  
	  pixel.SetPixel();
	  pixel.SetPrintColor(2);
	  box.AddSurface(pixel);
	  
	  
	}    
    }
  


  
  
  box.AddTransform(Transform3D(XYZVector(0,0,-half_ext_z))); // bar + 1/2 box

  AddDevice(box);
  



  
}
