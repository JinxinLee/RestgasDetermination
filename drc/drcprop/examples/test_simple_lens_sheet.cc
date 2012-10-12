


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
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptReflGeffcken.h"
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

  //int focus_option = 0; // nothing
  //int focus_option = 1; // lens
  int focus_option = 2; // focussing downstream spherical mirror
  //int focus_option = 3; // focussing downstream cylindrical mirror
  
  
  //int ioption = 1; // 1=cherenkov, 2=testbeam
  //int ioption = 3; // grid
  int ioption =4;// line
  
  int top_option = 0; // 0 with expansion volume, 1 without expansion volume
  // top_option=1 writes times versus x into the Screen.C file.


  //int coating = true;  
  int coating = false;  // don't turn on...



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



  //double lens_radius = 99999.9;     // mm
  double lens_radius = 100.6;       // mm
  double mirror_radius = 5600;
  if (top_option==1) mirror_radius = 5000; // only radiator bar
  

  PndDrcOptDevSys opt_system;
  
  PndDrcOptBrick sheet(half_width,half_thick,half_length);
  sheet.SetOptMaterial(PndDrcOptMatLithotecQ0());
  sheet.SetName("sheet");
  // move sheet such into positive z space such that end of sheet is at z=0
  sheet.AddTransform(Transform3D(XYZVector(0,0,half_length)));
  //sheet.SetVerbosity(5);
  if (top_option==1) sheet.Surface("side1")->SetPixel(); // time of propagation 

  // downstream mirror
  if (focus_option == 0 || focus_option == 1)
    {
      sheet.Surface("side6")->SetReflectivity(PndDrcOptReflSilver());      
    }

  opt_system.AddDevice(sheet);

  if (focus_option == 2) // spherical downstream mirror
    {
      PndDrcOptLens mirror(half_width,half_thick,10/2,9999,mirror_radius,0,-1); // spherical mirror
      mirror.SetOptMaterial(PndDrcOptMatLithotecQ0());
      mirror.SetName("mirror");
      mirror.Surface("side6")->SetReflectivity(PndDrcOptReflSilver()); // the mirror
      mirror.AddTransform(Transform3D(XYZVector(0,0, 2*half_length+10/2)));
      mirror.SetPrintColor(2);  
      opt_system.AddDevice(mirror);
      opt_system.CoupleDevice("mirror","sheet",         "side1","side6");
    }
  if (focus_option == 3) // cylindrical downstream mirror
    { // x=thick y=width --> will be later rotated by pi/2
      PndDrcOptCylLens mirror(half_thick,half_width,10/2,9999,mirror_radius);
      mirror.SetOptMaterial(PndDrcOptMatLithotecQ0());
      mirror.SetName("mirror");
      mirror.Surface("side6")->SetReflectivity(PndDrcOptReflSilver()); // the mirror
      mirror.AddTransform(Transform3D(XYZVector(0,0, 2*half_length+10/2)));
      mirror.AddTransform(Transform3D(RotationZ(-kPi/2)));
      mirror.SetPrintColor(2);  
      opt_system.AddDevice(mirror);
      opt_system.CoupleDevice("mirror","sheet",         "side1","side6");
    }

  // upstream cylindrical lens... or not

  // create two adjacent lenses, one made of quartz and one made of air.
  // and embedd the into sheet.
  // since the curvature is around the y-axis, create it rotated by 90 degree and rotate it back
  double lens_body_hthick = 5.0;

  if (focus_option == 1)
    {
      PndDrcOptCylLens lens_quartz(half_thick,half_width,lens_body_hthick,lens_radius,99999.9);
      lens_quartz.SetOptMaterial(PndDrcOptMatLithotecQ0());
      lens_quartz.AddTransform(Transform3D(XYZVector(0,0,-lens_body_hthick)));
      lens_quartz.AddTransform(Transform3D(RotationZ(kPi/2)));
      lens_quartz.SetName("lens_quartz");
      lens_quartz.SetPrintColor(2);
      if (coating) lens_quartz.Surface("side1")->SetReflectivity(PndDrcOptReflGeffcken());
      //lens_quartz.SetVerbosity(5);

      opt_system.AddDevice(lens_quartz);
      
      PndDrcOptCylLens lens_air(half_thick,half_width,lens_body_hthick,99999.9,-lens_radius);
      lens_air.AddTransform(Transform3D(XYZVector(0,0,-(2+1)*lens_body_hthick)));
      lens_air.AddTransform(Transform3D(RotationZ(kPi/2)));
      //lens_air.SetOptMaterial(PndDrcOptMatVacuum());
      lens_air.SetOptMaterial(PndDrcOptMatVacuum());
      lens_air.SetName("lens_air");
      lens_air.SetPrintColor(4);
      //lens_air.SetVerbosity(5);
      //if (lens_air.Surface("side6")) lens_air.Surface("side6")->SetVerbosity(5);
      
      lens_air.Surface("side56")->SetPrintColor(5);
      lens_air.Surface("side51")->SetPrintColor(5);
      
      opt_system.AddDevice(lens_air);
      
      // make the air gap extent larger than the lens
      PndDrcOptBrick addition_top(2*half_thick,half_width,lens_body_hthick);
      addition_top.SetOptMaterial(PndDrcOptMatVacuum());
      addition_top.SetName("addition_top");
      addition_top.SetPrintColor(4);
      PndDrcOptBrick addition_bot(addition_top);
      addition_bot.SetName("addition_bot");
      
      addition_top.AddTransform(Transform3D(XYZVector(3*half_thick,0,-(2+1)*lens_body_hthick)));
      addition_top.AddTransform(Transform3D(RotationZ(kPi/2)));
      addition_bot.AddTransform(Transform3D(XYZVector(-3*half_thick,0,-(2+1)*lens_body_hthick)));
      addition_bot.AddTransform(Transform3D(RotationZ(kPi/2)));
      
      opt_system.AddDevice(addition_top);
      opt_system.AddDevice(addition_bot);
      
      // couple surface 1 of device 1 with surface 2 of device 2
      //                       dev1   dev2    surf1     surf2
      opt_system.CoupleDevice("sheet"        ,"lens_quartz"  ,"side1", "side6");
      opt_system.CoupleDevice("lens_quartz"  ,"lens_air"     ,"side1", "side6");
      
      opt_system.CoupleDevice("addition_top" ,"lens_air",     "side3", "side56");
      opt_system.CoupleDevice("addition_top" ,"lens_air",     "side3", "side51");
      
      opt_system.CoupleDevice("addition_bot" ,"lens_air",     "side5", "side36");
      opt_system.CoupleDevice("addition_bot" ,"lens_air",     "side5", "side31");
    }
  
