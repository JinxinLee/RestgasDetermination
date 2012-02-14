// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptLens.h"

//#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//#include "PndDrcSurfAbs.h"
//#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//#include "PndDrcOptVol.h"
//#include "PndDrcUtil.h"
//#include "PndDrcSurfPolyFlat.h"
#include "PndDrcSurfPolySphere.h"
#include "PndDrcSurfPolyAsphere.h"
//#include "PndDrcSurfPolyCyl.h"
#include "PndDrcSurfPolyPara.h"
#include "PndDrcSurfQuadFlatDiff.h"
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
#include "Math/RotationY.h"
using ROOT::Math::RotationY;
//#include "Math/RotationZ.h"
//using ROOT::Math::RotationZ;
//#include "Math/Rotation3D.h"
//using ROOT::Math::Rotation3D;
//
#include <iostream>
using std::cout;
using std::cerr;
using std::cin;
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


PndDrcOptLens::PndDrcOptLens(double dx, double dy, double dz, 
			    double r1, double r6, 
			    double c1, double c6)
{
  fP1 = XYZPoint( dx, dy,0);          //        p16---------p26
  fP2 = XYZPoint(-dx, dy,0);          //       /| side6     /|
  fP3 = XYZPoint(-dx,-dy,0);          //      / |          / |
  fP4 = XYZPoint( dx,-dy,0);          //     /  p46-------/-p36              Y
  //                                        p1-----------p2 /                ^  Z             side46
  //                                       / | /        /| /                 | /        side56     side36
  //                                      /  |/        / |/                  |/               side26
  //                                     /   p4-------/--p3            X <---0
  //                                    p11---------p21 /
  //                                    | /side1     | /                           side41
  //                                    |/           |/                      side51       side31
  //                                    p41---------p31                            side21

  fDz = dz;
  
  fA0.AddPoint(fP1);
  fA0.AddPoint(fP2);
  fA0.AddPoint(fP3);
  fA0.AddPoint(fP4);
  fA0.SetName("side0");
  fA0.SetInternal();
  //addSurface(fA0); do not add

  if (fabs(r6)>9998) // use a plane
    {
      XYZPoint p16( dx, dy, dz);
      XYZPoint p26(-dx, dy, dz);         
      XYZPoint p36(-dx,-dy, dz);         
      XYZPoint p46( dx,-dy, dz);         
      PndDrcSurfPolyFlat a6;
      a6.AddPoint(p16); 
      a6.AddPoint(p26);
      a6.AddPoint(p36); 
      a6.AddPoint(p46);
      a6.SetName("side6");  
      AddSurface(a6);
      // 26 denotes surface2 towards 6
      PndDrcSurfPolyFlat a26;
      a26.AddPoint(fP4); 
      a26.AddPoint(fP3);
      a26.AddPoint(p36); 
      a26.AddPoint(p46);
      a26.SetName("side26");  
      AddSurface(a26);
      PndDrcSurfPolyFlat a36;
      a36.AddPoint(fP3); 
      a36.AddPoint(p36);
      a36.AddPoint(p26); 
      a36.AddPoint(fP2);
      a36.SetName("side36");  
      AddSurface(a36);
      PndDrcSurfPolyFlat a46;
      a46.AddPoint(fP1); 
      a46.AddPoint(fP2);
      a46.AddPoint(p26); 
      a46.AddPoint(p16);
      a46.SetName("side46");  
      AddSurface(a46);
      PndDrcSurfPolyFlat a56;
      a56.AddPoint(fP4); 
      a56.AddPoint(p46);
      a56.AddPoint(p16); 
      a56.AddPoint(fP1);
      a56.SetName("side56");  
      AddSurface(a56);
    }
  else
    {
      if (c6==0)
	{
	  PndDrcSurfPolySphere surf6;
	  Make_surf6(surf6,r6);
	}
      else if (c6==-1)
	{
	  PndDrcSurfPolyPara surf6;
	  Make_surf6(surf6,r6);
	}
      else
	{
	  PndDrcSurfPolyAsphere surf6;
	  surf6.SetConicalConstant(c6);
	  Make_surf6(surf6,r6);
	}
    }




  if (fabs(r1)>9998) // use a plane
    {
      XYZPoint p11( dx, dy, -dz);
      XYZPoint p21(-dx, dy, -dz);         
      XYZPoint p31(-dx,-dy, -dz);         
      XYZPoint p41( dx,-dy, -dz);         
      PndDrcSurfPolyFlat a1;
      a1.AddPoint(p11); 
      a1.AddPoint(p21);
      a1.AddPoint(p31); 
      a1.AddPoint(p41);
      a1.SetName("side1");  
      AddSurface(a1);
      // 26 denotes surface2 towards 1
      PndDrcSurfPolyFlat a21;
      a21.AddPoint(fP4); 
      a21.AddPoint(fP3);
      a21.AddPoint(p31); 
      a21.AddPoint(p41);
      a21.SetName("side21");  
      AddSurface(a21);
      PndDrcSurfPolyFlat a31;
      a31.AddPoint(fP3); 
      a31.AddPoint(p31);
      a31.AddPoint(p21); 
      a31.AddPoint(fP2);
      a31.SetName("side31");  
      AddSurface(a31);
      PndDrcSurfPolyFlat a41;
      a41.AddPoint(fP1); 
      a41.AddPoint(fP2);
      a41.AddPoint(p21); 
      a41.AddPoint(p11);
      a41.SetName("side41");  
      AddSurface(a41);
      PndDrcSurfPolyFlat a51;
      a51.AddPoint(fP4); 
      a51.AddPoint(p41);
      a51.AddPoint(p11); 
      a51.AddPoint(fP1);
      a51.SetName("side51");  
      AddSurface(a51);
    }
  else
    {
      if (c1==0)
	{
	  PndDrcSurfPolySphere surf1;
	  Make_surf1(surf1,r1);
	}
      else if (c1==-1)
	{
	  PndDrcSurfPolyPara surf1;
	  Make_surf1(surf1,r1);
	}
      else
	{
	  PndDrcSurfPolyAsphere surf1;
	  surf1.SetConicalConstant(c1);
	  Make_surf1(surf1,r1);
	}
    }


}

