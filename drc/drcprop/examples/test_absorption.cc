


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

#include "PndDrcSurfAbs.h"
#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"


int main(int argc, char *argv[])
{

  /*
    check absorption routine in a bar and more directly

    output is test_absorption.root
    with spectra hp1 and hp2
    hp1 is the range in meters of photons as it comes from the absorption 
    routine directly
    hp2 is the spectrum of randomly emitted photons at the end of a 20m long bar
    which make it to the detector. The ordinate is pathlength.
    
  */

  const double pi=3.1415926535;


  int verbosity = 0; // 0=quiet, 1=constructors,2=member,3=functionality
  //                    4=photons, 5=everything.



  // 8 points define a bar, in this case a 1x1x2 m large volume

  XYZPoint p1(-500.0, +500.0,-1000);    //         p5----------p8
  XYZPoint p2(-500.0, -500.0,-1000);    //        /|          /|
  XYZPoint p3(+500.0, -500.0,-1000);    //       / |         / |
  XYZPoint p4(+500.0, +500.0,-1000);    //      /  p6-------/--p7
  //                                         /  /        /  /
  //                                        /  /        /  /
  //                                       /  /        /  /
  //                                      /  /        /  /
  XYZPoint p5(-500.0, +500.0,+1000);        // p1---------p4  /
  XYZPoint p6(-500.0, -500.0,+1000);        // | /         | /
  XYZPoint p7(+500.0, -500.0,+1000);        // |/          |/
  XYZPoint p8(+500.0, +500.0,+1000);        // p2---------p3 


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

  PndDrcOptReflPerfect refl;
  a1.SetReflectivity(refl);
  a2.SetReflectivity(refl);
  a3.SetReflectivity(refl);
  a4.SetReflectivity(refl);
  a5.SetReflectivity(refl);
  a6.SetReflectivity(refl);


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
  opt_system.SetNameCopyNumber("opt_system",0);
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
  geo<<"    view->SetRange(-5000,-5000,-5000,5000,5000,5000);"<<endl;
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

  double lambda=400;
  PndDrcPhoton ph;
  list<PndDrcPhoton> list_photon;

  TRandom ran;

  cout<<" create 5000 photons ... "<<endl;;
  for (int i=0; i<5000; i++)
    {
      double costh = -1.0 + 2.0*ran.Uniform();
      double sinth = sqrt(1.0-costh*costh);
      double phi   = 2*pi*ran.Uniform();
      double xdir  = sinth * sin(phi);
      double ydir  = sinth * cos(phi);
      double zdir  = costh;
      double xpos  = -499 + 999 * ran.Uniform();
      double ypos  = -499 + 999 * ran.Uniform();
      double zpos  = -999 +1999 * ran.Uniform();
      ph.SetPosition(XYZPoint(xpos,ypos,zpos));
      ph.SetDirection(XYZVector(xdir,ydir,zdir));
      ph.SetWavelength(lambda);
      list_photon.push_back(ph);
    }
  manager->SetPhotonList(list_photon,"bar","opt_system",0,0);
  cout<<" done."<<endl;

  cout<<" propagate photons ... (10 minutes)"<<endl;
  manager->Propagate();                           // propagate photons
  cout<<" done."<<endl;

  list_photon = manager->PhotonList(); // get list
  


  // propagate writes to geo, that has finished, therefore, close geo
  geo<<"}"<<endl;
  geo.close();


  // write to screen...


  cout<<" analyse photon ... "<<endl;
  TFile* hfile = new TFile("test_absorption.root","RECREATE",
			   "Absorption test");
  TH1F*  hp2   = new TH1F("hp2","Absorption in bar [m] @ 400 nm",1000,0,2000);

  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;

  double absorption=0;
  int    icnt_abs=0;
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if ((*iph).Fate()==Drc::kPhotMeasured)
	{
	  icnt_measured++;
	}
      else if ((*iph).Fate()==Drc::kPhotFlying)   
	{
	  icnt_flying++; // should never happen.
	}
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) 
	{
	  double n = quartz.RefIndex(lambda);
	  double dndl = quartz.RefIndexDeriv(lambda);
	  double v_phase = 299.792/n;
	  double v_group =  v_phase * ( 1.0 - lambda/n*dndl);
	  double t    = (*iph).Time();
	  double len  = t*v_group/1000;
	  hp2->Fill(len);
	  absorption += len;
	  icnt_abs++;
	  icnt_absorbed++;
	}
      else                                       
	{
	  icnt_lost++;
	}
    }
  cout<<" done."<<endl;


  cout<<" absorption length in bar: "<<absorption/icnt_abs<<" m"<<endl;

  cout<<" calling routine directly...";
  absorption = 0;
  icnt_abs   = 0;
  TH1F*  hp1   = new TH1F("hp1","Absorption direct [m] @ 400 nm",1000,0,2000);
  for (int length=0; length<10000; length++)
    {
      for (int i=0;i<1000;i++)
	{
	  if (!quartz.AbsorptionFlag(lambda,(length*1000))) 
	    {
	      hp1->Fill(length);
	      absorption += length;
	      icnt_abs++;
	    }
	}
    }
  cout<<" done."<<endl;
  cout<<" absorption length directly: "<<absorption/icnt_abs<<" m"<<endl;

  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;

  hfile->Write();

  cout<<" result spectra in test_absorption.root"<<endl;
  delete manager;

  return EXIT_SUCCESS;

}
