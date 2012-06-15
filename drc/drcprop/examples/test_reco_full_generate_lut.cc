

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
#include "TFile.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TRotation.h"
#include "TH1.h"
#include "TH2D.h"
#include "TAxis.h"

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
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"

#include "test_reco_full_setup.h"

// Generate lookup tables (LUT) for reconstruction

int main(int argc, char *argv[])
{

 
  // Example for a simple bar with flat downstream mirror and expansion box.

  const double pi=3.1415926535;

 
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();


  setup_geometry(manager); // geometrical setup in test_reco_full_setup.cc
   


  string sfile = "Geo.C";
  fstream geo;
  geo.open(sfile.c_str(),std::ios::out);
  geo<<"{"<<endl;
  geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  cout<<" root version: "<<((TROOT*)gROOT)->GetVersionInt()<<endl;
  
  if ( ((TROOT*)gROOT)->GetVersionInt() < 51600)
    {
      geo<<"    TView *view = new TView(1);"<<endl;
    }
  else
    {  
      geo<<"    TView *view = TView::CreateView(1);"<<endl;
    }
  //geo<<"    view->SetRange(-400,-500,-1050,600,500,-50);"<<endl;
  geo<<"    view->SetRange(-500,-500,-900,500,500,100);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(90,90,90,i);"<<endl;
  for (int i=0; i<16;i++) // 7 or 16
    geo<<"    view->Zoom();"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  //manager->Print(geo);


  // event loop
  int     nevents        = 10000000;//10000000;
  TRandom ran;
  double phi,costh,sinth;
  

  TFile*  hfile = (TFile*)gROOT->FindObject("test_reco_full_generate_lut.root"); 
  if (hfile) hfile->Close();
  hfile = new TFile("test_reco_full_generate_lut.root","RECREATE","LUT files");

  TH2D*   angle2d = new TH2D("angle2d"   ,"theta phi",201,0,90,201,0,360);
  

  TH2D*   kbarx_side2 = new TH2D("kbarx_side2"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side2 = new TH2D("kbary_side2"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side2 = new TH2D("kbarz_side2"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarx_side3 = new TH2D("kbarx_side3"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side3 = new TH2D("kbary_side3"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side3 = new TH2D("kbarz_side3"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarx_side4 = new TH2D("kbarx_side4"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side4 = new TH2D("kbary_side4"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side4 = new TH2D("kbarz_side4"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarx_side5 = new TH2D("kbarx_side5"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side5 = new TH2D("kbary_side5"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side5 = new TH2D("kbarz_side5"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarx_side1 = new TH2D("kbarx_side1"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side1 = new TH2D("kbary_side1"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side1 = new TH2D("kbarz_side1"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side23 = new TH2D("kbarx_side23" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side23 = new TH2D("kbary_side23" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side23 = new TH2D("kbarz_side23" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side24 = new TH2D("kbarx_side24" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side24 = new TH2D("kbary_side24" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side24 = new TH2D("kbarz_side24" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side25 = new TH2D("kbarx_side25" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side25 = new TH2D("kbary_side25" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side25 = new TH2D("kbarz_side25" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side32 = new TH2D("kbarx_side32" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side32 = new TH2D("kbary_side32" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side32 = new TH2D("kbarz_side32" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side34 = new TH2D("kbarx_side34" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side34 = new TH2D("kbary_side34" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side34 = new TH2D("kbarz_side34" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side35 = new TH2D("kbarx_side35" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side35 = new TH2D("kbary_side35" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side35 = new TH2D("kbarz_side35" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side42 = new TH2D("kbarx_side42" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side42 = new TH2D("kbary_side42" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side42 = new TH2D("kbarz_side42" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side43 = new TH2D("kbarx_side43" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side43 = new TH2D("kbary_side43" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side43 = new TH2D("kbarz_side43" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side45 = new TH2D("kbarx_side45" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side45 = new TH2D("kbary_side45" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side45 = new TH2D("kbarz_side45" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side52 = new TH2D("kbarx_side52" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side52 = new TH2D("kbary_side52" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side52 = new TH2D("kbarz_side52" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side53 = new TH2D("kbarx_side53" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side53 = new TH2D("kbary_side53" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side53 = new TH2D("kbarz_side53" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side54 = new TH2D("kbarx_side54" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side54 = new TH2D("kbary_side54" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side54 = new TH2D("kbarz_side54" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side234 = new TH2D("kbarx_side234" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side234 = new TH2D("kbary_side234" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side234 = new TH2D("kbarz_side234" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side235 = new TH2D("kbarx_side235" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side235 = new TH2D("kbary_side235" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side235 = new TH2D("kbarz_side235" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side242 = new TH2D("kbarx_side242" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side242 = new TH2D("kbary_side242" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side242 = new TH2D("kbarz_side242" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side243 = new TH2D("kbarx_side243" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side243 = new TH2D("kbary_side243" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side243 = new TH2D("kbarz_side243" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side245 = new TH2D("kbarx_side245" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side245 = new TH2D("kbary_side245" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side245 = new TH2D("kbarz_side245" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side253 = new TH2D("kbarx_side253" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side253 = new TH2D("kbary_side253" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side253 = new TH2D("kbarz_side253" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side254 = new TH2D("kbarx_side254" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side254 = new TH2D("kbary_side254" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side254 = new TH2D("kbarz_side254" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  //TH2D*   kbarx_side321 = new TH2D("kbarx_side321" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  //TH2D*   kbary_side321 = new TH2D("kbary_side321" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  //TH2D*   kbarz_side321 = new TH2D("kbarz_side321" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side325 = new TH2D("kbarx_side325" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side325 = new TH2D("kbary_side325" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side325 = new TH2D("kbarz_side325" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);


  TH2D*   kbarx_side342 = new TH2D("kbarx_side342" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side342 = new TH2D("kbary_side342" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side342 = new TH2D("kbarz_side342" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side345 = new TH2D("kbarx_side345" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side345 = new TH2D("kbary_side345" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side345 = new TH2D("kbarz_side345" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side352 = new TH2D("kbarx_side352" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side352 = new TH2D("kbary_side352" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side352 = new TH2D("kbarz_side352" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side353 = new TH2D("kbarx_side353" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side353 = new TH2D("kbary_side353" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side353 = new TH2D("kbarz_side353" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side354 = new TH2D("kbarx_side354" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side354 = new TH2D("kbary_side354" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side354 = new TH2D("kbarz_side354" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side423 = new TH2D("kbarx_side423" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side423 = new TH2D("kbary_side423" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side423 = new TH2D("kbarz_side423" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side424 = new TH2D("kbarx_side424" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side424 = new TH2D("kbary_side424" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side424 = new TH2D("kbarz_side424" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side425 = new TH2D("kbarx_side425" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side425 = new TH2D("kbary_side425" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side425 = new TH2D("kbarz_side425" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side432 = new TH2D("kbarx_side432" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side432 = new TH2D("kbary_side432" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side432 = new TH2D("kbarz_side432" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side452 = new TH2D("kbarx_side452" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side452 = new TH2D("kbary_side452" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side452 = new TH2D("kbarz_side452" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side523 = new TH2D("kbarx_side523" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side523 = new TH2D("kbary_side523" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side523 = new TH2D("kbarz_side523" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side532 = new TH2D("kbarx_side532" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side532 = new TH2D("kbary_side532" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side532 = new TH2D("kbarz_side532" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side534 = new TH2D("kbarx_side534" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side534 = new TH2D("kbary_side534" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side534 = new TH2D("kbarz_side534" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side535 = new TH2D("kbarx_side535" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side535 = new TH2D("kbary_side535" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side535 = new TH2D("kbarz_side535" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side542 = new TH2D("kbarx_side542" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side542 = new TH2D("kbary_side542" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side542 = new TH2D("kbarz_side542" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  TH2D*   kbarx_side543 = new TH2D("kbarx_side543" ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side543 = new TH2D("kbary_side543" ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side543 = new TH2D("kbarz_side543" ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

   list<PndDrcPhoton> list_photon;
  
  for (int ievent=0; ievent<nevents; ievent++)
    {
      if (ievent%100000==0) cout<<" ievent="<<ievent<<endl;
      list_photon.clear();
      // create a list of photons in bar
      XYZPoint  pos(0,0,+2); // 1mm below bar at the
      // bars half length.
      phi   = ran.Uniform(0,2*pi);
      costh = ran.Uniform();
      sinth = sqrt(1.0L-costh*costh);
      
      double kx = sinth*sin(phi);
      double ky = sinth*cos(phi);
      double kz = -costh;

      angle2d->Fill(acos(costh)*180/3.141592,phi*180/3.1415926);
      
      XYZVector dir(kx,ky,kz); 
      PndDrcPhoton ph;
      ph.SetPosition(pos);
      ph.SetDirection(dir);
      ph.SetWavelength(400);
      list_photon.push_back(ph);
      manager->SetPhotonList(list_photon,"bar");
      manager->Propagate();              // propagate photon
      list_photon = manager->PhotonList();  // get list
      
      list<PndDrcPhoton>::iterator iph;
      int icnt_measured=0;
      for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	{
	  if      ((*iph).Fate()==Drc::kPhotMeasured) 
	    {
	      icnt_measured++;
	      // recontruction
	      // the end of the is at (0,0,0) the photon vector is
	      double xx=(*iph).Position().X();
	      double yy=(*iph).Position().Y();


	      //cout<<"--------"<<endl;
	      list<const PndDrcSurfAbs*>::iterator isurf;
	      list<const PndDrcSurfAbs*> list_surf;
	      list_surf = (*iph).SurfaceList();
	      vector<string> vec_surf(5);
	      vec_surf[0]="---";
	      vec_surf[1]="---";
	      vec_surf[2]="---";
	      vec_surf[3]="---";
	      vec_surf[4]="---";
	      
	      if (list_surf.begin() != list_surf.end())
		{
		  isurf = list_surf.end();
		  isurf--;
		  vec_surf[4] = (*isurf)->Name();
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[3] = (*isurf)->Name();
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[2] = (*isurf)->Name();
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[1] = (*isurf)->Name();
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[0] = (*isurf)->Name();
		    }
		}
	      //cout<<vec_surf[0]<<" "
	      //      <<vec_surf[1]<<" "
	      //      <<vec_surf[2]<<" "
	      //      <<vec_surf[3]<<" "
	      //      <<vec_surf[4]<<endl;


	      


	      if      (vec_surf[2]=="out" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{		  
		  kbarx_side2 -> SetBinContent(kbarx_side2->FindBin(xx,yy),kx);
		  kbary_side2 -> SetBinContent(kbary_side2->FindBin(xx,yy),ky);
		  kbarz_side2 -> SetBinContent(kbarz_side2->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[2]=="out" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side3 -> SetBinContent(kbarx_side3->FindBin(xx,yy),kx);
		  kbary_side3 -> SetBinContent(kbary_side3->FindBin(xx,yy),ky);
		  kbarz_side3 -> SetBinContent(kbarz_side3->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[2]=="out" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  

		  //cout<<" x,y,kx,ky: "<<xx<<" "<<yy<<" "<<kx<<" "<<ky<<endl;
		  kbarx_side4 -> SetBinContent(kbarx_side4->FindBin(xx,yy),kx);
		  kbary_side4 -> SetBinContent(kbary_side4->FindBin(xx,yy),ky);
		  kbarz_side4 -> SetBinContent(kbarz_side4->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[2]=="out" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side5 -> SetBinContent(kbarx_side5->FindBin(xx,yy),kx);
		  kbary_side5 -> SetBinContent(kbary_side5->FindBin(xx,yy),ky);
		  kbarz_side5 -> SetBinContent(kbarz_side5->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[3]=="out" && vec_surf[4]=="pixel") //direct hit
		{		  
		  kbarx_side1 -> SetBinContent(kbarx_side1->FindBin(xx,yy),kx);
		  kbary_side1 -> SetBinContent(kbary_side1->FindBin(xx,yy),ky);
		  kbarz_side1 -> SetBinContent(kbarz_side1->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side2" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side23 -> SetBinContent(kbarx_side23->FindBin(xx,yy),kx);
		  kbary_side23 -> SetBinContent(kbary_side23->FindBin(xx,yy),ky);
		  kbarz_side23 -> SetBinContent(kbarz_side23->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side2" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  kbarx_side24 -> SetBinContent(kbarx_side24->FindBin(xx,yy),kx);
		  kbary_side24 -> SetBinContent(kbary_side24->FindBin(xx,yy),ky);
		  kbarz_side24 -> SetBinContent(kbarz_side24->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side2" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side25 -> SetBinContent(kbarx_side25->FindBin(xx,yy),kx);
		  kbary_side25 -> SetBinContent(kbary_side25->FindBin(xx,yy),ky);
		  kbarz_side25 -> SetBinContent(kbarz_side25->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side3" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side32 -> SetBinContent(kbarx_side32->FindBin(xx,yy),kx);
		  kbary_side32 -> SetBinContent(kbary_side32->FindBin(xx,yy),ky);
		  kbarz_side32 -> SetBinContent(kbarz_side32->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side3" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  kbarx_side34 -> SetBinContent(kbarx_side34->FindBin(xx,yy),kx);
		  kbary_side34 -> SetBinContent(kbary_side34->FindBin(xx,yy),ky);
		  kbarz_side34 -> SetBinContent(kbarz_side34->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side3" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side35 -> SetBinContent(kbarx_side35->FindBin(xx,yy),kx);
		  kbary_side35 -> SetBinContent(kbary_side35->FindBin(xx,yy),ky);
		  kbarz_side35 -> SetBinContent(kbarz_side35->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side4" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side42 -> SetBinContent(kbarx_side42->FindBin(xx,yy),kx);
		  kbary_side42 -> SetBinContent(kbary_side42->FindBin(xx,yy),ky);
		  kbarz_side42 -> SetBinContent(kbarz_side42->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side4" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side43 -> SetBinContent(kbarx_side43->FindBin(xx,yy),kx);
		  kbary_side43 -> SetBinContent(kbary_side43->FindBin(xx,yy),ky);
		  kbarz_side43 -> SetBinContent(kbarz_side43->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side4" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side45 -> SetBinContent(kbarx_side45->FindBin(xx,yy),kx);
		  kbary_side45 -> SetBinContent(kbary_side45->FindBin(xx,yy),ky);
		  kbarz_side45 -> SetBinContent(kbarz_side45->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side5" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side52 -> SetBinContent(kbarx_side52->FindBin(xx,yy),kx);
		  kbary_side52 -> SetBinContent(kbary_side52->FindBin(xx,yy),ky);
		  kbarz_side52 -> SetBinContent(kbarz_side52->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side5" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side53 -> SetBinContent(kbarx_side53->FindBin(xx,yy),kx);
		  kbary_side53 -> SetBinContent(kbary_side53->FindBin(xx,yy),ky);
		  kbarz_side53 -> SetBinContent(kbarz_side53->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[1]=="out" && vec_surf[2]=="side5" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  kbarx_side54 -> SetBinContent(kbarx_side54->FindBin(xx,yy),kx);
		  kbary_side54 -> SetBinContent(kbary_side54->FindBin(xx,yy),ky);
		  kbarz_side54 -> SetBinContent(kbarz_side54->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side2" && vec_surf[2]=="side3" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  kbarx_side234 -> SetBinContent(kbarx_side234->FindBin(xx,yy),kx);
		  kbary_side234 -> SetBinContent(kbary_side234->FindBin(xx,yy),ky);
		  kbarz_side234 -> SetBinContent(kbarz_side234->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side2" && vec_surf[2]=="side3" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side235 -> SetBinContent(kbarx_side235->FindBin(xx,yy),kx);
		  kbary_side235 -> SetBinContent(kbary_side235->FindBin(xx,yy),ky);
		  kbarz_side235 -> SetBinContent(kbarz_side235->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side2" && vec_surf[2]=="side4" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side242 -> SetBinContent(kbarx_side242->FindBin(xx,yy),kx);
		  kbary_side242 -> SetBinContent(kbary_side242->FindBin(xx,yy),ky);
		  kbarz_side242 -> SetBinContent(kbarz_side242->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side2" && vec_surf[2]=="side4" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side243 -> SetBinContent(kbarx_side243->FindBin(xx,yy),kx);
		  kbary_side243 -> SetBinContent(kbary_side243->FindBin(xx,yy),ky);
		  kbarz_side243 -> SetBinContent(kbarz_side243->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side2" && vec_surf[2]=="side4" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side245 -> SetBinContent(kbarx_side245->FindBin(xx,yy),kx);
		  kbary_side245 -> SetBinContent(kbary_side245->FindBin(xx,yy),ky);
		  kbarz_side245 -> SetBinContent(kbarz_side245->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side2" && vec_surf[2]=="side5" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side253 -> SetBinContent(kbarx_side253->FindBin(xx,yy),kx);
		  kbary_side253 -> SetBinContent(kbary_side253->FindBin(xx,yy),ky);
		  kbarz_side253 -> SetBinContent(kbarz_side253->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side2" && vec_surf[2]=="side5" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  kbarx_side254 -> SetBinContent(kbarx_side254->FindBin(xx,yy),kx);
		  kbary_side254 -> SetBinContent(kbary_side254->FindBin(xx,yy),ky);
		  kbarz_side254 -> SetBinContent(kbarz_side254->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side3" && vec_surf[2]=="side2" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side325 -> SetBinContent(kbarx_side325->FindBin(xx,yy),kx);
		  kbary_side325 -> SetBinContent(kbary_side325->FindBin(xx,yy),ky);
		  kbarz_side325 -> SetBinContent(kbarz_side325->FindBin(xx,yy),kz);  
		}
	      //else if (vec_surf[0]=="out" && vec_surf[1]=="side3" && vec_surf[2]=="side2" && vec_surf[3]=="side1" && vec_surf[4]=="pixel")
	      //{
	      //  kbarx_side321 -> SetBinContent(kbarx_side321->FindBin(xx,yy),kx);
	      //  kbary_side321 -> SetBinContent(kbary_side321->FindBin(xx,yy),ky);
	      //  kbarz_side321 -> SetBinContent(kbarz_side321->FindBin(xx,yy),kz);  
	      //}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side3" && vec_surf[2]=="side4" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side342 -> SetBinContent(kbarx_side342->FindBin(xx,yy),kx);
		  kbary_side342 -> SetBinContent(kbary_side342->FindBin(xx,yy),ky);
		  kbarz_side342 -> SetBinContent(kbarz_side342->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side3" && vec_surf[2]=="side4" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side345 -> SetBinContent(kbarx_side345->FindBin(xx,yy),kx);
		  kbary_side345 -> SetBinContent(kbary_side345->FindBin(xx,yy),ky);
		  kbarz_side345 -> SetBinContent(kbarz_side345->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side3" && vec_surf[2]=="side5" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side352 -> SetBinContent(kbarx_side352->FindBin(xx,yy),kx);
		  kbary_side352 -> SetBinContent(kbary_side352->FindBin(xx,yy),ky);
		  kbarz_side352 -> SetBinContent(kbarz_side352->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side3" && vec_surf[2]=="side5" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side353 -> SetBinContent(kbarx_side353->FindBin(xx,yy),kx);
		  kbary_side353 -> SetBinContent(kbary_side353->FindBin(xx,yy),ky);
		  kbarz_side353 -> SetBinContent(kbarz_side353->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side3" && vec_surf[2]=="side5" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  kbarx_side354 -> SetBinContent(kbarx_side354->FindBin(xx,yy),kx);
		  kbary_side354 -> SetBinContent(kbary_side354->FindBin(xx,yy),ky);
		  kbarz_side354 -> SetBinContent(kbarz_side354->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side4" && vec_surf[2]=="side2" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side423 -> SetBinContent(kbarx_side423->FindBin(xx,yy),kx);
		  kbary_side423 -> SetBinContent(kbary_side423->FindBin(xx,yy),ky);
		  kbarz_side423 -> SetBinContent(kbarz_side423->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side4" && vec_surf[2]=="side2" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  kbarx_side424 -> SetBinContent(kbarx_side424->FindBin(xx,yy),kx);
		  kbary_side424 -> SetBinContent(kbary_side424->FindBin(xx,yy),ky);
		  kbarz_side424 -> SetBinContent(kbarz_side424->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side4" && vec_surf[2]=="side2" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side425 -> SetBinContent(kbarx_side425->FindBin(xx,yy),kx);
		  kbary_side425 -> SetBinContent(kbary_side425->FindBin(xx,yy),ky);
		  kbarz_side425 -> SetBinContent(kbarz_side425->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side4" && vec_surf[2]=="side3" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side432 -> SetBinContent(kbarx_side432->FindBin(xx,yy),kx);
		  kbary_side432 -> SetBinContent(kbary_side432->FindBin(xx,yy),ky);
		  kbarz_side432 -> SetBinContent(kbarz_side432->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side4" && vec_surf[2]=="side5" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side452 -> SetBinContent(kbarx_side452->FindBin(xx,yy),kx);
		  kbary_side452 -> SetBinContent(kbary_side452->FindBin(xx,yy),ky);
		  kbarz_side452 -> SetBinContent(kbarz_side452->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side5" && vec_surf[2]=="side2" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side523 -> SetBinContent(kbarx_side523->FindBin(xx,yy),kx);
		  kbary_side523 -> SetBinContent(kbary_side523->FindBin(xx,yy),ky);
		  kbarz_side523 -> SetBinContent(kbarz_side523->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side5" && vec_surf[2]=="side3" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side532 -> SetBinContent(kbarx_side532->FindBin(xx,yy),kx);
		  kbary_side532 -> SetBinContent(kbary_side532->FindBin(xx,yy),ky);
		  kbarz_side532 -> SetBinContent(kbarz_side532->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side5" && vec_surf[2]=="side3" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  kbarx_side534 -> SetBinContent(kbarx_side534->FindBin(xx,yy),kx);
		  kbary_side534 -> SetBinContent(kbary_side534->FindBin(xx,yy),ky);
		  kbarz_side534 -> SetBinContent(kbarz_side534->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side5" && vec_surf[2]=="side3" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side535 -> SetBinContent(kbarx_side535->FindBin(xx,yy),kx);
		  kbary_side535 -> SetBinContent(kbary_side535->FindBin(xx,yy),ky);
		  kbarz_side535 -> SetBinContent(kbarz_side535->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side5" && vec_surf[2]=="side4" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{
		  kbarx_side542 -> SetBinContent(kbarx_side542->FindBin(xx,yy),kx);
		  kbary_side542 -> SetBinContent(kbary_side542->FindBin(xx,yy),ky);
		  kbarz_side542 -> SetBinContent(kbarz_side542->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[0]=="out" && vec_surf[1]=="side5" && vec_surf[2]=="side4" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side543 -> SetBinContent(kbarx_side543->FindBin(xx,yy),kx);
		  kbary_side543 -> SetBinContent(kbary_side543->FindBin(xx,yy),ky);
		  kbarz_side543 -> SetBinContent(kbarz_side543->FindBin(xx,yy),kz);  
		}
	      else
		{
		  if (vec_surf[0]=="---" || vec_surf[0]=="out")
		    {
		      
		      cerr<<" *** untreated combination at event "<<ievent<<endl;
		      for (int i=0;i<5;i++) cout<<vec_surf[i]<<endl;
		      //if (vec_surf[0]=="out") exit (EXIT_FAILURE);
		    }
		  
		}
	      





		  //for(isurf=list_surf.begin(); isurf != list_surf.end(); ++isurf)
		  //{
		  //cout<<(*isurf)->Name()<<endl;
		  //}
	      
	      
	      
	      
	      // -----------------------------------------------------------------
	    }
	}
      
      //hphotons->Fill(icnt_measured);
      //cout<<"event:\t "<<ievent<<" measured photons: \t"<<icnt_measured<<endl;
    }
  
  hfile->Write();
  hfile->Close();
  
    geo<<"}"<<endl;
  geo.close();

  
  delete manager;
  
  return EXIT_SUCCESS;
  
}
