#include <iostream>
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

#include <valarray>
using std::valarray;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <list>
using std::list;

#include <limits>

#include <fstream>
using std::fstream;

#include <utility>
using std::pair;

#include <map>
using std::map;




//#include <cmath>

#include "TROOT.h"
#include "TRint.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TRotation.h"
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


#include "PndDrcSurfAbs.h"
#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcSurfPolyCyl.h"
#include "PndDrcSurfQuadFlatDiff.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatLiF.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"
#include "PndDrcOptTrd2.h"

int main(int argc, char *argv[])
{

  // Example for cylindrica surface limited by a polygon

  const double pi=3.1415926535;
  

  double radius =150;
  double width  = 30;
  //                                                    scyl
  //                                                   -----
  //                                               ---       --- 
  XYZPoint p1(+70.0, -width/2,  0);    //         /              /
  XYZPoint p2(+70.0, +width/2,  0);    //        /              /
  XYZPoint p3(-70.0, +width/2,  0);    //       p2-----p6-----p3
  XYZPoint p4(-70.0, -width/2,  0);    //       p1-----p5-----p4
  XYZPoint p5(  0.0, -width/2,  0);    //         s1  /      /
  XYZPoint p6(  0.0, +width/2,  0);    //            /      /
  XYZPoint p7(  0.0, -width/2,-60);    //           p8-----p9
  XYZPoint p8(  0.0, +width/2,-60);    //           p7-----p0
  XYZPoint p9(+70.0, +width/2,-60);    //              s2
  XYZPoint p0(+70.0, -width/2,-60); 

  XYZPoint pp1(p1);
  pp1.SetZ(0);
  XYZPoint pp2(p2);
  pp2.SetZ(0);
  XYZPoint pp3(p3);
  pp3.SetZ(0);
  XYZPoint pp4(p4);
  pp4.SetZ(0);
  
  PndDrcSurfPolyCyl scyl;
  scyl.AddPoint(pp1);
  scyl.AddPoint(pp2);
  scyl.AddPoint(pp3);
  scyl.AddPoint(pp4);
  scyl.SetRadius(radius);
  scyl.AddTransform(Transform3D(XYZVector(0,0,-110))); // now at 70 in z
  scyl.SetName("scylinder");
  

  PndDrcSurfPolyFlat s1;
  s1.AddPoint(p1);
  s1.AddPoint(p2);
  s1.AddPoint(p3);
  s1.AddPoint(p4);
  s1.SetName("surf1");
  
  PndDrcSurfQuadFlatDiff s1_side1;
  s1_side1.AddSurface(scyl,scyl.LimitingPoint(0),scyl.LimitingPoint(1));
  s1_side1.AddSurface(s1,p1,p2);
  
  PndDrcSurfQuadFlatDiff s1_side2;
  s1_side2.AddSurface(scyl,scyl.LimitingPoint(2),scyl.LimitingPoint(3));
  s1_side2.AddSurface(s1,p3,p4);

  PndDrcSurfQuadFlatDiff s1_side3;
  s1_side3.AddSurface(scyl,scyl.LimitingPoint(1),scyl.LimitingPoint(2));
  s1_side3.AddSurface(s1,p2,p3);

  PndDrcSurfQuadFlatDiff s1_side4;
  s1_side4.AddSurface(scyl,scyl.LimitingPoint(0),scyl.LimitingPoint(3));
  s1_side4.AddSurface(s1,p1,p4);

  PndDrcOptVol v1;
  v1.AddSurface(scyl);
  v1.AddSurface(s1);
  v1.AddSurface(s1_side1);
  v1.AddSurface(s1_side2);
  v1.AddSurface(s1_side3);
  v1.AddSurface(s1_side4);
  v1.SetOptMaterial(PndDrcOptMatLithotecQ0());
  v1.SetName("vol1");

  PndDrcOptBrick v2(70/2,width/2,60/2);
  v2.SetOptMaterial(PndDrcOptMatLithotecQ0());
  v2.SetName("vol2");
  v2.Surface("side3")->SetPixel();
  v2.Surface("side3")->SetPrintColor(2);
  v2.AddTransform(Transform3D(XYZVector(-70/2,0,-60/2)));

  PndDrcOptBrick v3(60/2,width/2,20./2);
  v3.SetOptMaterial(PndDrcOptMatLiF());
  v3.SetName("vol3");
  v3.AddTransform(Transform3D(XYZVector(40,0,-20./2)));


  //                        p5
  //                      /  |
  //                    /    |
  //                  /      |
  //                /        |
  //               p1        |
  //               |         |
  //               |         |
  //               p4        |
  //                _        |
  //		      _      |
  //                    _    |
  //                      _ p8


  int nseg=16;
  double dist_outer_rim = 800;
  double dist_inner_rim = 200;
  double angle = 2*pi/nseg;
  
  double length_inner_rim = dist_inner_rim * 2*tan(angle/2);
  double length_outer_rim = dist_outer_rim * 2*tan(angle/2);
  

  // disk in z-x plane
  PndDrcOptTrd2 cake(length_inner_rim/2,length_outer_rim/2,
		  10,10,
		  (dist_outer_rim-dist_inner_rim)/2);
  cake.SetOptMaterial(PndDrcOptMatLithotecQ0());
  cake.SetPrintColor(4);
  // rotation followed by shift in x-y plane
  cake.AddTransform(Transform3D(RotationX(-pi/2)));
  {
    double shift = dist_inner_rim + (dist_outer_rim-dist_inner_rim)/2;
    cake.AddTransform(Transform3D(XYZVector(0,shift,-10/2)));
  }
  cake.SetName("cake");
  




  double spac_min=5;  // at least 5 mm spacing
  int nhooks = int(length_outer_rim / (width+spac_min));
  double spac=length_outer_rim/nhooks-width;
  

  PndDrcOptDevSys opt_system;
  opt_system.SetNameCopyNumber("segment");
  
  opt_system.AddDevice(cake);

  PndDrcOptDevSys hook;
  hook.AddDevice(v1);
  hook.AddDevice(v2);
  hook.AddDevice(v3);
  hook.CoupleDevice("vol1","vol2","surf1","side6");
  hook.CoupleDevice("vol1","vol3","surf1","side6");
  hook.SetPrintColor(2);
  for (int ihook=0; ihook<nhooks; ihook++)
    {
      PndDrcOptDevSys hook1(hook);
      hook1.AddTransform(Transform3D(RotationZ(-pi/2)));
      hook1.AddTransform(Transform3D(XYZVector(-length_outer_rim/2+ihook*(width+spac)+width/2,
					       70+dist_outer_rim,5)));
      hook1.SetNameCopyNumber("hook",ihook);
      opt_system.AddDeviceSystem(hook1);
      //opt_system.print();
      
    }
  opt_system.Print();
      cout<<" opt: "<<opt_system.LongName()<<endl;





  


  

  //  opt_system.coupleDevice("vol3","disk","side5","side3");
  
  
  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  for (int i=0; i<nseg; i++)
    {
      PndDrcOptDevSys tmp(opt_system);
      cout<<" tmp: "<<tmp.LongName()<<endl;
      tmp.SetNameCopyNumber("RrotSeg",i);
      tmp.AddTransform(Transform3D(RotationZ(i*2*pi/nseg)));
      cout<<tmp.CopyNumber()<<endl;
      
      manager->AddDeviceSystem(tmp);
    }
  

  fstream geo;
  geo.open("Geo.C",std::ios::out);
  geo<<"{"<<endl;
  geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  if ( ((TROOT*)gROOT)->GetVersionInt() < 51600)
    {
      geo<<"    TView *view = new TView(1);"<<endl;
    }
  else
    {  
      geo<<"    TView *view = TView::CreateView(1);"<<endl;
    }
  geo<<"    view->SetRange(-500,300,-500,500,1200,500);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(0,90,90,i);"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager->Print(geo);
  //
  /*
    XYZPoint pos(1000/2+70,0,-10/2-20);
    XYZVector dir(0,0,1); 
    double   beta = 0.99;
    double range=2000;
  
    //bool photons_exist = manager->cerenkov(pos,dir,beta,300,range,400,650); // generate photons
    bool photons_exist = manager->cerenkov(pos,dir,beta,1000,range,400,405); // generate photons

    if (photons_exist)
    {
    manager->propagate();                           // propagate photons
    }


  */
  geo<<"}"<<endl;
  geo.close();
  
}


  
  

