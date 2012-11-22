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

void TrksAlgoResults(TString storePath="StripSensors")
//void TrksAlgoRes(TString storePath="PixelSensors")
//void TrksAlgoResults(TString storePath="/myResults/LUMI_Trk/pixelDesign/100000events")
{
  // double Ntrks[8]={1,2,3,4,5,10,15,20};
  double Ntrks[7]={1,2,3,4,5,10,15};
  double eNtrks[7]={0,0,0,0,0,0,0};
  //Track-Follow
  double reconstF[7];//number of reconstructed tracks
  double ghostF_I[7]; //percent of ghost tracks
  double missedF_I[7]; //percent of missed tracks
  double ghostF_II[7]; //percent of ghost tracks
  double missedF_II[7]; //percent of missed tracks

  double ghostF_I_mean[7]; //average of ghost tracks
  double missedF_I_mean[7]; //average of missed tracks
  double ghostF_II_mean[7]; //average of ghost tracks
  double missedF_II_mean[7]; //average of missed tracks
  double ghostF_I_rms[7]; //rms average of ghost tracks
  double missedF_I_rms[7]; //rms average of missed tracks
  double ghostF_II_rms[7]; //rms average of ghost tracks
  double missedF_II_rms[7]; //rms average of missed tracks
  double goodF_I_mean[7]; //average of good tracks
  double goodF_II_mean[7]; //average of good tracks

  //  double timeF[7]={25.3,32.28,41.28,52.47,68.69,334.81,1611.94};//time spent per 1 trk reconstruction , ms
  double timeF[7]={0,0,0,0,0,0,0};//time spent per 1 trk reconstruction , ms
  //Cellular Automaton
  double reconstCA[7];//number of reconstructed tracks
  double ghostCA_I[7]; //percent of ghost tracks
  double missedCA_I[7]; //percent of missed tracks
  double ghostCA_II[7]; //percent of ghost tracks
  double missedCA_II[7]; //percent of missed tracks

  double ghostCA_I_mean[7]; //average of ghost tracks
  double missedCA_I_mean[7]; //average of missed tracks
  double ghostCA_II_mean[7]; //average of ghost tracks
  double missedCA_II_mean[7]; //average of missed tracks
  double ghostCA_I_rms[7]; //rms average of ghost tracks
  double missedCA_I_rms[7]; //rms average of missed tracks
  double ghostCA_II_rms[7]; //rms average of ghost tracks
  double missedCA_II_rms[7]; //rms average of missed tracks
  double goodCA_I_mean[7]; //average of good tracks
  double goodCA_II_mean[7]; //average of good tracks

  //  double timeCA[7]={25.99,33.26,41.63,51.25,63.08,155.27,343.77};//time spent per 1 trk reconstruction , ms
  double timeCA[7]={0,0,0,0,0,0,0};//time spent per 1 trk reconstruction , ms
  for(int iNtrk=0;iNtrk<7;iNtrk++){
    // ///STRIP -----------------------------------------------------------------
    TString fileCAname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    fileCAname+="_CA_";
    TString fileFname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    fileFname+="_Follow_";
    fileCAname+=Ntrks[iNtrk];
    fileFname+=Ntrks[iNtrk];
    fileCAname+="trks_";
    fileFname+="trks_";
     fileCAname+="KalmanFillter_";
     fileFname+="KalmanFillter_";
    // fileCAname+="MinuitFit_";
    // fileFname+="MinuitFit_";
    fileCAname+="1.5.root";
    fileFname+="1.5.root";
    // ///STRIP -----------------------------------------------------------------

    // ///PIXEL -----------------------------------------------------------------
    // TString fileCAname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    // fileCAname+="_CA_";
    // TString fileFname=storePath+"/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    // fileFname+="_Follow_";
    // fileCAname+=Ntrks[iNtrk];
    // fileFname+=Ntrks[iNtrk];
    // fileCAname+="trks_";
    // fileFname+="trks_";
    // fileCAname+="mergedHits_true_";
    // fileFname+="mergedHits_true_";
    // //    fileCAname+="addMS_false_KalmanFillter_";
    // //    fileFname+="addMS_false_KalmanFillter_";
    // fileCAname+="addMS_true_MinuitFit_";
    // fileFname+="addMS_true_MinuitFit_";
    // fileCAname+="15.root";
    // fileFname+="15.root";
    // ///PIXEL -----------------------------------------------------------------

    TFile *fileCA = new TFile(fileCAname,"READ");
    TFile *fileF = new TFile(fileFname,"READ");
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
    int nmissCA_I_under = hntrkmissedCA_I->GetBinContent(0);
    nmissCA_I-=nmissCA_I_under;
    missedCA_I_mean[iNtrk] = hntrkmissedCA_I->GetMean();
    missedCA_I_rms[iNtrk] = hntrkmissedCA_I->GetMeanError();
  
    TH1F* hntrkgoodF_I = (TH1F*)fileF->Get("hntrkgood_I");
    goodF_I_mean[iNtrk] = hntrkgoodF_I->GetMean();
    TH1F* hntrkmissedF_I = (TH1F*)fileF->Get("hntrkmissed_I");
    int nmissF_I = hntrkmissedF_I->GetEntries();
    int nmissF_I_under = hntrkmissedF_I->GetBinContent(0);
    nmissF_I-=nmissF_I_under;
    missedF_I_mean[iNtrk] = hntrkmissedF_I->GetMean();
    missedF_I_rms[iNtrk] = hntrkmissedF_I->GetMeanError();

    TH1F* hntrkghostCA_I = (TH1F*)fileCA->Get("hntrkghost_I");
    int nghostCA_I = hntrkghostCA_I->GetEntries();
    int nghostCA_I_under = hntrkghostCA_I->GetBinContent(0);
    nghostCA_I-=nghostCA_I_under;
    ghostCA_I_mean[iNtrk] = hntrkghostCA_I->GetMean();
    ghostCA_I_rms[iNtrk] = hntrkghostCA_I->GetMeanError();

    TH1F* hntrkghostF_I = (TH1F*)fileF->Get("hntrkghost_I");
    int nghostF_I = hntrkghostF_I->GetEntries();
    int nghostF_I_under = hntrkghostF_I->GetBinContent(0);
    nghostF_I-=nghostF_I_under;
    ghostF_I_mean[iNtrk] = hntrkghostF_I->GetMean();
    ghostF_I_rms[iNtrk] = hntrkghostF_I->GetMeanError();

    //missed\ghost from definition II: based on hits info
    TH1F* hntrkgoodCA_II = (TH1F*)fileCA->Get("hntrkgood_II");
    goodCA_II_mean[iNtrk] = hntrkgoodCA_II->GetMean();
    TH1F* hntrkmissedCA_II = (TH1F*)fileCA->Get("hntrkmissed_II");
    int nmissCA_II = hntrkmissedCA_II->GetEntries();
    int nmissCA_II_under = hntrkmissedCA_II->GetBinContent(0);
    nmissCA_II-=nmissCA_II_under;
    missedCA_II_mean[iNtrk] = hntrkmissedCA_II->GetMean();
    missedCA_II_rms[iNtrk] = hntrkmissedCA_II->GetMeanError();  

    TH1F* hntrkgoodF_II = (TH1F*)fileF->Get("hntrkgood_II");
    goodF_II_mean[iNtrk] = hntrkgoodF_II->GetMean();
    TH1F* hntrkmissedF_II = (TH1F*)fileF->Get("hntrkmissed_II");
    int nmissF_II = hntrkmissedF_II->GetEntries();
    int nmissF_II_under = hntrkmissedF_II->GetBinContent(0);
    nmissF_II-=nmissF_II_under;
    missedF_II_mean[iNtrk] = hntrkmissedF_II->GetMean();
    missedF_II_rms[iNtrk] = hntrkmissedF_II->GetMeanError();

    TH1F* hntrkghostCA_II = (TH1F*)fileCA->Get("hntrkghost_II");
    int nghostCA_II = hntrkghostCA_II->GetEntries();
    int nghostCA_II_under = hntrkghostCA_II->GetBinContent(0);
    nghostCA_II-=nghostCA_II_under;
    ghostCA_II_mean[iNtrk] = hntrkghostCA_II->GetMean();
    ghostCA_II_rms[iNtrk] = hntrkghostCA_II->GetMeanError();

    TH1F* hntrkghostF_II = (TH1F*)fileF->Get("hntrkghost_II");
    int nghostF_II = hntrkghostF_II->GetEntries();
    int nghostF_II_under = hntrkghostF_II->GetBinContent(0);
    nghostF_II-=nghostF_II_under;
    ghostF_II_mean[iNtrk] = hntrkghostF_II->GetMean();
    ghostF_II_rms[iNtrk] = hntrkghostF_II->GetMeanError();

    TH2F* hnRecnMCCA = (TH2F*)fileCA->Get("hnRecnMC");
    TH1D* hnRecnMCCA_rec = hnRecnMCCA->ProjectionY();
    int nsimTrk = (hnRecnMCCA_rec->GetEntries()); 
   
    ghostCA_I[iNtrk]=100*double(nghostCA_I*ghostCA_I_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    missedCA_I[iNtrk]=100*double(nmissCA_I*missedCA_I_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    ghostF_I[iNtrk]=100*double(nghostF_I*ghostF_I_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    missedF_I[iNtrk]=100*double(nmissF_I*missedF_I_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    ghostCA_II[iNtrk]=100*double(nghostCA_II*ghostCA_II_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    missedCA_II[iNtrk]=100*double(nmissCA_II*missedCA_II_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    ghostF_II[iNtrk]=100*double(nghostF_II*ghostF_II_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    missedF_II[iNtrk]=100*double(nmissF_II*missedF_II_mean[iNtrk])/(nsimTrk*Ntrks[iNtrk]);
    timeCA[iNtrk]=1000*double(timeCA[iNtrk])/reconstCA[iNtrk];
    timeF[iNtrk]=1000*double(timeF[iNtrk])/reconstF[iNtrk];
    cout<<"nsimTrk = "<<nsimTrk<<" nrecF = "<<nrecF<<" nrecCA = "<<nrecCA<<endl;
    cout<<"(I:based on phi&theta diff between MC and REC), (II:based on hit info)"<<endl;
    cout<<" nmissF_I = "<<nmissF_I<<" nmissCA_I = "<<nmissCA_I<<endl;
    cout<<" nmissF_II = "<<nmissF_II<<" nmissCA_II = "<<nmissCA_II<<endl;
    cout<<" nghostF_I = "<<nghostF_I<<" nghostCA_I = "<<nghostCA_I<<endl;
    cout<<" nghostF_II = "<<nghostF_II<<" nghostCA_II = "<<nghostCA_II<<endl;
    cout<<" timeF = "<<timeF[iNtrk]<<" timeCA = "<<timeCA[iNtrk]<<endl;
    cout<<"***************************************"<<endl;
    cout<<" "<<endl;


  }
  TGraph *grSpeedCA = new TGraph(7,Ntrks,timeCA);
  grSpeedCA->SetMarkerStyle(20);
  grSpeedCA->SetMarkerColor(2);
  grSpeedCA->SetMarkerSize(1.2);
  TGraph *grSpeedF = new TGraph(7,Ntrks,timeF);
  grSpeedF->SetMarkerStyle(20);
  grSpeedF->SetMarkerColor(4);
  grSpeedF->SetMarkerSize(1.2);
  TMultiGraph *mgSpeed = new TMultiGraph("mgSpeed","Speed;N^{trk}_{MC};time per trk, ms");
  mgSpeed->Add(grSpeedCA);
  mgSpeed->Add(grSpeedF);
 

  // TGraph *grRecCA = new TGraph(7,Ntrks,reconstCA);
  // grRecCA->SetMarkerStyle(20);
  // grRecCA->SetMarkerColor(2);
  // grRecCA->SetMarkerSize(1.2);
  // TGraph *grRecF = new TGraph(7,Ntrks,reconstF);
  // grRecF->SetMarkerStyle(20);
  // grRecF->SetMarkerColor(4);
  // grRecF->SetMarkerSize(1.2);
  // TMultiGraph *mgTotRec = new TMultiGraph("mgRecTrks","RecTrks;N^{trk}_{MC};N^{trk}_{Rec}");
  // mgTotRec->Add(grRecCA);
  // mgTotRec->Add(grRecF);

  TGraph *grRecCA_I = new TGraph(7,Ntrks,goodCA_I_mean);
  grRecCA_I->SetMarkerStyle(20);
  grRecCA_I->SetMarkerColor(2);
  grRecCA_I->SetMarkerSize(1.2);
  TGraph *grRecF_I = new TGraph(7,Ntrks,goodF_I_mean);
  grRecF_I->SetMarkerStyle(20);
  grRecF_I->SetMarkerColor(4);
  grRecF_I->SetMarkerSize(1.2);
  
  TGraph *grRecCA_II = new TGraph(7,Ntrks,goodCA_II_mean);
  grRecCA_II->SetMarkerStyle(21);
  grRecCA_II->SetMarkerColor(2);
  grRecCA_II->SetMarkerSize(1.2);
  TGraph *grRecF_II = new TGraph(7,Ntrks,goodF_II_mean);
  grRecF_II->SetMarkerStyle(21);
  grRecF_II->SetMarkerColor(4);
  grRecF_II->SetMarkerSize(1.2);

  TMultiGraph *mgTotRec = new TMultiGraph("mgRecTrks","Good RecTrks;N^{trk}_{MC};N^{trk}_{Rec}");
  mgTotRec->Add(grRecCA_I);
  mgTotRec->Add(grRecCA_II);
  mgTotRec->Add(grRecF_I);
  mgTotRec->Add(grRecF_II);

  TGraph *grMissCA_I = new TGraph(7,Ntrks,missedCA_I);
  grMissCA_I->SetMarkerStyle(20);
  grMissCA_I->SetMarkerColor(2);
  grMissCA_I->SetMarkerSize(1.2);
  TGraph *grMissF_I = new TGraph(7,Ntrks,missedF_I);
  grMissF_I->SetMarkerStyle(20);
  grMissF_I->SetMarkerColor(4);
  grMissF_I->SetMarkerSize(1.2);
  TGraph *grMissCA_II = new TGraph(7,Ntrks,missedCA_II);
  grMissCA_II->SetMarkerStyle(21);
  grMissCA_II->SetMarkerColor(2);
  grMissCA_II->SetMarkerSize(1.2);
  TGraph *grMissF_II = new TGraph(7,Ntrks,missedF_II);
  grMissF_II->SetMarkerStyle(21);
  grMissF_II->SetMarkerColor(4);
  grMissF_II->SetMarkerSize(1.2);
  TMultiGraph *mgMissed = new TMultiGraph("mgMissed","Missed;N^{trk}_{MC};missed trks, %");
  mgMissed->Add(grMissCA_I);
  mgMissed->Add(grMissF_I);
  mgMissed->Add(grMissCA_II);
  mgMissed->Add(grMissF_II);
 

  TGraph *grGhostCA_I = new TGraph(7,Ntrks,ghostCA_I);
  grGhostCA_I->SetMarkerStyle(20);
  grGhostCA_I->SetMarkerColor(2);
  grGhostCA_I->SetMarkerSize(1.2);
  TGraph *grGhostF_I = new TGraph(7,Ntrks,ghostF_I);
  grGhostF_I->SetMarkerStyle(20);
  grGhostF_I->SetMarkerColor(4);
  grGhostF_I->SetMarkerSize(1.2);
  TGraph *grGhostCA_II = new TGraph(7,Ntrks,ghostCA_II);
  grGhostCA_II->SetMarkerStyle(21);
  grGhostCA_II->SetMarkerColor(2);
  grGhostCA_II->SetMarkerSize(1.2);
  TGraph *grGhostF_II = new TGraph(7,Ntrks,ghostF_II);
  grGhostF_II->SetMarkerStyle(21);
  grGhostF_II->SetMarkerColor(4);
  grGhostF_II->SetMarkerSize(1.2);
  TMultiGraph *mgGhost = new TMultiGraph("mgGhost","Ghost;N^{trk}_{MC};ghost trks, %");
  mgGhost->Add(grGhostCA_I);
  mgGhost->Add(grGhostF_I);
  mgGhost->Add(grGhostCA_II);
  mgGhost->Add(grGhostF_II);


  TGraphErrors *grMissCA_value_I = new TGraphErrors(7,Ntrks,missedCA_I_mean,eNtrks,missedCA_I_rms);
  grMissCA_value_I->SetMarkerStyle(20);
  grMissCA_value_I->SetMarkerColor(2);
  grMissCA_value_I->SetMarkerSize(1.2);
  TGraphErrors *grMissCA_value_II = new TGraphErrors(7,Ntrks,missedCA_II_mean,eNtrks,missedCA_II_rms);
  grMissCA_value_II->SetMarkerStyle(21);
  grMissCA_value_II->SetMarkerColor(2);
  grMissCA_value_II->SetMarkerSize(1.2);
  TGraphErrors *grGhostCA_value_I = new TGraphErrors(7,Ntrks,ghostCA_I_mean,eNtrks,ghostCA_I_rms);
  grGhostCA_value_I->SetMarkerStyle(20);
  grGhostCA_value_I->SetMarkerColor(2);
  grGhostCA_value_I->SetMarkerSize(1.2);
  TGraphErrors *grGhostCA_value_II = new TGraphErrors(7,Ntrks,ghostCA_II_mean,eNtrks,ghostCA_II_rms);
  grGhostCA_value_II->SetMarkerStyle(21);
  grGhostCA_value_II->SetMarkerColor(2);
  grGhostCA_value_II->SetMarkerSize(1.2);

  TGraphErrors *grMissF_value_I = new TGraphErrors(7,Ntrks,missedF_I_mean,eNtrks,missedF_I_rms);
  grMissF_value_I->SetMarkerStyle(20);
  grMissF_value_I->SetMarkerColor(4);
  grMissF_value_I->SetMarkerSize(1.2);
  TGraphErrors *grMissF_value_II = new TGraphErrors(7,Ntrks,missedF_II_mean,eNtrks,missedF_II_rms);
  grMissF_value_II->SetMarkerStyle(21);
  grMissF_value_II->SetMarkerColor(4);
  grMissF_value_II->SetMarkerSize(1.2);
  TGraphErrors *grGhostF_value_I = new TGraphErrors(7,Ntrks,ghostF_I_mean,eNtrks,ghostF_I_rms);
  grGhostF_value_I->SetMarkerStyle(20);
  grGhostF_value_I->SetMarkerColor(4);
  grGhostF_value_I->SetMarkerSize(1.2);
  TGraphErrors *grGhostF_value_II = new TGraphErrors(7,Ntrks,ghostF_II_mean,eNtrks,ghostF_II_rms);
  grGhostF_value_II->SetMarkerStyle(21);
  grGhostF_value_II->SetMarkerColor(4);
  grGhostF_value_II->SetMarkerSize(1.2);

  TMultiGraph *mgMissedMean = new TMultiGraph("mgMissedMean","Missed;N^{trk}_{MC};average number of missed trks");
  mgMissedMean->Add(grMissCA_value_I);
  mgMissedMean->Add(grMissCA_value_II);
  mgMissedMean->Add(grMissF_value_I);
  mgMissedMean->Add(grMissF_value_II);

  TMultiGraph *mgGhostMean = new TMultiGraph("mgGhostMean","Ghost;N^{trk}_{MC};average number of ghost trks");
  mgGhostMean->Add(grGhostCA_value_I);
  mgGhostMean->Add(grGhostCA_value_II);
  mgGhostMean->Add(grGhostF_value_I);
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

  mgSpeed->Draw("APL");
  TLatex latex;
  latex.SetTextSize(0.035);
  latex.SetTextAlign(12);  //align at center
 
  latex.DrawLatex(1.8,1.1,"#it{Cellular Automaton (Trks matching)}");
  latex.DrawLatex(1.8,1.0,"#it{Cellular Automaton (Hits matching)}");
  latex.DrawLatex(1.8,.8,"#it{Trk-Following (Trks matching)}");
  latex.DrawLatex(1.8,.7,"#it{Trk-Following (Hits matching)}");
  TMarker caI(1.2, 0.9, 20);
  caI.SetMarkerColor(2);
  caI.SetMarkerSize(1.2); 
  TMarker caII(1.2, 0.85, 21);
  caII.SetMarkerColor(2);
  caII.SetMarkerSize(1.2);
  caI.DrawMarker(1.2,1.1);
  caII.DrawMarker(1.2,1.0);

  TMarker fI(1.2, 0.9, 20);
  fI.SetMarkerColor(4);
  fI.SetMarkerSize(1.2); 
  TMarker fII(1.2, 0.85, 21);
  fII.SetMarkerColor(4);
  fII.SetMarkerSize(1.2);
  fI.DrawMarker(1.2,0.8);
  fII.DrawMarker(1.2,0.7);

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
