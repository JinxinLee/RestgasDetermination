//
//
//
//     2 lens readout
//
//
//
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

#include "TROOT.h"
#include "TVector3.h"
#include "TRotation.h"
#include "TRandom.h"
#include "TRandom1.h"
#include "TRotation.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TFile.h"
#include "TRint.h"
#include "TDecompLU.h"
#include "TDecompSVD.h"

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
#include "PndDrcSurfQuadFlatDiff.h"
#include "PndDrcSurfPolySphere.h"
#include "PndDrcSurfPolyPara.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatBK7G18.h"
#include "PndDrcOptMatNLAK33A.h"
#include "PndDrcOptMatPbF2.h"
#include "PndDrcOptMatMarcol7.h"
#include "PndDrcOptMatLLF1.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"
#include "PndDrcOptLens.h"
#include "PndDrcConicSection.h"



void cross(PndDrcPhoton ph1, PndDrcPhoton ph2, double& x, double& y, double& z)
{
  XYZPoint  pos1 = ph1.Position();
  XYZPoint  pos2 = ph2.Position();
  XYZVector dir1 = ph1.Direction();
  XYZVector dir2 = ph2.Direction();
  
  // y is zero
  // pos1_x + f1 * dir1_x = pos2_x + f2 *dir2_x
  // f1 = (pos2_x - pos1_x)/dir1_x + f2 * dir2_x/dir1_x
  // now for z
  // pos1_z + ((pos2_x - pos1_x)/dir1_x + f2 * dir2_x/dir1_x)*dir1_z = pos2_z + f2 *dir2_z
  // pos1_z + (pos2_x-pos1_x)* dir1_z/dir1_x + f2*dir2_x * dir1_z/dir1_x = pos2_z + f2 *dir2_z
  // pos1_z-pos2_z + (pos2_x-pos1_x)*dir1_z/dir1_x = f2 * (dir2_z -dir2_x* dir1_z/dir1_x)

  double f2 = pos1.Z()-pos2.Z() +  (pos2.X()-pos1.X())*dir1.Z()/dir1.X();

  f2 /= (dir2.Z()-dir2.X()*dir1.Z()/dir1.X());

  XYZPoint ppp = pos2 + f2*dir2;

  x = ppp.X();
  y = ppp.Y();
  z = ppp.Z();
  

  
}

