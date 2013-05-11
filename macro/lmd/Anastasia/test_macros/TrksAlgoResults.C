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
void TrksAlgoResults(TString storePath="${VMCWORKDIR}/macro/lmd/tmpOutnewDesign/")
{

  gROOT->Macro("/panda/pandaroot/macro/lmd/Anastasia/test_macros/Style_Imported_Style.C");
  gROOT->SetStyle("Imported_Style"); 
  const int numTrks = 9;
  // double Ntrks[8]={1,2,3,4,5,10,15,20};
  double Ntrks[numTrks]={1,2,3,4,5,7,10,15,20};
  double eNtrks[numTrks]={0,0,0,0,0,0,0};
  //Track-Follow
  double reconstF[numTrks];//number of reconstructed tracks
  double ghostF_I[numTrks]; //percent of ghost tracks
  double missedF_I[numTrks]; //percent of missed tracks
  double ghostF_II[numTrks]; //percent of ghost tracks
  double missedF_II[numTrks]; //percent of missed tracks

  double ghostF_I_mean[numTrks]; //average of ghost tracks
  double missedF_I_mean[numTrks]; //average of missed tracks
  double ghostF_II_mean[numTrks]; //average of ghost tracks
  double missedF_II_mean[numTrks]; //average of missed tracks
  double ghostF_I_rms[numTrks]; //rms average of ghost tracks
  double missedF_I_rms[numTrks]; //rms average of missed tracks
  double ghostF_II_rms[numTrks]; //rms average of ghost tracks
  double missedF_II_rms[numTrks]; //rms average of missed tracks
  double goodF_I_mean[numTrks]; //average of good tracks
  double goodF_II_mean[numTrks]; //average of good tracks


  //time spent per 1 trk reconstruction , ms
  //double timeF[numTrks]={7.73,8.97,10.41,11.71,13.95,24.46,39.42};// strip, 15 GeV
  //double timeF[numTrks]={7.75,9.14,10.76,12.42,14.05,24.02,43.3};// strip, 1.5 GeV
  double timeF[numTrks]={7.5,9.11,10.84,12.16,13.94,22.05,30.3};// pixel, 15 GeV
  //Cellular Automaton
  double reconstCA[numTrks];//number of reconstructed tracks
  double ghostCA_I[numTrks]; //percent of ghost tracks
  double missedCA_I[numTrks]; //percent of missed tracks
  double ghostCA_II[numTrks]; //percent of ghost tracks
  double missedCA_II[numTrks]; //percent of missed tracks

  double ghostCA_I_mean[numTrks]; //average of ghost tracks
  double missedCA_I_mean[numTrks]; //average of missed tracks
  double ghostCA_II_mean[numTrks]; //average of ghost tracks
  double missedCA_II_mean[numTrks]; //average of missed tracks
  double ghostCA_I_rms[numTrks]; //rms average of ghost tracks
  double missedCA_I_rms[numTrks]; //rms average of missed tracks
  double ghostCA_II_rms[numTrks]; //rms average of ghost tracks
  double missedCA_II_rms[numTrks]; //rms average of missed tracks
  double goodCA_I_mean[numTrks]; //average of good tracks
  double goodCA_II_mean[numTrks]; //average of good tracks


  //time spent per 1 trk reconstruction , ms
  // double timeCA[numTrks]={7.98,9.43,11.39,13.77,17.76,72.49,371.08};//strip, 15 GeV/c
  //  double timeCA[numTrks]={7.99,9.76,11.76,14.08,18.44,77.7,420.63};//strip, 1.5 GeV/c
    double timeCA[numTrks]={7.86,9.59,11.53,13.58,15.91,32.69,62.59};//pixel, 15 GeV/c
  for(int iNtrk=0;iNtrk<numTrks;iNtrk++){
    // // ///STRIP -----------------------------------------------------------------
    // TString fileCAname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    // fileCAname+="_CA_";
    // TString fileFname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    // fileFname+="_Follow_";
    // fileCAname+=Ntrks[iNtrk];
    // fileFname+=Ntrks[iNtrk];
    // fileCAname+="trks_";
    // fileFname+="trks_";
    // // fileCAname+="KalmanFillter_";
    // // fileFname+="KalmanFillter_";
    // fileCAname+="MinuitFit_";
    // fileFname+="MinuitFit_";
    // fileCAname+="15.root";
    // fileFname+="15.root";
    // // ///STRIP ----------------------------------------------------------------- 

    ///PIXEL -----------------------------------------------------------------
    TString fileCAname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    fileCAname+="_CA_";
    TString fileFname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    fileFname+="_Follow_";
    fileCAname+=Ntrks[iNtrk];
    fileFname+=Ntrks[iNtrk];
    fileCAname+="trks_";
    fileFname+="trks_";
    fileCAname+="mergedHits_true_";
    fileFname+="mergedHits_true_";
    // fileCAname+="addMS_false_KalmanFillter_";
    // fileFname+="addMS_false_KalmanFillter_";
    fileCAname+="addMS_true_MinuitFit_";
    fileFname+="addMS_true_MinuitFit_";
    fileCAname+="15.root";
    fileFname+="15.root";
    ///PIXEL -----------------------------------------------------------------

    TFile *fileCA = new TFile(fileCAname,"READ");
    if (fileCA->IsZombie()) {
      std::cout << "Error opening file: "<<fileCAname<< std::endl;
      reconstCA[iNtrk]=0;//number of reconstructed tracks
      ghostCA_I[iNtrk]=0; //percent of ghost tracks
      missedCA_I[iNtrk]=0; //percent of missed tracks
      ghostCA_II[iNtrk]=0; //percent of ghost tracks
      missedCA_II[iNtrk]=0; //percent of missed tracks
      
      ghostCA_I_mean[iNtrk]=0; //average of ghost tracks
      missedCA_I_mean[iNtrk]=0; //average of missed tracks
      ghostCA_II_mean[iNtrk]=0; //average of ghost tracks
      missedCA_II_mean[iNtrk]=0; //average of missed tracks
      ghostCA_I_rms[iNtrk]=0; //rms average of ghost tracks
      missedCA_I_rms[iNtrk]=0; //rms average of missed tracks
      ghostCA_II_rms[iNtrk]=0; //rms average of ghost tracks
      missedCA_II_rms[iNtrk]=0; //rms average of missed tracks
      goodCA_I_mean[iNtrk]=0; //average of good tracks
      goodCA_II_mean[iNtrk]=0; //average of good tracks
      continue;
    }
    
    TFile *fileF = new TFile(fileFname,"READ");
    if (fileF->IsZombie()) {
      std::cout << "Error opening file: " <<fileFname<< std::endl;
      reconstF[iNtrk]=0;//number of reconstructed tracks
      ghostF_I[iNtrk]=0; //percent of ghost tracks
      missedF_I[iNtrk]=0; //percent of missed tracks
      ghostF_II[iNtrk]=0; //percent of ghost tracks
      missedF_II[iNtrk]=0; //percent of missed tracks

      ghostF_I_mean[iNtrk]=0; //average of ghost tracks
      missedF_I_mean[iNtrk]=0; //average of missed tracks
      ghostF_II_mean[iNtrk]=0; //average of ghost tracks
      missedF_II_mean[iNtrk]=0; //average of missed tracks
      ghostF_I_rms[iNtrk]=0; //rms average of ghost tracks
      missedF_I_rms[iNtrk]=0; //rms average of missed tracks
      ghostF_II_rms[iNtrk]=0; //rms average of ghost tracks
      missedF_II_rms[iNtrk]=0; //rms average of missed tracks
      goodF_I_mean[iNtrk]=0; //average of good tracks
      goodF_II_mean[iNtrk]=0; //average of good tracks
       continue;
    }

    TH1F* hResThetaCA = (TH1F*)fileCA->Get("hResTheta");
    int nrecCA = hResThetaCA->GetEntries();
    reconstCA[iNtrk]=nrecCA;
    TH1F* hResThetaF = (TH1F*)fileF->Get("hResTheta");
    int nrecF = hResThetaF->GetEntries();
    reconstF[iNtrk]=nrecF;

    //good and missed\ghost from definition I: compare dPhi & dTheta for MC and REC trks
    TH1F* hntrkgoodCA_I = (TH1F*)fileCA->Get("hntrkgood_I");
    goodCA_I_mean[iNtrk] = hntrkgoodCA_I->GetMean();

    TH1F* hntrkmissedCA_I = (TH1F*)fileCA->Get("hntrkmissed_I");
    int nmissCA_I = hntrkmissedCA_I->GetEntries();
    // int nmissCA_I_under = hntrkmissedCA_I->GetBinContent(0);
    // nmissCA_I-=nmissCA_I_under;
    missedCA_I_mean[iNtrk] = hntrkmissedCA_I->GetMean();
    missedCA_I_rms[iNtrk] = hntrkmissedCA_I->GetMeanError();
  
    TH1F* hntrkgoodF_I = (TH1F*)fileF->Get("hntrkgood_I");
    goodF_I_mean[iNtrk] = hntrkgoodF_I->GetMean();
    TH1F* hntrkmissedF_I = (TH1F*)fileF->Get("hntrkmissed_I");
    int nmissF_I = hntrkmissedF_I->GetEntries();
    // int nmissF_I_under = hntrkmissedF_I->GetBinContent(0);
    // nmissF_I-=nmissF_I_under;
    missedF_I_mean[iNtrk] = hntrkmissedF_I->GetMean();
    missedF_I_rms[iNtrk] = hntrkmissedF_I->GetMeanError();

    TH1F* hntrkghostCA_I = (TH1F*)fileCA->Get("hntrkghost_I");
    int nghostCA_I = hntrkghostCA_I->GetEntries();
    // int nghostCA_I_under = hntrkghostCA_I->GetBinContent(0);
    // nghostCA_I-=nghostCA_I_under;
    ghostCA_I_mean[iNtrk] = hntrkghostCA_I->GetMean();
    ghostCA_I_rms[iNtrk] = hntrkghostCA_I->GetMeanError();

    TH1F* hntrkghostF_I = (TH1F*)fileF->Get("hntrkghost_I");
    int nghostF_I = hntrkghostF_I->GetEntries();
    // int nghostF_I_under = hntrkghostF_I->GetBinContent(0);
    // nghostF_I-=nghostF_I_under;
    ghostF_I_mean[iNtrk] = hntrkghostF_I->GetMean();
    ghostF_I_rms[iNtrk] = hntrkghostF_I->GetMeanError();

    //missed\ghost from definition II: based on hits info
    TH1F* hntrkgoodCA_II = (TH1F*)fileCA->Get("hntrkgood_II");
    goodCA_II_mean[iNtrk] = hntrkgoodCA_II->GetMean();
    TH1F* hntrkmissedCA_II = (TH1F*)fileCA->Get("hntrkmissed_II");
    int nmissCA_II = hntrkmissedCA_II->GetEntries();
    // int nmissCA_II_under = hntrkmissedCA_II->GetBinContent(0);
    // nmissCA_II-=nmissCA_II_under;
    missedCA_II_mean[iNtrk] = hntrkmissedCA_II->GetMean();
    missedCA_II_rms[iNtrk] = hntrkmissedCA_II->GetMeanError();  

    TH1F* hntrkgoodF_II = (TH1F*)fileF->Get("hntrkgood_II");
    goodF_II_mean[iNtrk] = hntrkgoodF_II->GetMean();
    TH1F* hntrkmissedF_II = (TH1F*)fileF->Get("hntrkmissed_II");
    int nmissF_II = hntrkmissedF_II->GetEntries();
    // int nmissF_II_under = hntrkmissedF_II->GetBinContent(0);
    // nmissF_II-=nmissF_II_under;
    missedF_II_mean[iNtrk] = hntrkmissedF_II->GetMean();
    missedF_II_rms[iNtrk] = hntrkmissedF_II->GetMeanError();

    TH1F* hntrkghostCA_II = (TH1F*)fileCA->Get("hntrkghost_II");
    int nghostCA_II = hntrkghostCA_II->GetEntries();
    // int nghostCA_II_under = hntrkghostCA_II->GetBinContent(0);
    // nghostCA_II-=nghostCA_II_under;
    ghostCA_II_mean[iNtrk] = hntrkghostCA_II->GetMean();
    ghostCA_II_rms[iNtrk] = hntrkghostCA_II->GetMeanError();

    TH1F* hntrkghostF_II = (TH1F*)fileF->Get("hntrkghost_II");
    int nghostF_II = hntrkghostF_II->GetEntries();
    // int nghostF_II_under = hntrkghostF_II->GetBinContent(0);
    // nghostF_II-=nghostF_II_under;
    ghostF_II_mean[iNtrk] = hntrkghostF_II->GetMean();
    ghostF_II_rms[iNtrk] = hntrkghostF_II->GetMeanError();

    TH2F* hnRecnMCCA = (TH2F*)fileCA->Get("hnRecnMC");
    TH1D* hnRecnMCCA_rec = hnRecnMCCA->ProjectionY();
    int nsimTrk = (hnRecnMCCA_rec->GetEntries()); 
   
    // ghostCA_I[iNtrk]=100*double(nghostCA_I*ghostCA_I_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    // missedCA_I[iNtrk]=100*double(nmissCA_I*missedCA_I_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    // ghostF_I[iNtrk]=100*double(nghostF_I*ghostF_I_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    // missedF_I[iNtrk]=100*double(nmissF_I*missedF_I_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    // ghostCA_II[iNtrk]=100*double(nghostCA_II*ghostCA_II_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    // missedCA_II[iNtrk]=100*double(nmissCA_II*missedCA_II_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    // ghostF_II[iNtrk]=100*double(nghostF_II*ghostF_II_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    // missedF_II[iNtrk]=100*double(nmissF_II*missedF_II_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);

    ghostCA_I[iNtrk]=100*double(nghostCA_I)/(nsimTrk);
    missedCA_I[iNtrk]=100*double(nmissCA_I)/(nsimTrk);
    ghostF_I[iNtrk]=100*double(nghostF_I)/(nsimTrk);
    missedF_I[iNtrk]=100*double(nmissF_I)/(nsimTrk);
    ghostCA_II[iNtrk]=100*double(nghostCA_II)/(nsimTrk);
    missedCA_II[iNtrk]=100*double(nmissCA_II)/(nsimTrk);
    ghostF_II[iNtrk]=100*double(nghostF_II)/(nsimTrk);
    missedF_II[iNtrk]=100*double(nmissF_II)/(nsimTrk);

    // timeCA[iNtrk]=1000*double(timeCA[iNtrk])/reconstCA[iNtrk];
    // timeF[iNtrk]=1000*double(timeF[iNtrk])/reconstF[iNtrk];
    timeCA[iNtrk]=20*1000*double(timeCA[iNtrk])/reconstCA[iNtrk];
    timeF[iNtrk]= 20*1000*double(timeF[iNtrk])/reconstF[iNtrk];

    cout<<"nsimTrk = "<<nsimTrk<<" (with "<<Ntrks[iNtrk]<<" trks/ev) nrecF = "<<nrecF<<" nrecCA = "<<nrecCA<<endl;
    cout<<"(I:based on phi&theta diff between MC and REC), (II:based on hit info)"<<endl;
    //    cout<<" nmissF_I = "<<nmissF_I<<" nmissCA_I = "<<nmissCA_I<<endl;
    cout<<" nmissF_II = "<<nmissF_II<<" nmissCA_II = "<<nmissCA_II<<endl;
    //    cout<<" nghostF_I = "<<nghostF_I<<" nghostCA_I = "<<nghostCA_I<<endl;
    cout<<" nghostF_II = "<<nghostF_II<<" nghostCA_II = "<<nghostCA_II<<endl;
    cout<<" timeF = "<<timeF[iNtrk]<<" timeCA = "<<timeCA[iNtrk]<<endl;
    cout<<"***************************************"<<endl;
    cout<<" "<<endl;


  }
  TGraph *grSpeedCA = new TGraph(numTrks,Ntrks,timeCA);
  grSpeedCA->SetMarkerStyle(20);
  grSpeedCA->SetMarkerColor(2);
  grSpeedCA->SetMarkerSize(1.2);
  TGraph *grSpeedF = new TGraph(numTrks,Ntrks,timeF);
  grSpeedF->SetMarkerStyle(20);
  grSpeedF->SetMarkerColor(4);
  grSpeedF->SetMarkerSize(1.2);
  TMultiGraph *mgSpeed = new TMultiGraph("mgSpeed","Speed;N^{trk}_{MC};time per trk, ms");
  mgSpeed->Add(grSpeedCA);
  mgSpeed->Add(grSpeedF);
 

  // TGraph *grRecCA = new TGraph(numTrks,Ntrks,reconstCA);
  // grRecCA->SetMarkerStyle(20);
  // grRecCA->SetMarkerColor(2);
  // grRecCA->SetMarkerSize(1.2);
  // TGraph *grRecF = new TGraph(numTrks,Ntrks,reconstF);
  // grRecF->SetMarkerStyle(20);
  // grRecF->SetMarkerColor(4);
  // grRecF->SetMarkerSize(1.2);
  // TMultiGraph *mgTotRec = new TMultiGraph("mgRecTrks","RecTrks;N^{trk}_{MC};N^{trk}_{Rec}");
  // mgTotRec->Add(grRecCA);
  // mgTotRec->Add(grRecF);

  TGraph *grRecCA_I = new TGraph(numTrks,Ntrks,goodCA_I_mean);
  grRecCA_I->SetMarkerStyle(20);
  grRecCA_I->SetMarkerColor(2);
  grRecCA_I->SetMarkerSize(1.2);
  TGraph *grRecF_I = new TGraph(numTrks,Ntrks,goodF_I_mean);
  grRecF_I->SetMarkerStyle(20);
  grRecF_I->SetMarkerColor(4);
  grRecF_I->SetMarkerSize(1.2);
  
  TGraph *grRecCA_II = new TGraph(numTrks,Ntrks,goodCA_II_mean);
  grRecCA_II->SetMarkerStyle(21);
  grRecCA_II->SetMarkerColor(2);
  grRecCA_II->SetMarkerSize(1.2);
  TGraph *grRecF_II = new TGraph(numTrks,Ntrks,goodF_II_mean);
  grRecF_II->SetMarkerStyle(21);
  grRecF_II->SetMarkerColor(4);
  grRecF_II->SetMarkerSize(1.2);

  TMultiGraph *mgTotRec = new TMultiGraph("mgRecTrks","Good RecTrks;N^{trk}_{MC};N^{trk}_{Rec}");
  // mgTotRec->Add(grRecCA_I);
  mgTotRec->Add(grRecCA_II);
  //mgTotRec->Add(grRecF_I);
  mgTotRec->Add(grRecF_II);

  TGraph *grMissCA_I = new TGraph(numTrks,Ntrks,missedCA_I);
  grMissCA_I->SetMarkerStyle(20);
  grMissCA_I->SetMarkerColor(2);
  grMissCA_I->SetMarkerSize(1.2);
  TGraph *grMissF_I = new TGraph(numTrks,Ntrks,missedF_I);
  grMissF_I->SetMarkerStyle(20);
  grMissF_I->SetMarkerColor(4);
  grMissF_I->SetMarkerSize(1.2);
  TGraph *grMissCA_II = new TGraph(numTrks,Ntrks,missedCA_II);
  grMissCA_II->SetMarkerStyle(21);
  grMissCA_II->SetMarkerColor(2);
  grMissCA_II->SetMarkerSize(1.2);
  TGraph *grMissF_II = new TGraph(numTrks,Ntrks,missedF_II);
  grMissF_II->SetMarkerStyle(21);
  grMissF_II->SetMarkerColor(4);
  grMissF_II->SetMarkerSize(1.2);
  TMultiGraph *mgMissed = new TMultiGraph("mgMissed","Missed;N^{trk}_{MC};events with missed trks, %");
  //  mgMissed->Add(grMissCA_I);
  // mgMissed->Add(grMissF_I);
  mgMissed->Add(grMissCA_II);
  mgMissed->Add(grMissF_II);
 

  TGraph *grGhostCA_I = new TGraph(numTrks,Ntrks,ghostCA_I);
  grGhostCA_I->SetMarkerStyle(20);
  grGhostCA_I->SetMarkerColor(2);
  grGhostCA_I->SetMarkerSize(1.2);
  TGraph *grGhostF_I = new TGraph(numTrks,Ntrks,ghostF_I);
  grGhostF_I->SetMarkerStyle(20);
  grGhostF_I->SetMarkerColor(4);
  grGhostF_I->SetMarkerSize(1.2);
  TGraph *grGhostCA_II = new TGraph(numTrks,Ntrks,ghostCA_II);
  grGhostCA_II->SetMarkerStyle(21);
  grGhostCA_II->SetMarkerColor(2);
  grGhostCA_II->SetMarkerSize(1.2);
  TGraph *grGhostF_II = new TGraph(numTrks,Ntrks,ghostF_II);
  grGhostF_II->SetMarkerStyle(21);
  grGhostF_II->SetMarkerColor(4);
  grGhostF_II->SetMarkerSize(1.2);
  TMultiGraph *mgGhost = new TMultiGraph("mgGhost","Ghost;N^{trk}_{MC};events with ghost trks, %");
  //mgGhost->Add(grGhostCA_I);
  // mgGhost->Add(grGhostF_I);
  mgGhost->Add(grGhostCA_II);
  mgGhost->Add(grGhostF_II);


  TGraphErrors *grMissCA_value_I = new TGraphErrors(numTrks,Ntrks,missedCA_I_mean,eNtrks,missedCA_I_rms);
  grMissCA_value_I->SetMarkerStyle(20);
  grMissCA_value_I->SetMarkerColor(2);
  grMissCA_value_I->SetMarkerSize(1.2);
  TGraphErrors *grMissCA_value_II = new TGraphErrors(numTrks,Ntrks,missedCA_II_mean,eNtrks,missedCA_II_rms);
  grMissCA_value_II->SetMarkerStyle(21);
  grMissCA_value_II->SetMarkerColor(2);
  grMissCA_value_II->SetMarkerSize(1.2);
  TGraphErrors *grGhostCA_value_I = new TGraphErrors(numTrks,Ntrks,ghostCA_I_mean,eNtrks,ghostCA_I_rms);
  grGhostCA_value_I->SetMarkerStyle(20);
  grGhostCA_value_I->SetMarkerColor(2);
  grGhostCA_value_I->SetMarkerSize(1.2);
  TGraphErrors *grGhostCA_value_II = new TGraphErrors(numTrks,Ntrks,ghostCA_II_mean,eNtrks,ghostCA_II_rms);
  grGhostCA_value_II->SetMarkerStyle(21);
  grGhostCA_value_II->SetMarkerColor(2);
  grGhostCA_value_II->SetMarkerSize(1.2);

  TGraphErrors *grMissF_value_I = new TGraphErrors(numTrks,Ntrks,missedF_I_mean,eNtrks,missedF_I_rms);
  grMissF_value_I->SetMarkerStyle(20);
  grMissF_value_I->SetMarkerColor(4);
  grMissF_value_I->SetMarkerSize(1.2);
  TGraphErrors *grMissF_value_II = new TGraphErrors(numTrks,Ntrks,missedF_II_mean,eNtrks,missedF_II_rms);
  grMissF_value_II->SetMarkerStyle(21);
  grMissF_value_II->SetMarkerColor(4);
  grMissF_value_II->SetMarkerSize(1.2);
  TGraphErrors *grGhostF_value_I = new TGraphErrors(numTrks,Ntrks,ghostF_I_mean,eNtrks,ghostF_I_rms);
  grGhostF_value_I->SetMarkerStyle(20);
  grGhostF_value_I->SetMarkerColor(4);
  grGhostF_value_I->SetMarkerSize(1.2);
  TGraphErrors *grGhostF_value_II = new TGraphErrors(numTrks,Ntrks,ghostF_II_mean,eNtrks,ghostF_II_rms);
  grGhostF_value_II->SetMarkerStyle(21);
  grGhostF_value_II->SetMarkerColor(4);
  grGhostF_value_II->SetMarkerSize(1.2);

  TMultiGraph *mgMissedMean = new TMultiGraph("mgMissedMean","Missed;N^{trk}_{MC};average number of missed trks");
  mgMissedMean->Add(grMissCA_value_I);
  mgMissedMean->Add(grMissCA_value_II);
  mgMissedMean->Add(grMissF_value_I);
  mgMissedMean->Add(grMissF_value_II);

  TMultiGraph *mgGhostMean = new TMultiGraph("mgGhostMean","Ghost;N^{trk}_{MC};average number of ghost trks");
  // mgGhostMean->Add(grGhostCA_value_I);
  mgGhostMean->Add(grGhostCA_value_II);
  //  mgGhostMean->Add(grGhostF_value_I);
  mgGhostMean->Add(grGhostF_value_II);

  TString fileOUTname=storePath+"/TrksAlgoResults.root";
  TString fileOUTnamepdf=storePath+"/TrksAlgoResults.pdf";
  TFile *fileOUT = new TFile(fileOUTname,"RECREATE");
  TCanvas *c1 = new TCanvas("Overview");
  c1->Divide(3,2);
  c1->cd(1);
  // const char *longstring = "K_{S}... K^{*0}... #frac{2s}{#pi#alpha^{2}}
  //  #frac{d#sigma}{dcos#theta} (e^{+}e^{-} #rightarrow f#bar{f} ) =
    //  #left| #frac{1}{1 - #Delta#alpha} #right|^{2} (1+cos^{2}#theta)";

  mgSpeed->Draw("A");
  TLatex latex;
  latex.SetTextSize(0.035);
  latex.SetTextAlign(12);  //align at center
 
  // latex.DrawLatex(1.8,2.1,"#it{Cellular Automaton (Trks matching)}");
  // latex.DrawLatex(1.8,2.0,"#it{Cellular Automaton (Hits matching)}");
  // latex.DrawLatex(1.8,1.8,"#it{Trk-Following (Trks matching)}");
  // latex.DrawLatex(1.8,1.7,"#it{Trk-Following (Hits matching)}");


  //  TPad* info = new TPad("info","information",0,0,0.3,0.5);
  //  info->Draw();
  //  latex.DrawLatex(3.9,0.8,"#it{Cellular Automaton (Trks matching)}");
  //  latex.DrawLatex(3.9,0.75,"#it{Cellular Automaton (Hits matching)}");
  //  latex.DrawLatex(3.9,0.7,"#it{Trk-Following (Trks matching)}");
  //latex.DrawLatex(3.9,0.65,"#it{Trk-Following (Hits matching)}");

    latex.DrawLatex(3.9,0.75,"#it{Cellular Automaton}");
    latex.DrawLatex(3.9,0.7,"#it{Cellular Automaton (hits&trk search missed)}");
    latex.DrawLatex(3.9,0.65,"#it{Trk-Following}");
    latex.DrawLatex(3.9,0.6,"#it{Trk-Following (hits&trk search missed)}");
  TMarker caI(1.2, 0.9, 20);
  caI.SetMarkerColor(2);
  caI.SetMarkerSize(1.2); 
  TMarker caII(1.2, 0.85, 21);
  caII.SetMarkerColor(2);
  caII.SetMarkerSize(1.2);
  // caI.DrawMarker(1.2,1.1);
  // caII.DrawMarker(1.2,1.0);
  caI.DrawMarker(3.4,0.7);
  caII.DrawMarker(3.4,0.75);


  TMarker fI(1.2, 0.9, 20);
  fI.SetMarkerColor(4);
  fI.SetMarkerSize(1.2); 
  TMarker fII(1.2, 0.85, 21);
  fII.SetMarkerColor(4);
  fII.SetMarkerSize(1.2);
  // fI.DrawMarker(1.2,0.8);
  // fII.DrawMarker(1.2,0.7);
  fI.DrawMarker(3.4,0.6);
  fII.DrawMarker(3.4,0.65);

  // latex.DrawLatex(.2,.8,longstring);
  c1->cd(4);
  mgTotRec->Draw("APL");
  c1->cd(3);
  mgMissed->Draw("APL");
  c1->cd(6);
  mgGhost->Draw("APL");
  c1->cd(2);
  mgMissedMean->Draw("APL");
  c1->cd(5);
  mgGhostMean->Draw("APL");
  c1->Write();
  mgSpeed->Write();
  mgTotRec->Write();
  mgMissed->Write();
  mgGhost->Write();
  mgMissedMean->Write();
  mgGhostMean->Write();
  fileOUT->Close();
  c1->SaveAs(fileOUTnamepdf);
}
