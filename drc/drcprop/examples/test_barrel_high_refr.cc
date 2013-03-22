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
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatNLAK33A.h"
#include "PndDrcOptMatF2G12.h"
#include "PndDrcOptMatBK7G18.h"
#include "PndDrcOptMatLF5G15.h"
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



  fstream out1;
  out1.open("nlak33a.dat",std::ios::out);
  PndDrcOptMatNLAK33A opt_mat;
  for (int i=300; i<=700; i++)
    {
      out1<<i<<" "<<opt_mat.RefIndex(i)<<endl;
    }
  out1.close();
  

  //int focus_opt = 0; // unfocussed
  int focus_opt = 1; // focussed

  //int iopt = 0;// only geometry
  //int iopt = 1;// straight lines
  //int iopt = 1;// C-cone
  int iopt = 3;// debug
   
  int lens_opt = 1;  // two thin lenses no air gap at all (no between ex_box and lens)

 
  bool l_fresnel = true;
  double dist_plane=300;
  double conical = 0;  

  double radius_lens1;
  double thick_lens1;
  double radius_lens2;
  double thick_lens2;
  
  //radius_lens1 = atof(argv[1]);
  //thick_lens1  = atof(argv[2]);
  
  if (lens_opt==1)
    {
      radius_lens1  = 73.502;
      thick_lens1   =   4.5;
      radius_lens2 = -radius_lens1;
      thick_lens2   = 8;//4; 
      if (focus_opt==0)
	{
	  radius_lens1  = 9999.9;
	  radius_lens2 = 9999.9;
	}
    }
  else
    {
      exit(EXIT_FAILURE);
    }
  
  

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
  //bar.Surface("side6")->SetReflectivity(PndDrcOptReflPerfect());

  

  PndDrcOptDevSys opt_system;

  opt_system.SetNameCopyNumber("optsys",0);
  opt_system.AddDevice(bar);



  PndDrcOptLens lens1(bar_half_w,bar_half_h,thick_lens1/2,radius_lens1,9999,
		      conical,conical);
  lens1.SetName("lens1");
  lens1.AddTransform(Transform3D(XYZVector(0,0,-bar_half_l
					   -thick_lens1/2)));
  //lens1.SetOptMaterial(PndDrcOptMatF2G12());
  lens1.SetOptMaterial(PndDrcOptMatNLAK33A());
  //lens1.Surface("side21")->SetReflectivity(PndDrcOptReflNone());
  //lens1.Surface("side26")->SetReflectivity(PndDrcOptReflNone());
  //lens1.Surface("side31")->SetReflectivity(PndDrcOptReflNone());
  //lens1.Surface("side36")->SetReflectivity(PndDrcOptReflNone());
  //lens1.Surface("side41")->SetReflectivity(PndDrcOptReflNone());
  //lens1.Surface("side46")->SetReflectivity(PndDrcOptReflNone());
  //lens1.Surface("side51")->SetReflectivity(PndDrcOptReflNone());
  //lens1.Surface("side56")->SetReflectivity(PndDrcOptReflNone());
  lens1.SetPrintColor(2);

  opt_system.AddDevice(lens1);
  opt_system.CoupleDevice("bar","lens1","side1","side6");
  
  
  
  PndDrcOptLens lens2(bar_half_w,bar_half_h,
		      thick_lens2/2,9999,radius_lens2,
			     conical,conical);
  //lens2.Surface("side21")->SetReflectivity(PndDrcOptReflNone());
  //lens2.Surface("side26")->SetReflectivity(PndDrcOptReflNone());
  //lens2.Surface("side31")->SetReflectivity(PndDrcOptReflNone());
  //lens2.Surface("side36")->SetReflectivity(PndDrcOptReflNone());
  //lens2.Surface("side41")->SetReflectivity(PndDrcOptReflNone());
  //lens2.Surface("side46")->SetReflectivity(PndDrcOptReflNone());
  //lens2.Surface("side51")->SetReflectivity(PndDrcOptReflNone());
  //lens2.Surface("side56")->SetReflectivity(PndDrcOptReflNone());
  lens2.SetName("lens2");
  lens2.AddTransform(Transform3D(XYZVector(0,0,
					   -bar_half_l
					   -thick_lens1
					   -thick_lens2/2)));
  lens2.SetOptMaterial(PndDrcOptMatLithotecQ0());
  lens2.SetPrintColor(3);

  opt_system.AddDevice(lens2);
  opt_system.CoupleDevice("lens1","lens2","side1","side6");
  


  PndDrcOptBrick box(dist_plane,dist_plane,dist_plane/2);
  box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  box.SetName("box");
  box.Surface("side1")->SetPixel();
  box.Surface("side1")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side2")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side3")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side4")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side5")->SetReflectivity(PndDrcOptReflNone());
  
  
  box.AddTransform( Transform3D(XYZVector(0,0,
					  -bar_half_l
					  -thick_lens1
					  -thick_lens2
					  -dist_plane/2)) );



  opt_system.AddDevice(box);
  opt_system.CoupleDevice("lens2","box","side1","side6");
  


  opt_system.SetFresnel(l_fresnel); // sets all surfaces
  


  // debugging
  PndDrcOptMatLithotecQ0 silica = PndDrcOptMatLithotecQ0();
  PndDrcOptMatBK7G18     bk7g18 = PndDrcOptMatBK7G18();
  PndDrcOptMatF2G12      f2g12  = PndDrcOptMatF2G12();
  PndDrcOptMatLF5G15     lf5g15 = PndDrcOptMatLF5G15();

  fstream out;
  out.open("debug.dat",std::ios::out);
  
  for (double lam=370; lam<800; lam+=1)
    {
      out<<lam<<" "
	 <<silica.RefIndex(lam)<<" "
	 <<bk7g18.RefIndex(lam)<<" "
	 <<f2g12.RefIndex(lam)<<" "
	 <<lf5g15.RefIndex(lam)<<endl;
    }
  out.close();
  





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
  //geo<<"    view->SetRange(-500,-500,-900,500,500,100);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(90,-90,90,i);"<<endl;
  for (int i=0; i<6;i++) // 7 or 16
    geo<<"    view->Zoom();"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  if (iopt!=1) manager->Print(geo);

  PndDrcPhoton ph;
  ph.SetReflectionLimit(200);
  
  list<PndDrcPhoton> list_photon; // get list
  
  double lambda = 400;
  

  if (iopt==1)  
    {
      
      // straight lines.
      TRandom ran;
      //for (double angle=20; angle<=20; angle+=5)
      for (double angle=0; angle<=45.5; angle+=10)
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
	      double step = 0.1*y1/angle;
	      for (double y=-y1; y<= y1; y+= step)
		{
		  double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		  double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		  ph.SetPosition(XYZPoint(xx,yy,200));
		  double x = x1;
		  ph.SetDirection(XYZVector(x,y,+1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		  ph.SetDirection(XYZVector(-x,y,+1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		}
	      for (double x=-x1; x<= x1; x+= step)
		{
		  double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		  double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		  ph.SetPosition(XYZPoint(xx,yy,200));
		  double y = y1;
		  ph.SetDirection(XYZVector(x,y,+1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		  ph.SetDirection(XYZVector(x,-y,+1).Unit());
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
      //manager->Cerenkov(pos,dir,beta,50000,1e16,300,600,50); // generate photons
      manager->Cerenkov(pos,dir,beta); // generate photons
    }
  else if (iopt==3)
    {
      double angle=0;
      ph.SetWavelength(400);
      
      
      for (int i=0; i<10; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/11.0,0,-395));
	  list_photon.push_back(ph);
	}
      
       angle=10;
      
      
      for (int i=0; i<10; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/11.0,0,-395));
	  list_photon.push_back(ph);
	}
      angle=20;
      
      
      for (int i=0; i<10; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/11.0,0,-395));
	  list_photon.push_back(ph);
	}
     angle=30;
      
      
      for (int i=0; i<10; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/11.0,0,-395));
	  list_photon.push_back(ph);
	}
      
      angle=40;
      
      
      for (int i=0; i<10; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/11.0,0,-395));
	  list_photon.push_back(ph);
	}
      
    }
  
  
  // propagate writes to geo, that has finished, therefore, close geo
  


  if (iopt!=2) 
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
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"test_barrel3_noairgap ";
  if (lens_opt == 1) scr<<" lopt=1 ";
  if (lens_opt == 2) scr<<" lopt=2 ";
  if (lens_opt == 3) scr<<" lopt=3 ";
  if (focus_opt== 1) scr<<" focussed \"";
  if (focus_opt== 0) scr<<" unfocussed \"";
  scr<<",500,-400,400);"<<endl;
  scr<<"    hgr->SetStats(0);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-400);"<<endl;
  scr<<"    hgr->SetMaximum(400);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;


  TFile* hfile = (TFile*)gROOT->FindObject("test_barrel3_noairgap.root"); 
  if (hfile) hfile->Close();
  hfile = new TFile("test_barrel3_noairgap.root","RECREATE","Spatial resolutions");
  
  //TH2D* hxy        = new TH2D("hxy",       "y vs x", 150,-300,300,100,-200,200);
  TH2D* h2       = new TH2D("h2",      "plane",    801,-400,400,801,-400,400);
  TH1D* h1       = new TH1D("h1",      "plane_x",    801,-400,400);



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
	  h2->Fill(xx,yy);
	  if (fabs(yy)<20) h1->Fill(xx);
	  
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

  hfile->Write();
  
  hfile->Close();
  

  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;
  
  delete manager;
  


  return EXIT_SUCCESS;

}
