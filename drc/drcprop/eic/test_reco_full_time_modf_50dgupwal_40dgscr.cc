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

#include <cmath>

#include "TROOT.h"
#include "TRint.h"
#include "TFile.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TRotation.h"
#include "TH1.h"
#include "TH2D.h"
#include "TAxis.h"

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
#include "PndDrcOptTrd2.h"


#include "test_reco_full_setup_lens.h"
//#include "test_reco_full_setup_lens_modf_50dgupwal_40dgscr.h"


// Generate lookup tables (LUT) for reconstruction

//const double pi = 3.1415926535;
//const double c = 29.9792458;

const double pi=3.1415926535;
const double nn=1.503; //refr. index
const double c=29.9792458; //speed of light



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
  //  cout<<1000*rel_ang(dir_part,XYZVector( xx, yy, zz))<<endl;
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

 

  //TFile*  hfile = new TFile("test_reco_full_generate_lut_lens_sym_300box_9mmlens_0.001.root","READ"); 
  //TFile*  hfile = new TFile("340mm_9mm_0.001_modf_3mmpix_300w_50dgupwal_40dgscr.root","READ");
  //TFile*  hfile = new TFile("check1.root","READ");
  //TFile*  hfile = new TFile("310mm_9mm_0.001_modf_3mmpix_430w_50dgupwal_40dgscr_60.root","READ");
  //TFile*  hfile = new TFile("310mm_9mm_0.001_modf_3mmpix_300w_50dgupwal_40dgscr_quartzlens.root","READ");
  TFile*  hfile = new TFile("300mm_box_9mm_0.001_3mmpix_quarzlens.root","READ");

  TH2D* lut_x[33];
  TH2D* lut_y[33];
  TH2D* lut_z[33];
  
  lut_x[0]  = (TH2D*)hfile->Get("kbarx_side1");
  lut_y[0]  = (TH2D*)hfile->Get("kbary_side1");
  lut_z[0]  = (TH2D*)hfile->Get("kbarz_side1");
  int limit_zero_reflections = 0;
  
  lut_x[1]  = (TH2D*)hfile->Get("kbarx_side2");
  lut_y[1]  = (TH2D*)hfile->Get("kbary_side2");
  lut_z[1]  = (TH2D*)hfile->Get("kbarz_side2");
  lut_x[2]  = (TH2D*)hfile->Get("kbarx_side3");
  lut_y[2]  = (TH2D*)hfile->Get("kbary_side3");
  lut_z[2]  = (TH2D*)hfile->Get("kbarz_side3");
  lut_x[3]  = (TH2D*)hfile->Get("kbarx_side4");
  lut_y[3]  = (TH2D*)hfile->Get("kbary_side4");
  lut_z[3]  = (TH2D*)hfile->Get("kbarz_side4");
  lut_x[4]  = (TH2D*)hfile->Get("kbarx_side5");
  lut_y[4]  = (TH2D*)hfile->Get("kbary_side5");
  lut_z[4]  = (TH2D*)hfile->Get("kbarz_side5");
  int limit_one_reflections = 4;
  
  lut_x[5]  = (TH2D*)hfile->Get("kbarx_side23");
  lut_y[5]  = (TH2D*)hfile->Get("kbary_side23");
  lut_z[5]  = (TH2D*)hfile->Get("kbarz_side23");
  lut_x[6]  = (TH2D*)hfile->Get("kbarx_side25");
  lut_y[6]  = (TH2D*)hfile->Get("kbary_side25");
  lut_z[6]  = (TH2D*)hfile->Get("kbarz_side25");
  lut_x[7]  = (TH2D*)hfile->Get("kbarx_side32");
  lut_y[7]  = (TH2D*)hfile->Get("kbary_side32");
  lut_z[7]  = (TH2D*)hfile->Get("kbarz_side32");
  lut_x[8]  = (TH2D*)hfile->Get("kbarx_side34");
  lut_y[8]  = (TH2D*)hfile->Get("kbary_side34");
  lut_z[8]  = (TH2D*)hfile->Get("kbarz_side34");
  lut_x[9]  = (TH2D*)hfile->Get("kbarx_side35");
  lut_y[9]  = (TH2D*)hfile->Get("kbary_side35");
  lut_z[9]  = (TH2D*)hfile->Get("kbarz_side35");
  lut_x[10] = (TH2D*)hfile->Get("kbarx_side43");
  lut_y[10] = (TH2D*)hfile->Get("kbary_side43");
  lut_z[10] = (TH2D*)hfile->Get("kbarz_side43");
  lut_x[11] = (TH2D*)hfile->Get("kbarx_side45");
  lut_y[11] = (TH2D*)hfile->Get("kbary_side45");
  lut_z[11] = (TH2D*)hfile->Get("kbarz_side45");
  lut_x[12] = (TH2D*)hfile->Get("kbarx_side52");
  lut_y[12] = (TH2D*)hfile->Get("kbary_side52");
  lut_z[12] = (TH2D*)hfile->Get("kbarz_side52");
  lut_x[13] = (TH2D*)hfile->Get("kbarx_side53");
  lut_y[13] = (TH2D*)hfile->Get("kbary_side53");
  lut_z[13] = (TH2D*)hfile->Get("kbarz_side53");
  lut_x[14] = (TH2D*)hfile->Get("kbarx_side54");
  lut_y[14] = (TH2D*)hfile->Get("kbary_side54");
  lut_z[14] = (TH2D*)hfile->Get("kbarz_side54");
  lut_x[15] = (TH2D*)hfile->Get("kbarx_side24");
  lut_y[15] = (TH2D*)hfile->Get("kbary_side24");
  lut_z[15] = (TH2D*)hfile->Get("kbarz_side24");
  lut_x[16] = (TH2D*)hfile->Get("kbarx_side42");
  lut_y[16] = (TH2D*)hfile->Get("kbary_side42");
  lut_z[16] = (TH2D*)hfile->Get("kbarz_side42");
  int limit_two_reflections = 16;

  lut_x[27] = (TH2D*)hfile->Get("kbarx_side235");
  lut_y[27] = (TH2D*)hfile->Get("kbary_side235");
  lut_z[27] = (TH2D*)hfile->Get("kbarz_side235");
  lut_x[28] = (TH2D*)hfile->Get("kbarx_side253");
  lut_y[28] = (TH2D*)hfile->Get("kbary_side253");
  lut_z[28] = (TH2D*)hfile->Get("kbarz_side253");

  lut_x[29] = (TH2D*)hfile->Get("kbarx_side234");
  lut_y[29] = (TH2D*)hfile->Get("kbary_side234");
  lut_z[29] = (TH2D*)hfile->Get("kbarz_side234");
  lut_x[30] = (TH2D*)hfile->Get("kbarx_side432");
  lut_y[30] = (TH2D*)hfile->Get("kbary_side432");
  lut_z[30] = (TH2D*)hfile->Get("kbarz_side432");
  lut_x[31] = (TH2D*)hfile->Get("kbarx_side254");
  lut_y[31] = (TH2D*)hfile->Get("kbary_side254");
  lut_z[31] = (TH2D*)hfile->Get("kbarz_side254");
  lut_x[32] = (TH2D*)hfile->Get("kbarx_side452");
  lut_y[32] = (TH2D*)hfile->Get("kbary_side452");
  lut_z[32] = (TH2D*)hfile->Get("kbarz_side452");


  lut_x[17] = (TH2D*)hfile->Get("kbarx_side325");
  lut_y[17] = (TH2D*)hfile->Get("kbary_side325");
  lut_z[17] = (TH2D*)hfile->Get("kbarz_side325");
  lut_x[18] = (TH2D*)hfile->Get("kbarx_side345");
  lut_y[18] = (TH2D*)hfile->Get("kbary_side345");
  lut_z[18] = (TH2D*)hfile->Get("kbarz_side345");
  lut_x[19] = (TH2D*)hfile->Get("kbarx_side352");
  lut_y[19] = (TH2D*)hfile->Get("kbary_side352");
  lut_z[19] = (TH2D*)hfile->Get("kbarz_side352");
  lut_x[20] = (TH2D*)hfile->Get("kbarx_side353");
  lut_y[20] = (TH2D*)hfile->Get("kbary_side353");
  lut_z[20] = (TH2D*)hfile->Get("kbarz_side353");
  lut_x[21] = (TH2D*)hfile->Get("kbarx_side354");
  lut_y[21] = (TH2D*)hfile->Get("kbary_side354");
  lut_z[21] = (TH2D*)hfile->Get("kbarz_side354");
  lut_x[22] = (TH2D*)hfile->Get("kbarx_side523");
  lut_y[22] = (TH2D*)hfile->Get("kbary_side523");
  lut_z[22] = (TH2D*)hfile->Get("kbarz_side523");
  lut_x[23] = (TH2D*)hfile->Get("kbarx_side532");
  lut_y[23] = (TH2D*)hfile->Get("kbary_side532");
  lut_z[23] = (TH2D*)hfile->Get("kbarz_side532");
  lut_x[24] = (TH2D*)hfile->Get("kbarx_side534");
  lut_y[24] = (TH2D*)hfile->Get("kbary_side534");
  lut_z[24] = (TH2D*)hfile->Get("kbarz_side534");
  lut_x[25] = (TH2D*)hfile->Get("kbarx_side535");
  lut_y[25] = (TH2D*)hfile->Get("kbary_side535");
  lut_z[25] = (TH2D*)hfile->Get("kbarz_side535");
  lut_x[26] = (TH2D*)hfile->Get("kbarx_side543");
  lut_y[26] = (TH2D*)hfile->Get("kbary_side543");
  lut_z[26] = (TH2D*)hfile->Get("kbarz_side543");
  int limit_three_reflections = 32;

  //TFile*  h2file = (TFile*)gROOT->FindObject("reco_340mm_9mm_0.001_modf_3mmpix_300w_50dgupwal_40dgscr.root"); 
  //TFile*  h2file = (TFile*)gROOT->FindObject("check.root"); 
  //TFile*  h2file = (TFile*)gROOT->FindObject("reco_310mm_9mm_0.001_modf_3mmpix_430w_50dgupwal_40dgscr_1.root"); 
   TFile*  h2file = (TFile*)gROOT->FindObject("reco_300mm_box_9mm_0.001_quartzlens_1.root"); 
  
  if (h2file) h2file->Close();
  //h2file = new TFile("reco_340mm_9mm_0.001_modf_3mmpix_300w_50dgupwal_40dgscr.root","RECREATE","Reconstruction of theta_c");
  //h2file = new TFile("check.root","RECREATE","Reconstruction of theta_c");
  // h2file = new TFile("reco_310mm_9mm_0.001_modf_3mmpix_430w_50dgupwal_40dgscr_1.root","RECREATE","Reconstruction of theta_c");
   h2file = new TFile("reco_300mm_box_9mm_0.001_quartzlens_1.root","RECREATE","Reconstruction of theta_c");

  

  TH1D*   hthetac0          = new TH1D("hthetac0"   ,"theta_c (mrad) 0   reflections ",800,0,1000*pi);
  TH1D*   hthetac1          = new TH1D("hthetac1"   ,"theta_c (mrad) =1 reflections ",800,0,1000*pi);
  TH1D*   hthetac1_2        = new TH1D("hthetac1_2"   ,"theta_c (mrad) =1 reflections:side2 ",800,0,1000*pi);
  TH1D*   hthetac1_3        = new TH1D("hthetac1_3"   ,"theta_c (mrad) =1 reflections:side3",800,0,1000*pi);
  TH1D*   hthetac1_5        = new TH1D("hthetac1_5"   ,"theta_c (mrad) =1 reflections:side5 ",800,0,1000*pi);
  TH1D*   hthetac1_4        = new TH1D("hthetac1_4"   ,"theta_c (mrad) =1 reflections:side4 ",800,0,1000*pi);
 
  TH1D*   hthetac2        = new TH1D("hthetac2"   ,"theta_c (mrad) <=2 reflections ",800,0,1000*pi);
  TH1D*   hthetac3        = new TH1D("hthetac3"   ,"theta_c (mrad) <=2 reflections ",800,0,1000*pi);
  TH1D*   hthetac4        = new TH1D("hthetac4"   ,"theta_c (mrad) =2 reflections ",800,0,1000*pi);
  TH2D*   ang_vs_ang2     = new TH2D("ang_vs_ang2"  ,"ang ang",180,0,180,75,750,900);
  TH2D*   ang_vs_ang2_nct       = new TH2D("ang_vs_ang2_nct"  ,"ang ang",180,0,180,75,750,900);

  TH1D*   hphotons       = new TH1D("hphotons" ,"measured photons",2000,0,200);
  TH1D*   hlambda        = new TH1D("hlambda"  ,"measured photons",2000,0,1000);
  TH1D*   dtime          = new TH1D("dtime"  ,"dtof",2000,-10,10);

  TH1D*   dtime1        = new TH1D("dtime1"  ,"dtof11",2000,-10,10);
  TH1D*   dtime2        = new TH1D("dtime2"  ,"dtof12",2000,-10,10);
  TH1D*   dtime3        = new TH1D("dtime3"  ,"dtof13",2000,-10,10);


  TH1D*   n_theta_event      = new TH1D("n_theta_event"," ",180,0.,180.);
  TH1D*   n_phot_theta       = new TH1D("n_phot_theta"," ",180,0.,180.);
  TH2D*   dtimeang           = new TH2D("dtimeang"  ,"dtof ang",200,-10,10,1000,0,1000*pi);
  

  PndDrcOptDevManager* manager = new PndDrcOptDevManager();


  setup_geometry_lens(manager); // geometrical setup in test_reco_full_setup.cc
  //setup_geometry_lens_modf_50dgupwal_40dgscr(manager);

  string sfile = "Geo.C";
  fstream geo;
  geo.open(sfile.c_str(),std::ios::out);
  geo<<"{"<<endl;
  geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  //cout<<" root version: "<<((TROOT*)gROOT)->GetVersionInt()<<endl;
  
  if ( ((TROOT*)gROOT)->GetVersionInt() < 51600)
    {
      geo<<"    TView *view = new TView(1);"<<endl;
    }
  else
    {  
      geo<<"    TView *view = TView::CreateView(1);"<<endl;
    }
  geo<<"    view->SetRange(-500,-500,-900,500,500,100);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(90,90,90,i);"<<endl;

  //manager->Print(geo);


  XYZPoint pos_part(0,-10,200);

  TRandom ran;
  double phi=90;
  double theta;
  double beta = 0.99;

  
  string sfile1 = "Screen1.C";
  fstream scr;
  scr.open(sfile1.c_str(),std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"a simple graph\",600,-300,300);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-100);"<<endl;
  scr<<"    hgr->SetMaximum(400);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;

  
  int imeasureda = 0;
  for (int ievent=0; ievent<10; ievent++)
    {

      theta=ran.Uniform(30,89);
      //theta=60;
      n_theta_event->Fill(theta);
      
 
      XYZVector dir_part(sin(theta*pi/180.)*cos(phi*pi/180.),sin(theta*pi/180.)*sin(phi*pi/180.),cos(theta*pi/180.));
      int imeasured = 0;
      cout<<ievent<<"  "<< theta<<endl;
      bool photons_exist = manager->Cerenkov(pos_part,dir_part,beta,0,2000,300,700);  // generate photon
      if (photons_exist) manager->Propagate();  // propagate photons
      list<PndDrcPhoton> list_photon = manager->PhotonList();   // get list

     	 
      int nphot=0;
      list<PndDrcPhoton>::iterator iph;
      for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	{  nphot++;
	  if ((*iph).Fate()==Drc::kPhotMeasured && effi((*iph).Wavelength())) 
	    {
	      imeasured++;
	      n_phot_theta->Fill(theta);

	      // cout<<"event:\t "<<ievent<<" measured photons: \t"<<imeasured<<endl;
	      hlambda->Fill((*iph).Wavelength());
	      
	      double ph_lambda=(*iph).Wavelength();
	      double ph_tof=(*iph).Time();

	      double xx=(*iph).Position().X();
	      double yy=(*iph).Position().Y();
	      double xx0=((*iph).Position().X());
	      double yy0=((*iph).Position().Y());
	      //double zz=310;

	      //if (fabs(xx)<150. || fabs(yy)<150.) break;


	      double zz=fabs((*iph).Position().Z());
	      // cout<<"event:\t "<<ievent<<" measured photons: \t"<<imeasured<<endl;
	      //	      if(zz>109.86&zz<=310)zz==zz;
	      //    else{
	      //	zz==0;}


	      scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	      scr<<"    t->SetMarkerColor("
		 <<(*iph).ColorNumber((*iph).Wavelength())
		 <<");"<<endl;
	      scr<<"    t->SetMarkerSize(0.2);"<<endl;
	      scr<<"    t->Draw();"<<endl;




	      list<const PndDrcSurfAbs*>::iterator isurf;
	      list<const PndDrcSurfAbs*> list_surf;
	      list_surf = (*iph).SurfaceList();
	      
	      vector<string> vec_surf(5);
	      vec_surf[0]="---";
	      vec_surf[1]="---";
	      vec_surf[2]="---";
	      vec_surf[3]="---";
	      vec_surf[4]="---";

	     
	      int iii=5;
	      if (list_surf.begin() != list_surf.end())
		{
		  isurf = list_surf.end();
		  isurf--;
		  iii--;
		  vec_surf[4] = (*isurf)->Name();
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[3] = (*isurf)->Name();
		      iii--;
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[2] = (*isurf)->Name();
		      iii--;
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--; iii--;
		      vec_surf[1] = (*isurf)->Name();
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--; iii--;
		      vec_surf[0] = (*isurf)->Name();
		    }
		}



	      for (int ilut=0; ilut<=limit_two_reflections; ilut++)
		//	      for (int ilut=0; ilut<=4; ilut++)
		{

		  list<const PndDrcSurfAbs*>::iterator isurf;
		  list<const PndDrcSurfAbs*> list_surf;
		  list_surf = (*iph).SurfaceList();
		  
		  vector<string> vec_surf(5);
	      vec_surf[0]="---";
	      vec_surf[1]="---";
	      vec_surf[2]="---";
	      vec_surf[3]="---";
	      vec_surf[4]="---";

	     
	      int iii=5;
	      if (list_surf.begin() != list_surf.end())
		{
		  isurf = list_surf.end();
		  isurf--;
		  iii--;
		  vec_surf[4] = (*isurf)->Name();
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[3] = (*isurf)->Name();
		      iii--;
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[2] = (*isurf)->Name();
		      iii--;
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--; iii--;
		      vec_surf[1] = (*isurf)->Name();
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--; iii--;
		      vec_surf[0] = (*isurf)->Name();
		    }
		}		      


		  double kx = lut_x[ilut]->GetBinContent(lut_x[ilut]->FindBin(xx,yy));
		  double ky = lut_y[ilut]->GetBinContent(lut_y[ilut]->FindBin(xx,yy));
		  double kz = lut_z[ilut]->GetBinContent(lut_z[ilut]->FindBin(xx,yy));



		  double alpha  = acos(fabs(kz));
		  double alpha1 = 50*pi/180.;
		  double alpha2 = 40*pi/180.;
		  		  
		  double ev_width  = 300.0;
		  double ev_length = 310;
		  
		  double bar_height = 17.0;
		  double step_up    = 10.0;
		  double step_down  = 10.0;
		  double h0 = 2*step_up+bar_height;
		  double w2 = (ev_length*tan(alpha2)-h0)/(tan(alpha1) + tan(alpha2));
		  double w1 = ev_length - w2;
		  double h1 = w2*tan(alpha1);
		  //double ymax = h0 +h1;
		  double ymax =bar_height;
		  double l0 = h1/sin(alpha1);//170.92 for 310mm




		  //if (fabs(kz)<0.000001)
		  //{
		      //cout<<" lut missing for ilut="<<ilut<<endl;
		      //cout<<xx<<" " <<yy<<endl;
		  // }
		  //else
		    {
		    XYZVector dir_pix(kx,ky,kz);
		    if (ilut<=limit_zero_reflections)  ambiguities(hthetac0,dir_part,dir_pix);
		    //  if (ilut>limit_zero_reflections&&ilut<=limit_one_reflections)   ambiguities(hthetac1,dir_part,dir_pix);


		    /*  if (!(vec_surf[2]=="out" && vec_surf[3]=="side2" && vec_surf[4]=="pixel") || 
			(vec_surf[2]=="out" && vec_surf[3]=="side3" && vec_surf[4]=="pixel") || 
			(vec_surf[2]=="out" && vec_surf[3]=="side4" && vec_surf[4]=="pixel") ||
			(vec_surf[2]=="out" && vec_surf[3]=="side5" && vec_surf[4]=="pixel") 
			) break; */
		    
		    if (ilut!=0 && ilut<=4) ambiguities(hthetac1,dir_part,dir_pix);
		    if (ilut==2)   ambiguities(hthetac1_3,dir_part,dir_pix);
		    if (ilut==1)   ambiguities(hthetac1_2,dir_part,dir_pix);
		    if (ilut==3)   ambiguities(hthetac1_4,dir_part,dir_pix);
		    if (ilut==4)   ambiguities(hthetac1_5,dir_part,dir_pix);

		  //cout<<ilut<<" "<<(*iph).Direction().X()<<" "<<(*iph).Direction().Y()<<" "<<(*iph).Direction().Z()<<endl;
		  //cout<<imeasured<<" "<<ilut<<" "<<xx<<" "<<yy<<" "<<zz<<" "<<kx<<" "<<ky<<" "<<kz<<endl;


		    
		    double dist_in_bar=(pos_part.Z()-pos_part.Y()/tan(theta*pi/180.))/fabs(kz)/10.;   // forw ///10. is to convert to cm for speed of light
		    double dist_in_bar_refl=(2*500-(pos_part.Z()-pos_part.Y()/tan(theta*pi/180.)))/fabs(kz)/10.;   // backward
		    double dist_in_bar_refl2=(3*500-(pos_part.Z()-pos_part.Y()/tan(theta*pi/180.)))/fabs(kz)/10.;   // backward



		    //cout<<dist_in_bar <<" "<<endl;

		    double dist_in_box=0;
		    if(ilut==0)
		      dist_in_box=zz/(zz/sqrt(xx*xx+yy*yy+zz*zz))/10.; /// No refl
		    
		     if(ilut==1)
		       dist_in_box=zz/(zz/sqrt(xx*xx+(yy+(bar_height/2.+step_up)*2)*(yy+(bar_height/2.+step_up)*2)+zz*zz)); /// refl 2
		     if(ilut==2)
		       dist_in_box=zz/(zz/sqrt(yy*yy+(xx+2*ev_width)*(xx+2*ev_width)+zz*zz))/10.; /// refl 3
		     if(ilut==3)
		       dist_in_box=zz/(zz/sqrt(xx*xx+(-yy+2*ymax-(bar_height/2.+step_down)*2)*(-yy+2*ymax-(bar_height/2.+step_down)*2)+zz*zz)); /// refl 4
		     if(ilut==4)
		       dist_in_box=zz/(zz/sqrt(yy*yy+(-xx+2*ev_width)*(-xx+2*ev_width)+zz*zz))/10.; /// refl 5
		     
		     if(ilut==5)
		       dist_in_box=zz/(zz/sqrt((yy+(bar_height/2.+step_up)*2)*(yy+(bar_height/2.+step_up)*2)+(xx+2*ev_width)*(xx+2*ev_width)+zz*zz))/10.; /// refl 23
		     if(ilut==6)
		       dist_in_box=zz/(zz/sqrt((yy+(bar_height/2.+step_up)*2)*(yy+(bar_height/2.+step_up)*2)+(-xx+2*ev_width)*(-xx+2*ev_width)+zz*zz))/10.; /// refl 25
		     if(ilut==7)
		       dist_in_box=zz/(zz/sqrt((yy+(bar_height/2.+step_up)*2)*(yy+(bar_height/2.+step_up)*2)+(xx+2*ev_width)*(xx+2*ev_width)+zz*zz))/10.; /// refl 32
		     if(ilut==12)
		       dist_in_box=zz/(zz/sqrt((yy+(bar_height/2.+step_up)*2)*(yy+(bar_height/2.+step_up)*2)+(-xx+2*ev_width)*(-xx+2*ev_width)+zz*zz))/10.; /// refl 52
		     if(ilut==10)
		       dist_in_box=zz/(zz/sqrt((-yy+2*ymax-2*(bar_height/2.+step_up))*(-yy+2*ymax-2*(bar_height/2.+step_up))+(xx+2*ev_width)*(xx+2*ev_width)+zz*zz))/10.; /// refl 43
		     if(ilut==8)
		       dist_in_box=zz/(zz/sqrt((-yy+2*ymax-2*(bar_height/2.+step_up))*(-yy+2*ymax-2*(bar_height/2.+step_up))+(xx+2*ev_width)*(xx+2*ev_width)+zz*zz))/10.; /// refl 34
		     if(ilut==11)
		       dist_in_box=zz/(zz/sqrt((-yy+2*ymax-2*(bar_height/2.+step_up))*(-yy+2*ymax-2*(bar_height/2.+step_up))+(-xx+2*ev_width)*(-xx+2*ev_width)+zz*zz))/10.; /// refl 45
		     if(ilut==14)
		       dist_in_box=zz/(zz/sqrt((-yy+2*ymax-2*(bar_height/2.+step_up))*(-yy+2*ymax-2*(bar_height/2.+step_up))+(-xx+2*ev_width)*(-xx+2*ev_width)+zz*zz))/10.; /// refl 54
		     
		     if(ilut==9)
		       dist_in_box=zz/(zz/sqrt((-xx+4*ev_width)*(-xx+4*ev_width)+(yy)*(yy)+zz*zz))/10.; /// refl 35
		     if(ilut==13)
		       dist_in_box=zz/(zz/sqrt((xx+4*ev_width)*(xx+4*ev_width)+(yy)*(yy)+zz*zz))/10.; /// refl 53
		     if(ilut==15)
		       dist_in_box=zz/(zz/sqrt((xx)*(xx)+(-yy+2*ymax)*(-yy+2*ymax)+zz*zz))/10.; /// refl 24
		     if(ilut==16)
		       dist_in_box=zz/(zz/sqrt((xx)*(xx)+(yy+2*ymax)*(yy+2*ymax)+zz*zz))/10.; /// refl 42
		     
		     

		    // if(ilut==1 && alpha1<alpha){
		    //   if(2*alpha1-alpha>0){
		    // 	double hh=(300*tan(2*alpha1-alpha) - sqrt(yy*yy + xx*xx))/(tan(2*alpha1-alpha)/tan(alpha)+yy0/yy);
		    // 	cout<<alpha1*180/pi<<"  "<<kz<<" "<<acos(kz)*180/pi<<" "<<yy0<<endl;
		    // 	dist_in_box= (hh/sin(alpha) + (sqrt(yy*yy + xx*xx) +yy0/yy*hh)/sin(2*alpha1-alpha))/10.; /// refl 2
		    // 	cout << hh << " 2 " << dist_in_box <<" "<<30/(zz/sqrt(xx*xx+yy*yy+zz*zz))<< endl;
		    //   }
		    //   else{
		    // 	double hh=(300*tan(-2*alpha1+alpha) - sqrt(yy*yy + xx*xx))/(tan(-2*alpha1+alpha)/tan(alpha)+yy0/yy);
		    // 	cout<<alpha1*180/pi<<"  "<<kz<<" "<<acos(kz)*180/pi<<" "<<yy0<<endl;
		    // 	dist_in_box= (hh/sin(alpha) + (sqrt(yy*yy + xx*xx) + yy0/yy*hh)/sin(-2*alpha1+alpha))/10.; /// refl 2
		    // 	cout << hh << " 21  " << dist_in_box <<" "<<30/(zz/sqrt(xx*xx+yy*yy+zz*zz))<< endl;
		    //   }
		    // }
		    // if(ilut==2)
		    //   dist_in_box=30/(zz/sqrt(yy*yy+(xx+300)*(xx+300)+zz*zz)); /// refl 3

		    /* if(ilut==3){
		       if(sin(alpha-alpha1)!=0){
			 double d1 = (bar_height/2.+step_up)*cos(alpha1)/sin(alpha-alpha1);
			 if(cos(alpha-alpha1)!=0) {
			     double d2 = (l0 + (bar_height/2.+step_up)*sin(alpha1) - (bar_height/2.+step_up)*cos(alpha1)*cos(alpha-alpha1)/sin(alpha-alpha1))/cos(alpha-alpha1);
			     double d0 = d1 + d2;
			     dist_in_box = zz/(zz/sqrt((xx)*(xx)+d0*d0))/10.;
			   }
			   }
		       }
		       
		       if(ilut==1){
			 if(sin(alpha)!=0) {
			   double d1 = (bar_height/2. + step_up)/sin(alpha);
			   double d2 = (ev_length - (bar_height/2. + step_up)*cos(alpha)/sin(alpha))*tan(alpha2)/(tan(alpha2)*cos(alpha) + sin(alpha));
			     double d0 = d1 + d2;
			     dist_in_box = zz/(zz/sqrt((xx)*(xx) + d0*d0))/10.;
			   }
			   }*/
	     
		    //cout<<ilut<<" "<<zz<<" "<<dist_in_box<<endl;
   
		    double dist11,time11,dist12,time12,dist13,time13;
		    if(dist_in_box>0){
		    dist11= dist_in_bar + dist_in_box;
		    time11=dist11/c*nn;
		    dist12= dist_in_bar_refl + dist_in_box;
		    time12=dist12/c*nn;
		    dist13= dist_in_bar_refl2 + dist_in_box;
		    time13=dist13/c*nn;
		    }
		    else{
		      time11=0;
		      time12=0;
		      time13=0;
		    }

		    
		    double ang1=1000*rel_ang(dir_part,XYZVector( kx, ky, kz));
		    double ang2=1000*rel_ang(dir_part,XYZVector( -kx, ky, kz));
		    double ang3=1000*rel_ang(dir_part,XYZVector( kx, -ky, kz));
		    double ang4=1000*rel_ang(dir_part,XYZVector( kx, ky, -kz));
		    double ang5=1000*rel_ang(dir_part,XYZVector( -kx, -ky, kz));
		    double ang6=1000*rel_ang(dir_part,XYZVector( -kx, ky, -kz));
		    double ang7=1000*rel_ang(dir_part,XYZVector( kx, -ky, -kz));
		    double ang8=1000*rel_ang(dir_part,XYZVector( -kx, -ky, -kz));
		    //		    if(ilut==9&&(fabs(ang1-813)<13||fabs(ang2-813)<13||fabs(ang3-813)<13||fabs(ang4-813)<13||fabs(ang5-813)<13||fabs(ang6-813)<13||fabs(ang7-813)<13||fabs(ang8-813)<13)){
		    dtime->Fill(ph_tof-time11);
		    dtime->Fill(ph_tof-time12);
		    dtime->Fill(ph_tof-time13);
		    
		    dtime1->Fill(ph_tof-time11);
		    dtime2->Fill(ph_tof-time12);
		    dtime3->Fill(ph_tof-time13);

		    dtimeang->Fill(ph_tof-time11,ang1);
		    dtimeang->Fill(ph_tof-time11,ang2);
		    dtimeang->Fill(ph_tof-time11,ang3);
		    dtimeang->Fill(ph_tof-time11,ang4);
		    dtimeang->Fill(ph_tof-time11,ang5);
		    dtimeang->Fill(ph_tof-time11,ang6);
		    dtimeang->Fill(ph_tof-time11,ang7);
		    dtimeang->Fill(ph_tof-time11,ang8);
		    dtimeang->Fill(ph_tof-time12,ang1);
		    dtimeang->Fill(ph_tof-time12,ang2);
		    dtimeang->Fill(ph_tof-time12,ang3);
		    dtimeang->Fill(ph_tof-time12,ang4);
		    dtimeang->Fill(ph_tof-time12,ang5);
		    dtimeang->Fill(ph_tof-time12,ang6);
		    dtimeang->Fill(ph_tof-time12,ang7);
		    dtimeang->Fill(ph_tof-time12,ang8);
		    dtimeang->Fill(ph_tof-time13,ang1);
		    dtimeang->Fill(ph_tof-time13,ang2);
		    dtimeang->Fill(ph_tof-time13,ang3);
		    dtimeang->Fill(ph_tof-time13,ang4);
		    dtimeang->Fill(ph_tof-time13,ang5);
		    dtimeang->Fill(ph_tof-time13,ang6);
		    dtimeang->Fill(ph_tof-time13,ang7);
		    dtimeang->Fill(ph_tof-time13,ang8);


		    //if (fabs(ph_tof-time12-3)<1) cout<< ilut<<" "<<dist_in_box<< endl;

		    //		    }

		    // double traveled_disance_in_bar=zz/sqrt(xx*xx+yy*yy+zz*zz);
		    // cout<< ph_tof<<" "<<traveled_disance_in_bar/30.*1.47<<" "<<traveled_disance_in_bar/30.*1.47*3<<" "<<xx<<endl;


		    //		      if (ilut<=limit_zero_reflections)  ambiguities(hthetac0,dir_part,dir_pix);
		    //if (ilut==limit_one_reflections)   ambiguities(hthetac1,dir_part,dir_pix);
		      //cout<<ilut<<endl;
		      if (ilut>limit_one_reflections&&ilut<=limit_two_reflections) ambiguities(hthetac4,dir_part,dir_pix);
			//cout<< "two refl" <<endl;}

		      if (ilut<=limit_two_reflections){
			ang_vs_ang2_nct->Fill(theta,ang1);
			ang_vs_ang2_nct->Fill(theta,ang2);
			ang_vs_ang2_nct->Fill(theta,ang3);
			ang_vs_ang2_nct->Fill(theta,ang4);
			ang_vs_ang2_nct->Fill(theta,ang5);
			ang_vs_ang2_nct->Fill(theta,ang6);
			ang_vs_ang2_nct->Fill(theta,ang7);
			ang_vs_ang2_nct->Fill(theta,ang8);
			
			//if(fabs(ph_tof-time11-0.2)<1.5||fabs(ph_tof-time12-0.2)<1.5||fabs(ph_tof-time13-0.2)<1.5){
			  // cout<<ilut<<" "<<ph_tof<<" "<<time11<<" "<<time12<<" "<<ang1<<" "<<ang2<<" "
			  //  <<ang3<<" "<<ang4<<" "<<ang5<<" "<<ang6<<" "<<ang7<<" "<<ang8<<" "<<dist_in_bar<<endl;

			    ang_vs_ang2->Fill(theta,ang1);
			    ang_vs_ang2->Fill(theta,ang2);
			    ang_vs_ang2->Fill(theta,ang3);
			    ang_vs_ang2->Fill(theta,ang4);
			    ang_vs_ang2->Fill(theta,ang5);
			    ang_vs_ang2->Fill(theta,ang6);
			    ang_vs_ang2->Fill(theta,ang7);
			    ang_vs_ang2->Fill(theta,ang8);
			  
			  if (ilut<=limit_two_reflections)   ambiguities(hthetac2,dir_part,dir_pix);
			  if(fabs(ph_tof-time11-0.26)<0.28||fabs(ph_tof-time12-0.4)<0.5)
			    if (ilut<=limit_two_reflections) ambiguities(hthetac3,dir_part,dir_pix);


			  


			  //}
		      }
		  }
		}


	     // */	      
	    }
	  
	  // -----------------------------------------------------------------
	}
      

      //cout<< nphot <<endl;
      hphotons->Fill(imeasured);
      //cout<<"event:\t "<<ievent<<" measured photons: \t"<<imeasured<<endl;
      
      
      manager->ClearPhotonList();
      imeasureda = imeasureda + imeasured;

    }
  scr<<"}"<<endl;
  scr.close();      
  
  
  //cout << imeasureda << endl;
  
  // propagate writes to geo, that has finished, therefore, close geo
  geo<<"}"<<endl;
  geo.close();


  
  h2file->Write();


  exit(0);
  
}
