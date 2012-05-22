


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
#include "TNtuple.h"
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


int main(int argc, char *argv[])
{

  // Example for a simple bar with screen (photon detection) and mirror.

  int verbosity = 0; // 0=quiet, 1=constructors,2=member,3=functionality
  //                    4=photons, 5=everything.


  // 8 points define a bar

  XYZPoint p1(-100.0, +8.5, +1000);    //         p5----------p8
  XYZPoint p2(-100.0, -8.5, +1000);    //        /|          /|
  XYZPoint p3(+100.0, -8.5, +1000);    //       / |         / |
  XYZPoint p4(+100.0, +8.5, +1000);    //      /  p6-------/--p7
  //                                     /  /        /  /
  //                                    /  /        /  /
  //                                   /  /        /  /
  //                                  /  /        /  /
  XYZPoint p5(-100.0, +8.5, -1000);  // p1---------p4  /
  XYZPoint p6(-100.0, -8.5, -1000);  // | /         | /
  XYZPoint p7(+100.0, -8.5, -1000);  // |/          |/
  XYZPoint p8(+100.0, +8.5, -1000);  // p2---------p3 


  // Define from points 6 surfaces of the bar. The points have to be given in 
  // the sequence going around the surface, clock- or counterclock-wise.
  // There are 2 additional surfaces, a mirror and a screen.

  // How to produce surfaces by shift and rotate operation is for sake of clearness
  // not shown here, but in one of the other examples.


  // Declare flat surfaces with arbitrary number of points.
  PndDrcSurfPolyFlat a1,a2,a3,a4,a5,a6;

  a1.SetVerbosity(verbosity);
  a1.SetReflectivity( PndDrcOptReflSilver());  // mirror
  a1.AddPoint(p1);
  a1.AddPoint(p2);
  a1.AddPoint(p3);
  a1.AddPoint(p4);
  a1.SetName("adown");

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
  a6.SetPixel();         // screen
  a6.SetName("aup");




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
  //opt_system.AddDevice(mirror1);



  // couple surface 1 of device 1 with surface 2 of device 2
  //                       dev1   dev2    surf1     surf2
  //opt_system.coupleDevice("bar","mirror1","aup"  ,"bmirror_front");
  //opt_system.coupleDevice("mirror1","screen","bmirror_back","ascreen");

  
  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->SetVerbosity(verbosity);
  manager->AddDeviceSystem(opt_system);

  fstream geo;

  bool l_vis=false;
  
  if (l_vis)
    {
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
      geo<<"    view->SetRange(-900,-900,-900,900,900,900);"<<endl;
      geo<<"    Int_t i;"<<endl;
      geo<<"    view->SetView(0,90,90,i);"<<endl;
      //manager->Print(geo);
    }
  
  // create a list of photons in bar

  XYZPoint pos(0,-500,0);
  XYZVector dir(0,1,1.7); 


  TFile *f = new TFile("test_top.root","RECREATE");
  TNtuple *ntuple = new TNtuple("ntuple","test_top data","x:y:z:t:m");  

  bool photons_exist=false;
  
  bool l_dispersion=true;

  int nph = 1000  ;

  
  // pions: 
  double   beta = 1.0/sqrt(1.0+0.140*0.140); // = p/E
  if (l_dispersion)
    {
      photons_exist = manager->Cerenkov(pos,dir,beta,nph,1000,400,600); // gen photons
    }
  else
    {
      photons_exist = manager->Cerenkov(pos,dir,beta,nph,1000,400,400); // gen photons
    }
  if (photons_exist) manager->Propagate();                   // propagate photons
  list<PndDrcPhoton> list_photon = manager->PhotonList(); // get list
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if ((*iph).Fate()==Drc::kPhotMeasured)
	{
	  double x     = (*iph).Position().X();
	  double y     = (*iph).Position().Y();
	  double z     = (*iph).Position().Z();
	  double t     = (*iph).Time();
	  double m=140.0;
	  ntuple->Fill(x,y,z,t,m);
	}
    }
  manager->ClearPhotonList();





  // kaons
  beta = 1.0/sqrt(1.0+0.500*0.500);
  
  
  if (l_dispersion)
    {
      photons_exist = manager->Cerenkov(pos,dir,beta,nph,1000,400,600); // gen photons
    }
  else
    {
      photons_exist = manager->Cerenkov(pos,dir,beta,nph,1000,400,400); // gen photons
    }
  
  if (photons_exist) manager->Propagate();                   // propagate photons
  //list_photon.clear();
  list_photon = manager->PhotonList(); // get list
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if ((*iph).Fate()==Drc::kPhotMeasured)
	{
	  double x     = (*iph).Position().X();
	  double y     = (*iph).Position().Y();
	  double z     = (*iph).Position().Z();
	  double t     = (*iph).Time();
	  double m=500.0;
	  ntuple->Fill(x,y,z,t,m);
	}
    }



  f->Write();

  if (l_vis)
    {    
      // propagate writes to geo, that has finished, therefore, close geo
      geo<<"}"<<endl;
      geo.close();
    }
  
  delete manager;


 


  return EXIT_SUCCESS;

}
