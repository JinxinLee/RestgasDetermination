//draw multiplicity  of events versus time stamp
// g++ $(root-config --cflags --glibs) DrawMulti.C -o Multi
// ./Multi 1_5
#include "TString.h"
#include "TH2D.h"
#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include<iostream>
using namespace std;
//void DrawMulti(){
int main(int nargs, char** args){
  //  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");
  TString momStr = " ";
	if (nargs == 2) {
	  momStr = args[1];
	}
  //	gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");
  //	TFile *f = new TFile("/home/karavdina/soft/pandaroot/macro/lmd/DPM_el_inel_15/Lumi_digi_0.root","READ");
  //	TFile *f = new TFile("/home/karavdina/soft/pandaroot/macro/lmd/DPMelinel_125000ev/mom_15/Lumi_DigisQA_0.root","READ");
	TString fname = "/panda/myResults/ONLINE/DPM_el_inel_";
	fname +=momStr;
	fname +="/Lumi_DigisQA_0.root";
	TString foutname = "MCtrk_in_25ns_";
	foutname +=momStr;
	foutname +="GeV.root";
	TFile *f = new TFile(fname,"READ");
	//	TFile *f = new TFile("/panda/myResults/ONLINE/DPM_el_inel_1_5/Lumi_DigisQA_0.root","READ");
	TTree *trec = (TTree*)f->Get("cbmsim");
	const int nstep = 1e2;
	//const int nstep = 10;
  double scl = 1./nstep;
  TH2D *hmlt[nstep];
  TString name[nstep];
  TH1 *hMCtrk[4]; // = new TH1I("hMCtrk0","plane #0; trk/25ns",2e2,0,2e2);
  TLegend *leg = new TLegend(0.88,0.7,0.98,0.95);

  for(int j=0;j<4;j++){
    TString name = "hnMCtrk_25ns_pl";
    name +=j;
    TString plname = "plane #";
    plname +=j;
    TString title = "; trk/25ns";
    hMCtrk[j] = new TH1D(name,title,1e2,0,1e2);
    hMCtrk[j]->SetLineWidth(2);
    hMCtrk[j]->SetMaximum(100);
    hMCtrk[j]->SetLineColor(1+j);
    //   hMCtrk[j]->SetBit(TH1::kIsAverage);
    leg->AddEntry(hMCtrk[j],plname,"l");
  }
  for(int i=0;i<nstep;i++){
    name[i] = "hmlt";
    name[i] +=i;
    //    hmlt[i] = new TH2D(name[i],"; time, ns; number of tracks fired the same pixel",40,0,1000,25,0,25);
    hmlt[i] = new TH2D(name[i],"; time, ns; MC trk ID",40,0,1000,1e3,0,1e3);
    hmlt[i]->SetBit(TH2::kIsAverage);
    //    TString var = "LMDPixelDigisQ.GetNIndices():(LMDPixelDigisQ.fTimeStamp+LMDPixelDigisQ.fEvTime)-";
    TString var = "LMDPixelDigisQ.fMCtrkID:(LMDPixelDigisQ.fTimeStamp+LMDPixelDigisQ.fEvTime)-";
    //    var += i*1e3;
    var += i*25;
    TString condGl = "(LMDPixelDigisQ.fTimeStamp+LMDPixelDigisQ.fEvTime)<";
    condGl += (i+1)*25;
    condGl += " && (LMDPixelDigisQ.fTimeStamp+LMDPixelDigisQ.fEvTime)>";
    condGl += i*25;
    double Ngl = trec->GetEntries(condGl);
    cout<<i<<": Ngl "<<Ngl<<endl;
    if(Ngl>0){
      for(int j=0;j<4;j++){// loop over planes
	TString cond = condGl;
	cond += " && LMDPixelDigisQ.fSide==0 && LMDPixelDigisQ.fPl==";
	cond +=j;
	double nEv =trec->GetEntries(cond);
	if (nEv>0) hMCtrk[j]->Fill(nEv);
	cout<<j<<": nEv "<<nEv<<endl;
      }
 }
    // TString cond = "LMDPixelDigisQ.fPl==0 && LMDPixelDigisQ.fSide==0 && (LMDPixelDigisQ.fTimeStamp+LMDPixelDigisQ.fEvTime)<";
    // //  TString cond = "LMDPixelDigisQ.fSensorID==1 && (LMDPixelDigisQ.fTimeStamp+LMDPixelDigisQ.fEvTime)<";
    // //    cond += (i+1)*1e3;
    // cond += (i+1)*25;
    // cond += " && (LMDPixelDigisQ.fTimeStamp+LMDPixelDigisQ.fEvTime)>";
    // //    cond += i*1e3;
    // cond += i*25;
    // //    double nEv =trec->Project(name[i],var,cond);
    // double nEv =trec->GetEntries(cond);
    // if (nEv>0) hMCtrk->Fill(nEv);
    // cout<<i<<": nEv "<<nEv<<endl;
  }
  // for(int i=1;i<nstep;i++){
  //   hmlt[0]->Add(hmlt[i]);
  // }
  
  // 	hmlt[0]->SetZTitle("Number of fired pixels");
  // 	// double scl = 100/hmlt->Integral();
  // 	//  hmlt[0]->Scale(scl);
  // 	// hmlt->GetZaxis()->SetRangeUser(1e-6,100);
  // 	hmlt[0]->SetContour(1e3);
  // 	// cout<<"Scale: "<<scl<<endl;

  
	TCanvas c1;
	c1.Divide(1,2);
	c1.cd(1);
	c1.SetLogy();
	//	c1.SetLogx();
	//	hmlt[0]->Draw("colz");
	cout<<"hMCtrk[0]->Integral() = "<<hMCtrk[0]->Integral()<<endl;
	double scl0 = 100.*1/(hMCtrk[0]->Integral());
	cout<<"scl0 = "<<scl0<<endl;
	hMCtrk[0]->Scale(scl0);
	hMCtrk[0]->GetYaxis()->SetTitle(" events, %");
	hMCtrk[0]->SetLineColor(1);
	hMCtrk[0]->SetLineWidth(2);
	hMCtrk[0]->Draw();
	for(int j=1;j<4;j++){
	  double scl1 = 100.*1/(hMCtrk[j]->Integral());
	  hMCtrk[j]->Scale(scl1);
	  hMCtrk[j]->Draw("same");
	}
	leg->Draw();
	c1.cd(2);
	c1.SetLogy();
	//	c1.SetLogx();
	TH1D*hsum[4];
	//  TH1D*hsum = (TH1D*)hMCtrk[0]->Clone("hsum");
	for(int j=0;j<4;j++){
	  hsum[j] = (TH1D*)hMCtrk[j]->Clone("hsum");
	  //	  hsum[j]->SetBit(TH1::kIsAverage);
	  if(j>0) hsum[0]->Add(hsum[j]);
	}
	double scl2 = 100.*1/(hsum[0]->Integral());
	//	  cout<<"scl2 = "<<scl2<<endl;
	hsum[0]->Scale(scl2);
	hsum[0]->Draw();
	c1.Print(foutname);
	//	hml->Scale(100/hml->Integral());
	//	hml->Draw();
	return 0;
}
