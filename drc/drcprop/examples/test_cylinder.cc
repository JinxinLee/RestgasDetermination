

//#include <cstdlib>
//#include <cstdio>
//#include <string>

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


#include <map>
using std::map;

#include <limits>

#include <fstream>
using std::fstream;

#include <utility>
using std::pair;
//#include <cmath>

#include "TROOT.h"
#include "TRint.h"
#include "TVector3.h"
#include "TRandom.h"
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
#include "PndDrcSurfCyl.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"


int main(int argc, char *argv[])
{
  const double pi = 3.1415926535;

  int verbosity = 0; // 0=quiet, 1=constructors,2=member,3=functionality
  //                    4=photons, 5=everything.

  XYZPoint p1(0,0,-1000); 
  XYZPoint p2(0,0,+1000);
  double   radius1 = 480; 
  double   radius2 = 497; 

  PndDrcSurfCyl scyl_out;
  scyl_out.Set(p1,p2,radius2);
  scyl_out.SetName("scyl_out");

  PndDrcSurfCyl scyl_in;
  scyl_in.Set(p1,p2,radius1);
  scyl_in.SetName("scyl_in");



  //     p4  p3
  //
  //     p1  p2

  XYZPoint r1(-500,-500,0);
  XYZPoint r2(+500,-500,0);
  XYZPoint r3(+500,+500,0);
  XYZPoint r4(-500,+500,0);
  PndDrcSurfPolyFlat s1;
  s1.AddPoint(r1);
  s1.AddPoint(r2);
  s1.AddPoint(r3);
  s1.AddPoint(r4);

  Transform3D trans_plus(XYZVector(0,0,1000));
  Transform3D trans_minus(XYZVector(0,0,-1000));


  PndDrcOptReflSilver refl;

  PndDrcSurfPolyFlat sfront(s1);
  sfront.SetReflectivity(refl);
  sfront.AddTransform(trans_plus);
  sfront.SetName("sfront");

  PndDrcSurfPolyFlat sback(s1);
  sback.AddTransform(trans_minus);
  sback.SetPixel();
  sback.SetPrintColor(2);
  sback.SetName("sback");


  PndDrcOptVol vcyl;
  PndDrcOptMatLithotecQ0 quartz;
  vcyl.SetOptMaterial(quartz);
  vcyl.AddSurface(scyl_out);
  //vcyl.AddSurface(scyl_in);
  vcyl.AddSurface(sfront);
  vcyl.AddSurface(sback);
  vcyl.SetName("cylinder");
  vcyl.SetVerbosity(verbosity);


  PndDrcOptDevSys sys;
  sys.AddDevice(vcyl);

  PndDrcOptDevManager manager;
  manager.AddDeviceSystem(sys);



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
  geo<<"    view->SetRange(-1000,-1000,-1000,1000,1000,1000);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(0,90,90,i);"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager.Print(geo);


  //  TVector3 pos(0,-250,+200);
  XYZPoint pos(0,-600,0);
  XYZVector dir(0,1,1.9);
  bool photons_exist;
  photons_exist = manager.Cerenkov(pos,dir,0.70,1000); // generate photons

  if (photons_exist)
    {
      fstream scr;
      scr.open("Screen2.C",std::ios::out);
      scr<<"{"<<endl;
      scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
      scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"Screen\",100,-100,100);"<<endl;
      scr<<"    hgr->SetMarkerStyle(20);"<<endl;
      scr<<"    hgr->SetMinimum(-100);"<<endl;
      scr<<"    hgr->SetMaximum(+100);"<<endl;
      scr<<"    hgr->Draw(\"POL\");"<<endl;

     fstream scr2;
      scr2.open("TOP.C",std::ios::out);
      scr2<<"{"<<endl;
      scr2<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
      scr2<<"    TH1F *hgr = new TH1F(\"hgr\",\"TOP (ns) vs x (deg)\",100,-3.14,3.14);"
	  <<endl;
      scr2<<"    hgr->SetMarkerStyle(20);"<<endl;
      scr2<<"    hgr->SetMinimum(0);"<<endl;
      scr2<<"    hgr->SetMaximum(+50);"<<endl;
      scr2<<"    hgr->Draw(\"POL\");"<<endl;


      int icnt_measured = 0;
      int icnt_absorbed = 0;
      int icnt_lost     = 0;
      

      manager.Propagate();                           // propagate photons

      list<PndDrcPhoton> list_photon = manager.PhotonList(); // get list

      list<PndDrcPhoton>::iterator iph;
      for (iph = list_photon.begin(); iph != list_photon.end(); ++iph)
	{
	  //cout<<(*iph).fate()<<endl;
	  if ((*iph).Fate()==Drc::kPhotMeasured)
	    {
	      icnt_measured++;
	      double x     = (*iph).Direction().X();
	      double y     = (*iph).Direction().Y();
	      double z     = (*iph).Direction().Z();
	      double theta = atan2(sqrt(x*x+y*y),fabs(z));
	      double phi   = atan2(y,x);
	      double r     = theta;
	      double xx = r*cos(phi)*180/3.1415;
	      double yy = r*sin(phi)*180/3.1415;


	      //cout<<" time = "<<(*iph).time()<<endl;//###

	      scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	      scr<<"    t->SetMarkerColor("
		 <<(*iph).ColorNumber((*iph).Wavelength())
		 <<");"<<endl;
	      scr<<"    t->SetMarkerSize(0.7);"<<endl;
	      scr<<"    t->Draw();"<<endl;

	      double phi1 = XYZPoint(0,1,0).Phi();
	      double phi2 = (*iph).Position().Phi();
	      phi = phi2-phi1;
	      if (phi >  pi) phi-=2*pi;
	      if (phi < -pi) phi+=2*pi;
	      //cout<<(*iph).position().X()<<" "<<(*iph).position().Y()<<" "
	      //  <<(*iph).time()<<endl;
	      x = phi;
	      scr2<<"    TMarker* t = new TMarker("<<x<<","<<(*iph).Time()<<",20);"
		  <<endl;
	      scr2<<"    t->SetMarkerColor("
		 <<(*iph).ColorNumber((*iph).Wavelength())
		 <<");"<<endl;
	      scr2<<"    t->SetMarkerSize(0.7);"<<endl;
	      scr2<<"    t->Draw();"<<endl;





	    }
	  if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
	  if ((*iph).Fate()==Drc::kPhotLost)     icnt_lost++;

	  
	}

      scr<<"}"<<endl;
      scr.close();

      scr2<<"}"<<endl;
      scr2.close();

      int icnt = icnt_measured+icnt_absorbed+icnt_lost;
      cout<<" Total photons: "<<icnt<<endl;
      cout<<"      measured: "<<icnt_measured<<endl;
      cout<<"      absorbed: "<<icnt_absorbed<<endl;
      cout<<"          lost: "<<icnt_lost<<endl;
    }









  geo<<"}"<<endl;
  geo.close();
  















  return 0;

}
