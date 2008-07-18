// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptTrd2.h"

//#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//#include "PndDrcSurfAbs.h"
//#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//#include "PndDrcOptVol.h"
//#include "PndDrcUtil.h"
#include "PndDrcSurfPolyFlat.h"
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
//using std::cout;
using std::cerr;
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
//#include <vector>
//using std::vector;
//
//#include <limits>
//using std::numeric_limits;



PndDrcOptTrd2::PndDrcOptTrd2(double dx1, double dx2,
		       double dy1, double dy2, 
		       double dz)
{
  SetSize(dx1,dx2,dy1,dy2,dz);
}
//----------------------------------------------------------------------
void PndDrcOptTrd2::SetSize(double dx1, double dx2,
			 double dy1, double dy2, 
			 double dz)
{
  XYZPoint p1( dx1, dy1,-dz);        //        p5----------p8
  XYZPoint p2( dx1,-dy1,-dz);        //       /|           /|
  XYZPoint p3(-dx1,-dy1,-dz);        //      / |          / |
  XYZPoint p4(-dx1, dy1,-dz);        //     /  p6--------/-p7               Y
  //                                     /   /        /  /                ^  Z
  //                                    /   /        /  /                 | /
  //                                   /   /        /  /                  |/
  //                                  /   /        /  /             X <---0
  XYZPoint p5( dx2, dy2, dz);        //p1----------p4  /
  XYZPoint p6( dx2,-dy2, dz);        //| /          | /
  XYZPoint p7(-dx2,-dy2, dz);        //|/           |/
  XYZPoint p8(-dx2, dy2, dz);        //p2----------p3 

  PndDrcSurfPolyFlat a1;
  a1.AddPoint(p1);
  a1.AddPoint(p2);
  a1.AddPoint(p3);
  a1.AddPoint(p4);
  a1.SetName("side1");

  PndDrcSurfPolyFlat a2;
  a2.AddPoint(p4);
  a2.AddPoint(p3);
  a2.AddPoint(p7);
  a2.AddPoint(p8);
  a2.SetName("side3");

  PndDrcSurfPolyFlat a3;
  a3.AddPoint(p2);
  a3.AddPoint(p3);
  a3.AddPoint(p7);
  a3.AddPoint(p6);
  a3.SetName("side2");

  PndDrcSurfPolyFlat a4;
  a4.AddPoint(p1);
  a4.AddPoint(p2);
  a4.AddPoint(p6);
  a4.AddPoint(p5);
  a4.SetName("side5");

  PndDrcSurfPolyFlat a5;
  a5.AddPoint(p1);
  a5.AddPoint(p4);
  a5.AddPoint(p8);
  a5.AddPoint(p5);
  a5.SetName("side4");
  
  PndDrcSurfPolyFlat a6;
  a6.AddPoint(p5);
  a6.AddPoint(p6);
  a6.AddPoint(p7);
  a6.AddPoint(p8);
  a6.SetName("side6");

  AddSurface(a1);
  AddSurface(a2);
  AddSurface(a3);
  AddSurface(a4);
  AddSurface(a5);
  AddSurface(a6);
}
//----------------------------------------------------------------------
PndDrcSurfAbs* const PndDrcOptTrd2::Surface(string name)
{
  
  PndDrcSurfAbs* result=0;
  
  list<PndDrcSurfAbs*>::const_iterator kSurf;

  for(kSurf=SurfaceList().begin(); kSurf != SurfaceList().end(); ++kSurf)
    {
      if ( (*kSurf)->Name() == name )
	{
	  if (!result)
	    {
	      result = *kSurf;
	    }
	  else
	    {
	      cerr<<" *** PndDrcOptTrd2::surface: surface name "<<name<<" exists more than one time"<<endl;
	      exit(EXIT_FAILURE);
	    }
	}
    }
  return result;
}
  
