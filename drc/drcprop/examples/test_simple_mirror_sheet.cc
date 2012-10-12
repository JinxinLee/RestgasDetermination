


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
  static const double pi=3.1415926535;

  // Example for a simple sheet with lens and expansion box.


  //---------------------------------------------------------

  
  int ioption = 1; // 1=cherenkov
  //int ioption = 4; // angular frames
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

  XYZPoint p1(-half_width, +half_thick, +half_length);    //         p1----------p4
  XYZPoint p2(-half_width, -half_thick, +half_length);    //        /|          /|
  XYZPoint p3(+half_width, -half_thick, +half_length);    //       / |         / |
  XYZPoint p4(+half_width, +half_thick, +half_length);    //      /  p2-------/--p3
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
  
  a1.SetFocalPoint(2*half_length+300,XYZPoint(0.5*(p1.X()+p3.X()), 
					      0.5*(p1.Y()+p3.Y()), 
					      0.5*(p1.Z()+p3.Z()))); // before trafo
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
      photons_exist = manager->Cerenkov(pos,dir,beta,10000);//,1.e16,440,450); // generate photons


      list_photon = manager->PhotonList();  // get list
      list<PndDrcPhoton>::iterator iph;
      for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	{
	  //(*iph).SetPrintFlag(false);
	  (*iph).SetListLimit(1000);
	}
      manager->SetPhotonList(list_photon,"sheet");
    }
  else if (ioption==2)  
    {
      
      double xx=0;
      double yy=0;
      
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
      
      
      
      photons_exist = true;
      manager->SetPhotonList(list_photon,"sheet");
    }
  else if (ioption==3)
    {
      TRandom ran;
      double lambda = 600;
      
      for (double angle=20; angle<=20; angle+=5)
	{
	  double slab_width = p4.X()*1.9;
	  double slab_height =p4.Y()*1.9;
	  
	  //cout<<" angle = "<<angle<<endl;
	  //for (double lambda1=630; lambda1>329; lambda1-=30)
	  //{
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
      photons_exist=true;
      manager->SetPhotonList(list_photon,"sheet");
    }
  else if (ioption==4)  
    {

      // draw a line
      TRandom ran;
      
      double xdir,ydir,zdir;
      zdir = 10;
      for (xdir = -10; xdir<10; xdir+=0.01)
	{
	  ydir = xdir*1;
	  
	  
	  //double xx=0;
	  //double yy=0;
	  double xx=0;//ran.Uniform( -half_width*0.9, +half_width*0.9);
	  double yy=0;//ran.Uniform( -half_thick*0.9, +half_thick*0.9);
	  double zz=1200;//ran.Uniform(100,+2*half_length*0.9);
	  
	  PndDrcPhoton ph;
	  ph.SetListLimit(1000);
	  
	  ph.SetPosition(XYZPoint(xx,yy,zz));
	  ph.SetDirection(XYZVector(xdir,ydir,zdir).Unit());
	  ph.SetWavelength(650);
	  list_photon.push_back(ph);
	  
	  
	}
      photons_exist = true;
      manager->SetPhotonList(list_photon,"sheet");
    }
  
  
  
  (*list_photon.begin()).Print();
  
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
	  int iside_refl = 0;
	  int irefl=0;
	  
	  double xx=(*iph).Position().X();
	  double yy=(*iph).Position().Y();

	  if (icnt_measured==1) (*iph).Print();
	  

	  list<const PndDrcSurfAbs*> surf = (*iph).SurfaceList();
	  list<const PndDrcSurfAbs*>::iterator isu;
	  int down = 0;
	  for(isu=surf.begin(); isu != surf.end(); ++isu) 
	    {
	      //cout<<icnt_measured<<" "<<(*isu)->Name()<<" "<<endl;
	      //if ((*isu)->Name()=="adown") down=1;
	      if (/*down==1 &&*/
	      ((*isu)->Name()=="aside1" || 
	       (*isu)->Name()=="aside2" ||
	       (*isu)->Name()=="aside3" ||
	       (*isu)->Name()=="aside4")) 
	      {
		irefl ++;
	      }
	      

	      if (/*down==1 &&*/
		  (/*(*isu)->Name()=="aside1" ||*/ 
		   (*isu)->Name()=="aside2" ||
		   (*isu)->Name()=="aside3" /*||
					      (*isu)->Name()=="aside4"*/)) 
		{
		  iside_refl++;
		  /*if ((*isu)->Name()=="aside2" ||
		    (*isu)->Name()=="aside3")*/
		  //cout<<icnt_measured<<" "<<iside_refl<<" "<<(*isu)->Name()<<" "<<endl;
		}
	    }
	  
	  
	  //cout<<"---"<<endl;
	  
	  
	  
	  //int irefl;// = (*iph).Reflections()-1; // -1 for mirror
	  //irefl/=5;

	  irefl=iside_refl;
	  
	  irefl=irefl%10;
	  
	  int icol=29;

	  //cout<<" irefl="<<irefl<<endl;
	  
	  if (irefl == 0) icol = kBlack; // black
	  if (irefl == 1) icol = kOrange-7; // brown
	  if (irefl == 2) icol = kRed;//2; // red
	  if (irefl == 3) icol = kOrange-3;//42; // orange
	  if (irefl == 4) icol = kYellow;//5; // yellow
	  if (irefl == 5) icol = kGreen;//3; // green
	  if (irefl == 6) icol = kBlue;//4; // blue
	  if (irefl == 7) icol = kViolet;//6; // violett
	  if (irefl == 8) icol = kGray+2;//14; // gray
	  if (irefl == 9) icol = kGray;//18; // white

	  //cout<<" side, total "<<iside_refl<<" "<<irefl<<endl;
		  //if (iside_refl==11)
	  {
	    //if (irefl==9) cout<<" 9="<<icol<<endl;
	    
	      
	    scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	    scr<<"    t->SetMarkerColor("
	       <<icol
	       <<");"<<endl;
	    //	  scr<<"    t->SetMarkerColor("
	    //<<(*iph).ColorNumber((*iph).Wavelength())
	    //<<");"<<endl;
	    scr<<"    t->SetMarkerSize(1);"<<endl;
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
  
