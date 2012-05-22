

//#include <cstdlib>
//#include <cstdio>
//#include <string>

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


#include <map>
using std::map;

#include <limits>

#include <fstream>
using std::fstream;

#include <utility>
using std::pair;
//#include <cmath>

#include "TROOT.h"
#include "TRint.h"
#include "TVector3.h"
#include "TRandom.h"
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
#include "PndDrcSurfCyl.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"


int main(int argc, char *argv[])
{
  //const double pi = 3.1415926535;

  int verbosity = 0; // 0=quiet, 1=constructors,2=member,3=functionality
  //                    4=photons, 5=everything.

  // 8 points define a bar

  XYZPoint p1(-1500.0, 490+8.5, 1000);    //         p5----------p8
  XYZPoint p2(-1500.0, 490-8.5, 1000);    //        /|          /|
  XYZPoint p3(+1500.0, 490-8.5, 1000);    //       / |         / |
  XYZPoint p4(+1500.0, 490+8.5, 1000);    //      /  p6-------/--p7
  //                                     /  /        /  /
  //                                    /  /        /  /
  //                                   /  /        /  /
  //                                  /  /        /  /
  XYZPoint p5(-1500.0, 490+8.5, -1000);  // p1---------p4  /
  XYZPoint p6(-1500.0, 490-8.5, -1000);  // | /         | /
  XYZPoint p7(+1500.0, 490-8.5, -1000);  // |/          |/
  XYZPoint p8(+1500.0, 490+8.5, -1000);  // p2---------p3 


  // Define from points 6 surfaces of the bar. The points have to be given in 
  // the sequence going around the surface, clock- or counterclock-wise.
  // There are 2 additional surfaces, a mirror and a screen.

  // How to produce surfaces by shift and rotate operation is for sake of clearness
  // not shown here, but in one of the other examples.

  PndDrcOptReflSilver refl;

  // Declare flat surfaces with arbitrary number of points.
  PndDrcSurfPolyFlat a1,a2,a3,a4,a5,a6;

  a1.SetVerbosity(verbosity);
  a1.SetReflectivity(refl);
  a1.AddPoint(p1);
  a1.AddPoint(p2);
  a1.AddPoint(p3);
  a1.AddPoint(p4);
  a1.SetName("pfront");

  a2.SetVerbosity(verbosity);
  a2.AddPoint(p2);
  a2.AddPoint(p6);
  a2.AddPoint(p7);
  a2.AddPoint(p3);
  a2.SetName("pside1");

  a3.SetVerbosity(verbosity);
  a3.AddPoint(p1);
  a3.AddPoint(p5);
  a3.AddPoint(p6);
  a3.AddPoint(p2);
  a3.SetName("pside2");

  a4.SetVerbosity(verbosity);
  a4.AddPoint(p4);
  a4.AddPoint(p3);
  a4.AddPoint(p7);
  a4.AddPoint(p8);
  a4.SetName("pside3");

  a5.SetVerbosity(verbosity);
  a5.AddPoint(p1);
  a5.AddPoint(p4);
  a5.AddPoint(p8);
  a5.AddPoint(p5);
  a5.SetName("pside4");

  a6.SetVerbosity(verbosity);
  a6.AddPoint(p8);
  a6.AddPoint(p7);
  a6.AddPoint(p6);
  a6.AddPoint(p5);
  a6.SetPixel();
  a6.SetPrintColor(2);
  a6.SetName("pback");

  PndDrcSurfPolyFlat as;
  as = a6;
  as.SetName("screen_front");
  as.SetPrintColor(2);                     // root red


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
  
  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager manager;
  manager.SetVerbosity(verbosity);
  manager.AddDeviceSystem(opt_system);

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
  geo<<"    view->SetRange(-1000,-1000,-1000,1000,1000,1000);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(0,90,90,i);"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager.Print(geo);


  //  XYZPoint pos(0,-250,+200);
  XYZPoint  pos(0,0,0);
  XYZVector dir(0,1,1.9);
  bool photons_exist;
  double range = 10000;
  photons_exist = manager.Cerenkov(pos,dir,0.70,1000,range,300,650); // generate photons

  if (photons_exist)
    {
      fstream scr;
      scr.open("Screen1.C",std::ios::out);
      scr<<"{"<<endl;
      scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
      scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"Screen\",100,-100,100);"<<endl;
      scr<<"    hgr->SetMarkerStyle(20);"<<endl;
      scr<<"    hgr->SetMinimum(-100);"<<endl;
      scr<<"    hgr->SetMaximum(+100);"<<endl;
      scr<<"    hgr->Draw(\"POL\");"<<endl;

     fstream scr2;
      scr2.open("TOP.C",std::ios::out);
      scr2<<"{"<<endl;
      scr2<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
      scr2<<"    TH1F *hgr = new TH1F(\"hgr\",\"TOP (ns) vs x [mm]\",100,-1600,1600);"
	  <<endl;
      scr2<<"    hgr->SetMarkerStyle(20);"<<endl;
      scr2<<"    hgr->SetMinimum(0);"<<endl;
      scr2<<"    hgr->SetMaximum(50);"<<endl;
      scr2<<"    hgr->Draw(\"POL\");"<<endl;


      int icnt_measured = 0;
      int icnt_absorbed = 0;
      int icnt_lost     = 0;
      

      manager.Propagate();                           // propagate photons

      list<PndDrcPhoton> list_photon = manager.PhotonList(); // get list

      list<PndDrcPhoton>::iterator iph;
      for (iph = list_photon.begin(); iph != list_photon.end(); ++iph)
	{
	  //cout<<(*iph).fate()<<endl;
	  if ((*iph).Fate()==Drc::kPhotMeasured)
	    {
	      icnt_measured++;
	      double x     = (*iph).Direction().X();
	      double y     = (*iph).Direction().Y();
	      double z     = (*iph).Direction().Z();

	      //cout<<x<<" "<<y<<" "<<z<<endl;//###


	      double theta = atan2(sqrt(x*x+y*y),fabs(z));
	      double phi   = atan2(y,x);
	      double r     = theta;
	      double xx = r*cos(phi)*180/3.1415;
	      double yy = r*sin(phi)*180/3.1415;


	      //cout<<" time = "<<(*iph).time()<<endl;//###

	      scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	      scr<<"    t->SetMarkerColor("
		 <<(*iph).ColorNumber((*iph).Wavelength())
		 <<");"<<endl;
	      scr<<"    t->SetMarkerSize(0.7);"<<endl;
	      scr<<"    t->Draw();"<<endl;

	      double phi1 = XYZPoint(0,-1,0).Phi();
	      double phi2 = (*iph).Position().Phi();
	      phi = phi2-phi1;
	      //if (phi >  pi) phi-=2*pi;
	      //if (phi < -pi) phi+=2*pi;
	      //cout<<(*iph).position().X()<<" "<<(*iph).position().Y()<<" "
	      //  <<(*iph).time()<<endl;

	      //x = (*iph).position().X();

	      scr2<<"    TMarker* t = new TMarker("
		  <<(*iph).Position().X()<<","<<(*iph).Time()<<",20);"
		  <<endl;
	      scr2<<"    t->SetMarkerColor("
		 <<(*iph).ColorNumber((*iph).Wavelength())
		 <<");"<<endl;
	      scr2<<"    t->SetMarkerSize(0.7);"<<endl;
	      scr2<<"    t->Draw();"<<endl;





	    }
	  if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
	  if ((*iph).Fate()==Drc::kPhotLost)     icnt_lost++;

	  
	}

      scr<<"}"<<endl;
      scr.close();

      scr2<<"}"<<endl;
      scr2.close();

      int icnt = icnt_measured+icnt_absorbed+icnt_lost;
      cout<<" Total photons: "<<icnt<<endl;
      cout<<"      measured: "<<icnt_measured<<endl;
      cout<<"      absorbed: "<<icnt_absorbed<<endl;
      cout<<"          lost: "<<icnt_lost<<endl;
    }









  geo<<"}"<<endl;
  geo.close();
  















  return 0;

}
