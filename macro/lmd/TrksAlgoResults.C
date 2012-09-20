///////////////////////////////////////////////
///                                         /// 
/// Script for comparision results          ///
/// between CA and Trk-Follow algorithms    ///
/// author: Anastasia Karavdina             ///
/// date: 20/09/2012                        ///
///                                         /// 
///////////////////////////////////////////////
#include "TString.h"
#include "TStopwatch.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
using namespace std;
void TrksAlgoResults(TString storePath="/myResults/LUMI_Trk/newCA_oldDesign/100000events")
{
  double Ntrks[8]={1,2,3,4,5,10,15,20};
  //Track-Follow
  double reconstF[8];//number of reconstructed tracks
  double ghostF[8]; //percent of ghost tracks
  double missedF[8]; //percent of missed tracks
  double timeF[8]={4.87,5.51,6.22,6.93,7.73,12.83,21.58,36.61};//time spent per 1 trk reconstruction , ms

  //Cellular Automaton
  double reconstCA[8];//number of reconstructed tracks
  double ghostCA[8]; //percent of ghost tracks
  double missedCA[8]; //percent of missed tracks
  double timeCA[8]={4.96,5.64,6.52,7.48,8.72,18.2,37.44,74.65};//time spent per 1 trk reconstruction , ms
 
  for(int iNtrk=0;iNtrk<8;iNtrk++){
    TString fileCAname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    fileCAname+="_CA";
    TString fileFname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    fileFname+="_Follow";
    fileCAname+="_new_100000_";
    fileFname+="_new_100000_";
    fileCAname+=Ntrks[iNtrk];
    fileFname+=Ntrks[iNtrk];
    fileCAname+="trk_11_91GeV.root";
    fileFname+="trk_11_91GeV.root";

    TFile *fileCA = new TFile(fileCAname,"READ");
    TFile *fileF = new TFile(fileFname,"READ");
    TH1F* hResThetaCA = (TH1F*)fileCA->Get("hResTheta");
    int nrecCA = hResThetaCA->GetEntries();
    reconstCA[iNtrk]=nrecCA;
    TH1F* hResThetaF = (TH1F*)fileF->Get("hResTheta");
    int nrecF = hResThetaF->GetEntries();
    reconstF[iNtrk]=nrecF;

    TH1F* hntrkmissedCA = (TH1F*)fileCA->Get("hntrkmissed");
    int nmissCA = hntrkmissedCA->GetEntries();
    int nmissCA_under = hntrkmissedCA->GetBinContent(0);
    nmissCA-=nmissCA_under;
  
    TH1F* hntrkmissedF = (TH1F*)fileF->Get("hntrkmissed");
    int nmissF = hntrkmissedF->GetEntries();
    int nmissF_under = hntrkmissedF->GetBinContent(0);
    nmissF-=nmissF_under;
  

    TH2F* hnRecnMCCA = (TH2F*)fileCA->Get("hnRecnMC");
    TH1D* hnRecnMCCA_rec = hnRecnMCCA->ProjectionY();
    int nsimTrk = Ntrks[iNtrk]*(hnRecnMCCA_rec->GetEntries()); 
    int nbinghost = hnRecnMCCA_rec->GetNbinsX();//should be equal for CA&F
    int nghostCA=0;
    for(int ibg=Ntrks[iNtrk]+2;ibg<nbinghost;ibg++)
      nghostCA += (ibg-Ntrks[iNtrk])*hnRecnMCCA_rec->GetBinContent(ibg);
        
    int nghostF=0;
    TH2F* hnRecnMCF = (TH2F*)fileF->Get("hnRecnMC");
    TH1D* hnRecnMCF_rec = hnRecnMCF->ProjectionY();
    for(int ibg=Ntrks[iNtrk]+2;ibg<nbinghost;ibg++)
      nghostF += (ibg-Ntrks[iNtrk])*hnRecnMCF_rec->GetBinContent(ibg);

    ghostCA[iNtrk]=100*double(nghostCA)/nsimTrk;
    missedCA[iNtrk]=100*double(nmissCA)/nsimTrk;
    ghostF[iNtrk]=100*double(nghostF)/nsimTrk;
    missedF[iNtrk]=100*double(nmissF)/nsimTrk;
    timeCA[iNtrk]=1000*double(timeCA[iNtrk])/nrecCA;
    timeF[iNtrk]=1000*double(timeF[iNtrk])/nrecF;
    cout<<"nsimTrk = "<<nsimTrk<<" nrecF = "<<nrecF<<" nrecCA = "<<nrecCA<<endl;
    cout<<" nmissF = "<<nmissF<<" nmissCA = "<<nmissCA<<endl;
    cout<<" nghostF = "<<nghostF<<" nghostCA = "<<nghostCA<<endl;
    cout<<" timeF = "<<timeF[iNtrk]<<" timeCA = "<<timeCA[iNtrk]<<endl;
    cout<<"***************************************"<<endl;
    cout<<" "<<endl;


  }
  TGraph *grSpeedCA = new TGraph(8,Ntrks,timeCA);
  grSpeedCA->SetMarkerStyle(20);
  grSpeedCA->SetMarkerColor(2);
  grSpeedCA->SetMarkerSize(1.5);
  TGraph *grSpeedF = new TGraph(8,Ntrks,timeF);
  grSpeedF->SetMarkerStyle(21);
  grSpeedF->SetMarkerColor(4);
  grSpeedF->SetMarkerSize(1.5);
  TMultiGraph *mgSpeed = new TMultiGraph("mgSpeed","Speed;N^{trk}_{MC};time per trk, ms");
  mgSpeed->Add(grSpeedCA);
  mgSpeed->Add(grSpeedF);
 

  TGraph *grRecCA = new TGraph(8,Ntrks,reconstCA);
  grRecCA->SetMarkerStyle(20);
  grRecCA->SetMarkerColor(2);
  grRecCA->SetMarkerSize(1.5);
  TGraph *grRecF = new TGraph(8,Ntrks,reconstF);
  grRecF->SetMarkerStyle(21);
  grRecF->SetMarkerColor(4);
  grRecF->SetMarkerSize(1.5);
  TMultiGraph *mgTotRec = new TMultiGraph("mgRecTrks","RecTrks;N^{trk}_{MC};N^{trk}_{Rec}");
  mgTotRec->Add(grRecCA);
  mgTotRec->Add(grRecF);

  TGraph *grMissCA = new TGraph(8,Ntrks,missedCA);
  grMissCA->SetMarkerStyle(20);
  grMissCA->SetMarkerColor(2);
  grMissCA->SetMarkerSize(1.5);
  TGraph *grMissF = new TGraph(8,Ntrks,missedF);
  grMissF->SetMarkerStyle(21);
  grMissF->SetMarkerColor(4);
  grMissF->SetMarkerSize(1.5);
  TMultiGraph *mgMissed = new TMultiGraph("mgMissed","Missed;N^{trk}_{MC};missed trks, %");
  mgMissed->Add(grMissCA);
  mgMissed->Add(grMissF);

  TGraph *grGhostCA = new TGraph(8,Ntrks,ghostCA);
  grGhostCA->SetMarkerStyle(20);
  grGhostCA->SetMarkerColor(2);
  grGhostCA->SetMarkerSize(1.5);
  TGraph *grGhostF = new TGraph(8,Ntrks,ghostF);
  grGhostF->SetMarkerStyle(21);
  grGhostF->SetMarkerColor(4);
  grGhostF->SetMarkerSize(1.5);
  TMultiGraph *mgGhost = new TMultiGraph("mgGhost","Ghost;N^{trk}_{MC};ghost trks, %");
  mgGhost->Add(grGhostCA);
  mgGhost->Add(grGhostF);
  
  TString fileOUTname=storePath+"/TrksAlgoResults.root";
  TString fileOUTnamepdf=storePath+"/TrksAlgoResults.pdf";
  TFile *fileOUT = new TFile(fileOUTname,"RECREATE");
  TCanvas *c1 = new TCanvas("Overview");
  c1->Divide(2,2);
  c1->cd(1);
  mgSpeed->Draw("APL");
  c1->cd(2);
  mgTotRec->Draw("APL");
  c1->cd(3);
  mgMissed->Draw("APL");
  c1->cd(4);
  mgGhost->Draw("APL");
  c1->Write();
  mgSpeed->Write();
  mgTotRec->Write();
  mgMissed->Write();
  mgGhost->Write();
  fileOUT->Close();
  c1->SaveAs(fileOUTnamepdf);
}
