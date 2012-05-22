


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
  bar2.Surface("side1")->SetPixel();
  bar2.Surface("side2")->SetReflectivity(refl);
  bar2.Surface("side3")->SetReflectivity(refl);
  bar2.Surface("side4")->SetReflectivity(refl);
  bar2.Surface("side5")->SetReflectivity(refl);

  bar1.AddTransform(Transform3D(XYZVector(0,0, 100))); // shift by half length
  bar2.AddTransform(Transform3D(XYZVector(0,0,-100))); 




  PndDrcOptDevSys opt_system;
  opt_system.AddDevice(bar1);
  opt_system.AddDevice(bar2);
  

  // couple surface 1 of device 1 with surface 2 of device 2
  //                       dev1   dev2    surf1     surf2
  opt_system.CoupleDevice("bar1","bar2","side1","side6");


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
  double   beta = 0.69;
  bool photons_exist = manager->Cerenkov(pos,dir,beta); // generate photons
  if (photons_exist) manager->Propagate();              // propagate photons
  list<PndDrcPhoton> list_photon = manager->PhotonList();  // get list
  

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
