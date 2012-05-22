
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
  //int ioption = 1; // parallel beam (0 deg)
  //int ioption = 2; // 0,  +- 20,  +-40 deg
  int ioption = 1; // C-cone

  //int imirror=1; // spherical
  int imirror=2; // paraboloid

  // Example for a simple bar with focussing mirror.

  int verbosity = 0; // 0=quiet, 1=constructors,2=member,3=functionality
  //                    4=photons, 5=everything.

  PndDrcOptReflPerfect refl_perfect;
  PndDrcOptReflNone    refl_none;

  // create a 10x10x10 cm3 cube to serve as scale
  XYZPoint s1(-50.0,150.0, -600);  //         p5----------p8
  XYZPoint s2(-50.0,+50.0, -600);  //        /|          /|
  XYZPoint s3(+50.0,+50.0, -600);  //       / |         / |
  XYZPoint s4(+50.0,150.0, -600);  //      /  p6-------/--p7
  XYZPoint s5(-50.0,150.0, -700);  // p1---------p4  /
  XYZPoint s6(-50.0,+50.0, -700);  // | /         | /
  XYZPoint s7(+50.0,+50.0, -700);  // |/          |/
  XYZPoint s8(+50.0,150.0, -700);  // p2---------p3 

  PndDrcSurfPolyFlat c1,c2,c3,c4,c5,c6;
  c1.AddPoint(s1);
  c1.AddPoint(s2);
  c1.AddPoint(s3);
  c1.AddPoint(s4);
  c1.SetName("cdown");
  c2.AddPoint(s2);
  c2.AddPoint(s6);
  c2.AddPoint(s7);
  c2.AddPoint(s3);
  c2.SetName("cside1");
  c3.AddPoint(s1);
  c3.AddPoint(s5);
  c3.AddPoint(s6);
  c3.AddPoint(s2);
  c3.SetName("cside2");
  c4.AddPoint(s4);
  c4.AddPoint(s3);
  c4.AddPoint(s7);
  c4.AddPoint(s8);
  c4.SetName("cside3");
  c5.AddPoint(s1);
  c5.AddPoint(s4);
  c5.AddPoint(s8);
  c5.AddPoint(s5);
  c5.SetName("cside4");
  c6.AddPoint(s8);
  c6.AddPoint(s7);
  c6.AddPoint(s6);
  c6.AddPoint(s5);
  c6.SetName("cup");
  PndDrcOptVol cube;
  cube.AddSurface(c6);
  cube.AddSurface(c2);
  cube.AddSurface(c3);
  cube.AddSurface(c4);
  cube.AddSurface(c5);
  cube.AddSurface(c1); // colored objects last to prevent black lines on top
  cube.SetName("cube");




  // 8 points define a bar
  
  XYZPoint p1(-10.0, +5.0, 1100);  //         p5----------p8
  XYZPoint p2(-10.0, -5.0, 1100);  //        /|          /|
  XYZPoint p3(+10.0, -5.0, 1100);  //       / |         / |
  XYZPoint p4(+10.0, +5.0, 1100);  //      /  p6-------/--p7
  //                                      /  /        /  /
  //                                     /  /        /  /
  //                                    /  /        /  /
  //                                   /  /        /  /
  XYZPoint p5(-10.0, +5.0, -800);  // p1---------p4  /
  XYZPoint p6(-10.0, -5.0, -800);  // | /         | /
  XYZPoint p7(+10.0, -5.0, -800);  // |/          |/
  XYZPoint p8(+10.0, +5.0, -800);  // p2---------p3 

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
  XYZPoint q1(-100.0,  -5.0,-1300.0);
  XYZPoint q2(-100.0,  -5.0, -800.0);
  XYZPoint q3(-100.0, 595.0, -800.0);
  XYZPoint q4(-100.0, 595.0,-1300.0);

  XYZPoint q5( 100.0,  -5.0,-1300.0);
  XYZPoint q6( 100.0,  -5.0, -800.0);
  XYZPoint q7( 100.0, 595.0, -800.0);
  XYZPoint q8( 100.0, 595.0,-1300.0);


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
  radius     = (imirror==1) ? 400 : 500;
  half_width = (imirror==1) ? 210 : 300;
  XYZPoint  rim(0,-half_width,radius);
  XYZPoint  ref(rim);

  
  Transform3D trans1;
  trans1 = Transform3D(RotationY(pi));

  if (imirror==1)  trans1 *= Transform3D(RotationX(pi/180.0*(8)));
  if (imirror==2)  trans1 *= Transform3D(RotationX(pi/180.0*(0)));
  rim = trans1 * rim;
  XYZPoint pos1(0,-100,-1299);// here the position should be
  XYZVector shift = pos1-rim;
  trans1 = Transform3D(shift)*trans1; // multiply from the left!
  ref = trans1 * ref;

  PndDrcSurfPolySphere bmirror1a;
  bmirror1a.AddPoint(XYZPoint(-half_width,-half_width,0));
  bmirror1a.AddPoint(XYZPoint(-half_width,+half_width,0));
  bmirror1a.AddPoint(XYZPoint(+half_width,+half_width,0));
  bmirror1a.AddPoint(XYZPoint(+half_width,-half_width,0));
  bmirror1a.SetRadius(radius);
  bmirror1a.AddTransform(trans1);
  

  bmirror1a.SetName("bmirror1a");
  bmirror1a.SetPrintColor(4);
  bmirror1a.SetReflectivity(refl_perfect);

  PndDrcSurfPolyPara bmirror1b;
  bmirror1b.AddPoint(XYZPoint(-half_width,-half_width,0));
  bmirror1b.AddPoint(XYZPoint(-half_width,+half_width,0));
  bmirror1b.AddPoint(XYZPoint(+half_width,+half_width,0));
  bmirror1b.AddPoint(XYZPoint(+half_width,-half_width,0));
  bmirror1b.SetRadius(radius);
  bmirror1b.AddTransform(trans1);
  bmirror1b.SetName("bmirror1b");
  bmirror1b.SetPrintColor(4);
  bmirror1b.SetReflectivity(refl_perfect);


  // the second flat mirror 
  XYZPoint qq2(-100.0, -5.0,  -970);
  XYZPoint qq6( 100.0, -5.0,  -970);
  XYZPoint qq3(-100.0,150.0, -1000);
  XYZPoint qq7( 100.0,150.0, -1000);
  shift = XYZVector(0,0,60);
  qq2 += shift;
  qq6 += shift;
  qq3 += shift;
  qq7 += shift;

  PndDrcSurfPolyFlat bmirror2;  
  bmirror2.SetVerbosity(verbosity);
  bmirror2.AddPoint(qq2);
  bmirror2.AddPoint(qq6);
  bmirror2.AddPoint(qq7);
  bmirror2.AddPoint(qq3);
  bmirror2.SetName("bmirror2");
  bmirror2.SetPrintColor(4);               
  bmirror2.SetReflectivity(refl_perfect);

  // focussing mirror
  radius     = (imirror==1) ? 250 : 180;
  half_width = (imirror==1) ? 150 : 90;
  rim.SetXYZ(0,-half_width,radius);

  Transform3D trans2;
  if (imirror==1) trans2 = Transform3D(RotationX(pi/180.0*(15)));
  if (imirror==2) trans2 = Transform3D(RotationX(pi/180.0*(10)));
  rim = trans2 * rim;
  if (imirror==1) pos1.SetXYZ(0,140,-880);// here the position should be
  if (imirror==2) pos1.SetXYZ(0,320,-830);// here the position should be
  shift = pos1-rim;
  trans2 = Transform3D(shift) * trans2;

  PndDrcSurfPolySphere bmirror3a;
  bmirror3a.AddPoint(XYZPoint(-half_width,-half_width,0));
  bmirror3a.AddPoint(XYZPoint(-half_width,+half_width,0));
  bmirror3a.AddPoint(XYZPoint(+half_width,+half_width,0));
  bmirror3a.AddPoint(XYZPoint(+half_width,-half_width,0));
  bmirror3a.SetRadius(radius);
  bmirror3a.AddTransform(trans2);
  bmirror3a.SetName("bmirror3a");
  bmirror3a.SetPrintColor(4);
  bmirror3a.SetReflectivity(refl_perfect);

  PndDrcSurfPolyPara bmirror3b;
  bmirror3b.AddPoint(XYZPoint(-half_width,-half_width,0));
  bmirror3b.AddPoint(XYZPoint(-half_width,+half_width,0));
  bmirror3b.AddPoint(XYZPoint(+half_width,+half_width,0));
  bmirror3b.AddPoint(XYZPoint(+half_width,-half_width,0));
  bmirror3b.SetRadius(radius);
  bmirror3b.AddTransform(trans2);
  bmirror3b.SetName("bmirror3b");
  bmirror3b.SetPrintColor(4);
  bmirror3b.SetReflectivity(refl_perfect);


  
  double angle_screen = (imirror==1) ? 20 : 75;
  RotationX rot_screen(pi/180.0*angle_screen);
  XYZVector shift_screen;
  (imirror==1) ? shift_screen.SetXYZ(0,370,-950): shift_screen.SetXYZ(0,462,-910);
  half_width = (imirror==1) ? 100 : 100;
  PndDrcSurfPolyFlat bscreen;

  Transform3D trans3(rot_screen);
  trans3 = Transform3D(shift_screen)*trans3;

  bscreen.AddPoint(XYZPoint(-half_width,-half_width,-10));
  bscreen.AddPoint(XYZPoint(-half_width,+half_width,-10));
  bscreen.AddPoint(XYZPoint(+half_width,+half_width,-10));
  bscreen.AddPoint(XYZPoint(+half_width,-half_width,-10));

  //bscreen.AddPoint(p5);
  //bscreen.AddPoint(p6);
  //bscreen.AddPoint(p7);
  //bscreen.AddPoint(p8);



  bscreen.SetName("bscreen");
  bscreen.SetPrintColor(2);
  bscreen.SetPixel();
  bscreen.AddTransform(trans3);

  PndDrcOptVol box;
  box.SetVerbosity(verbosity);
  box.SetOptMaterial(quartz);
  box.AddSurface(b1);
  box.AddSurface(b2);
  box.AddSurface(b3);
  box.AddSurface(b4);
  box.AddSurface(b5);
  box.AddSurface(b6);
  if (imirror==1) box.AddSurface(bmirror1a);
  if (imirror==2) box.AddSurface(bmirror1b);
  if (imirror==1) box.AddSurface(bmirror3a);
  if (imirror==2) box.AddSurface(bmirror3b);
  box.AddSurface(bscreen);
  box.SetName("box");




  // Build a optical system consisting out of several volumes,
  // mirrors and screens.
  // This layer has the advantage, that a device consisting out 
  // of many equal subsystems
  // like a bar box, easily can be reproduced. 
  // See one of the forth comming test examples.

  PndDrcOptDevSys opt_system;
  opt_system.SetNameCopyNumber("total");
  opt_system.SetVerbosity(verbosity);
  //opt_system.AddDevice(cube);
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
  geo<<"    view->SetRange(-500,000,-1300,500,920,-500);"<<endl;
  //geo<<"    view->SetRange(-500,-300,-1300,500,700,1300);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(180,90,90,i);"<<endl;
  //geo<<"    view->Zoom();"<<endl;
  //geo<<"    view->Zoom();"<<endl;
  //geo<<"    view->Zoom();"<<endl;
  //geo<<"    view->Zoom();"<<endl;
  //geo<<"    view->Zoom();"<<endl;
  //geo<<"    view->Zoom();"<<endl;
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
      for (double xx=-9; xx<=9; xx+=1.0)
	{
	  for (double yy=-4; yy<=4; yy+=1.0)
	    {
	      ph.SetPosition(XYZPoint(xx,yy,0));
	      ph.SetDirection(XYZVector(0,0,-1));
	      ph.SetWavelength(600);
	      list_photon.push_back(ph);
	    }
	}
      photons_exist = true;
      manager->SetPhotonList(list_photon,"bar","total",0,0);
    }
  if (ioption==2)  
    {
      for (double xx=-9; xx<=9; xx+=3)
	{
	  for (double yy=-4; yy<=4; yy+=2)
	    {
	      //double xx=0;
	      //double yy=0;
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
      manager->SetPhotonList(list_photon,"bar","total",0,0);
    }
  if (ioption==3)
    {
      XYZPoint pos(0,-10,1000);
      XYZVector dir(0,1,4); 
      double   beta = 0.69;
      double range = 500;
      photons_exist = manager->Cerenkov(pos,dir,beta,100,range,400,405); // generate photons
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

  Transform3D trans3_inv = trans3.Inverse();
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if ((*iph).Fate()==Drc::kPhotMeasured)
	{
	  icnt_measured++;
	  XYZPoint pos = (*iph).Position();
	  pos = trans3_inv * pos;

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
