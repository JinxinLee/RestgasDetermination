#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include <iostream>

Double_t xmin;
Double_t xmax;

TRandom3 *myran;

TF1 *func=NULL;
TF1 *fResponse=NULL;
TF1 *fSignal=NULL;
TF1 *fBW=NULL;

Double_t nastyFunction(Double_t *x, Double_t *par)
{
  Double_t f1=TMath::Exp((-(par[0]-x[0])*(par[0]-x[0]))/(2.*par[1]*par[1]));
  Double_t f2=(x[0]-par[2])*(x[0]-par[2])+par[3]*par[3]/4.;
  return f1/f2;	 
}

/*
 par[0]=scale
 par[1]=background strength
 par[2]=signal strength
 par[3]=beam resolution
 par[4]=mass of resonance
 par[5]=width of resonance
 */

Double_t pandaResponse(Double_t *x, Double_t *par)
{
  if (!func) func=new TF1("func",nastyFunction,xmin,xmax,4);
  func->SetNpx(1000);
  Double_t xx =x[0];
  func->SetParameter(0,xx);
  func->SetParameter(1,par[3]);
  func->SetParameter(2,par[4]);
  func->SetParameter(3,par[5]);
  Double_t f1 = func->Integral(xx-10*par[3],xx+10*par[3]);
  Double_t f2 = par[1]+f1*par[2]*par[5]*par[5]/(4.*TMath::Sqrt(2*TMath::Pi())*par[3]);
  return par[0]*f2;
}


Double_t signalResponse(Double_t *x, Double_t *par)
{
  if (!func) func=new TF1("func",nastyFunction,xmin,xmax,4);
  func->SetNpx(1000);
  Double_t xx =x[0];
  func->SetParameter(0,xx);
  func->SetParameter(1,par[3]);
  func->SetParameter(2,par[4]);
  func->SetParameter(3,par[5]);
  Double_t f1 = func->Integral(xx-10*par[3],xx+10*par[3]);
  Double_t f2 = f1*par[2]*par[5]*par[5]/(4.*TMath::Sqrt(2*TMath::Pi())*par[3]);
  return par[0]*f2;
}

Double_t breitWigner(Double_t *x, Double_t *par)
{
  Double_t f1=par[0]*par[2]*par[2];
  Double_t f2=(x[0]-par[1])*(x[0]-par[1])+par[2]*par[2]/4.;
  return f1/f2;	 
}

void setShape(Double_t stat,Double_t peaktobackground, Double_t beamresolution,Double_t resonancewidth, Double_t resonancemass)
{
  if (NULL==fResponse) 
    {
      fResponse=new TF1("fResponse",pandaResponse,xmin,xmax,6);
      fResponse->SetNpx(1000);
    }

  fResponse->SetParameter(0,stat);
  fResponse->SetParameter(1,1./peaktobackground);
  fResponse->FixParameter(2,1);
  fResponse->FixParameter(3,beamresolution*1e-6);
  fResponse->SetParameter(4,resonancemass);
  fResponse->SetParameter(5,resonancewidth*1e-6);

  return;
}

void getCounts(Int_t n,Double_t *masses, Double_t *counts)
{
  for (Int_t i=0; i<n; i++)
	{
           counts[i]=myran->Poisson(fResponse->Eval(masses[i]));
        }
   return;	
} 

