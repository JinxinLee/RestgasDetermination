// ***************************************************************
//
// Create a new map for the trapping fraction of the DISC DIRC
// 
// Results will be stored in trapfrac_disc.root which has to be
// copied to pandaroot/fsim and is then read in by PndFsmDrcDisc
//
// execute in CINT like:
//
// .x trapmap_disc.C+(#bins, #photons, pmax, H)
//
// #bins    = number of bins in p and theta
// #photons = number of generated photons to determine fraction
//            (1000 -> accuracy = 0.001)
// pmax     = upper momentum limit
// H        = magnetic field strength used
//
// default theta range: 0 < theta < 25 
//
// ***************************************************************


#include <iostream>
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TVector3.h"
#include "TBox.h"
#include "TFile.h"
#include <math.h>

using std::cout;
using std::endl;

void config_histo(TH1 *h, TString tx, TString ty,double offy=1.65)
{
  h->SetLineWidth(1);
  h->SetLineColor(1);
  h->SetFillColor(3);
  //h->Sumw2();

  h->GetXaxis()->SetTitleOffset(1.3);
  h->GetXaxis()->SetTitleColor(1);
  h->GetXaxis()->SetTitleFont(42);
  h->GetXaxis()->SetLabelSize(0.045);
  h->GetXaxis()->SetTitleSize(0.045);
  //h->GetXaxis()->SetNdivisions(510);
  
  h->GetYaxis()->SetTitleOffset(offy);
  h->GetYaxis()->SetTitleFont(42);
  h->GetYaxis()->SetLabelSize(0.045);
  h->GetYaxis()->SetTitleSize(0.045);
  
  h->SetXTitle(tx);
  h->SetYTitle(ty);

}

int trapmap_disc(double steps=100, double phst=500, double pmax=6.0, double H=2)
{
  double mass[5]={0.000511,0.10566,0.13957,0.493677,0.938272};

  TString title[5];
  title[0]="Electrons";
  title[1]="Muons";
  title[2]="Pions";
  title[3]="Kaons";
  title[4]="Protons";
  
  TCanvas *c1=new TCanvas("c1","c1",800,600);
  c1->Divide(3,2);

  int i;
  
  double Pi=3.141592653589793;
  
  double n=1.472;
  
  double thtmin=0,thtmax=25;
  double pmin=0;
  double thtsteps=steps;
  double phisteps=phst;
  double psteps=steps;
  
  double pbin=(pmax-pmin)/psteps;
  double thtbin=(thtmax-thtmin)/thtsteps;

  TH1F *hftht2=new TH1F("hftht2","part total refl no curvature",(int)thtsteps+1,thtmin-0.5*thtbin,thtmax+0.5*thtbin);
  hftht2->SetLineColor(2);

  TH2F *hacc[5];
  for (i=0;i<5;i++)
  {
    char tmp[20];
    sprintf(tmp,"hacc%d",i);
    hacc[i]=new TH2F(tmp,title[i],(int)psteps+1,pmin-0.5*pbin,pmax+0.5*pbin,(int)thtsteps+1,thtmin-0.5*thtbin,thtmax+0.5*thtbin);
    hacc[i]->SetMaximum(1);
    config_histo(hacc[i],"p[GeV/c^{2}]","#theta [deg]");
  }
  

  double tht_tref=asin(1./n);
  cout <<tht_tref/3.1416*180<<endl;

  double phi=0;

  
  int pid=0;
  int ip=0;

  for (pid=0;pid<5;pid++){
  cout <<"pid="<<pid<<endl;
  for (ip=0;ip<=psteps;ip++)
  {
    double p=pmin+ip*pbin;
    double thetac=0;
    double cthetac=sqrt(mass[pid]*mass[pid]+p*p)/(p*n);
   

    if (cthetac<=1) thetac=acos(cthetac);
  
    double tht;//=theta/180.*3.1416;
    double thc=thetac;// /180.*3.1416;
    
    if (thc>0)
    for (double itht=thtmin;itht<=thtmax;itht+=thtbin)
    {
      tht=itht/180.*Pi;
     
       
      double phistsize=Pi*2/phisteps;
      
      double count=0;
    
    
      for( phi=0;phi<2*Pi;phi+=phistsize)
      {
        TVector3 v1(1,sin(phi)*tan(thc),cos(phi)*tan(thc));
	
        v1.RotateY(tht);

        TVector3 v2(1,0,0);

        double ang=v1.Angle(v2);
	
	if (ang>Pi/2) ang=fabs(ang-Pi);
	
        if (ang>=tht_tref) ++count;
      }
      hacc[pid]->Fill(p,itht,count/phisteps);
    }
  }
  }  
  
  TString opt("surf1");
  
  for (pid=0;pid<5;pid++) 
  {
     c1->cd(pid+1);
     hacc[pid]->Draw(opt);
  }
  c1->cd();

  TFile *f=new TFile("trapfrac_disc.root","recreate");
  for (pid=0;pid<5;pid++) 
      hacc[pid]->Write();
  f->Close();
  return 0;
}
