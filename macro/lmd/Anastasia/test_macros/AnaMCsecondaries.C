// Script for secondaries counting
// author: A.Karavdina
// to compile: g++ $(root-config --cflags --glibs) AnaMCsecondaries.C -o /tmp/anaMCsecondaries
//to run:  /tmp/anaMCsecondaries
#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TH2.h"
#include "TH1.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TCanvas.h"
#include<iostream>
#include<sstream>
#include<iomanip>
#include<cmath>
//using namespace std;
int main(int nargs, char** args) {
  //void AnaMCsecondaries(){
  //  gStyle->SetOptStat("i");
  TStyle *tmpStyle = new TStyle("miniPlots", "Style for miniplots");
  tmpStyle->SetOptStat("e");
  tmpStyle->SetLabelSize(0.05, "x");
  tmpStyle->SetLabelSize(0.05, "y");
  tmpStyle->SetTitleSize(0.05, "x");
  tmpStyle->SetTitleSize(0.05, "y");
  //gROOT->SetStyle("miniPlots");
  TFile * fin = new TFile("/panda/pandaroot/macro/lmd/tmpOutnewDesign/secondaries/fullLMD/theta_4_8mrad/mom_1_5//1trks/Lumi_MC_0.root","READ");
  TTree *MCtree = (TTree*)fin->Get("cbmsim");
  // TH2D *hmcsecondyz = new TH2D("hmcsecondyz","",1.2e3,0,1200,100,-100,100);
  // MCtree->Project("hmcsecondyz","MCTrack.fStartY:MCTrack.fStartZ","MCTrack.fMotherID>0");
  // hmcsecondyz->Draw("colz");
  Long64_t  nPrimary = MCtree->GetEntries("MCTrack.fMotherID<0");
  Long64_t  nSecondary = MCtree->GetEntries("MCTrack.fMotherID>=0");
  Long64_t  nSecondarySm = MCtree->GetEntries("MCTrack.fMotherID>=0 && MCTrack.fStartZ>1000.");
  const double zstep=10;
  const int nSteps = 1200/zstep;
  const double zstep2=2;
  const int nSteps2 = 200/zstep2;

  TH1D *hzseconlv = new TH1D("hzseconlv",";z,cm;secondaries/primaries, %",nSteps,0,1200);
  TH1D *hzseconlv2 = new TH1D("hzseconlv2",";z,cm;secondaries/primaries, %",nSteps2,1000,1200);
  for(int iz=0;iz<nSteps;iz++){
    double zcur1 = zstep*iz;
    double zcur2 = zstep*(iz+1);
    double zcur = zcur1+0.5*zstep;
    TString cond = "MCTrack.fMotherID>=0 && (MCTrack.fStartZ>";
    cond+=zcur1;
    cond+=" && MCTrack.fStartZ<";
    cond+=zcur2;
    cond+=")";
    Long64_t  nSecondCur = MCtree->GetEntries(cond.Data());
    double secondaries_perslv  = 100*double(nSecondCur)/double(nPrimary);
    //  std::cout<<"z = "<<zcur<<" nSecondCur = "<<nSecondCur<<"  secondaries_perslv  = "<< secondaries_perslv<<std::endl;
    hzseconlv->Fill(zcur,secondaries_perslv);
  }
  for(int jz=0;jz<nSteps2;jz++){
    double zcur1 = 1000+zstep2*jz;
    double zcur2 = 1000+zstep2*(jz+1);
    double zcur = zcur1+0.5*zstep2;
    TString cond = "MCTrack.fMotherID>=0 && (MCTrack.fStartZ>";
    cond+=zcur1;
    cond+=" && MCTrack.fStartZ<";
    cond+=zcur2;
    cond+=")";
    Long64_t  nSecondCur = MCtree->GetEntries(cond.Data());
    double secondaries_perslv  = 100*double(nSecondCur)/double(nPrimary);
    hzseconlv2->Fill(zcur,secondaries_perslv);
  }
  // double errIall, errIsm;
  // double Iall, Ism;
  // Iall = hzseconlv->IntegralAndError(0.2*nSteps,nSteps,errIall);
  // Ism = hzseconlv2->IntegralAndError(0,nSteps,errIsm);

  double TotSec_Pri = 100*double(nSecondary)/double(nPrimary);
  double errTotSec_Pri = 100*sqrt(nSecondary*(1-double(nSecondary)/nPrimary))/double(nPrimary);
  double TotSec_PriSm = 100*double(nSecondarySm)/double(nPrimary);
  double errTotSec_PriSm = 100*sqrt(nSecondarySm*(1-double(nSecondarySm)/nPrimary))/double(nPrimary);

  std::ostringstream ossAll; // output to string
  ossAll << std::setprecision(4) << TotSec_Pri;

  std::ostringstream osserAll; // output to string
  osserAll << std::setprecision(2) << errTotSec_Pri;

  TString IntgAll = "Integral = ";
  IntgAll += ossAll.str();
  IntgAll += " #pm ";
  IntgAll += osserAll.str();
  //  std::cout<<"small Int = "<<ossAll.str()<<" +\- "<<osserAll.str()<<" "<<Iall<<std::endl;
  TLatex *InegTXT = new TLatex();
  TCanvas c1;
  hzseconlv->Draw();
  c1.SetLogy(1);
  InegTXT->DrawLatex(200,1.4,IntgAll.Data());
  c1.SaveAs("/panda/pandaroot/macro/lmd/secondaries/beampipe_201303/BOX/secondaries_to_primaries_persent_theta_4_8_1_5GeV.root");
  c1.SaveAs("/panda/pandaroot/macro/lmd/secondaries/beampipe_201303/BOX/secondaries_to_primaries_persent_theta_4_8_1_5GeV.pdf");

  std::ostringstream ossSm; // output to string
 ossSm << std::setprecision(4) << TotSec_PriSm;

  std::ostringstream osserSm; // output to string
  osserSm << std::setprecision(2) << errTotSec_PriSm;
  //  std::cout<<"small Int = "<<ossSm.str()<<" +\- "<<osserSm.str()<<" "<<Ism<<std::endl;
  TString IntgSm = "Integral = ";
  IntgSm += ossSm.str();
  IntgSm += " #pm ";
  IntgSm += osserSm.str();

  TCanvas c2;
  hzseconlv2->Draw();
  c2.SetLogy(1);
  InegTXT->DrawLatex(1000,1.1,IntgSm.Data());
  c2.SaveAs("/panda/pandaroot/macro/lmd/secondaries/beampipe_201303/BOX/secondaries_to_primaries_persent_theta_4_8_1_5GeV_after10m.root");
  c2.SaveAs("/panda/pandaroot/macro/lmd/secondaries/beampipe_201303/BOX/secondaries_to_primaries_persent_theta_4_8_1_5GeV_after10m.pdf");
 

  //  double errTotSec_Pri = 100*sqrt(pow(nSecondary,2)*(double(nSecondary)+double(nPrimary))/pow(nPrimary,3));
  std::cout<<"Tot.primary = "<<nPrimary<<" Tot.secondary = "<<nSecondary<<std::endl;
  std::cout<<" z=0-12 m: secondaries "<<100*double(nSecondary)/double(nPrimary)<<" +\- "<<errTotSec_Pri<<" %"<<std::endl;
  std::cout<<" z=10-12 m: secondaries "<<TotSec_PriSm<<" +\- "<<errTotSec_PriSm<<" %"<<std::endl;

 }