//---------------------------------------------------------------------
PndDrcSurfAbs* const PndDrcOptLens::Surface(string name)
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
	      cerr<<" *** PndDrcOptLens::surface: surface name "<<name<<" exists more than one time"<<endl;
	      exit(EXIT_FAILURE);
	    }
	}
    }
  return result;
}
//----------------------------------------------------------------------
void PndDrcOptLens::Make_surf6(PndDrcSurfPolyFlat& surf6,double r6)
{
  static const double kPi=3.1415926535;

  
  //cout<<" point 1 : "<<fP1.X()<<" "<<fP1.Y()<<" "<<fP1.Z()<<endl;
  //cout<<" point 2 : "<<fP2.X()<<" "<<fP2.Y()<<" "<<fP2.Z()<<endl;
  //cout<<" point 3 : "<<fP3.X()<<" "<<fP3.Y()<<" "<<fP3.Z()<<endl;
  //cout<<" point 4 : "<<fP4.X()<<" "<<fP4.Y()<<" "<<fP4.Z()<<endl;
  


  if (r6>0)
    {
      surf6.AddPoint(fP1);
      surf6.AddPoint(fP2);
      surf6.AddPoint(fP3);
      surf6.AddPoint(fP4);
      surf6.SetRadius(r6);
    }
  else
    {
      surf6.AddPoint(fP2);
      surf6.AddPoint(fP1);
      surf6.AddPoint(fP4);
      surf6.AddPoint(fP3);
      surf6.SetRadius(fabs(r6));
    }
  surf6.SetName("side6");
  double dist[4];
  dist[0] = surf6.LimitingPoint(0).Z();
  dist[1] = surf6.LimitingPoint(1).Z();
  dist[2] = surf6.LimitingPoint(2).Z();
  dist[3] = surf6.LimitingPoint(3).Z();

  //cout<<" dist[0..3]="<<endl;
  //cout<<dist[0]<<" "
  //  <<dist[1]<<" "
  //  <<dist[2]<<" "
  //  <<dist[3]<<" "<<endl;
  


  double dist_min=9999;
  for (int i=0; i<4; i++)
    {
      if (dist[i]<dist_min) dist_min = dist[i];
    }
  
  if (r6>0)
    {
      surf6.AddTransform(Transform3D(XYZVector(0,0,fDz-dist_min)));
    }
  else
    {
      surf6.AddTransform(Transform3D(XYZVector(0,0,-fDz-dist_min)));
      surf6.AddTransform( Transform3D(RotationY(kPi)));
    }
  AddSurface(surf6);
  
  PndDrcSurfQuadFlatDiff side26;
  side26.AddSurface(fA0,fP4,fP3);
  //side26.AddSurface(fA0,fP3,fP4);
  side26.AddSurface(surf6,surf6.LimitingPoint(3),surf6.LimitingPoint(2));
  //side26.AddSurface(surf6,surf6.LimitingPoint(2),surf6.LimitingPoint(3));
  side26.SetName("side26");
  //side26.Print();
  
  AddSurface(side26);
  PndDrcSurfQuadFlatDiff side36;
  side36.AddSurface(fA0,fP3,fP2);
  //side36.AddSurface(fA0,fP2,fP3);
  side36.AddSurface(surf6,surf6.LimitingPoint(2),surf6.LimitingPoint(1));
  //side36.AddSurface(surf6,surf6.LimitingPoint(1),surf6.LimitingPoint(2));
  side36.SetName("side36");
  AddSurface(side36);
  PndDrcSurfQuadFlatDiff side46;
  side46.AddSurface(fA0,fP2,fP1);
  //side46.AddSurface(fA0,fP1,fP2);
  side46.AddSurface(surf6,surf6.LimitingPoint(1),surf6.LimitingPoint(0));
  //side46.AddSurface(surf6,surf6.LimitingPoint(0),surf6.LimitingPoint(1));
  side46.SetName("side46");
  AddSurface(side46);
  PndDrcSurfQuadFlatDiff side56;
  side56.AddSurface(fA0,fP1,fP4);
  //side56.AddSurface(fA0,fP4,fP1);
  side56.AddSurface(surf6,surf6.LimitingPoint(0),surf6.LimitingPoint(3));
  //side56.AddSurface(surf6,surf6.LimitingPoint(3),surf6.LimitingPoint(0));
  side56.SetName("side56");
  AddSurface(side56);
  
  
  
}
//----------------------------------------------------------------------
void PndDrcOptLens::Make_surf1(PndDrcSurfPolyFlat& surf1,double r1)
{
  static const double kPi=3.1415926535;

  if (r1<0)
    {
      surf1.AddPoint(fP1);
      surf1.AddPoint(fP2);
      surf1.AddPoint(fP3);
      surf1.AddPoint(fP4);
      surf1.SetRadius(fabs(r1));
    }
  else
    {
      surf1.AddPoint(fP2);
      surf1.AddPoint(fP1);
      surf1.AddPoint(fP4);
      surf1.AddPoint(fP3);
      surf1.SetRadius(r1);
    }
  surf1.SetName("side1");
  double dist[4];
  dist[0] = surf1.LimitingPoint(0).Z();
  dist[1] = surf1.LimitingPoint(1).Z();
  dist[2] = surf1.LimitingPoint(2).Z();
  dist[3] = surf1.LimitingPoint(3).Z();
  double dist_min=9999;
  for (int i=0; i<4; i++)
    {
      if (dist[i]<dist_min) dist_min = dist[i];
    }
  
  if (r1<0)
    {
      surf1.AddTransform(Transform3D(XYZVector(0,0,-fDz-dist_min)));
    }
  else
    {
      surf1.AddTransform(Transform3D(XYZVector(0,0,fDz-dist_min)));
      surf1.AddTransform( Transform3D(RotationY(kPi)));
    }
  AddSurface(surf1);
  
  PndDrcSurfQuadFlatDiff side21;
  side21.AddSurface(fA0,fP4,fP3);
  side21.AddSurface(surf1,surf1.LimitingPoint(3),surf1.LimitingPoint(2));
  side21.SetName("side21");
  AddSurface(side21);
  PndDrcSurfQuadFlatDiff side31;
  side31.AddSurface(fA0,fP3,fP2);
  side31.AddSurface(surf1,surf1.LimitingPoint(2),surf1.LimitingPoint(1));
  side31.SetName("side31");
  AddSurface(side31);
  PndDrcSurfQuadFlatDiff side41;
  side41.AddSurface(fA0,fP2,fP1);
  side41.AddSurface(surf1,surf1.LimitingPoint(1),surf1.LimitingPoint(0));
  side41.SetName("side41");
  AddSurface(side41);
  PndDrcSurfQuadFlatDiff side51;
  side51.AddSurface(fA0,fP1,fP4);
  side51.AddSurface(surf1,surf1.LimitingPoint(0),surf1.LimitingPoint(3));
  side51.SetName("side51");
  AddSurface(side51);
  
  
  
}