void fittest(Int_t nroftests=1, Char_t option[]="LQN", Double_t stat=200,Double_t peaktobackground=8, Double_t beamresolution=50.,Double_t resonancewidth=500., Double_t resonancemass=3.526, Int_t seed=0, Bool_t silent=kFALSE)
{
  Double_t mass[2],width[2],chisqr;
  Double_t xbins[13]={3.523, 3.525, 3.5252, 3.5254, 3.5256, 3.5258, 3.5260, 3.5262, 3.5264, 3.5266, 3.5268, 3.527, 3.529};
  Double_t masses[12], counts[12];

  myran=new TRandom3(seed);

  xmin=xbins[0];xmax=xbins[12];

  for (Int_t i=0; i<13; i++)
   {
    masses[i]=(xbins[i]+xbins[i+1])/2.;
   }

  char command[128];
  sprintf(command,"output_stat=%i_p2b=%i_pres=%i_width=%i.root",(int) stat, (int) peaktobackground, (int) beamresolution, (int) resonancewidth);
  TFile *f=new TFile(command,"RECREATE");
  
  TH1D *hisData=new TH1D("hisData","",12,xbins);
  TH1D *his=new TH1D("hisWidth","",100,resonancewidth-10*beamresolution,resonancewidth+10*beamresolution);
  TH1D *his2=new TH1D("hisMass","",100,-resonancewidth*2e-7+(xmin+xmax)/2.,resonancewidth*2e-7+(xmin+xmax)/2.);

  TH1D *his3=new TH1D("hiseMass","",100,0,beamresolution);
  TH1D *his4=new TH1D("hiseWidth","",100,0,resonancewidth);

  TH2D *hisMassvsError=new TH2D("hisMassvsError","",100,-resonancewidth*2e-7+(xmin+xmax)/2.,resonancewidth*2e-7+(xmin+xmax)/2.,100,0,beamresolution);
  TH2D *hisWidthvsError=new TH2D("hisWidthvsError","",100,resonancewidth-10*beamresolution,resonancewidth+10*beamresolution,100,0,resonancewidth);

  TH1D *hisxsq=new TH1D("hisxsq","",100,0,5);

  Double_t sumemass=0;
  Double_t sumewidth=0;
  Int_t count=0;

  TGraphErrors *grData=new TGraphErrors(12);

  for (Int_t i=0; i<nroftests; i++)
  {
    setShape(stat,peaktobackground,beamresolution,resonancewidth,resonancemass);
    getCounts(12,masses,counts);
    for (Int_t j=0; j<12; j++)
	{
         hisData->SetBinContent(j+1,counts[j]);
	 grData->SetPointError(j,0,TMath::Sqrt(counts[j]));
	 grData->SetPoint(j,masses[j],counts[j]);
        }

   if (!silent) cout << i << " of the " << nroftests << endl;
   Int_t retval = hisData->Fit("fResponse",option);
   if (0==retval)
    {
     count++;
     mass[0]=fResponse->GetParameter(4);
     mass[1]=fResponse->GetParError(4);
     width[0]=fabs(1e6*fResponse->GetParameter(5));
     width[1]=fabs(1e6*fResponse->GetParError(5));
     chisqr=fResponse->GetChisquare()/fResponse->GetNDF();

     if (!silent)
     {
	 cout << "Chisq = " << chisqr << endl;
	 cout << "Mass  = " << (Double_t) mass[0] << " +- " << (Double_t) mass[1] << " GeV" << endl;
	 cout << "Width = " << (Double_t) width[0] << " +- " << (Double_t) width[1] << " keV" << endl;
     }
     his->Fill(width[0]);
     his2->Fill(mass[0]);
     hisxsq->Fill(chisqr);

     his3->Fill(mass[1]*1e6);
     his4->Fill(width[1]);

     hisMassvsError->Fill(mass[0],mass[1]*1e6);
     hisWidthvsError->Fill(width[0],width[1]);

     sumemass+=mass[1];
     sumewidth+=width[1];

     if (NULL==fSignal) 
       {
	 fSignal=new TF1("fSignal",signalResponse,xmin,xmax,6);
	 fSignal->SetNpx(1000);
	 fSignal->SetLineColor(kRed);
       }

     for (Int_t i=0; i<6; i++)
       {
	 fSignal->SetParameter(i,fResponse->GetParameter(i));
       }

     if (NULL==fBW) 
       {
	 fBW=new TF1("fBW",breitWigner,xmin,xmax,3);
	 fBW->SetNpx(1000);
	 fBW->SetLineColor(kBlue);
       }

     fBW->SetParameter(0,fResponse->GetParameter(0));
     fBW->SetParameter(1,fResponse->GetParameter(4));
     fBW->SetParameter(2,fResponse->GetParameter(5));

    }
   if (!silent)
   {
       cout << "Mass error  " << 1e6*sumemass/((Double_t) count) << endl;
       cout << "Width error " << sumewidth/((Double_t) count) << endl; 
   }
  }

  TH1D *dum=new TH1D("dum","",100,xmin,xmax);
  dum->SetMinimum(0);
  dum->SetMaximum(1.2*fResponse->Eval(resonancemass));

  Double_t val=1.1*fResponse->Eval(resonancemass)/fBW->Eval(resonancemass);
  fBW->SetParameter(0,val*fBW->GetParameter(0));

  dum->Write();
  fResponse->Write();
  fBW->Write();
  grData->Write();
  hisData->Write();
  his->Write();
  his2->Write();
  his3->Write();
  his4->Write();
  hisxsq->Write();
  hisMassvsError->Write();
  hisWidthvsError->Write();

  f->Close();
}
