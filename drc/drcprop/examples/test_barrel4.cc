


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
#include "PndDrcSurfAbs.h"
#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"


int main(int argc, char *argv[])
{

  // Example for a simple bar with expansion box with a shape which can be changed

  int g_opt;

  g_opt = 0; // rectangular box
  g_opt = 1; // half trapezoidal
  g_opt = 2; // full trapezoidal



  int verbosity = 0; // 0=quiet, 1=constructors,2=member,3=functionality
  //                    4=photons, 5=everything.


  // 8 points define a bar

  XYZPoint p1(-15.0, +8.5, 1100);    //         p5----------p8
  XYZPoint p2(-15.0, -8.5, 1100);    //        /|          /|
  XYZPoint p3(+15.0, -8.5, 1100);    //       / |         / |
  XYZPoint p4(+15.0, +8.5, 1100);    //      /  p6-------/--p7
  //                                     /  /        /  /
  //                                    /  /        /  /
  //                                   /  /        /  /
  //                                  /  /        /  /
  XYZPoint p5(-15.0, +8.5, -800);  // p1---------p4  /
  XYZPoint p6(-15.0, -8.5, -800);  // | /         | /
  XYZPoint p7(+15.0, -8.5, -800);  // |/          |/
  XYZPoint p8(+15.0, +8.5, -800);  // p2---------p3 


  // Define from points 6 surfaces of the bar. The points have to be given in 
  // the sequence going around the surface, clock- or counterclock-wise.
  // There are 2 additional surfaces, a mirror and a screen.

  // How to produce surfaces by shift and rotate operation is for sake of clearness
  // not shown here, but in one of the other examples.

  PndDrcOptReflPerfect refl;

  // Declare flat surfaces with arbitrary number of points.
  PndDrcSurfPolyFlat a1,a2,a3,a4,a5,a6;

  a1.SetVerbosity(verbosity);
  a1.SetReflectivity(refl);  // this acts as a mirror
  a1.AddPoint(p1);
  a1.AddPoint(p2);
  a1.AddPoint(p3);
  a1.AddPoint(p4);
  a1.SetName("adown");

  a2.SetVerbosity(verbosity);
  a2.AddPoint(p2);
  a2.AddPoint(p6);
  a2.AddPoint(p7);
  a2.AddPoint(p3);
  a2.SetName("aside1");

  a3.SetVerbosity(verbosity);
  a3.AddPoint(p1);
  a3.AddPoint(p5);
  a3.AddPoint(p6);
  a3.AddPoint(p2);
  a3.SetName("aside2");

  a4.SetVerbosity(verbosity);
  a4.AddPoint(p4);
  a4.AddPoint(p3);
  a4.AddPoint(p7);
  a4.AddPoint(p8);
  a4.SetName("aside3");

  a5.SetVerbosity(verbosity);
  a5.AddPoint(p1);
  a5.AddPoint(p4);
  a5.AddPoint(p8);
  a5.AddPoint(p5);
  a5.SetName("aside4");

  a6.SetVerbosity(verbosity);
  a6.AddPoint(p8);
  a6.AddPoint(p7);
  a6.AddPoint(p6);
  a6.AddPoint(p5);
  a6.SetPrintColor(1);
  a6.SetName("aup");


  // create a volume consiting of surfaces
  // create a material the bar will consist of
  PndDrcOptVol bar;
  PndDrcOptMatLithotecQ0 quartz;
  bar.SetVerbosity(verbosity);
  bar.SetOptMaterial(quartz);
  bar.AddSurface(a1);
  bar.AddSurface(a2);
  bar.AddSurface(a3);
  bar.AddSurface(a4);
  bar.AddSurface(a5);
  bar.AddSurface(a6);
  bar.SetName("bar");


  PndDrcOptDevSys opt_system;
  opt_system.SetVerbosity(verbosity);
  opt_system.AddDevice(bar);



  // 8 points define a 1st expansion box

  XYZPoint pe1(-300, +300, -800);    //       p5----------p8
  XYZPoint pe2(-300, -300, -800);    //      /|          /|
  XYZPoint pe3(+300, -300, -800);    //     / |         / |
  XYZPoint pe4(+300, +300, -800);    //    /  p6-------/--p7
  //                                     /  /        /  /
  //                                    /  /        /  /
  //                                   /  /        /  /
  //                                  /  /        /  /
  XYZPoint pe5(-300, +300, -950);    // p1---------p4  /
  XYZPoint pe6(-300, -300, -950);    // | /         | /
  XYZPoint pe7(+300, -300, -950);    // |/          |/
  XYZPoint pe8(+300, +300, -950);    // p2---------p3 
  // 8 points define a 2nd expansion box

  XYZPoint pf1(-300, +300, -950);    //       p5----------p8
  XYZPoint pf2(-300, -300, -950);    //      /|          /|
  XYZPoint pf3(+300, -300, -950);    //     / |         / |
  XYZPoint pf4(+300, +300, -950);    //    /  p6-------/--p7
  //                                     /  /        /  /
  //                                    /  /        /  /
  //                                   /  /        /  /
  //                                  /  /        /  /
  XYZPoint pf5(-300, +300,-1100);    // p1---------p4  /
  XYZPoint pf6(-300, -300,-1100);    // | /         | /
  XYZPoint pf7(+300, -300,-1100);    // |/          |/
  XYZPoint pf8(+300, +300,-1100);    // p2---------p3 



  // gopt==0 do nothing => rectangular expansion voulume
  if (g_opt==1)
    {
      pe1 = p5;
      pe2 = p6;
      pe3 = p7;
      pe4 = p8;
    } 
  if (g_opt==2)
    {
      pe1 = p5;
      pe2 = p6;
      pe3 = p7;
      pe4 = p8;

      pe5.SetX(pe5.X()/2); pe5.SetY(pe5.Y()/2);
      pe6.SetX(pe6.X()/2); pe6.SetY(pe6.Y()/2);
      pe7.SetX(pe7.X()/2); pe7.SetY(pe7.Y()/2);
      pe8.SetX(pe8.X()/2); pe8.SetY(pe8.Y()/2);

      pf1 = pe5;
      pf2 = pe6;
      pf3 = pe7;
      pf4 = pe8;
    } 








  // Declare flat surfaces with arbitrary number of points.
  PndDrcSurfPolyFlat e1,e2,e3,e4,e5,e6;

  e1.SetVerbosity(verbosity);
  e1.AddPoint(pe1);
  e1.AddPoint(pe2);
  e1.AddPoint(pe3);
  e1.AddPoint(pe4);
  e1.SetName("edown");

  e2.SetVerbosity(verbosity);
  e2.AddPoint(pe2);
  e2.AddPoint(pe6);
  e2.AddPoint(pe7);
  e2.AddPoint(pe3);
  e2.SetName("eside1");

  e3.SetVerbosity(verbosity);
  e3.AddPoint(pe1);
  e3.AddPoint(pe5);
  e3.AddPoint(pe6);
  e3.AddPoint(pe2);
  e3.SetName("eside2");

  e4.SetVerbosity(verbosity);
  e4.AddPoint(pe4);
  e4.AddPoint(pe3);
  e4.AddPoint(pe7);
  e4.AddPoint(pe8);
  e4.SetName("eside3");

  e5.SetVerbosity(verbosity);
  e5.AddPoint(pe1);
  e5.AddPoint(pe4);
  e5.AddPoint(pe8);
  e5.AddPoint(pe5);
  e5.SetName("eside4");

  e6.SetVerbosity(verbosity);
  e6.AddPoint(pe8);
  e6.AddPoint(pe7);
  e6.AddPoint(pe6);
  e6.AddPoint(pe5);
  e6.SetPrintColor(2);
  e6.SetName("eup");


  // Declare flat surfaces with arbitrary number of points.
  PndDrcSurfPolyFlat f1,f2,f3,f4,f5,f6;

  f1.SetVerbosity(verbosity);
  f1.AddPoint(pf1);
  f1.AddPoint(pf2);
  f1.AddPoint(pf3);
  f1.AddPoint(pf4);
  f1.SetName("fdown");

  f2.SetVerbosity(verbosity);
  f2.AddPoint(pf2);
  f2.AddPoint(pf6);
  f2.AddPoint(pf7);
  f2.AddPoint(pf3);
  f2.SetName("fside1");

  f3.SetVerbosity(verbosity);
  f3.AddPoint(pf1);
  f3.AddPoint(pf5);
  f3.AddPoint(pf6);
  f3.AddPoint(pf2);
  f3.SetName("fside2");

  f4.SetVerbosity(verbosity);
  f4.AddPoint(pf4);
  f4.AddPoint(pf3);
  f4.AddPoint(pf7);
  f4.AddPoint(pf8);
  f4.SetName("fside3");

  f5.SetVerbosity(verbosity);
  f5.AddPoint(pf1);
  f5.AddPoint(pf4);
  f5.AddPoint(pf8);
  f5.AddPoint(pf5);
  f5.SetName("fside4");

  f6.SetVerbosity(verbosity);
  f6.AddPoint(pf8);
  f6.AddPoint(pf7);
  f6.AddPoint(pf6);
  f6.AddPoint(pf5);
  f6.SetPixel();            // sensitive plane
  f6.SetPrintColor(3);
  f6.SetName("fup");


  // create a volume consiting of surfaces
  // create a material the ev1 will consist of
  PndDrcOptVol ev1;
  ev1.SetVerbosity(verbosity);
  ev1.SetOptMaterial(quartz);
  ev1.AddSurface(e1);
  ev1.AddSurface(e2);
  ev1.AddSurface(e3);
  ev1.AddSurface(e4);
  ev1.AddSurface(e5);
  ev1.AddSurface(e6);
  ev1.SetName("ev1");

  opt_system.AddDevice(ev1);

  // create a volume consiting of surfaces
  // create a material the ev2 will consist of
  PndDrcOptVol ev2;
  ev2.SetVerbosity(verbosity);
  ev2.SetOptMaterial(quartz);
  ev2.AddSurface(f1);
  ev2.AddSurface(f2);
  ev2.AddSurface(f3);
  ev2.AddSurface(f4);
  ev2.AddSurface(f5);
  ev2.AddSurface(f6);
  ev2.SetName("ev2");

  opt_system.AddDevice(ev2);

  opt_system.CoupleDevice("bar","ev1","aup","edown");
  opt_system.CoupleDevice("ev1","ev2","eup","fdown");


  
  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->SetVerbosity(verbosity);
  manager->AddDeviceSystem(opt_system);

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
  //geo<<"    view->SetRange(-50,-50,-50,50,50,50);"<<endl;
  geo<<"    view->SetRange(-1150,-200,-1150,-750,200,-750);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(0,-90,90,i);"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager->Print(geo);
  //
  // the intention is to play around with routines.


  // create a list of photons in bar

  XYZPoint pos(0,-10,0);
  XYZVector dir(0,1,1); 
  double   beta = 0.79;
 
  bool photons_exist = manager->Cerenkov(pos,dir,beta); // generate photon
  //bool photons_exist = manager->Cerenkov(pos,dir,beta,100,range,400,650); // generate photons
  //bool photons_exist = manager->Cerenkov(pos,dir,beta,100,range,400,405); // generate photons

  cout<<" photons exist: "<<photons_exist<<endl;
  

  if (photons_exist)
    {
      manager->Propagate();                           // propagate photons
   }

  list<PndDrcPhoton> list_photon = manager->PhotonList(); // get list
  


  // propagate writes to geo, that has finished, therefore, close geo
  geo<<"}"<<endl;
  geo.close();


  // write to screen...

  fstream scr;
  scr.open("Screen.C",std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  //scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"a simple graph\",100,-100,100);"<<endl;
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"test_barrel4\",100,-400,400);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-400);"<<endl;
  scr<<"    hgr->SetMaximum(400);"<<endl;
  //scr<<"    hgr->SetMinimum(-100);"<<endl;
  //scr<<"    hgr->SetMaximum(+100);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;
  scr<<"    hgr->SetStats(0);"<<endl;

  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;



 
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if ((*iph).Fate()==Drc::kPhotMeasured)
	{

	  
	  icnt_measured++;
	  double xx     = (*iph).Position().X();
	  double yy     = (*iph).Position().Y();
	  

	  scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	  scr<<"    t->SetMarkerColor("
		<<(*iph).ColorNumber((*iph).Wavelength())
		<<");"<<endl;
	  scr<<"    t->SetMarkerSize(0.7);"<<endl;
	  scr<<"    t->Draw();"<<endl;
	}
      else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
      else                                       icnt_lost++;
    }
  scr<<"}"<<endl;
  scr.close();

 
  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;

  delete manager;


  return EXIT_SUCCESS;

}
