#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TH2.h"
using namespace std;
void AnaCellCuts(){
  gStyle->SetOptStat("e");
  TFile * fin = new TFile("Lumi_Track_0_15GeV_Large_2_11mrad_secondariesFullLMD.root","READ");
  TNtuple *ntgthetatgphiFit = (TNtuple*)fin->Get("htthetatphiTrkFit");
  Long64_t  nAll = ntgthetatgphiFit->GetEntries();//trk
  double thetaMax=100;//mrad
  double phiMax=500;//mrad
  const int nStep=10;
  TH2D *heffth = new TH2D("heffth","trks;#Delta#hat{#theta}, mrad;#Delta#hat{#phi}, mrad",nStep,0,thetaMax,nStep,0,phiMax);
  const double sizeTh=thetaMax/(1e3*nStep);
  const double sizePhi=phiMax/(1e3*nStep);
  for(int itheta=0;itheta<nStep;itheta++){
    for(int iphi=0;iphi<nStep;iphi++){
      cout<<itheta<<","<<iphi<<endl;
      double thL=itheta*sizeTh;
      double phiL=iphi*sizePhi;
      TString cond = "abs(tg_theta-0.0400)<";//trk
      cond+=thL;
      cond+=" && abs(tg_phi)<";
      cond+=phiL;
      Long64_t  nLeft = ntgthetatgphiFit ->GetEntries(cond.Data());
      //  cout<<" nLeft =  "<<nLeft<<" th="<<thL<<" phi="<<phiL<<endl;
      double effTh = double(nLeft)/double(nAll);
      heffth->Fill(1e3*thL,1e3*phiL,effTh);
    }
  }
  Long64_t nLeftTFitCut = ntgthetatgphiFit ->GetEntries("abs(tg_theta-0.0400)<=11 && abs(tg_phi)<=0.250");
  TCanvas c1;
  heffth->Draw("colz");
  c1.SaveAs("RESfit_CA_trkLim_15GeV_2_11mradNEW_secondariesFullLMD.pdf");
  c1.SaveAs("RESfit_CA_trkLim_15GeV_2_11mradNEW_secondariesFullLMD.root");
  fin->Close();

  TFile * fin2 = new TFile("Lumi_TCand_0_15GeV_Large_2_11mrad_secondariesFullLMD.root","READ");
  TNtuple *ntgthetatgphi2 = (TNtuple*)fin2->Get("htthetatphiTrk");
  TNtuple *ntgthetatgphiC = (TNtuple*)fin2->Get("htthetatphiCells");
  //  Long64_t  nAll2 = ntgthetatgphi2->GetEntries("nHits>3");//trk-cand
  Long64_t  nAll2 = ntgthetatgphi2->GetEntries();//trk-cand
 
  TH2D *heffth2 = new TH2D("heffth","trks;#delta#theta, mrad;#delta#phi, mrad",nStep,0,thetaMax,nStep,0,phiMax);
  for(int itheta=0;itheta<nStep;itheta++){
    for(int iphi=0;iphi<nStep;iphi++){
      cout<<itheta<<","<<iphi<<endl;
      double thL=itheta*sizeTh;
      double phiL=iphi*sizePhi;
      //   TString cond = "nHits>3 && abs(tg_theta-0.0400)<"; //trk-cand
      TString cond = "abs(tg_theta-0.0400)<"; //trk-cand
      cond+=thL;
      cond+=" && abs(tg_phi)<";
      cond+=phiL;
      Long64_t  nLeft = ntgthetatgphi2 ->GetEntries(cond.Data());
      //  cout<<" nLeft =  "<<nLeft<<" th="<<thL<<" phi="<<phiL<<endl;
      double effTh = double(nLeft)/double(nAll2);
      heffth2->Fill(1e3*thL,1e3*phiL,effTh);
    }
  }
 Long64_t nLeftTCandCut = ntgthetatgphi2 ->GetEntries("abs(tg_theta-0.0400)<=11 && abs(tg_phi)<=0.250");
  TCanvas c3;
  heffth2->Draw("colz");
  c3.SaveAs("REStcand_CA_trkLim_15GeV_2_11mradNEW_secondariesFullLMD.pdf");
  c3.SaveAs("REStcand_CA_trkLim_15GeV_2_11mradNEW_secondariesFullLMD.root");

  Long64_t  nAllC = ntgthetatgphiC->GetEntries();
  TH2D *heffthC = new TH2D("heffthC","cells;#delta#theta, mrad;#delta#phi, mrad",nStep,0,thetaMax,nStep,0,phiMax);
  for(int itheta=0;itheta<nStep;itheta++){
    for(int iphi=0;iphi<nStep;iphi++){
      cout<<itheta<<","<<iphi<<endl;
      double thL=itheta*sizeTh;
      double phiL=iphi*sizePhi;
      TString cond = "abs(tg_theta-0.0400)<";
      cond+=thL;
      cond+=" && abs(tg_phi)<";
      cond+=phiL;
      Long64_t  nLeft = ntgthetatgphiC->GetEntries(cond.Data());
      //  cout<<" nLeft =  "<<nLeft<<" th="<<thL<<" phi="<<phiL<<endl;
      double effTh = double(nLeft)/double(nAllC);
      heffthC->Fill(1e3*thL,1e3*phiL,effTh);
    }
  }
  Long64_t nLeftTCandCellsCut = ntgthetatgphiC ->GetEntries("abs(tg_theta-0.0400)<=11 && abs(tg_phi)<=0.250");
  TCanvas c2;
  heffthC->Draw("colz");
  c2.SaveAs("REStcand_CA_cellLim_15GeV_2_11mradNEW_secondariesFullLMD.pdf");
  c2.SaveAs("REStcand_CA_cellLim_15GeV_2_11mradNEW_secondariesFullLMD.root");
  cout<<" --------- Summary ----------- "<<endl;
  cout<<"There are "<<nAll<<" fitted trks from "<<nAll2<<" trk-cands form "<<nAllC<<" cells"<<endl;
  cout<<"Cut 11 mrad & 250 mrad has left:"<<endl;
  double effTfit = 100.*(double(nLeftTFitCut)/double(nAll));
  double erreffTfit = 100.*sqrt(double(nLeftTFitCut)*(1-nLeftTFitCut/double(nAll)))/nAll;
  cout<<"After fit "<<effTfit<<" +\- "<<erreffTfit<<endl;
  double effTCand = 100.*(double(nLeftTCandCut)/double(nAll2));
  double erreffTCand = 100.*sqrt(double(nLeftTCandCut)*(1-nLeftTCandCut/double(nAll2)))/nAll2;
  cout<<"After TrkSearch "<<effTCand<<" +\- "<<erreffTCand<<endl;
  double effTCandCell = 100.*(double(nLeftTCandCellsCut)/double(nAllC));
  double erreffTCandCell = 100.*sqrt(double(nLeftTCandCellsCut)*(1-nLeftTCandCellsCut/double(nAllC)))/nAllC;
  cout<<"After TrkSearch Cells "<<effTCandCell<<" +\- "<<erreffTCandCell<<endl;
}
