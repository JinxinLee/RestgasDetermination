


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
#include "PndDrcSurfAbs.h"
#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcOptReflSilver.h"
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

  XYZPoint p1(-15.0, +8.5, 1100);    //         p5----------p8
  XYZPoint p2(-15.0, -8.5, 1100);    //        /|          /|
  XYZPoint p3(+15.0, -8.5, 1100);    //       / |         / |
  XYZPoint p4(+15.0, +8.5, 1100);    //      /  p6-------/--p7
  //                                     /  /        /  /
  //                                    /  /        /  /
  //                                   /  /        /  /
  //                                  /  /        /  /
  XYZPoint p5(-15.0, +8.5, -800);  // p1---------p4  /
  XYZPoint p6(-15.0, -8.5, -800);  // | /         | /
  XYZPoint p7(+15.0, -8.5, -800);  // |/          |/
  XYZPoint p8(+15.0, +8.5, -800);  // p2---------p3 


  // Define from points 6 surfaces of the bar. The points have to be given in 
  // the sequence going around the surface, clock- or counterclock-wise.
  // There are 2 additional surfaces, a mirror and a screen.

  // How to produce surfaces by shift and rotate operation is for sake of clearness
  // not shown here, but in one of the other examples.

  PndDrcOptReflSilver refl;

  // Declare flat surfaces with arbitrary number of points.
  PndDrcSurfPolyFlat a1,a2,a3,a4,a5,a6;

  a1.SetVerbosity(verbosity);
  a1.SetReflectivity(refl);  // this acts as a mirror
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
  a6.SetPixel();      // this acts as a screen
  a6.SetPrintColor(2);
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
  geo<<"    view->SetRange(-50,-50,-50,50,50,50);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(0,90,90,i);"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  //manager->Print(geo);
  //
  // the intention is to play around with routines.


  // create a list of photons in bar

  XYZPoint pos(0,-10,0);
  XYZVector dir(0,1,1); 
  double   beta = 0.89;
  double range=500;
  
  bool photons_exist = manager->Cerenkov(pos,dir,beta); // generate photon
  //bool photons_exist = manager->Cerenkov(pos,dir,beta,100,range,400,650); // generate photons
  //bool photons_exist = manager->Cerenkov(pos,dir,beta,100,range,400,405); // generate photons

  cout<<" photons exist: "<<photons_exist<<endl;
  

  if (photons_exist)
    {
      manager->Propagate();                           // propagate photons
   }

  list<PndDrcPhoton> list_photon = manager->PhotonList(); // get list
  


  // propagate writes to geo, that has finished, therefore, close geo
  geo<<"}"<<endl;
  geo.close();


  // write to screen...

  fstream scr;
  scr.open("Screen.C",std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  //scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"a simple graph\",100,-100,100);"<<endl;
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"a simple graph\",100,-30,30);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-40);"<<endl;
  scr<<"    hgr->SetMaximum(40);"<<endl;
  //scr<<"    hgr->SetMinimum(-100);"<<endl;
  //scr<<"    hgr->SetMaximum(+100);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;
  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;



  vector<XYZVector> dir_list;
  
  double time400[200]={200*0};
  double time600[200]={200*0};
  
  fstream out1;
  out1.open("debug.dat",std::ios::out);
  
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if ((*iph).Fate()==Drc::kPhotMeasured)
	{

	  dir_list.push_back((*iph).Direction());
	  
	  icnt_measured++;
	  double rx     = (*iph).Position().X();
	  double ry     = (*iph).Position().Y();
	  double rz     = (*iph).Position().Z();
	  double x     = (*iph).Direction().X();
	  double y     = (*iph).Direction().Y();
	  double z     = (*iph).Direction().Z();
	  double theta = atan2(sqrt(x*x+y*y),fabs(z));
	  double phi   = atan2(y,x);
	  double r     = theta;
	  double xx = r*cos(phi)*180/3.1415;
	  double yy = r*sin(phi)*180/3.1415;
	  double time = (*iph).Time();
	  double lambda = (*iph).Wavelength();

	  //if (xx<1 && xx>-1)  
	    {
	      if (lambda<450 && lambda>400) time400[int(time*10)]++;
	      if (lambda<650 && lambda>600) time600[int(time*10)]++;
	    }
	  
	    double scale = 300.0/z;
	    double sx = x*scale;
	    double sy = y*scale;
	    double sz = z*scale;
	    rx += sx;
	    ry += sy;
	    

	    out1<<rx<<" "<<ry<<endl;
	    
	  

	  scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	  scr<<"    t->SetMarkerColor("
		<<(*iph).ColorNumber((*iph).Wavelength())
		<<");"<<endl;
	  scr<<"    t->SetMarkerSize(0.7);"<<endl;
	  scr<<"    t->Draw();"<<endl;
	}
      else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
      else                                       icnt_lost++;
    }
  scr<<"}"<<endl;
  scr.close();


  //for (int i=0; i<200; i++)
  //{
  //  out1<<i*0.1<<" "<<time400[i]<<" "<<time600[i]<<endl;
  //}
  
      
    
  

  out1.close();
  
  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;

  delete manager;


  return EXIT_SUCCESS;

}