// expansion box
  double ex_box_hw = 300;
  double ex_box_ht = 300;
  double ex_box_hl = 150;
  
  PndDrcOptBrick ex_box(ex_box_hw,ex_box_ht,ex_box_hl);
  ex_box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  ex_box.SetName("expansion box");
  //ex_box.Surface("side6")->SetPixel();
  if (focus_option==1)
    {
      ex_box.AddTransform(Transform3D(XYZVector(0,0,-ex_box_hl-(2+2)*lens_body_hthick)));
    }
  else
    {
      ex_box.AddTransform(Transform3D(XYZVector(0,0,-ex_box_hl)));
    }
  
  ex_box.Surface("side1")->SetPixel();
  if (coating) ex_box.Surface("side6")->SetReflectivity(PndDrcOptReflGeffcken());
  opt_system.AddDevice(ex_box);
  


  if (focus_option==1)
    {
      opt_system.CoupleDevice("lens_air"     ,"expansion box","side1", "side6");
      opt_system.CoupleDevice("addition_top" ,"expansion box","side1", "side6");
      opt_system.CoupleDevice("addition_bot" ,"expansion box","side1", "side6");
    }
  else
    {
      opt_system.CoupleDevice("sheet" ,"expansion box","side1", "side6");
    }
  
  

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
  
  if (ioption==1)
    {
      photons_exist = manager->Cerenkov(pos,dir,beta,100000,1.e16,440,450); // generate photons
    }
  else if (ioption==3)
    {
      TRandom ran;
      double lambda = 600;
      double pi = 3.1415926535;
      list<PndDrcPhoton> list_photon;
      
      
      for (double angle=20; angle<=20; angle+=5)
	{
	  double slab_width = half_width*1.9;//p4.X()*1.9;
	  double slab_height = half_thick*1.9;
	  
	  //cout<<" angle = "<<angle<<endl;
	  //for (double lambda1=630; lambda1>329; lambda1-=30)
	    {
	      double lambda1=lambda;
	      // go in x dir
	      double y1 = tan(angle*pi/180);
	      double x1 = y1;
	      double scale = 3/angle;
	      for (double y=-y1; y<= y1; y+= 2*y1/100*scale)
		{
		  PndDrcPhoton ph;
		  double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		  double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		  ph.SetPosition(XYZPoint(xx,yy,half_length+ran.Uniform(0,100)));
		  double x = x1;
		  ph.SetDirection(XYZVector(x,y,1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		  ph.SetDirection(XYZVector(-x,y,1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		}
	      for (double x=-x1; x<= x1; x+= 2*y1/100*scale)
		{
		  PndDrcPhoton ph;
		  double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		  double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		  ph.SetPosition(XYZPoint(xx,yy,half_length+ran.Uniform(0,100)));
		  double y = y1;
		  ph.SetDirection(XYZVector(x,y,1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		  ph.SetDirection(XYZVector(x,-y,1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		}
	      
	    }
	}
      
      photons_exist=true;
      manager->SetPhotonList(list_photon,"sheet");
    }
  if (ioption==4)  
    {
      TRandom ran;
      list<PndDrcPhoton> list_photon;
      // draw a line
      double xdir,ydir,zdir;
      zdir = 500;
      for (xdir = -10; xdir<10; xdir+=0.001)
	{
	  ydir = xdir*1;
	  
	  
	  //double xx=0;
	  //double yy=0;
	  //double zz=1225;
	  double xx=ran.Uniform( -half_width*0.9, +half_width*0.9);
	  double yy=ran.Uniform( -half_thick*0.9, +half_thick*0.9);
	  double zz=ran.Uniform(-half_length*0.9,+half_length*0.9);
	  

	  PndDrcPhoton ph;
	  
	  ph.SetPosition(XYZPoint(xx,yy,zz));
	  ph.SetDirection(XYZVector(xdir,ydir,zdir).Unit());
	  ph.SetWavelength(650);
	  list_photon.push_back(ph);
	  
	  
	}
      photons_exist = true;
      manager->SetPhotonList(list_photon,"sheet");
    }
    else
    {
      PndDrcPhoton ph;
      ph.SetReflectionLimit(200);  
      list<PndDrcPhoton> list_photon;
      int imax=20; // rays in one dimension
      for (int ix=0; ix<imax; ix++)
	//  int ix=0;
	{
	  for (int iy=0; iy<imax; iy++)	
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
  scr.open("ScreenTOP3.C",std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  if (top_option==1)
    {
      scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"test_simple_lens_sheet time vs x\""
	 <<",500,-500,500);"<<endl;
    }
  else
    {
      scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"test_simple_lens_sheet y vs x\""
	 <<",500,-500,500);"<<endl;
    }
  scr<<"    hgr->SetStats(0);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-500);"<<endl;
  scr<<"    hgr->SetMaximum(500);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;




  // analyse list

  list<PndDrcPhoton> list_photon = manager->PhotonList();  // get list

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
	  //irefl/=10;
	  irefl=irefl%10;
	  
	  int icol=29;

	  if (irefl == 0) icol = kBlack; // black
	  if (irefl == 1) icol = kOrange-7; // brown
	  if (irefl == 2) icol = kRed;//2; // red
	  if (irefl == 3) icol = kOrange-3;//42; // orange
	  if (irefl == 4) icol = kYellow;//5; // yellow
	  if (irefl == 5) icol = kGreen;//3; // green
	  if (irefl == 6) icol = kBlue;//4; // blue
	  if (irefl == 7) icol = kViolet;//6; // violett
	  if (irefl == 8) icol = kGray;//14; // gray
	  if (irefl == 9) icol = kWhite;//18; // white


	  scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	  scr<<"    t->SetMarkerColor("
		<<icol
		<<");"<<endl;
	  //	  scr<<"    t->SetMarkerColor("
	  //<<(*iph).ColorNumber((*iph).Wavelength())
	  //<<");"<<endl;
	  scr<<"    t->SetMarkerSize(0.5);"<<endl;
	  scr<<"    t->Draw();"<<endl;
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
