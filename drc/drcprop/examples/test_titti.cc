


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
#include "PndDrcOptReflSurf.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"


int main(int argc, char *argv[])
{

  PndDrcOptReflSurf refl_surf;
  
  string filename;
  double prob;
  
  cout<< "Give filename and probability" <<endl;
  cin>>filename;
  cin>>prob;
  


  refl_surf.SetProb(prob);
  
  cout<<" probability: "<<refl_surf.Prob()<<endl;
  


  TRandom ran;


  // Example for a simple bar with screen (photon detection) and mirror.

  int verbosity = 0; // 0=quiet, 1=constructors,2=member,3=functionality
  //                    4=photons, 5=everything.


  // 8 points define a bar

  XYZPoint p1(-17.0, +8.0, +400);    //         p5----------p8
  XYZPoint p2(-17.0, -8.0, +400);    //        /|          /|
  XYZPoint p3(+17.0, -8.0, +400);    //       / |         / |
  XYZPoint p4(+17.0, +8.0, +400);    //      /  p6-------/--p7
  //                                     /  /        /  /
  //                                    /  /        /  /
  //                                   /  /        /  /
  //                                  /  /        /  /
  XYZPoint p5(-17.0, +8.0, -400);  // p1---------p4  /
  XYZPoint p6(-17.0, -8.0, -400);  // | /         | /
  XYZPoint p7(+17.0, -8.0, -400);  // |/          |/
  XYZPoint p8(+17.0, +8.0, -400);  // p2---------p3 


  // Define from points 6 surfaces of the bar. The points have to be given in 
  // the sequence going around the surface, clock- or counterclock-wise.
  // There are 2 additional surfaces, a mirror and a screen.

  // How to produce surfaces by shift and rotate operation is for sake of clearness
  // not shown here, but in one of the other examples.

  // Declare flat surfaces with arbitrary number of points.
  PndDrcSurfPolyFlat a1,a2,a3,a4,a5,a6;

  a1.SetVerbosity(verbosity);
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
  a6.SetName("aup");


  a6.SetPixel();
  a1.SetPixel();

  a2.SetReflectivity(refl_surf);
  a3.SetReflectivity(refl_surf);
  a4.SetReflectivity(refl_surf);
  a5.SetReflectivity(refl_surf);
  
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

  // Build a optical system consisting out of several volumes,
  // mirrors and screens.
  // This layer has the advantage, that a device consisting out 
  // of many equal subsystems
  // like a bar box, easily can be reproduced. 
  // See one of the forth comming test examples.

  PndDrcOptDevSys opt_system;
  opt_system.SetVerbosity(verbosity);
  opt_system.AddDevice(bar);


  
  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->SetVerbosity(verbosity);
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
  geo<<"    view->SetRange(-100,-100,-100,100,100,100);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(0,90,90,i);"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  // manager->print(geo);
  //
  // the intention is to play around with routines.


  // create a list of photons in bar




  fstream out;
  out.open(filename.c_str(),std::ios::out);

  for (double zz=-390; zz<=390; zz+=10)
    {

      XYZPoint pos(0,-9,zz);
      double nevents=1000;
      double pi=3.1415926535;



      bool photons_exist = false;
      int icnt_coinc    = 0;
      int icnt_measured = 0;
      int icnt_flying   = 0;
      int icnt_lost     = 0;
      int icnt_absorbed = 0;

      for (int ievent=0; ievent<nevents; ievent++)
	{

	  double costh = ran.Uniform(0,1);// 90-0 deg
	  double phi   = ran.Uniform(0,2*pi);
	  double sinth = sqrt(1-costh*costh);
	  // emission in half sphere
      
	  XYZVector dir(sinth*sin(phi),costh,sinth*cos(phi)); 
	  double    beta = 0.8;
      
	  if (manager->Cerenkov(pos,dir,beta,300,200,400,600)) photons_exist = true; 
  
	  if (photons_exist)
	    {
	      manager->Propagate();                           // propagate photons
	    }

	  list<PndDrcPhoton> list_photon = manager->PhotonList(); // get list
	  manager->ClearPhotonList();
	  



	  bool det1 = false;
	  bool det2 = false;
	  double time1=0;
	  double time2=0;
	  
	  list<PndDrcPhoton>::iterator iph;
	  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	    {
	      //if ((*iph).fate()==PndDrc::kPhotMeasured) cout<<" Measured"<<endl;
	      //if ((*iph).fate()==PndDrc::kPhotFlying)   cout<<" Flying"<<endl;
	      //if ((*iph).fate()==PndDrc::kPhotAbsorbed) cout<<" Absorbed"<<endl;
	      //if ((*iph).fate()==PndDrc::kPhotLost)     cout<<" Lost"<<endl;
	      


	      if ((*iph).Fate()==Drc::kPhotMeasured)
		{
		  icnt_measured++;
		  //double x     = (*iph).position().X();
		  //double y     = (*iph).position().Y();
		  double z     = (*iph).Position().Z();
		  double t     = (*iph).Time();
		  
		  if (fabs(-400-z)<0.1) {det1=true;time1=t;}
		  
		  if (fabs(+400-z)<0.1) {det2=true;time2=t;}

		  //cout<<z<<" "<<(*iph).time()<<endl;
		}
	      else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
	      else if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
	      else                                       icnt_lost++;
	    }

	  if (det1 && det2 /*&& (time2 > time1)*/)
	    {
	      icnt_coinc++;
	      //cout<<" coincident hit of both detectors"<<endl;
	    }
	  else
	    {
	      //cout<<" no coinc."<<endl;
	    }
	  // cout<<" event = "<<ievent<<endl;
	}
      out<<zz<<" "<<icnt_coinc<<endl;
      cout<<zz<<" "<<icnt_coinc<<endl;
    }
  //int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  //cout<<" generated photons: "<<icnt<<endl;
  //cout<<" measured  photons: "<<icnt_measured<<endl;
  //cout<<" coinc.    photons: "<<icnt_coinc<<endl;
  //cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  //cout<<" lost      photons: "<<icnt_lost<<endl;


  // propagate writes to geo, that has finished, therefore, close geo
  geo<<"}"<<endl;
  geo.close();

  delete manager;

  return EXIT_SUCCESS;

}
