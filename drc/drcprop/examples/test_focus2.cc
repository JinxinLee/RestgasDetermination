
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
#include "PndDrcSurfPolySphere.h"
#include "PndDrcSurfPolyPara.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcUtil.h"

int main(int argc, char *argv[])
{
  double pi=3.1415926535;

  // Example for a simple bar with focussing mirror.

  int verbosity = 0; // 0=quiet, 1=constructors,2=member,3=functionality
  //                    4=photons, 5=everything.


  int ioption=2;
  


  PndDrcOptReflPerfect refl_perfect;
  PndDrcOptReflNone    refl_none;

  // 8 points define a bar
  
  XYZPoint p1(-10.0, 500+15.0, 1100);  //         p5----------p8
  XYZPoint p2(-10.0, 500-15.0, 1100);  //        /|          /|
  XYZPoint p3(+10.0, 500-15.0, 1100);  //       / |         / |
  XYZPoint p4(+10.0, 500+15.0, 1100);  //      /  p6-------/--p7
  //                                      /  /        /  /
  //                                     /  /        /  /
  //                                    /  /        /  /
  //                                   /  /        /  /
  XYZPoint p5(-10.0, 500+15.0, -800);  // p1---------p4  /
  XYZPoint p6(-10.0, 500-15.0, -800);  // | /         | /
  XYZPoint p7(+10.0, 500-15.0, -800);  // |/          |/
  XYZPoint p8(+10.0, 500+15.0, -800);  // p2---------p3 

  // Define from points 6 surfaces of the bar. The points have to be given in 
  // the sequence going around the surface, clock- or counterclock-wise.
  // There are 2 additional surfaces, a mirror and a screen.

  // How to produce surfaces by shift and rotate operation is for sake of clearness
  // not shown here, but in one of the other examples.

  // Declare flat surfaces with arbitrary number of points.
  PndDrcSurfPolyFlat a1,a2,a3,a4,a5,a6;

  a1.SetVerbosity(verbosity);
  a1.AddPoint(p1);
  a1.AddPoint(p2);
  a1.AddPoint(p3);
  a1.AddPoint(p4);
  a1.SetName("adown");
  a1.SetReflectivity(refl_perfect);
  a1.SetPrintColor(4);

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
  a6.SetName("aup");

  // create a volume consiting of surfaces
  // create a material the bar will consist of
  PndDrcOptVol bar;
  PndDrcOptMatLithotecQ0 quartz;
  bar.SetVerbosity(verbosity);
  bar.SetOptMaterial(quartz);
  bar.AddSurface(a6);
  bar.AddSurface(a2);
  bar.AddSurface(a3);
  bar.AddSurface(a4);
  bar.AddSurface(a5);
  bar.AddSurface(a1); // colored objects last to prevent black lines on top
  bar.SetName("bar");


  // define the photon detector box
  //               7
  //             q3
  //            /|
  //           / |
  //          /  |
  //         /8  |
  //       q4    | 
  //       |     |
  //       |     |
  //       |     |
  //       |     |
  //       |     |
  //       | 5   |6
  //       q1----q2
  //
  // 
  XYZPoint q1(-1000.0,-1200.0,-1500.0);
  XYZPoint q2(-1000.0,-1200.0, -800.0);
  XYZPoint q3(-1000.0, 1200.0, -800.0);
  XYZPoint q4(-1000.0, 1200.0,-1500.0);

  XYZPoint q5( 1000.0,-1200.0,-1500.0);
  XYZPoint q6( 1000.0,-1200.0, -800.0);
  XYZPoint q7( 1000.0, 1200.0, -800.0);
  XYZPoint q8( 1000.0, 1200.0,-1500.0);


  // Declare flat surfaces with arbitrary number of points.
  PndDrcSurfPolyFlat b1,b2,b3,b4,b5,b6;

  b1.SetVerbosity(verbosity);
  b1.AddPoint(q1);
  b1.AddPoint(q2);
  b1.AddPoint(q3);
  b1.AddPoint(q4);
  b1.SetReflectivity(refl_none);
  b1.SetName("bright");

  b2.SetVerbosity(verbosity);
  b2.AddPoint(q5);
  b2.AddPoint(q6);
  b2.AddPoint(q7);
  b2.AddPoint(q8);
  b2.SetReflectivity(refl_none);
  b2.SetName("bleft");

  b3.SetVerbosity(verbosity);
  b3.AddPoint(q1);
  b3.AddPoint(q5);
  b3.AddPoint(q8);
  b3.AddPoint(q4);
  b3.SetReflectivity(refl_none);
  b3.SetName("bup");

  b4.SetVerbosity(verbosity);
  b4.AddPoint(q2);
  b4.AddPoint(q6);
  b4.AddPoint(q7);
  b4.AddPoint(q3);
  b4.SetReflectivity(refl_none);
  b4.SetName("bdown");

  b5.SetVerbosity(verbosity);
  b5.AddPoint(q4);
  b5.AddPoint(q8);
  b5.AddPoint(q7);
  b5.AddPoint(q3);
  b5.SetReflectivity(refl_none);
  b5.SetName("btop");

  b6.SetVerbosity(verbosity);
  b6.AddPoint(q1);
  b6.AddPoint(q2);
  b6.AddPoint(q6);
  b6.AddPoint(q5);
  b6.SetReflectivity(refl_none);
  b6.SetName("bbottom");




  // focussing mirror
  double    radius;
  double    half_width;
  double    low_edge1 = 450;
  radius     = 1000;
  half_width =  700;  

  XYZPoint  rim(0,low_edge1,radius);
  XYZPoint  ref(rim);
  Transform3D trans1;
  trans1 = Transform3D(RotationY(0/*pi*/));
  rim = trans1 * rim;
  XYZPoint pos1(0,low_edge1,-950);// here the position should be
  XYZVector shift = pos1-rim;
  trans1 = Transform3D(shift)*trans1; // multiply from the left!
  PndDrcSurfPolyPara bmirror1a;
  bmirror1a.AddPoint(XYZPoint(-half_width, low_edge1,0));
  bmirror1a.AddPoint(XYZPoint(-half_width,+half_width,0));
  bmirror1a.AddPoint(XYZPoint(+half_width,+half_width,0));
  bmirror1a.AddPoint(XYZPoint(+half_width, low_edge1,0));
  bmirror1a.SetRadius(radius);
  bmirror1a.AddTransform(trans1);
  bmirror1a.SetName("bmirror1a");
  bmirror1a.SetPrintColor(4);
  bmirror1a.SetReflectivity(refl_perfect);

  double    radius2;
  double    half_width2;
  double    low_edge2 = 540;
  radius2     = 1400;
  half_width2 =  1000;  
  XYZPoint  rim2(0,low_edge2,radius2);
  XYZPoint  ref2(rim2);
  Transform3D trans2;
  trans2 = Transform3D(RotationY(0));
  rim2 = trans2 * rim2;
  XYZPoint pos2(0,low_edge2,-700);// here the position should be
  XYZVector shift2 = pos2-rim2;
  trans2 = Transform3D(shift2)*trans2; // multiply from the left!
  PndDrcSurfPolyPara bmirror2a;
  bmirror2a.AddPoint(XYZPoint(-half_width2,low_edge2,0));
  bmirror2a.AddPoint(XYZPoint(-half_width2,+half_width2,0));
  bmirror2a.AddPoint(XYZPoint(+half_width2,+half_width2,0));
  bmirror2a.AddPoint(XYZPoint(+half_width2,low_edge2,0));
  bmirror2a.SetRadius(radius2);
  bmirror2a.AddTransform(trans2);
  bmirror2a.SetName("bmirror2a");
  bmirror2a.SetPrintColor(3);
  bmirror2a.SetReflectivity(refl_perfect);


  PndDrcOptVol box;
  box.SetVerbosity(verbosity);
  box.SetOptMaterial(quartz);
  box.AddSurface(b1);
  box.AddSurface(b2);
  box.AddSurface(b3);
  box.AddSurface(b4);
  box.AddSurface(b5);
  box.AddSurface(b6);
  box.AddSurface(bmirror1a);
  box.AddSurface(bmirror2a);
  //box.AddSurface(bscreen);
  box.SetName("box");




  // Build a optical system consisting out of several volumes,
  // mirrors and screens.
  // This layer has the advantage, that a device consisting out 
  // of many equal subsystems
  // like a bar box, easily can be reproduced. 
  // See one of the forth comming test examples.

  PndDrcOptDevSys opt_system;
  opt_system.SetNameCopyNumber("opt_system");
  opt_system.SetVerbosity(verbosity);
  opt_system.AddDevice(bar);
  opt_system.AddDevice(box);

  // couple surface 1 of device 1 with surface 2 of device 2
  //                       dev1   dev2    surf1     surf2
  opt_system.CoupleDevice("bar","box","aup",  "bdown");
  
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
  geo<<"    view->SetRange(-2000,-1200,-3000,2000,2800,1000);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(180,90,90,i);"<<endl;
  geo<<"    view->Zoom();"<<endl;
  geo<<"    view->Zoom();"<<endl;
  geo<<"    view->Zoom();"<<endl;
  geo<<"    view->Zoom();"<<endl;
  geo<<"    view->Zoom();"<<endl;
  geo<<"    view->Zoom();"<<endl;
  //geo<<"    view->Zoom();"<<endl;
  //geo<<"    view->Zoom();"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager->Print(geo);
  //
  // the intention is to play around with routines.

  // create a list of photons in bar
  //exit(1);

  list<PndDrcPhoton> list_photon; // get list

  PndDrcPhoton ph;


  bool photons_exist = false;

  if (ioption==1)  
    {
      //for (double xx=-9; xx<=9; xx+=0.5)
	{
	  //  for (double yy=-4; yy<=4; yy+=0.5)
	  double xx = 0;
	  double yy = 500;
	  
	    {
	      ph.SetPosition(XYZPoint(xx,yy,0));
	      ph.SetDirection(XYZVector(0,1,2));
	      ph.SetWavelength(600);
	      list_photon.push_back(ph);
	    }
	}
      photons_exist = true;
      manager->SetPhotonList(list_photon,"bar","opt_system",0,0);
    }
  if (ioption==2)  
    {
      for (double xx=-9; xx<=9; xx+=3)
	{
	  for (double yy=500-4; yy<=500+4; yy+=2)
	    {
	      //double xx=0;
	      //double yy=500;
	      ph.SetPosition(XYZPoint(xx,yy,0));
	      ph.SetDirection(XYZVector(0,0,-1));
	      ph.SetWavelength(650);
	      list_photon.push_back(ph);
	      
	      double angle = pi/180.0 * 20.0;
	      ph.SetDirection(XYZVector(0,sin(+angle),-cos(+angle)));
	      ph.SetWavelength(521);
	      list_photon.push_back(ph);
	      ph.SetDirection(XYZVector(0,sin(-angle),-cos(-angle)));
	      ph.SetWavelength(521);
	      list_photon.push_back(ph);
	     
	      angle = pi/180.0 * 40.0;
	      ph.SetDirection(XYZVector(0,sin(+angle),-cos(+angle)));
	      ph.SetWavelength(451);
	      list_photon.push_back(ph);
	      ph.SetDirection(XYZVector(0,sin(-angle),-cos(-angle)));
	      ph.SetWavelength(451);
	      list_photon.push_back(ph);
	      
	    }
	}
      photons_exist = true;
      manager->SetPhotonList(list_photon,"bar","opt_system",0,0);
    }
  if (ioption==3)
    {
      XYZPoint pos(0,-10,0);
      XYZVector dir(0.0,1,-4); 
      double   beta = 0.75;
      double range = 150;
      photons_exist = manager->Cerenkov(pos,dir,beta,1000,range,300,600); // generate photons
    }


  if (photons_exist)
    {
      manager->Propagate();                           // propagate photons
   }

  list_photon = manager->PhotonList(); // get list
  


  // propagate writes to geo, that has finished, therefore, close geo
  geo<<"}"<<endl;
  geo.close();


  // write to screen...

  fstream scr;
  scr.open("Screen.C",std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"spatial position [mm]\",200,-100,100);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMarkerSize(0.2);"<<endl;
  scr<<"    hgr->SetMinimum(-100);"<<endl;
  scr<<"    hgr->SetMaximum(+100);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;
  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;

  //Transform3D trans3_inv = trans3.Inverse();
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if ((*iph).Fate()==Drc::kPhotMeasured)
	{
	  icnt_measured++;
	  XYZPoint pos = (*iph).Position();
	  //pos = trans3_inv * pos;

	  double x     = pos.X();
	  double y     = pos.Y();


	  scr<<"    TMarker* t = new TMarker("<<x<<","<<y<<",20);"<<endl;
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
