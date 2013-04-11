#include "TString.h"
#include "TStopwatch.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
using namespace std;
void MisAlignmentResults()
{
 
  double dr[9]={0,5,10,20,50,100,200,500,1000};
  double da[4]={0,0.5,1,10};
  double meanTheta[9][4];
  double resTheta[9][4];
  TF1 *funrth = new TF1("fitrth","gaus",-0.01,0.01);
  funrth->SetParameters(100,0,3e-3);
  funrth->SetParNames("Constant","Mean","Sigma");
  TH1F *hresTheta[36];
  for(int i=0;i<9;i++){
    for(int j=0;j<4;j++){
      TString File = "tmpOutputAlign/results/Lumi_out_MC_and_REC_trks_matches_with_IDs_";
      File += dr[i];
      File += "_";
      File += da[j];
      File += ".root";
      TFile *f = new TFile(File,"READ");
      hresTheta[9*j+i] = (TH1F*)f->Get("hResTheta");
      TString htitle = "#delta_{r} = ";
      htitle += dr[i];
      htitle += ",#mu m #delta_{a} = ";
      htitle += da[j];
      htitle += ",mrad";
      hresTheta[9*j+i]->SetTitle(htitle);
      hresTheta[9*j+i]->Fit(funrth,"r");
      double meanTh = funrth->GetParameter("Mean");
      meanTheta[i][j] = meanTh;
      double sigTh  = funrth->GetParameter("Sigma");
      resTheta[i][j] = sigTh;
    }
  }
  TFile *fout = new TFile("MisAlignmentResults.root","RECREATE");
  TMultiGraph *mgM = new TMultiGraph();
  TMultiGraph *mgS = new TMultiGraph();
  TMultiGraph *mgM_1 = new TMultiGraph();
  TMultiGraph *mgS_1 = new TMultiGraph();
  TGraph *grM[4];
  TGraph *grS[4];
  TGraph *grM_1[4];
  TGraph *grS_1[4];
  for(int j=0;j<4;j++){
    double x[9];
    double yM[9];
    double yS[9];
    for(int i=0;i<9;i++){
      x[i]=dr[i];
      yM[i]=meanTheta[i][j];
      yS[i]=resTheta[i][j];
    }
    grM[j] = new TGraph(9,x,yM);
    grM[j]->SetMarkerStyle(21);
    grM[j]->SetMarkerColor(j+1);
    mgM->Add(grM[j]);
    grM_1[j] = new TGraph(7,x,yM);
    grM_1[j]->SetMarkerStyle(21);
    grM_1[j]->SetMarkerColor(j+1);
    if(j<3) mgM_1->Add(grM_1[j]);

    grS[j] = new TGraph(9,x,yS);
    grS[j]->SetMarkerStyle(21);
    grS[j]->SetMarkerColor(j+1);
    mgS->Add(grS[j]);
    grS_1[j] = new TGraph(7,x,yS);
    grS_1[j]->SetMarkerStyle(21);
    grS_1[j]->SetMarkerColor(j+1);
    if(j<3)  mgS_1->Add(grS_1[j]);
  }
  
  //  TCanvas *c1 = new TCanvas("Theta","Theta",900,900);
  TCanvas *c1 = new TCanvas("ResultOverview","ResultOverview");
  c1->Divide(2,3);
  c1->cd(1);
  mgM->Draw("AP");
  mgM->GetXaxis()->SetTitle("#delta_{r}, #mu m");
  mgM->GetYaxis()->SetTitle("#theta_{mean}, rad");
  c1->cd(2);
  mgS->Draw("AP");
  mgS->GetXaxis()->SetTitle("#delta_{r}, #mu m");
  mgS->GetYaxis()->SetTitle("#theta_{sigma}, rad");
  c1->cd(3);
  mgM_1->Draw("AP");
  mgM_1->GetXaxis()->SetTitle("#delta_{r}, #mu m");
  mgM_1->GetYaxis()->SetTitle("#theta_{mean}, rad");
  c1->cd(4);
  mgS_1->Draw("AP");
  mgS_1->GetXaxis()->SetTitle("#delta_{r}, #mu m");
  mgS_1->GetYaxis()->SetTitle("#theta_{sigma}, rad");
  c1->cd(5);
  grM_1[0]->Draw("AP");
  c1->cd(6);
  grS_1[0]->Draw("AP");
  c1->Write();

  TCanvas *c2 = new TCanvas("Theta1","Theta #delta_{a} = 0");
  c2->Divide(3,3);
  for(int h=0;h<9;h++){
    c2->cd(h+1);
    hresTheta[h]->Draw();
  }
  c2->Write();

  TCanvas *c3 = new TCanvas("Theta2","Theta #delta_{a} = 0.5");
  c3->Divide(3,3);
  for(int h=0;h<9;h++){
    c3->cd(h+1);
    hresTheta[h+9]->Draw();
  }
  c3->Write();
  
  TCanvas *c4 = new TCanvas("Theta3","Theta #delta_{a} = 1");
  c4->Divide(3,3);
  for(int h=0;h<9;h++){
    c4->cd(h+1);
    hresTheta[h+18]->Draw();
  }
  c4->Write();

  TCanvas *c5 = new TCanvas("Theta4","Theta #delta_{a} = 10");
  c5->Divide(3,3);
  for(int h=0;h<9;h++){
    c5->cd(h+1);
    hresTheta[h+3*9]->Draw();
  }
  c5->Write();

  fout->Write();
}
