

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

int main(int argc, char *argv[])
{

 
  // Example for a simple bar with flat downstream mirror and expansion box.

  const double pi=3.1415926535;

 
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


  // event loop
  int     nevents        = 10000;
  TRandom ran;
  double phi,costh,sinth;
  

  TFile*  hfile = (TFile*)gROOT->FindObject("test_reco_full_generate_lut.root"); 
  if (hfile) hfile->Close();
  hfile = new TFile("test_reco_full_generate_lut.root","RECREATE","LUT files");

  TH2D*   kbarx_side2 = new TH2D("kbarx_side2"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side2 = new TH2D("kbary_side2"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side2 = new TH2D("kbarz_side2"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarx_side3 = new TH2D("kbarx_side3"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side3 = new TH2D("kbary_side3"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side3 = new TH2D("kbarz_side3"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarx_side4 = new TH2D("kbarx_side4"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side4 = new TH2D("kbary_side4"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side4 = new TH2D("kbarz_side4"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarx_side5 = new TH2D("kbarx_side5"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side5 = new TH2D("kbary_side5"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side5 = new TH2D("kbarz_side5"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarx_side1 = new TH2D("kbarx_side1"   ,"kbarx",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbary_side1 = new TH2D("kbary_side1"   ,"kbary",200,-300.5,+299.5,200,-300.5,+299.5);
  TH2D*   kbarz_side1 = new TH2D("kbarz_side1"   ,"kbarz",200,-300.5,+299.5,200,-300.5,+299.5);

  list<PndDrcPhoton> list_photon;
  
  for (int ievent=0; ievent<nevents; ievent++)
    {
      list_photon.clear();
      // create a list of photons in bar
      XYZPoint  pos(0,0,+2); // 1mm below bar at the
      // bars half length.
      phi   = ran.Uniform(0,2*pi);
      costh = ran.Uniform();
      sinth = sqrt(1.0L-costh*costh);
      double kx = sinth*sin(phi);
      double ky = costh*cos(phi);
      double kz = -costh;
      XYZVector dir(kx,ky,kz); 
      PndDrcPhoton ph;
      ph.SetPosition(pos);
      ph.SetDirection(dir);
      ph.SetWavelength(400);
      list_photon.push_back(ph);
      manager->SetPhotonList(list_photon,"bar");
      manager->Propagate();              // propagate photon
      list_photon = manager->PhotonList();  // get list
      
      list<PndDrcPhoton>::iterator iph;
      int icnt_measured=0;
      for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	{
	  if      ((*iph).Fate()==Drc::kPhotMeasured) 
	    {
	      icnt_measured++;
	      // recontruction
	      // the end of the is at (0,0,0) the photon vector is
	      double xx=(*iph).Position().X();
	      double yy=(*iph).Position().Y();


	      //cout<<"--------"<<endl;
	      list<const PndDrcSurfAbs*>::iterator isurf;
	      list<const PndDrcSurfAbs*> list_surf;
	      list_surf = (*iph).SurfaceList();
	      vector<string> vec_surf(5);
	      vec_surf[0]="---";
	      vec_surf[1]="---";
	      vec_surf[2]="---";
	      vec_surf[3]="---";
	      vec_surf[4]="---";
	      
	      if (list_surf.begin() != list_surf.end())
		{
		  isurf = list_surf.end();
		  isurf--;
		  vec_surf[4] = (*isurf)->Name();
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[3] = (*isurf)->Name();
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[2] = (*isurf)->Name();
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[1] = (*isurf)->Name();
		    }
		  if (isurf != list_surf.begin())
		    {
		      isurf--;
		      vec_surf[0] = (*isurf)->Name();
		    }
		}


	      


	      if      (vec_surf[2]=="out" && vec_surf[3]=="side2" && vec_surf[4]=="pixel")
		{		  
		  kbarx_side2 -> SetBinContent(kbarx_side2->FindBin(xx,yy),kx);
		  kbary_side2 -> SetBinContent(kbary_side2->FindBin(xx,yy),ky);
		  kbarz_side2 -> SetBinContent(kbarz_side2->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[2]=="out" && vec_surf[3]=="side3" && vec_surf[4]=="pixel")
		{
		  kbarx_side3 -> SetBinContent(kbarx_side3->FindBin(xx,yy),kx);
		  kbary_side3 -> SetBinContent(kbary_side3->FindBin(xx,yy),ky);
		  kbarz_side3 -> SetBinContent(kbarz_side3->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[2]=="out" && vec_surf[3]=="side4" && vec_surf[4]=="pixel")
		{
		  kbarx_side4 -> SetBinContent(kbarx_side4->FindBin(xx,yy),kx);
		  kbary_side4 -> SetBinContent(kbary_side4->FindBin(xx,yy),ky);
		  kbarz_side4 -> SetBinContent(kbarz_side4->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[2]=="out" && vec_surf[3]=="side5" && vec_surf[4]=="pixel")
		{
		  kbarx_side5 -> SetBinContent(kbarx_side5->FindBin(xx,yy),kx);
		  kbary_side5 -> SetBinContent(kbary_side5->FindBin(xx,yy),ky);
		  kbarz_side5 -> SetBinContent(kbarz_side5->FindBin(xx,yy),kz);  
		}
	      else if (vec_surf[3]=="out" && vec_surf[4]=="pixel") //direct hit
		{
		  kbarx_side1 -> SetBinContent(kbarx_side1->FindBin(xx,yy),kx);
		  kbary_side1 -> SetBinContent(kbary_side1->FindBin(xx,yy),ky);
		  kbarz_side1 -> SetBinContent(kbarz_side1->FindBin(xx,yy),kz);  
		}
	      else
		{
		  cerr<<" *** untreated combination: "<<endl;
		  for (int i=0;i<5;i++) cout<<vec_surf[i]<<endl;
		  exit (EXIT_FAILURE);
		}
	      





		  //for(isurf=list_surf.begin(); isurf != list_surf.end(); ++isurf)
		  //{
		  //cout<<(*isurf)->Name()<<endl;
		  //}
	      
	      
	      
	      
	      // -----------------------------------------------------------------
	    }
	}
      
      //hphotons->Fill(icnt_measured);
      //cout<<"event:\t "<<ievent<<" measured photons: \t"<<icnt_measured<<endl;
    }
  
  hfile->Write();
  hfile->Close();
  
    geo<<"}"<<endl;
  geo.close();

  
  delete manager;
  
  return EXIT_SUCCESS;
  
}
