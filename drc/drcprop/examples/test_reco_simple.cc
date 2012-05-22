

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
#include "TH1D.h"
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
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"


double rel_ang(const XYZVector& dir1,
	       const XYZVector& dir2)
{
  return acos( (dir1.Dot(dir2) ) / sqrt(dir1.Mag2()*dir2.Mag2()) );
}

void ambiguities(TH1D* spc, XYZVector& dir_part, XYZVector& dir_pix)
{
  // The ambiguities are 
  // left-right      reflections
  // top-down        reflections
  // mirror-no mirr. reflections
  double xx = dir_pix.X();
  double yy = dir_pix.Y();
  double zz = dir_pix.Z();

  spc->Fill(1000*rel_ang(dir_part,XYZVector( xx, yy, zz)));
  spc->Fill(1000*rel_ang(dir_part,XYZVector(-xx, yy, zz)));
  spc->Fill(1000*rel_ang(dir_part,XYZVector( xx,-yy, zz)));
  spc->Fill(1000*rel_ang(dir_part,XYZVector(-xx,-yy, zz)));
  spc->Fill(1000*rel_ang(dir_part,XYZVector( xx, yy,-zz)));
  spc->Fill(1000*rel_ang(dir_part,XYZVector(-xx, yy,-zz)));
  spc->Fill(1000*rel_ang(dir_part,XYZVector( xx,-yy,-zz)));
  spc->Fill(1000*rel_ang(dir_part,XYZVector(-xx,-yy,-zz)));
}


int main(int argc, char *argv[])
{


 
  // Example for a simple bar with flat downstream mirror and expansion box.

  const double pi=3.1415926535;

 
  double bar_height = 17.5;
  double bar_width  = 35.0/2;
  double bar_length = 1250;


  PndDrcOptBrick bar(bar_width/2,bar_height/2,bar_length/2);
  bar.SetOptMaterial(PndDrcOptMatLithotecQ0());
  bar.SetName("bar");
  bar.Surface("side6")->SetReflectivity(PndDrcOptReflPerfect());
  // shift bar in z to have bars end at 0,0,0
  bar.AddTransform(Transform3D(XYZVector(0,0,bar_length/2)));

  PndDrcOptBrick box(300,300,300/2); // expansion box  600x600x300mm
  box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  box.SetName("box");
  // eliminate unwanted ambiguities
  box.Surface("side2")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side3")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side4")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side5")->SetReflectivity(PndDrcOptReflNone());
  // shift bar in z to have bars end at 0,0,0
  box.AddTransform(Transform3D(XYZVector(0,0,-300/2)));
  box.Surface("side1")->SetPixel();

  PndDrcOptDevSys opt_system;
  opt_system.AddDevice(bar);
  opt_system.AddDevice(box);
  opt_system.CoupleDevice("bar","box","side1","side6");


  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->AddDeviceSystem(opt_system);




  // event loop
  int     nevents        = 100;
  double  th_part        = 30*pi/180;;
  double  ph_part        = 20;
  double  beta           = 0.99;
  double  effi           = 0.10; // photon detection efficiency
  TRandom ran;

  TFile*  hfile = (TFile*)gROOT->FindObject("test_reco_simple.root"); 
  if (hfile) hfile->Close();
  hfile = new TFile("test_reco_simple.root","RECREATE","Reconstruction of theta_c");

  TH1D*   hthetac        = new TH1D("hthetac"   ,"theta_c (mrad)",1000,0,1000*pi);
  TH1D*   hthetac10      = new TH1D("hthetac10" ,"theta_c (mrad) 1st pixel",1000,0,1000*pi);
  TH1D*   hthetac11      = new TH1D("hthetac11" ,"theta_c (mrad) 2nd pixel",1000,0,1000*pi);
  TH1D*   hthetac12      = new TH1D("hthetac12" ,"theta_c (mrad) 3rd pixel",1000,0,1000*pi);
  TH1D*   hthetac13      = new TH1D("hthetac13" ,"theta_c (mrad) 4th pixel",1000,0,1000*pi);
  TH1D*   hthetac14      = new TH1D("hthetac14" ,"theta_c (mrad) 5th pixel",1000,0,1000*pi);
  TH1D*   hthetac2       = new TH1D("hthetac2"  ,"theta_c (mrad) one event",1000,0,1000*pi);
  TH1D*   hphotons       = new TH1D("hphotons"  ,"measured photons",200,0,200);

  for (int ievent=0; ievent<nevents; ievent++)
    {
      // create a list of photons in bar
      XYZPoint  pos_part(0,-bar_height/2-1,bar_length/2); // 1mm below bar at the
      // bars half length.
      XYZVector dir_part(sin(th_part)*sin(ph_part),
		    sin(th_part)*cos(ph_part),
		    cos(th_part)); 
      bool photons_exist = manager->Cerenkov(pos_part,dir_part,beta); // generate photons
      if (photons_exist) manager->Propagate();              // propagate photons
      list<PndDrcPhoton> list_photon = manager->PhotonList();  // get list
  

      // analyse list
      int icnt_measured = 0;
      int icnt_pixel    = 0;
      list<PndDrcPhoton>::iterator iph;
      for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	{
	  if      ((*iph).Fate()==Drc::kPhotMeasured && ran.Uniform()<effi) 
	    {
	      icnt_measured++;
	      // recontruction
	      // the end of the is at (0,0,0) the photon vector is
	      double xx=fabs((*iph).Position().X());
	      double yy=fabs((*iph).Position().Y());
	      double zz=fabs((*iph).Position().Z());



	      XYZVector dir_pix(xx,yy,zz);
	      ambiguities(hthetac,dir_part,dir_pix);


	      //-----------------------------------------------
	      // for illustration purposes only
	      if (ievent==1)
		{
		  ambiguities(hthetac2,dir_part,dir_pix);
		  if (icnt_pixel==0) // 1st pixel
		    {
		      ambiguities(hthetac10,dir_part,dir_pix);
		    }
		  if (icnt_pixel==1) // 2nd pixel
		    {
		      ambiguities(hthetac11,dir_part,dir_pix);
		    }
		  if (icnt_pixel==2) // 3rd pixel
		    {
		      ambiguities(hthetac12,dir_part,dir_pix);
		    }
		  if (icnt_pixel==3) // 4th pixel
		    {
		      ambiguities(hthetac13,dir_part,dir_pix);
		    }
		  if (icnt_pixel==4) // 5th pixel
		    {
		      ambiguities(hthetac14,dir_part,dir_pix);
		    }
		  icnt_pixel++;
		}
	      // -----------------------------------------------------------------
	    }
	}
      hphotons->Fill(icnt_measured);
      cout<<"event:\t "<<ievent<<" measured photons: \t"<<icnt_measured<<endl;
    }

  hfile->Write();
  


  delete manager;

  return EXIT_SUCCESS;

}