int main(int argc, char *argv[]) 
{
  const double pi=3.1415926535;



  //int iopt = 0;// only geometry
  int iopt = 1;// straight lines
  //int iopt = 2;// C-cone
  //int iopt = 3;// ???
  //int iopt = 4; // debug


 
  bool l_fresnel = true;
  double dist_plane=300;
  double conical = 0;  

  double radius_lens1;
  double thick_lens1;
  double radius_lens2b;
  double radius_lens2a;
  double thick_lens2;
  double thick_lens3; 

  fstream out1;
  out1.open("pbf2.dat",std::ios::out);
  PndDrcOptMatPbF2 opt_mat;
  for (int i=300; i<=700; i++)
    {
      out1<<i<<" "<<opt_mat.RefIndex(i)<<endl;
    }
  out1.close();

  TFile*  hfile = (TFile*)gROOT->FindObject("test_barrel3.root"); 
  if (hfile) hfile->Close();
  hfile = new TFile("test_barrel3.root","RECREATE","Reconstruction of theta_c");

  TH1D*   hpd_hori        = new TH1D("hpd_hori"   ,"x position (mm)",601,-300,300);
  TH1D*   hpd_vert        = new TH1D("hpd_vert"   ,"t position (mm)",601,-300,300);


  // for lens parameter determination use iopt 3 and the following lines
  //radius_lens1=atof(argv[1]);
  //radius_lens2b=atof(argv[2]);
  //thick_lens2 = atof(argv[3]);
  //thick_lens3 = atof(argv[4]);
  
  
  
  radius_lens1  =   37;
  radius_lens2b =   26;
  thick_lens2   = 1;//11.6; 
  thick_lens3    =9; 
  thick_lens1   =   1;
  radius_lens2a = -radius_lens1;

  double bar_half_w = 34/2;
  double bar_half_h = 17/2;
  double bar_half_l = 800/2;
  

  XYZPoint q0(-bar_half_w,-bar_half_h,0);
  XYZPoint q1(+bar_half_w,-bar_half_h,0);
  XYZPoint q2(+bar_half_w,+bar_half_h,0);
  XYZPoint q3(-bar_half_w,+bar_half_h,0);

  


  PndDrcOptBrick bar(bar_half_w,bar_half_h,bar_half_l);
  bar.SetOptMaterial(PndDrcOptMatLithotecQ0());
  bar.SetName("bar");
  bar.Surface("side6")->SetReflectivity(PndDrcOptReflSilver());

  PndDrcOptDevSys opt_system;
  opt_system.SetNameCopyNumber("optsys",0);
  opt_system.AddDevice(bar);

  PndDrcOptLens lens1(bar_half_w,bar_half_h,thick_lens1/2,radius_lens1,9999,
		      conical,conical);
  lens1.SetName("lens1");
  lens1.AddTransform(Transform3D(XYZVector(0,0,-400
					   //-thick_air0
					   -thick_lens1/2)));
  lens1.SetOptMaterial(PndDrcOptMatBK7G18());
  lens1.SetPrintColor(2);
  opt_system.AddDevice(lens1);
  //opt_system.CoupleDevice("air0","lens1","side1","side6");
  opt_system.CoupleDevice("bar","lens1","side1","side6");
  
  
  
  PndDrcOptLens lens2(bar_half_w,bar_half_h,
		      thick_lens2/2,radius_lens2b,radius_lens2a,
		      conical,conical);
  lens2.SetName("lens2");
  lens2.AddTransform(Transform3D(XYZVector(0,0,
					   -400
					   //-thick_air0
					   -thick_lens1
					   -thick_lens2/2)));
  lens2.SetOptMaterial(PndDrcOptMatPbF2());
  lens2.SetPrintColor(3);
  opt_system.AddDevice(lens2);
  opt_system.CoupleDevice("lens1","lens2","side1","side6");
  


  PndDrcOptLens lens3(bar_half_w,bar_half_h,
		      thick_lens3/2,9999,-radius_lens2b,
		      conical,conical);
  lens3.SetName("lens3");
  lens3.Surface("side21")->SetReflectivity(PndDrcOptReflNone());
  lens3.Surface("side26")->SetReflectivity(PndDrcOptReflNone());
  lens3.Surface("side31")->SetReflectivity(PndDrcOptReflNone());
  lens3.Surface("side36")->SetReflectivity(PndDrcOptReflNone());
  lens3.Surface("side41")->SetReflectivity(PndDrcOptReflNone());
  lens3.Surface("side46")->SetReflectivity(PndDrcOptReflNone());
  lens3.Surface("side51")->SetReflectivity(PndDrcOptReflNone());
  lens3.Surface("side56")->SetReflectivity(PndDrcOptReflNone());
  lens3.AddTransform(Transform3D(XYZVector(0,0,
					   -400
					   -thick_lens1
					   -thick_lens2
					   -thick_lens3/2)));
  if (iopt==4)
    {
      lens3.SetOptMaterial(PndDrcOptMatLithotecQ0());
    }
  else
    {
      lens3.SetOptMaterial(PndDrcOptMatBK7G18());
      //lens3.SetOptMaterial(PndDrcOptMatVacuum());
    }
  lens3.SetPrintColor(1);
  opt_system.AddDevice(lens3);
  opt_system.CoupleDevice("lens2","lens3","side1","side6");


  PndDrcOptBrick box(dist_plane,dist_plane,dist_plane/2);
  if (iopt==4)
    {
      box.SetOptMaterial(PndDrcOptMatLithotecQ0());
    }
  else
    {
      box.SetOptMaterial(PndDrcOptMatLithotecQ0());
      //box.SetOptMaterial(PndDrcOptMatMarcol7());
    }
  box.SetName("box");
  box.Surface("side1")->SetPixel();
  box.Surface("side2")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side3")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side4")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side5")->SetReflectivity(PndDrcOptReflNone());
  
  box.AddTransform( Transform3D(XYZVector(0,0,
					  -400
					  -thick_lens1
					  -thick_lens2
					  -thick_lens3
					  -dist_plane/2)) );



  opt_system.AddDevice(box);
  opt_system.CoupleDevice("lens3","box","side1","side6");
  


  opt_system.SetFresnel(l_fresnel); // sets all surfaces
  

  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->AddDeviceSystem(opt_system);
  //manager->setVerbosity(5);

  string sfile = "Geo.C";
  

  fstream geo;
  geo.open(sfile.c_str(),std::ios::out);
  geo<<"{"<<endl;
  geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  cout<<" root version: "<<((TROOT*)gROOT)->GetVersionInt()<<endl;
  
  if ( ((TROOT*)gROOT)->GetVersionInt() < 51600)
    {
      geo<<"    TView *view = new TView(1);"<<endl;
    }
  else
    {  
      geo<<"    TView *view = TView::CreateView(1);"<<endl;
    }
  //geo<<"    view->SetRange(-400,-500,-1050,600,500,-50);"<<endl;
  geo<<"    view->SetRange(-500,-500,-1500,500,500,500);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(90,-90,0,i);"<<endl;
  for (int i=0; i<3;i++) // 7 or 16
    geo<<"    view->Zoom();"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  if (iopt==0 || iopt==3) manager->Print(geo);

  PndDrcPhoton ph;
  ph.SetReflectionLimit(200);
  
  list<PndDrcPhoton> list_photon; // get list
  
  if (iopt==1)  
    {
      
      // straight lines.
      TRandom ran;
      //for (double angle=20; angle<=20; angle+=5)
      for (double angle=5; angle<=45.5; angle+=10)
	//double angle=10;
	{
	  double slab_width = bar_half_w*1.9;
	  double slab_height =bar_half_h*1.9;
	  for (double lambda1=600; lambda1>349; lambda1-=50)
	    {
	      //double lambda1=lambda;
	      // go in x dir



	      double y1 = tan(angle*pi/180);
	      double x1 = y1;
	      double step = 0.03*y1/angle;
	      
	      for (double y=-y1; y<= y1; y+= step)
		{
		  double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		  double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		  ph.SetPosition(XYZPoint(xx,yy,200));
		  double x = x1;
		  ph.SetDirection(XYZVector(x,y,-1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		  ph.SetDirection(XYZVector(-x,y,-1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		}
	      
	      
	      for (double x=-x1; x<= x1; x+= step)
		{
		  double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		  double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		  ph.SetPosition(XYZPoint(xx,yy,200));
		  double y = y1;
		  ph.SetDirection(XYZVector(x,y,-1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		  ph.SetDirection(XYZVector(x,-y,-1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		}
	      
	    }
	}
    }
  
  else if (iopt==2)
    {
      XYZPoint  pos(0,-18,200);
      XYZVector dir(0,sin(50*pi/180.),cos(50*pi/180.)); 
      double   beta = 0.99;
      manager->Cerenkov(pos,dir,beta,5000,1e16,300,600,50); // generate photons
    }
  else if (iopt==3)
    {
      double angle=0;
      for (int i=0; i<30; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/31.0,0,-398));
	  ph.SetWavelength(420);
	  list_photon.push_back(ph);
	}
      
      
      angle=10;
      for (int i=0; i<30; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/31.0,0,-398));
	  ph.SetWavelength(420);
	  list_photon.push_back(ph);
	}

      angle=20;      
      for (int i=0; i<30; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/31.0,0,-398));
	  ph.SetWavelength(420);
	  list_photon.push_back(ph);
	}
      angle=30;      
      for (int i=0; i<30; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/31.0,0,-398));
	  ph.SetWavelength(420);
	  list_photon.push_back(ph);
	}
      angle=40;      
      for (int i=0; i<30; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/31.0,0,-398));
	  ph.SetWavelength(420);
	  list_photon.push_back(ph);
	}








    }
  else if (iopt==4)
    {
      XYZPoint  pos(0,-18,200);
      XYZVector dir(0,sin(50*pi/180.),cos(50*pi/180.)); 
      double   beta = 0.99;
      manager->Cerenkov(pos,dir,beta); // generate photons
    }
  
  
  // propagate writes to geo, that has finished, therefore, close geo
  


  if (iopt!=2 && iopt!=4) 
    {
      cout<<" list size = "<<list_photon.size()<<endl;
      manager->SetPhotonList(list_photon,"bar","optsys");
    }
  manager->Propagate();  
  

  geo<<"}"<<endl;
  geo.close();



  list_photon = manager->PhotonList();  // get list
  
  sfile = "Screen.C";
  

  fstream scr;
  scr.open(sfile.c_str(),std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"test_barrel3 \" ";
  scr<<",500,-300,300);"<<endl;
  scr<<"    hgr->SetStats(0);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-300);"<<endl;
  scr<<"    hgr->SetMaximum(300);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;





  // analyse list
  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;


  
  int isize=0;

 
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if ((*iph).Fate()==Drc::kPhotMeasured)
	{
	  isize++;
	  
	  double xx=(*iph).Position().X();
	  double yy=(*iph).Position().Y();


	  hpd_hori->Fill(xx);
	  hpd_vert->Fill(yy);
	  

	  scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	  scr<<"    t->SetMarkerColor("
	     <<(*iph).ColorNumber((*iph).Wavelength())
	     <<");"<<endl;
	  scr<<"    t->SetMarkerSize(0.2);"<<endl;
	  scr<<"    t->Draw();"<<endl;
	}
      

      if      ((*iph).Fate()==Drc::kPhotMeasured) icnt_measured++;
      else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
      else                                       icnt_lost++;
    }
  
  
  
      
    
  scr<<"}"<<endl;
  scr.close();

  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;
  
  hfile->Write();


  delete manager;
  


  return EXIT_SUCCESS;

}
