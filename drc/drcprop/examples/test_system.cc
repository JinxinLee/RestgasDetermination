

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
#include "TFile.h"
#include "TH1F.h"
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
#include "PndDrcEffiPerfect.h"
#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"

#include "PndDrcOptBurle.h"

int main(int argc, char *argv[])
{



  /*  

    Combine several optical systems to one system

  */

  const double pi=3.1415926535;


  PndDrcOptBrick bar(17,17.5/2,1000);
  bar.SetOptMaterial(PndDrcOptMatLithotecQ0());
  bar.SetName("bar");

  bar.Surface("side6")->SetReflectivity(PndDrcOptReflPerfect()); //**********
  //bar.Surface("side1")->SetPixel();


  bar.AddTransform(Transform3D(XYZVector(0,500,0)));
  

  PndDrcOptBrick box(600,600,200);
  box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  box.SetName("box");
  //box.Surface("side6")->SetReflectivity(PndDrcOptReflSilver());
  //box.Surface("side1")->SetPixel();
  box.AddTransform(Transform3D(XYZVector(0,0,-1200))); // bar + 1/2 box
  

  PndDrcOptDevSys opt_sys_box;
  opt_sys_box.AddDevice(box); // box side6 is to couple to the bars
  opt_sys_box.SetNameCopyNumber("sysbox",0);

  PndDrcOptDevSys opt_sys_bar;
  opt_sys_bar.AddDevice(bar); // box side6 is to couple to the bars

  PndDrcOptDevSys opt_system;
  opt_system.AddDeviceSystem(opt_sys_box);
  
  
  for (int ibar=0; ibar<90; ibar++)
    {
      opt_sys_bar.AddTransform(Transform3D(RotationZ(pi/180.*4)));
      opt_sys_bar.SetNameCopyNumber("sysbar",ibar);
      opt_system.AddDeviceSystem(opt_sys_bar);
      opt_system.CoupleDeviceSystem("sysbar","sysbox","bar","box","side1","side6",ibar);
    }
  
  // 16x16 array of burles
  double wp = 58;
  double ws = (1200 - 16*wp)/15;
  int icnt1=0;
  
  
  PndDrcEffiPerfect effi;

  for (int ix=0; ix<16; ix++)
    {
      for (int iy=0; iy<16; iy++)
	{
	  //int ix=0,iy=0;
	  
	  PndDrcOptBurle burle;
	  //burle.SetEffi(effi);  // default is bialkali
	  //burle.SetPosCorr(false); // default is turned on.
	  burle.AddTransform(
			     Transform3D(XYZVector(-600+wp/2 + ix*(wp+ws),
						   -600+wp/2 + iy*(wp+ws),-1400)));
	  burle.SetNameCopyNumber("burle",icnt1);
	  opt_system.AddDeviceSystem(burle);
	  //opt_system.Print();
	  opt_system.CoupleDeviceSystem("burle","sysbox","housing","box",
					"front","side1",icnt1);
	  icnt1++;
	  
	}
    }
  
 

  //opt_system.Print();
  
  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  //manager->SetVerbosity(5);
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
  geo<<"    view->SetRange(-1000,-1000,-1000,1000,1000,1000);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(0,90,90,i);"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager->Print(geo);
  //
  // the intention is to play around with routines.



  // create a list of photons in bar

  XYZPoint  pos(0,-10,0);
  XYZVector dir(0,1,1); 
  double   beta = 0.85;
  bool photons_exist = manager->Cerenkov(pos,dir,beta); // gen.photons
  if (photons_exist) manager->Propagate();              // propagate photons
  list<PndDrcPhoton> list_photon = manager->PhotonList();  // get list
  
  // propagate writes to geo, that has finished, therefore, close geo
  geo<<"}"<<endl;
  geo.close();

  // analyse list
  fstream scr;
  scr.open("Screen.C",std::ios::out);
  scr << "{" << endl;
  scr << "gStyle->SetCanvasColor(0);"       << endl;      
  scr << "gStyle->SetCanvasBorderMode(0);"  << endl;      
  scr << "gStyle->SetFrameBorderMode(0);"   << endl;       
  scr << "gStyle->SetTitleFillColor(0);"    << endl;        
  scr << "gStyle->SetTitleFontSize(0.05);"  << endl;
  scr << "TCanvas *c1 = new TCanvas( \"c1\", \"\" ,200, 10, 700, 500 );" << endl;
  TString str_beta;
  str_beta+=beta;
  str_beta.Remove(TString::kLeading,' ');
  scr << "TString title;"<<endl;
  scr << "title=\"spatial position [mm] (beta=" << str_beta << ")\";"<< endl;
  scr << "    TH1F *hgr = new TH1F(\"hgr1\",title,600,-600,600);"<<endl;
  scr << "hgr->SetStats( 0 );" << endl;  
  scr << "    hgr->SetMarkerStyle(7);"<<endl;
  scr << "    hgr->SetMarkerSize(0.5);"<<endl;
  scr<<"    hgr->SetMinimum(-600);"<<endl;
  scr<<"    hgr->SetMaximum(+600);"<<endl;
  scr << "    hgr->Draw(\"POL\");"<<endl;
  int spectrum[100] = {100*0};
  int absorpt1[30] = {30*0};
  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;
  list<PndDrcPhoton>::iterator iph;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      if      ((*iph).Fate()==Drc::kPhotMeasured) icnt_measured++;
      else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) 
	{
	  icnt_absorbed++;
	  XYZPoint pos = (*iph).Position();
	  double z     = pos.Z();
	  absorpt1[(int)((z+1500)/100+0.5)]++; // -150cm ... 150cm steps of 10 cm
	}
      else                                       icnt_lost++;

      if ((*iph).Fate()==Drc::kPhotMeasured)
	{
	  XYZPoint pos = (*iph).Position();
	  double x     = pos.X();
	  double y     = pos.Y();

	  scr<<"    TMarker* t = new TMarker("<<x<<","<<y<<",20);"<<endl;
	  scr<<"    t->SetMarkerStyle(7);"<<endl;
	  scr<<"    t->SetMarkerColor("<<(*iph).ColorNumber((*iph).Wavelength())<<");"<<endl;
	  scr<<"    t->SetMarkerSize(0.7);"<<endl;
	  scr<<"    t->Draw();"<<endl; 

	  spectrum[(int)(((*iph).Wavelength()/10)+0.5)]++;
	}
    }
  scr<<"}"<<endl;
  scr.close();
 
  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;

  cout<<" measured photon wavelength:"<<endl;
  for (int i=20; i<40; i++)
    {
      cout<<i*10<<"\t\t"<<spectrum[i]<<"      "<<(i+20)*10<<"\t\t"<<spectrum[i+20]<<endl;
    } 

  cout<<" absorption z [cm]:"<<endl;

  for (int i=0; i<30; i++)
    {
      cout<<i*10-150<<"\t\t"<<absorpt1[i]<<endl;
    } 


  delete manager;

  return EXIT_SUCCESS;



  
}

