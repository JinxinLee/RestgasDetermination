
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

#include "test_reco_full_setup.h"

// Generate lookup tables (LUT) for reconstruction



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

  TFile*  hfile = new TFile("test_reco_full_generate_lut.root","READ"); 

  TH2D* lut_x[99];
  TH2D* lut_y[99];
  TH2D* lut_z[99];
  


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
  lut_x[6]  = (TH2D*)hfile->Get("kbarx_side24");
  lut_y[6]  = (TH2D*)hfile->Get("kbary_side24");
  lut_z[6]  = (TH2D*)hfile->Get("kbarz_side24");
  lut_x[7]  = (TH2D*)hfile->Get("kbarx_side25");
  lut_y[7]  = (TH2D*)hfile->Get("kbary_side25");
  lut_z[7]  = (TH2D*)hfile->Get("kbarz_side25");
  lut_x[8]  = (TH2D*)hfile->Get("kbarx_side32");
  lut_y[8]  = (TH2D*)hfile->Get("kbary_side32");
  lut_z[8]  = (TH2D*)hfile->Get("kbarz_side32");
  lut_x[9]  = (TH2D*)hfile->Get("kbarx_side34");
  lut_y[9]  = (TH2D*)hfile->Get("kbary_side34");
  lut_z[9]  = (TH2D*)hfile->Get("kbarz_side34");
  lut_x[10] = (TH2D*)hfile->Get("kbarx_side35");
  lut_y[10] = (TH2D*)hfile->Get("kbary_side35");
  lut_z[10] = (TH2D*)hfile->Get("kbarz_side35");
  lut_x[11] = (TH2D*)hfile->Get("kbarx_side42");
  lut_y[11] = (TH2D*)hfile->Get("kbary_side42");
  lut_z[11] = (TH2D*)hfile->Get("kbarz_side42");
  lut_x[12] = (TH2D*)hfile->Get("kbarx_side43");
  lut_y[12] = (TH2D*)hfile->Get("kbary_side43");
  lut_z[12] = (TH2D*)hfile->Get("kbarz_side43");
  lut_x[13] = (TH2D*)hfile->Get("kbarx_side45");
  lut_y[13] = (TH2D*)hfile->Get("kbary_side45");
  lut_z[13] = (TH2D*)hfile->Get("kbarz_side45");
  lut_x[14] = (TH2D*)hfile->Get("kbarx_side52");
  lut_y[14] = (TH2D*)hfile->Get("kbary_side52");
  lut_z[14] = (TH2D*)hfile->Get("kbarz_side52");
  lut_x[15] = (TH2D*)hfile->Get("kbarx_side53");
  lut_y[15] = (TH2D*)hfile->Get("kbary_side53");
  lut_z[15] = (TH2D*)hfile->Get("kbarz_side53");
  lut_x[16] = (TH2D*)hfile->Get("kbarx_side54");
  lut_y[16] = (TH2D*)hfile->Get("kbary_side54");
  lut_z[16] = (TH2D*)hfile->Get("kbarz_side54");
  int limit_two_reflections = 16;

  lut_x[17] = (TH2D*)hfile->Get("kbarx_side234");
  lut_y[17] = (TH2D*)hfile->Get("kbary_side234");
  lut_z[17] = (TH2D*)hfile->Get("kbarz_side234");
  lut_x[18] = (TH2D*)hfile->Get("kbarx_side235");
  lut_y[18] = (TH2D*)hfile->Get("kbary_side235");
  lut_z[18] = (TH2D*)hfile->Get("kbarz_side235");
  lut_x[19] = (TH2D*)hfile->Get("kbarx_side242");
  lut_y[19] = (TH2D*)hfile->Get("kbary_side242");
  lut_z[19] = (TH2D*)hfile->Get("kbarz_side242");
  lut_x[20] = (TH2D*)hfile->Get("kbarx_side243");
  lut_y[20] = (TH2D*)hfile->Get("kbary_side243");
  lut_z[20] = (TH2D*)hfile->Get("kbarz_side243");
  lut_x[21] = (TH2D*)hfile->Get("kbarx_side245");
  lut_y[21] = (TH2D*)hfile->Get("kbary_side245");
  lut_z[21] = (TH2D*)hfile->Get("kbarz_side245");
  lut_x[22] = (TH2D*)hfile->Get("kbarx_side253");
  lut_y[22] = (TH2D*)hfile->Get("kbary_side253");
  lut_z[22] = (TH2D*)hfile->Get("kbarz_side253");
  lut_x[23] = (TH2D*)hfile->Get("kbarx_side254");
  lut_y[23] = (TH2D*)hfile->Get("kbary_side254");
  lut_z[23] = (TH2D*)hfile->Get("kbarz_side254");
  lut_x[24] = (TH2D*)hfile->Get("kbarx_side325");
  lut_y[24] = (TH2D*)hfile->Get("kbary_side325");
  lut_z[24] = (TH2D*)hfile->Get("kbarz_side325");
  lut_x[25] = (TH2D*)hfile->Get("kbarx_side342");
  lut_y[25] = (TH2D*)hfile->Get("kbary_side342");
  lut_z[25] = (TH2D*)hfile->Get("kbarz_side342");
  lut_x[26] = (TH2D*)hfile->Get("kbarx_side345");
  lut_y[26] = (TH2D*)hfile->Get("kbary_side345");
  lut_z[26] = (TH2D*)hfile->Get("kbarz_side345");
  lut_x[27] = (TH2D*)hfile->Get("kbarx_side352");
  lut_y[27] = (TH2D*)hfile->Get("kbary_side352");
  lut_z[27] = (TH2D*)hfile->Get("kbarz_side352");
  lut_x[28] = (TH2D*)hfile->Get("kbarx_side353");
  lut_y[28] = (TH2D*)hfile->Get("kbary_side353");
  lut_z[28] = (TH2D*)hfile->Get("kbarz_side353");
  lut_x[29] = (TH2D*)hfile->Get("kbarx_side354");
  lut_y[29] = (TH2D*)hfile->Get("kbary_side354");
  lut_z[29] = (TH2D*)hfile->Get("kbarz_side354");
  lut_x[30] = (TH2D*)hfile->Get("kbarx_side423");
  lut_y[30] = (TH2D*)hfile->Get("kbary_side423");
  lut_z[30] = (TH2D*)hfile->Get("kbarz_side423");
  lut_x[31] = (TH2D*)hfile->Get("kbarx_side424");
  lut_y[31] = (TH2D*)hfile->Get("kbary_side424");
  lut_z[31] = (TH2D*)hfile->Get("kbarz_side424");
  lut_x[32] = (TH2D*)hfile->Get("kbarx_side425");
  lut_y[32] = (TH2D*)hfile->Get("kbary_side425");
  lut_z[32] = (TH2D*)hfile->Get("kbarz_side425");
  lut_x[33] = (TH2D*)hfile->Get("kbarx_side432");
  lut_y[33] = (TH2D*)hfile->Get("kbary_side432");
  lut_z[33] = (TH2D*)hfile->Get("kbarz_side432");
  lut_x[34] = (TH2D*)hfile->Get("kbarx_side452");
  lut_y[34] = (TH2D*)hfile->Get("kbary_side452");
  lut_z[34] = (TH2D*)hfile->Get("kbarz_side452");
  lut_x[35] = (TH2D*)hfile->Get("kbarx_side523");
  lut_y[35] = (TH2D*)hfile->Get("kbary_side523");
  lut_z[35] = (TH2D*)hfile->Get("kbarz_side523");
  lut_x[36] = (TH2D*)hfile->Get("kbarx_side532");
  lut_y[36] = (TH2D*)hfile->Get("kbary_side532");
  lut_z[36] = (TH2D*)hfile->Get("kbarz_side532");
  lut_x[37] = (TH2D*)hfile->Get("kbarx_side534");
  lut_y[37] = (TH2D*)hfile->Get("kbary_side534");
  lut_z[37] = (TH2D*)hfile->Get("kbarz_side534");
  lut_x[38] = (TH2D*)hfile->Get("kbarx_side535");
  lut_y[38] = (TH2D*)hfile->Get("kbary_side535");
  lut_z[38] = (TH2D*)hfile->Get("kbarz_side535");
  lut_x[39] = (TH2D*)hfile->Get("kbarx_side542");
  lut_y[39] = (TH2D*)hfile->Get("kbary_side542");
  lut_z[39] = (TH2D*)hfile->Get("kbarz_side542");
  lut_x[40] = (TH2D*)hfile->Get("kbarx_side543");
  lut_y[40] = (TH2D*)hfile->Get("kbary_side543");
  lut_z[40] = (TH2D*)hfile->Get("kbarz_side543");
  int limit_three_reflections = 40;

  TFile*  h2file = (TFile*)gROOT->FindObject("test_reco_full.root"); 
  if (h2file) h2file->Close();
  h2file = new TFile("test_reco_full.root","RECREATE","Reconstruction of theta_c");

  TH1D*   hthetac0        = new TH1D("hthetac0"   ,"theta_c (mrad) 0   reflections ",200,0,1000*pi);
  TH1D*   hthetac1        = new TH1D("hthetac1"   ,"theta_c (mrad) <=1 reflections ",200,0,1000*pi);
  TH1D*   hthetac2        = new TH1D("hthetac2"   ,"theta_c (mrad) <=2 reflections ",200,0,1000*pi);
  TH1D*   hthetac3        = new TH1D("hthetac3"   ,"theta_c (mrad) <=3 reflections ",200,0,1000*pi);


  TH1D*   hphotons       = new TH1D("hphotons"  ,"measured photons",200,0,200);
  TH1D*   hlambda        = new TH1D("hlambda"  ,"measured photons",200,0,1000);


  PndDrcOptDevManager* manager = new PndDrcOptDevManager();


  setup_geometry(manager); // geometrical setup in test_reco_full_setup.cc
   


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
  geo<<"    view->SetRange(-500,-500,-900,500,500,100);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(90,90,90,i);"<<endl;
  for (int i=0; i<16;i++) // 7 or 16
    geo<<"    view->Zoom();"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  //manager->Print(geo);

  XYZPoint  pos_part(0,-10,625);
  XYZVector dir_part(0,1,1); 
  double    beta = 0.99;
  TRandom   ran;

  for (int ievent=0; ievent<100; ievent++)
    {
  
      int imeasured = 0;
      
      bool photons_exist = manager->Cerenkov(pos_part,dir_part,beta);// generate photon
      if (photons_exist) manager->Propagate();                           // propagate photons
	
      list<PndDrcPhoton> list_photon = manager->PhotonList();            // get list
  


      list<PndDrcPhoton>::iterator iph;
      for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	{
	  if      ((*iph).Fate()==Drc::kPhotMeasured && effi((*iph).Wavelength())) 
	    {
	      imeasured++;
	      hlambda->Fill((*iph).Wavelength());
	  
	      // recontruction
	      // the end of the is at (0,0,0) the photon vector is
	      double xx=fabs((*iph).Position().X());
	      double yy=fabs((*iph).Position().Y());

	      for (int ilut=0; ilut<=limit_three_reflections; ilut++)
		{
		  double kx = lut_x[ilut]->GetBinContent(lut_x[ilut]->FindBin(xx,yy));
		  double ky = lut_y[ilut]->GetBinContent(lut_y[ilut]->FindBin(xx,yy));
		  double kz = lut_z[ilut]->GetBinContent(lut_z[ilut]->FindBin(xx,yy));
		  if (fabs(kz)>0.001) 
		    {		      
		      XYZVector dir_pix(kx,ky,kz);
		      if (ilut<=limit_zero_reflections)  ambiguities(hthetac0,dir_part,dir_pix);
		      if (ilut<=limit_one_reflections)   ambiguities(hthetac1,dir_part,dir_pix);
		      if (ilut<=limit_two_reflections)   ambiguities(hthetac2,dir_part,dir_pix);
		      if (ilut<=limit_three_reflections) ambiguities(hthetac3,dir_part,dir_pix);
		    }
		  
		}
	      
	      
	    }
	  
	  // -----------------------------------------------------------------
	}
      
      hphotons->Fill(imeasured);
      cout<<"event:\t "<<ievent<<" measured photons: \t"<<imeasured<<endl;
      
      
      
      manager->ClearPhotonList();
      
      
    }
  
  
  // propagate writes to geo, that has finished, therefore, close geo
  geo<<"}"<<endl;
  geo.close();
  
  
  h2file->Write();









  exit(0);
  
}
