


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
#include "PndDrcOptMatLithotecQ0.h"
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


  double half_width  = 200.0/2;   //mm
  double half_thick  = 17.0/2;    //mm
  double half_length = 2500.0/2;  //mm

  double lens_radius = 8999.0;     // mm


  PndDrcOptDevSys opt_system;
  
  PndDrcOptBrik sheet(half_width,half_thick,half_length);
  sheet.Surface("side6")->SetReflectivity(PndDrcOptReflSilver());
  sheet.SetOptMaterial(PndDrcOptMatLithotecQ0());
  sheet.SetName("sheet");
  //sheet.Surface("side1")->SetPixel();
  sheet.SetVerbosity(0);
  
  // move sheet such into positive z space such that end of sheet is at z=0
  sheet.AddTransform(Transform3D(XYZVector(0,0,half_length)));

  opt_system.AddDevice(sheet);


  // create two adjacent lenses, one made of quartz and one made of air.
  // and embedd the into sheet.
  // since the curvature is around the y-axis, create it rotated by 90 degree and rotate it back
  double lens_body_hthick = 5.0;
  

  PndDrcOptCylLens lens_quartz(half_thick,half_width,lens_body_hthick,lens_radius,99999.9);
  lens_quartz.SetOptMaterial(PndDrcOptMatLithotecQ0());
  lens_quartz.AddTransform(Transform3D(XYZVector(0,0,-lens_body_hthick)));
  lens_quartz.AddTransform(Transform3D(RotationZ(kPi/2)));
  lens_quartz.SetName("lens_quartz");
  lens_quartz.SetPrintColor(2);
  lens_quartz.SetVerbosity(0);

  opt_system.AddDevice(lens_quartz);

  PndDrcOptCylLens lens_air(half_thick,half_width,lens_body_hthick,99999.9,-lens_radius);
  lens_air.AddTransform(Transform3D(XYZVector(0,0,-(2+1)*lens_body_hthick)));
  lens_air.AddTransform(Transform3D(RotationZ(kPi/2)));
  //lens_air.SetOptMaterial(PndDrcOptMatVacuum());
  lens_air.SetOptMaterial(PndDrcOptMatLithotecQ0());
  lens_air.SetName("lens_air");
  lens_air.SetPrintColor(4);
  lens_air.SetVerbosity(0);
  

  opt_system.AddDevice(lens_air);

  double ex_box_hw = 300;
  double ex_box_ht = 300;
  double ex_box_hl = 100;
  
  PndDrcOptBrik ex_box(ex_box_hw,ex_box_ht,ex_box_hl);
  ex_box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  ex_box.SetName("expansion box");
  ex_box.Surface("side6")->SetPixel();
  ex_box.AddTransform(Transform3D(XYZVector(0,0,-ex_box_hl-(2+2)*lens_body_hthick)));
  ex_box.Surface("side1")->SetPixel();
  //ex_box.SetVerbosity(5);
  

  opt_system.AddDevice(ex_box);
  


  // coupling
  // -z                                                                                                +z
  // (side1--ex_box--side6)  (side1--lens_air--side6)  (side1--lena_quartz--side6) (side1_sheet--side6)



  // couple surface 1 of device 1 with surface 2 of device 2
  //                       dev1   dev2    surf1     surf2
  opt_system.CoupleDevice("sheet"      ,"lens_quartz"  ,"side1", "side6");
  opt_system.CoupleDevice("lens_quartz","lens_air"     ,"side1", "side6");
  opt_system.CoupleDevice("lens_air"   ,"expansion box","side1", "side6");
  

  



  
  // The manager must be created as pointer. It is created as singleton, that is only 
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

  XYZPoint  pos(0,-half_thick-10.0,half_length);
  XYZVector dir(0,1,10); 
  double   beta = 0.784;
  bool photons_exist = manager->Cerenkov(pos,dir,beta); // generate photons

  /*
  PndDrcPhoton ph;
  ph.SetReflectionLimit(200);  
  list<PndDrcPhoton> list_photon; // get list
  TRandom ran;
  for (int i=0; i<50; i++)
    {
      ph.SetPosition(XYZPoint(ran.Uniform(-100,100),ran.Uniform(-8.5,8.5),10));
      //ph.SetPosition(XYZPoint(0,0,10));
    double y = ran.Uniform(-0.4,0.4);
    double z = sqrt(1-y*y);
    
    ph.SetDirection(XYZVector(0,y,z));
    ph.SetWavelength(400);
    ph.SetDevice(manager->Device("sheet")); 
    list_photon.push_back(ph);
  }
  manager->SetPhotonList(list_photon,"sheet");
  */


  list<PndDrcPhoton> list_photon0 = manager->PhotonList();  // get list


  if (photons_exist) manager->Propagate();              // propagate photons


  list<PndDrcPhoton> list_photon = manager->PhotonList();  // get list
  
  geo<<"}"<<endl;     // here it is...




  // analyse list

  fstream out;
  out.open("debug.dat",std::ios::out);
  
  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;
  list<PndDrcPhoton>::iterator iph;
  list<PndDrcPhoton>::iterator iph0;
  for(iph=list_photon.begin(), iph0=list_photon0.begin(); iph != list_photon.end(); ++iph,++iph0) 
    {
      int icode = 4;
      
      if      ((*iph).Fate()==Drc::kPhotMeasured) {icode=1;icnt_measured++;}
      
      else if ((*iph).Fate()==Drc::kPhotFlying)   {icode=2;icnt_flying++;} // should never happen.
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) {icode=3;icnt_absorbed++;}
      else icnt_lost++;
 
     out
       <<(*iph).Position().X()<<" "
       <<(*iph).Position().Y()<<" "
       <<(*iph).Position().Z()<<" "
       <<(*iph).Direction().X()<<" "
       <<(*iph).Direction().Y()<<" "
       <<(*iph).Direction().Z()<<" "
       <<(*iph).Wavelength()<<" "<<icode<<endl;
     
       
     
    }
  out.close();
  
  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;

  delete manager;

  return EXIT_SUCCESS;

}
