


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


#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflGray.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"

int main(int argc, char *argv[])
{

  // Example for a simple bar with screen (photon detection) and mirror.


  PndDrcOptReflGray refl;
  refl.SetReflProb(0.99);
  

  PndDrcOptBrick bar1(17,17.5/2,100);
  bar1.SetOptMaterial(PndDrcOptMatLithotecQ0());
  bar1.SetName("bar1");
  bar1.Surface("side2")->SetReflectivity(refl);
  bar1.Surface("side3")->SetReflectivity(refl);
  bar1.Surface("side4")->SetReflectivity(refl);
  bar1.Surface("side5")->SetReflectivity(refl);
  bar1.Surface("side6")->SetReflectivity(PndDrcOptReflSilver()); // the mirror


  PndDrcOptBrick bar2(17,17.5/2,100);
  bar2.SetOptMaterial(PndDrcOptMatLithotecQ0());
  bar2.SetName("bar2");
  //bar2.Surface("side1")->SetPixel();
  bar2.Surface("side2")->SetReflectivity(refl);
  bar2.Surface("side3")->SetReflectivity(refl);
  bar2.Surface("side4")->SetReflectivity(refl);
  bar2.Surface("side5")->SetReflectivity(refl);

  bar1.AddTransform(Transform3D(XYZVector(0,0, 100))); // shift by half length
  bar2.AddTransform(Transform3D(XYZVector(0,0,-100))); 

  // define an expansion volume with limitin points in z-y:
  //
  //              p3a
  //            /   |
  //          /     |
  //        /       |                 y
  //     p2a        |                 ^
  //     |          |                 |
  //     p1a------p4a            z <--0
  //
  //  the left side will be connected to the quartz bar the right side will be the pixel.
  //
  //
  //  seen from -z it looks like
  //
  //  p3a----p3b              Y
  //   |      |               ^
  //   |      |               |
  //   |      |          x <--0
  //   |      |
  //   |      |
  //  p4a----p4b
  XYZPoint p1a(-300, -30,-200);
  XYZPoint p1b(+300, -30,-200);
  XYZPoint p2a(-300, +30,-200);
  XYZPoint p2b(+300, +30,-200);
  XYZPoint p3a(-300,+500,-500);
  XYZPoint p3b(+300,+500,-500);
  XYZPoint p4a(-300, -30,-500);
  XYZPoint p4b(+300, -30,-500);

  PndDrcSurfPolyFlat a1,a2,a3,a4,a5,a6;
  a1.SetName("side-z");
  a1.AddPoint(p4a);
  a1.AddPoint(p4b);
  a1.AddPoint(p3b);
  a1.AddPoint(p3a);
  a1.SetPrintColor(2);  // paint it in red
  a1.SetPixel();        // pixel

  a2.SetName("side+z");
  a2.AddPoint(p1a);
  a2.AddPoint(p1b);
  a2.AddPoint(p2b);
  a2.AddPoint(p2a);

  a3.SetName("side+x");
  a3.AddPoint(p1a);
  a3.AddPoint(p4a);
  a3.AddPoint(p3a);
  a3.AddPoint(p2a);

  a4.SetName("side-x");
  a4.AddPoint(p1b);
  a4.AddPoint(p4b);
  a4.AddPoint(p3b);
  a4.AddPoint(p2b);

  a5.SetName("side+y");
  a5.AddPoint(p2a);
  a5.AddPoint(p3a);
  a5.AddPoint(p3b);
  a5.AddPoint(p2b);

  a6.SetName("side-y");
  a6.AddPoint(p1a);
  a6.AddPoint(p4a);
  a6.AddPoint(p4b);
  a6.AddPoint(p1b);

  // create from surfaces the volume

  PndDrcOptVol ex_box;
  PndDrcOptMatLithotecQ0 quartz;
  ex_box.SetOptMaterial(quartz);
  ex_box.AddSurface(a1);
  ex_box.AddSurface(a2);
  ex_box.AddSurface(a3);
  ex_box.AddSurface(a4);
  ex_box.AddSurface(a5);
  ex_box.AddSurface(a6);
  ex_box.SetName("expansion_box");







  PndDrcOptDevSys opt_system;
  opt_system.AddDevice(bar1);
  opt_system.AddDevice(bar2);
  opt_system.AddDevice(ex_box);

  // couple surface 1 of device 1 with surface 2 of device 2
  //                       dev1   dev2    surf1     surf2
  opt_system.CoupleDevice("bar1","bar2",         "side1","side6");
  opt_system.CoupleDevice("bar2","expansion_box","side1","side+z");


  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
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
  geo<<"    view->SetRange(-50,-50,-50,50,50,50);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(0,90,90,i);"<<endl;
   // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager->Print(geo);
  //
  // the intention is to play around with routines.





  // create a list of photons in bar

  XYZPoint  pos(0,-10,50);
  XYZVector dir(0,1,1); 
  double   beta = 0.80;
  bool photons_exist = manager->Cerenkov(pos,dir,beta); // generate photons
  if (photons_exist) manager->Propagate();              // propagate photons
  list<PndDrcPhoton> list_photon = manager->PhotonList();  // get list
  

  // write to screen...

  fstream scr;
  scr.open("Screen.C",std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"a simple graph\",100,-300,300);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-30);"<<endl;
  scr<<"    hgr->SetMaximum(500);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;
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
	  double rx     = (*iph).Position().X();
	  double ry     = (*iph).Position().Y();
	    
	  

	  scr<<"    TMarker* t = new TMarker("<<rx<<","<<ry<<",20);"<<endl;
	  scr<<"    t->SetMarkerColor("
	     <<(*iph).ColorNumber((*iph).Wavelength())
	     <<");"<<endl;
	  scr<<"    t->SetMarkerSize(0.7);"<<endl;
	  scr<<"    t->Draw();"<<endl;
	}
      else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
      else                                        icnt_lost++;
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
