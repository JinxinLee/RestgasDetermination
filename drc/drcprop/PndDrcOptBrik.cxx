// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptBrik.h"

//#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//#include "PndDrcSurfAbs.h"
//#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//#include "PndDrcOptVol.h"
//#include "PndDrcUtil.h"
#include "PndDrcSurfPolyFlat.h"
#include <iostream>
//using std::cout;
using std::cerr;
//using std::cin;
using std::endl;
using std::numeric_limits;


PndDrcOptBrik::PndDrcOptBrik(double dx, double dy, double dz)
{
  SetSize(dx,dy,dz);
}
//----------------------------------------------------------------------
void PndDrcOptBrik::SetSize(double dx, double dy, double dz)
{
  XYZPoint p1( dx, dy,-dz);        //        p5----------p8
  XYZPoint p2( dx,-dy,-dz);        //       /|           /|
  XYZPoint p3(-dx,-dy,-dz);        //      / |          / |
  XYZPoint p4(-dx, dy,-dz);        //     /  p6--------/-p7               Y
  //                                     /   /        /  /                ^  Z
  //                                    /   /        /  /                 | /
  //                                   /   /        /  /                  |/
  //                                  /   /        /  /             X <---0
  XYZPoint p5( dx, dy, dz);        //p1----------p4  /
  XYZPoint p6( dx,-dy, dz);        //| /          | /
  XYZPoint p7(-dx,-dy, dz);        //|/           |/
  XYZPoint p8(-dx, dy, dz);        //p2----------p3 

  PndDrcSurfPolyFlat a1;
  a1.AddPoint(p1);
  a1.AddPoint(p2);
  a1.AddPoint(p3);
  a1.AddPoint(p4);
  a1.SetName("side1");

  //a1.SetVerbosity(5);//###
  


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
PndDrcSurfAbs* const PndDrcOptBrik::Surface(string name)
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
	      cerr<<" *** PndDrcOptBrik::surface: surface name "<<name<<" exists more than one time"<<endl;
	      exit(EXIT_FAILURE);
	    }
	}
    }
  return result;
}
//----------------------------------------------------------------------
 
