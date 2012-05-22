



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
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptLens.h"

int main(int argc, char *argv[])
{

  // Example for construction of a lens.

  PndDrcOptLens lens(20,20,10);//,-100,+100);
  lens.SetOptMaterial(PndDrcOptMatLithotecQ0());
  lens.SetName("lens");

  lens.Print();

  PndDrcOptDevSys opt_system;
  opt_system.AddDevice(lens);
  
  // The manager must be created as pointer. 
  // It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->AddDeviceSystem(opt_system);


  // create a list of photons in bar

  fstream geo;
  geo.open("Geo.C",std::ios::out);
  geo<<"{"<<endl;
  geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  cout<<" root version: "<< (long int)((TROOT*)gROOT -> GetVersionInt())<<endl;
  if ( ((TROOT*)gROOT)->GetVersionInt() < 51600)
    {
      geo<<"    TView *view = new TView(1);"<<endl;
    }
  else
    {  
      geo<<"    TView *view = TView::CreateView(1);"<<endl;
    }
  geo<<"    view->SetRange(-100,-100,-100,100,100,100);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(180,45,90,i);"<<endl;
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager->Print(geo);
  geo<<"}"<<endl;
  geo.close();


  delete manager;

  return EXIT_SUCCESS;

}
