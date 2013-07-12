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

//void TrksAlgoResults(TString storePath="StripSensors/1e4_events/noHitsNumberCheck")
//void TrksAlgoResults(TString storePath="/home/karavdin/datastorage/TrackPerformanceStrip/1000000/")
void TrksCApsiValueSearch(TString storePath="${VMCWORKDIR}/macro/lmd/tmpOutnewDesign/noFilterInCA_4aTaskForAll/mom_15/")
{

  gROOT->Macro("/panda/pandaroot/macro/lmd/Anastasia/test_macros/Style_Imported_Style.C");
  gROOT->SetStyle("Imported_Style"); 
  const int numTrks = 3;
  double Ntrks[numTrks]={1,5,10};
  double eNtrks[numTrks]={0,0,0};

  const int npsiVal = 12;
  double psiVal[npsiVal] = {0.0000001,0.000001,0.000002,0.000004,0.000006,0.000008,0.00001,0.00002,0.00004,0.00006,0.00008,0.0001};
  TString psiValName[npsiVal] = {"0_0000001","0_000001","0_000002","0_000004","0_000006","0_000008","0_00001","0_00002","0_00004","0_00006",
				 "0_00008","0_0001"};
 
  //Cellular Automaton
  double ghostCA_II[numTrks][npsiVal]; //percent of ghost tracks
  double missedCA_II[numTrks][npsiVal]; //percent of missed tracks
  TGraph *grGhostCA_II[numTrks];
  TGraph *grMissCA_II[numTrks]; 
  TMultiGraph *mgMess = new TMultiGraph("mgMess"," ;#delta#psi;events with missed/ghost trks, %");
  for(int iNtrk=0;iNtrk<numTrks;iNtrk++){
    for(int ipsiVal=0;ipsiVal<npsiVal;ipsiVal++){ 

    ///PIXEL -----------------------------------------------------------------
      //${pathG}/Lumi_out_MC_and_REC_trks_matches_with_IDs0_CA_${numTrks}trks_psi$CAparamStr}.root
    TString fileCAname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    fileCAname+="_CA_";
    fileCAname+=Ntrks[iNtrk];
    fileCAname+="trks_psi"+psiValName[ipsiVal]+".root";
    ///PIXEL -----------------------------------------------------------------

    TFile *fileCA = new TFile(fileCAname,"READ");
    if (fileCA->IsZombie()) {
      std::cout << "Error opening file: "<<fileCAname<< std::endl;
      ghostCA_II[iNtrk]=0; //percent of ghost tracks
      missedCA_II[iNtrk]=0; //percent of missed tracks
      continue;
    }
    
  

    //missed\ghost from definition II: based on hits info
    TH1F* hntrkmissedCA_II = (TH1F*)fileCA->Get("hntrkmissed_II");
    int nmissCA_II = hntrkmissedCA_II->GetEntries();
    
    TH1F* hntrkghostCA_II = (TH1F*)fileCA->Get("hntrkghost_II");
    int nghostCA_II = hntrkghostCA_II->GetEntries();
    
    TH2F* hnRecnMCCA = (TH2F*)fileCA->Get("hnRecnMC");
    TH1D* hnRecnMCCA_rec = hnRecnMCCA->ProjectionY();
    int nsimTrk = (hnRecnMCCA_rec->GetEntries()); 
   
    ghostCA_II[iNtrk][ipsiVal]=100*double(nghostCA_II)/(nsimTrk);
    missedCA_II[iNtrk][ipsiVal]=100*double(nmissCA_II)/(nsimTrk);
    
    cout<<"nsimTrk = "<<nsimTrk<<endl;
    cout<<"(only trk lost    ): nmissCA_II = "<<nmissCA_II<<endl;
    cout<<" nghostCA_II = "<<nghostCA_II<<endl;
    cout<<"***************************************"<<endl;
    cout<<" "<<endl;
    }
    grMissCA_II[iNtrk] = new TGraph(npsiVal,psiVal,missedCA_II[iNtrk]);
    grMissCA_II[iNtrk]->SetMarkerSize(3.);
    grMissCA_II[iNtrk]->SetMarkerStyle(21);
    grMissCA_II[iNtrk]->SetMarkerColor(iNtrk+1);
    grGhostCA_II[iNtrk] = new TGraph(npsiVal,psiVal,ghostCA_II[iNtrk]);
    grGhostCA_II[iNtrk]->SetMarkerSize(3.);
    grGhostCA_II[iNtrk]->SetMarkerStyle(20);
    grGhostCA_II[iNtrk]->SetMarkerColor(iNtrk+1);
    mgMess->Add(grMissCA_II[iNtrk]);
    mgMess->Add(grGhostCA_II[iNtrk]);
  }


  TLegend *leg = new TLegend(0.55,0.57,0.95,0.95);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.05);
 for(int iNtrk=0;iNtrk<numTrks;iNtrk++){
   TString signMissed = "missed, ";
   signMissed +=Ntrks[iNtrk];
   signMissed +=" trk(s)/ev";
   leg->AddEntry(grMissCA_II[iNtrk],signMissed,"lep");
   TString signGhost = "ghost, ";
   signGhost +=Ntrks[iNtrk];
   signGhost +=" trk(s)/ev";
   leg->AddEntry(grGhostCA_II[iNtrk],signGhost,"lep");
 }

  TString fileOUTname=storePath+"/CAruleParamCheck.root";
  TString fileOUTnamepdf=storePath+"/CAruleParamCheck.pdf";
  TFile *fileOUT = new TFile(fileOUTname,"RECREATE");
  TCanvas *c1 = new TCanvas("LaLa");
  mgMess->Draw("ALP");
  leg->Draw();
  c1->Write();
  for(int iNtrk=0;iNtrk<numTrks;iNtrk++){
    grMissCA_II[iNtrk]->Write();
    grGhostCA_II[iNtrk]->Write();
  }
  mgMess->Write();
  leg->Write();
  fileOUT->Close();
  c1->SaveAs(fileOUTnamepdf);
}
