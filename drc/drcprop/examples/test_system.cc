

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
#include "TFile.h"
#include "TH1F.h"
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
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrik.h"

#include "PndDrcOptBurle.h"

int main(int argc, char *argv[])
{



  /*  ***        STILL WORKING ON IT, this is like a test bench.

    Combine several optical systems to one system

  */

  const double pi=3.1415926535;


  PndDrcOptBrik bar(17,17.5/2,1000);
  bar.SetOptMaterial(PndDrcOptMatLithotecQ0());
  bar.SetName("bar");

  bar.Surface("side6")->SetReflectivity(PndDrcOptReflSilver());
  //bar.Surface("side1")->SetPixel();


  bar.AddTransform(Transform3D(XYZVector(0,500,0)));
  

  PndDrcOptBrik box(600,600,200);
  box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  box.SetName("box");
  //box.Surface("side6")->SetReflectivity(PndDrcOptReflSilver());
  //box.Surface("side1")->SetPixel();
  box.AddTransform(Transform3D(XYZVector(0,0,-1200))); // bar + 1/2 box
  

  PndDrcOptDevSys opt_sys_box;
  opt_sys_box.AddDevice(box); // box side6 is to couple to the bars
  opt_sys_box.SetNameCopyNumber("sysbox",0);

  PndDrcOptDevSys opt_sys_bar;
  opt_sys_bar.AddDevice(bar); // box side6 is to couple to the bars
  

  PndDrcOptDevSys opt_system;
  opt_system.AddDeviceSystem(opt_sys_box);
  

  //opt_system.SetVerbosity(5);
  
  for (int ibar=0; ibar<90; ibar++)
    {
      opt_sys_bar.AddTransform(Transform3D(RotationZ(pi/180.*4)));
      opt_sys_bar.SetNameCopyNumber("sysbar",ibar);
      opt_system.AddDeviceSystem(opt_sys_bar);
      opt_system.CoupleDeviceSystem("sysbar","sysbox","bar","box","side1","side6",ibar);
    }
  
  // 16x16 array of burles
  double wp = 58;
  double ws = (1200 - 16*wp)/15;
  int icnt1=0;
  
  
  for (int ix=0; ix<16; ix++)
    {
      for (int iy=0; iy<16; iy++)
	{
	  //int ix=0,iy=0;
	  
	  PndDrcOptBurle burle;
	  burle.AddTransform(Transform3D(XYZVector(-600+wp/2 + ix*(wp+ws),-600+wp/2 + iy*(wp+ws),-1400)));
	  burle.SetNameCopyNumber("burle",icnt1);
	  opt_system.AddDeviceSystem(burle);
	  //opt_system.Print();
	  opt_system.CoupleDeviceSystem("burle","sysbox","housing","box","front","side1",icnt1);
	  icnt1++;
	  
	}
    }
  
  




  //opt_system.Print();
  



  //for (int i=0; i<90; i++)
  //{     
  //  bar.AddTransform(Transform3D(RotationZ(pi/180.0*4)));
  //  bar.SetCopyNumber(i);
  //  opt_system.AddDevice(bar);
  //  cout<<" *** added bar "<<i<<endl;
  //  opt_system.CoupleDevice("bar","box","side1","side6",i,0);
  //  }
  



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
  geo<<"    view->SetRange(-1000,-1000,-1000,1000,1000,1000);"<<endl;
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

  XYZPoint  pos(0,-10,0);
  XYZVector dir(0,1,1); 
  double   beta = 0.69;
  bool photons_exist = manager->Cerenkov(pos,dir,beta); // generate photons
  if (photons_exist) manager->Propagate();              // propagate photons
  list<PndDrcPhoton> list_photon = manager->PhotonList();  // get list
  
  // propagate writes to geo, that has finished, therefore, close geo
  geo<<"}"<<endl;
  geo.close();

  // analyse list
  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if      ((*iph).Fate()==Drc::kPhotMeasured) icnt_measured++;
      else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
      else                                       icnt_lost++;
    }

  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;

  delete manager;

  return EXIT_SUCCESS;



  
}

