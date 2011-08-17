


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
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptReflGeffcken.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatMarcol7.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrik.h"
#include "PndDrcOptCylLens.h"
#include "PndDrcOptLens.h"

int main(int argc, char *argv[])
{

  static const double kPi=3.1415926535;

  // Example for a simple sheet with lens and expansion box.



  PndDrcOptDevSys opt_system;
  
  PndDrcOptBrik cube(10.0,10.0,10.0);
  cube.SetOptMaterial(PndDrcOptMatLithotecQ0());
  cube.Surface("side1")->SetReflectivity(PndDrcOptReflGeffcken());
  cube.SetName("cube");



  // move sheet such into positive z space such that end of sheet is at z=0
  cube.AddTransform(Transform3D(XYZVector(0,0,10)));
  
  opt_system.AddDevice(cube);

  PndDrcOptBrik ex_box(100.0,100.0,100.0);
  ex_box.SetOptMaterial(PndDrcOptMatVacuum());
  ex_box.SetName("ex_box");
  // move sheet such into positive z space such that end of sheet is at z=0
  ex_box.AddTransform(Transform3D(XYZVector(0,0,-100)));
  ex_box.Surface("side1")->SetPixel();
  

  opt_system.AddDevice(ex_box);
  
  
  // couple surface 1 of device 1 with surface 2 of device 2
  //                       dev1   dev2    surf1     surf2
  opt_system.CoupleDevice("cube"        ,"ex_box"  ,"side1", "side6");


  // The manager must be created as pointer.		
  // It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->AddDeviceSystem(opt_system);

  // enable geometry print into Geo.C
  //
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
  geo<<"    view->SetRange(-1000,-1000,-1000,3000,3000,3000);"<<endl;
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
  // there hast to come another geo output after propagation...
  // manager->Print(); // print out to screen everything...

  // create a list of photons in sheet

  
  PndDrcPhoton ph;
  ph.SetReflectionLimit(200);  
  list<PndDrcPhoton> list_photon;

  for (double lambda=200; lambda<600; lambda+=50)
    {
      for (int i=0; i<5; i++)	
	{
	  
	  double theta=0; 
	    {    
	      ph.SetPosition(XYZPoint(0,0,5));
	      double z = -cos(theta*kPi/180);
	      double y = sin(theta*kPi/180);
	      ph.SetDirection(XYZVector(0,y,z));
	      ph.SetWavelength(lambda);
	      ph.SetDevice(manager->Device("cube")); 
	      list_photon.push_back(ph);
	    }
	}
      
	  
    }
  manager->SetPhotonList(list_photon,"cube");
  manager->Propagate();              // propagate photons

  
  geo<<"}"<<endl;     // here it is...

  fstream scr;
  scr.open("Screen.C",std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"test_simple_lens_sheet time vs x\",500,-500,500);"<<endl;
  scr<<"    hgr->SetStats(0);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-500);"<<endl;
  scr<<"    hgr->SetMaximum(500);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;




  // analyse list

  list_photon = manager->PhotonList();  // get list

  fstream out;
  out.open("debug.dat",std::ios::out);
  

  //int icnt1=0;
  
  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if      ((*iph).Fate()==Drc::kPhotMeasured) 
	{
	  icnt_measured++;
	  double xx=(*iph).Position().X();
	  //double yy=(*iph).Position().Y();
	  double yy=(*iph).Time();
	  scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	  scr<<"    t->SetMarkerColor("
		<<(*iph).ColorNumber((*iph).Wavelength())
		<<");"<<endl;
	  scr<<"    t->SetMarkerSize(0.2);"<<endl;
	  scr<<"    t->Draw();"<<endl;
	  out<<(*iph).Position().X()<<" "
	     <<(*iph).Position().Y()<<" "
	     <<(*iph).Position().Z()<<" "
	     <<(*iph).Wavelength()<<endl;
	}
      else if ((*iph).Fate()==Drc::kPhotFlying)   {icnt_flying++;}
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) {icnt_absorbed++;}
      else 
	{
	  icnt_lost++;
	}
      
    }
  out.close();
  

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
