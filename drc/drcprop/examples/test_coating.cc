


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
  cube.SetName("cube");
  // move sheet such into positive z space such that end of sheet is at z=0
  cube.AddTransform(Transform3D(XYZVector(0,0,12)));
  opt_system.AddDevice(cube);
  
  PndDrcOptBrik sheet(10.0,10.0,1.0);
  sheet.SetOptMaterial(PndDrcOptMatVacuum());
  sheet.SetName("sheet");
  // move sheet such into positive z space such that end of sheet is at z=0
  sheet.AddTransform(Transform3D(XYZVector(0,0,1)));
  opt_system.AddDevice(sheet);


  PndDrcOptBrik ex_box(100.0,100.0,100.0);
  ex_box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  ex_box.Surface("side6")->SetReflectivity(PndDrcOptReflGeffcken());
  ex_box.SetName("ex_box");
  // move sheet such into positive z space such that end of sheet is at z=0
  ex_box.AddTransform(Transform3D(XYZVector(0,0,-100)));
  ex_box.Surface("side1")->SetPixel();
  

  opt_system.AddDevice(ex_box);
  
  
  // couple surface 1 of device 1 with surface 2 of device 2
  //                       dev1   dev2    surf1     surf2
  opt_system.CoupleDevice("cube"        ,"sheet"  ,"side1", "side6");
  opt_system.CoupleDevice("sheet",       "ex_box" ,"side1", "side6");


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

  bool print = false;
  

  if (print) manager->Print(geo);
  //
  // the intention is to play around with routines.
  // there hast to come another geo output after propagation...
  // manager->Print(); // print out to screen everything...

  // create a list of photons in sheet

  
  PndDrcPhoton ph;
  ph.SetReflectionLimit(200);  
  int icnt=1000;
  
  if (!print)
    {
      fstream out;
      out.open("debug.dat",std::ios::out);
      
      cout<<" lambda loss --> debug.dat"<<endl;
      
      for (double lambda=200; lambda<600; lambda+=20)
	{
      
	  list<PndDrcPhoton> list_photon;
	  for (int i=0; i<icnt; i++)	
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
	  manager->SetPhotonList(list_photon,"cube");
	  manager->Propagate();              // propagate photons
	  list_photon = manager->PhotonList();
	  
	  int icnt_measured = 0;
	  
	  list<PndDrcPhoton>::iterator iph;
	  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	    {
	      if      ((*iph).Fate()==Drc::kPhotMeasured) 
		{
		  icnt_measured++;
		}
	    }
	  cout<<lambda<<" "<<double(icnt-icnt_measured)/icnt<<endl;
	  out<< lambda<<" "<<double(icnt-icnt_measured)/icnt<<endl;
      
	}
  out.close();

    }
  
  
  geo<<"}"<<endl;     // here it is...
  

  delete manager;

  return EXIT_SUCCESS;

}
