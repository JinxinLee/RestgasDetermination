


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
#include "PndDrcSurfPolyFlatFocus.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatMarcol7.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"
#include "PndDrcOptCylLens.h"
#include "PndDrcOptLens.h"

int main(int argc, char *argv[])
{

  static const double kPi=3.1415926535;

  // Example for a simple sheet with lens and expansion box.


  //---------------------------------------------------------

  
  //int ioption = 1; // 1=cherenkov
  int ioption = 2; // angular frames
  // else=testbeam

  int verbosity = 0;
  
  double half_width  = 200.0  /2;   //mm
  double half_thick  = 17.0   /2;    //mm
  double half_length = 2500.0 /2;  //mm

  //---------------------------------------------------------


  // ex_box=300mm + 10mm air gap => 
  // for air filled box radius = 310mm = 1/(1.47-1) R
  // R = 310 * 0.47 = 145.7
  //
  // the medium in the box enhances the focal length to 300mm *1.47 
  // therefore use a focal length
  // of 145.7/1.47 = 99.1
  // or more precise: R = (300/1.47 + 10) *0.47 = 100.6

  // mirror:
  // create a focussing mirror at the downstream side
  // with width like quartz bar, length 40 mm
  // the focusing distance is 2500+300 mm => radius = 5600 mm


  

  PndDrcOptDevSys opt_system;

  // 8 points define a sheet

  XYZPoint p1(-half_width, +half_thick, half_length);    //         p1----------p4
  XYZPoint p2(-half_width, -half_thick, half_length);    //        /|          /|
  XYZPoint p3(+half_width, -half_thick, half_length);    //       / |         / |
  XYZPoint p4(+half_width, +half_thick, half_length);    //      /  p2-------/--p3
  //                                                            /  /        /  /
  //                                                           /  /        /  /
  //                                                          /  /        /  /
  //                                                         /  /        /  /
  XYZPoint p5(-half_width, +half_thick, -half_length);  // p5---------p8  /
  XYZPoint p6(-half_width, -half_thick, -half_length);  // | /         | /
  XYZPoint p7(+half_width, -half_thick, -half_length);  // |/          |/
  XYZPoint p8(+half_width, +half_thick, -half_length);  // p6---------p7 


  // How to produce surfaces by shift and rotate operation is for sake of clearness
  // not shown here, but in one of the other examples.

  PndDrcOptReflPerfect refl;

  // Declare flat surfaces with arbitrary number of points.

  PndDrcSurfPolyFlatFocus a1;
  PndDrcSurfPolyFlat a2,a3,a4,a5,a6;

  a1.SetVerbosity(verbosity);
  a1.SetReflectivity(refl);  // this acts as a mirror
  a1.AddPoint(p1);
  a1.AddPoint(p2);
  a1.AddPoint(p3);
  a1.AddPoint(p4);
  //a1.SetFocalPoint(XYZPoint(0,0,-half_length-300)); // before trafo
  a1.SetFocalPoint(XYZPoint(0,0,-half_length-5)); // before trafo
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
  a6.SetPrintColor(2);
  a6.SetName("aup");


  // create a volume consiting of surfaces
  // create a material the sheet will consist of
  PndDrcOptVol sheet;
  PndDrcOptMatLithotecQ0 quartz;
  sheet.SetVerbosity(verbosity);
  sheet.SetOptMaterial(quartz);
  sheet.AddSurface(a1);
  sheet.AddSurface(a2);
  sheet.AddSurface(a3);
  sheet.AddSurface(a4);
  sheet.AddSurface(a5);
  sheet.AddSurface(a6);
  sheet.SetName("sheet");

  sheet.AddTransform(Transform3D(XYZVector(0,0,half_length)));
  opt_system.AddDevice(sheet);

  
// expansion box
  double ex_box_hw = 300;
  double ex_box_ht = 300;
  double ex_box_hl = 150;
  
  PndDrcOptBrick ex_box(ex_box_hw,ex_box_ht,ex_box_hl);
  ex_box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  ex_box.SetName("expansion box");
  ex_box.AddTransform(Transform3D(XYZVector(0,0,-ex_box_hl)));
  ex_box.Surface("side1")->SetPixel();
  opt_system.AddDevice(ex_box);
  
  opt_system.CoupleDevice("sheet" ,"expansion box","aup", "side6");
  
  

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
  if (ioption==2) manager->Print(geo);
  //
  // the intention is to play around with routines.
  // there hast to come another geo output after propagation...
  // manager->Print(); // print out to screen everything...

  // create a list of photons in sheet

  XYZPoint  pos(0,-half_thick-10.0,half_length);
  XYZVector dir(0,1,1); 
  double   beta = 0.80;//0.684;
  bool photons_exist = false;
  
  list<PndDrcPhoton> list_photon;  

  if (ioption==1)
    {
      photons_exist = manager->Cerenkov(pos,dir,beta,1000000,1.e16,440,450); // generate photons


      list_photon = manager->PhotonList();  // get list
      list<PndDrcPhoton>::iterator iph;
      for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	{
	  (*iph).SetPrintFlag(false);
	}
      manager->SetPhotonList(list_photon,"sheet");
    }
  if (ioption==2)  
    {
      for (double xx=-80; xx<=80; xx+=5)
	{
	  for (double yy=-8; yy<=8; yy+=2)
	    {
	      PndDrcPhoton ph;
	      
	      //double xx=0;
	      //double yy=0;
	      ph.SetPosition(XYZPoint(xx,yy,1225));
	      ph.SetDirection(XYZVector(0,0,+1));
	      ph.SetWavelength(650);
	      list_photon.push_back(ph);
	      
	      double angle = kPi/180.0 * 20.0;
	      ph.SetDirection(XYZVector(0,sin(+angle),+cos(+angle)));
	      ph.SetWavelength(521);
	      list_photon.push_back(ph);
	      ph.SetDirection(XYZVector(0,sin(-angle),+cos(-angle)));
	      ph.SetWavelength(521);
	      list_photon.push_back(ph);
	     
	      angle = kPi/180.0 * 40.0;
	      ph.SetDirection(XYZVector(0,sin(+angle),+cos(+angle)));
	      ph.SetWavelength(451);
	      list_photon.push_back(ph);
	      ph.SetDirection(XYZVector(0,sin(-angle),+cos(-angle)));
	      ph.SetWavelength(451);
	      list_photon.push_back(ph);
	      
	    }
	}
      photons_exist = true;
      manager->SetPhotonList(list_photon,"sheet");
    }
  else
    {
      PndDrcPhoton ph;
      ph.SetReflectionLimit(200);  
      list<PndDrcPhoton> list_photon;
      int imax=10; // rays in one dimension
      for (int ix=0; ix<imax; ix++)
	//  int ix=0;
	{
	  for (int iy=2; iy<imax; iy++)	
	{
	  
	  //for (double theta=35; theta<=35; theta+=10)
	  double theta = 0;
	  
	    {    
	      ph.SetPosition(XYZPoint(
				      -half_width+1+ix*(2*half_width-2)/(imax-1),
				      -half_thick+1+iy*(2*half_thick-2)/(imax-1),
				      half_length));
	      double z = cos(theta*kPi/180);
	      double y = sin(theta*kPi/180);
	      ph.SetDirection(XYZVector(0,y,z));
	      ph.SetWavelength(550);
	      ph.SetDevice(manager->Device("sheet")); 
	      //ph.SetSeed();
	      
	      list_photon.push_back(ph);
	    }
	}
      
	  
	}
      photons_exist=true;
      manager->SetPhotonList(list_photon,"sheet");
    }
  
  if (photons_exist) manager->Propagate();              // propagate photons

  
  geo<<"}"<<endl;     // here it is...

  fstream scr;
  scr.open("Screen.C",std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
      scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"test_simple_mirror_sheet y vs x\""
	 <<",500,-500,500);"<<endl;
  scr<<"    hgr->SetStats(0);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-500);"<<endl;
  scr<<"    hgr->SetMaximum(500);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;




  // analyse list

  list_photon = manager->PhotonList();  // get list

  //fstream out;
  //out.open("debug.dat",std::ios::out);
  

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
	  double yy=(*iph).Position().Y();

	  int irefl = (*iph).Reflections()-1; // -1 for mirror
	  cout<<irefl<<endl;
	  //irefl/=5;
	  irefl=irefl%10;
	  
	  int icol=29;

	  if (irefl == 0) icol = 1; // black
	  if (irefl == 1) icol = 28; // brown
	  if (irefl == 2) icol = 2; // red
	  if (irefl == 3) icol = 42; // orange
	  if (irefl == 4) icol = 5; // yellow
	  if (irefl == 5) icol = 3; // green
	  if (irefl == 6) icol = 4; // blue
	  if (irefl == 7) icol = 6; // violett
	  if (irefl == 8) icol = 14; // gray
	  if (irefl == 9) icol = 18; // white

	  //if (irefl<10)
	    {
	      
	      scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	      scr<<"    t->SetMarkerColor("
		 <<icol
		 <<");"<<endl;
	      //	  scr<<"    t->SetMarkerColor("
	      //<<(*iph).ColorNumber((*iph).Wavelength())
	      //<<");"<<endl;
	      scr<<"    t->SetMarkerSize(0.2);"<<endl;
	      scr<<"    t->Draw();"<<endl;
	    }
	  
	}
      else if ((*iph).Fate()==Drc::kPhotFlying)   {icnt_flying++;}
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) {icnt_absorbed++;}
      else 
	{
	  //out<<(icnt1++)<<" "
	  // <<(*iph).Position().X()<<" "
	  // <<(*iph).Position().Y()<<" "
	  // <<(*iph).Position().Z()<<" "
	  // <<(*iph).Wavelength()<<endl;
	  icnt_lost++;
	}
      
    }
  //out.close();
  

  scr<<"}"<<endl;
  scr.close();
  
  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<" \t100%"<<endl;
  cout<<" measured  photons: "<<icnt_measured<<" \t"<<icnt_measured/float(icnt)*100<<"%"<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<" \t\t"<<icnt_absorbed/float(icnt)*100<<"%"<<endl;
  cout<<" lost      photons: "<<icnt_lost<<" \t\t"<<icnt_lost/float(icnt)*100<<"%"<<endl;

  delete manager;

  return EXIT_SUCCESS;

}
