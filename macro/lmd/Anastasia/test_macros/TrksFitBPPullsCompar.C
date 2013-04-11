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
void TrksFitBPPullsCompar(TString pathName="/panda/pandaroot/macro/lmd/testPixel/09042013/")
{
  const int nBeamPoints=5;
  double p_beam[nBeamPoints]={1.5, 4.06, 8.9, 11.91, 15};
  // const int nBeamPoints=4;
  // double p_beam[nBeamPoints]={1.5, 4.06, 8.9, 15};
 //  const int nMethods=6;
 //  TString Method[nMethods]={"_MINUIT_BPGEANE","_MINUIT_BPRK","_KALMANGEANE_BPGEANE","_KALMANGEANE_BPRK","_KALMANRK_BPGEANE","_KALMANRK_BPRK"};
 // TString MethodName[nMethods]={"MINUIT, BP=GEANE","MINUIT, BP=RK","KALMAN=GEANE, BP=GEANE","KALMAN=GEANE, BP=RK","KALMAN=RK, BP=GEANE","KALMAN=RK, BP=RK"};
 const int nMethods=2;
  TString Method[nMethods]={"_MINUIT_BPGEANE","_MINUIT_BPRK"};
 TString MethodName[nMethods]={"MINUIT, BP=GEANE","MINUIT, BP=RK"};

// const int nMethods=4;
//   TString Method[nMethods]={"_KALMANGEANE_BPGEANE","_KALMANGEANE_BPRK","_KALMANRK_BPGEANE","_KALMANRK_BPRK"};
//  TString MethodName[nMethods]={"KALMAN=GEANE, BP=GEANE","KALMAN=GEANE, BP=RK","KALMAN=RK, BP=GEANE","KALMAN=RK, BP=RK"};


  double PullPx_mean[nMethods][nBeamPoints], PullPx_sigma[nMethods][nBeamPoints], PullPy_mean[nMethods][nBeamPoints], PullPy_sigma[nMethods][nBeamPoints], PullPz_mean[nMethods][nBeamPoints], PullPz_sigma[nMethods][nBeamPoints];
  double PullX_mean[nMethods][nBeamPoints], PullX_sigma[nMethods][nBeamPoints],PullY_mean[nMethods][nBeamPoints], PullY_sigma[nMethods][nBeamPoints],PullZ_mean[nMethods][nBeamPoints], PullZ_sigma[nMethods][nBeamPoints];
  double PullTheta_mean[nMethods][nBeamPoints], PullTheta_sigma[nMethods][nBeamPoints], PullPhi_mean[nMethods][nBeamPoints], PullPhi_sigma[nMethods][nBeamPoints];
  double PullX_mean_in[nMethods][nBeamPoints], PullX_sigma_in[nMethods][nBeamPoints],PullY_mean_in[nMethods][nBeamPoints], PullY_sigma_in[nMethods][nBeamPoints],PullZ_mean_in[nMethods][nBeamPoints], PullZ_sigma_in[nMethods][nBeamPoints];
  double PullPx_mean_in[nMethods][nBeamPoints],PullPx_sigma_in[nMethods][nBeamPoints], PullPy_mean_in[nMethods][nBeamPoints],PullPy_sigma_in[nMethods][nBeamPoints], PullPz_mean_in[nMethods][nBeamPoints],PullPz_sigma_in[nMethods][nBeamPoints];

  double erPullPx_mean[nMethods][nBeamPoints], erPullPx_sigma[nMethods][nBeamPoints], erPullPy_mean[nMethods][nBeamPoints], erPullPy_sigma[nMethods][nBeamPoints], erPullPz_mean[nMethods][nBeamPoints], erPullPz_sigma[nMethods][nBeamPoints];
  double erPullX_mean[nMethods][nBeamPoints], erPullX_sigma[nMethods][nBeamPoints], erPullY_mean[nMethods][nBeamPoints], erPullY_sigma[nMethods][nBeamPoints], erPullZ_mean[nMethods][nBeamPoints], erPullZ_sigma[nMethods][nBeamPoints];
  double erPullTheta_mean[nMethods][nBeamPoints], erPullTheta_sigma[nMethods][nBeamPoints], erPullPhi_mean[nMethods][nBeamPoints], erPullPhi_sigma[nMethods][nBeamPoints];
  double erPullX_mean_in[nMethods][nBeamPoints], erPullX_sigma_in[nMethods][nBeamPoints], erPullY_mean_in[nMethods][nBeamPoints], erPullY_sigma_in[nMethods][nBeamPoints], erPullZ_mean_in[nMethods][nBeamPoints], erPullZ_sigma_in[nMethods][nBeamPoints];
  double erPullPx_mean_in[nMethods][nBeamPoints],erPullPx_sigma_in[nMethods][nBeamPoints], erPullPy_mean_in[nMethods][nBeamPoints], erPullPy_sigma_in[nMethods][nBeamPoints], erPullPz_mean_in[nMethods][nBeamPoints], erPullPz_sigma_in[nMethods][nBeamPoints];

  for(int ip=0;ip<nBeamPoints;ip++){
    TString fileNameG = pathName+"/mom_";
    switch (ip){
    case 0: fileNameG+="1_5/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    break;
    case 1: fileNameG+="4_06/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    break;
    case 2: fileNameG+="8_9/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    break;
    case 3: fileNameG+="11_91/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    break;
    case 4: fileNameG+="15/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    break;
    default: cout << "?????";
    break;
    }
 // switch (ip){
 //    case 0: fileNameG+="1_5/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
 //    break;
 //    case 1: fileNameG+="4_06/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
 //    break;
 //    case 2: fileNameG+="8_9/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
 //    break;
 //    case 3: fileNameG+="15/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
 //    break;
 //    default: cout << "?????";
 //    break;
 //    }

  for(int im=0;im<nMethods;im++){
    TString fileName = fileNameG;
    fileName+=Method[im];
    fileName+=".root";
    TFile *fileIN = new TFile(fileName,"READ");

  ///Pull distributions
  TH1F* hPullPointPx = (TH1F*)fileIN->Get("hPullPointPx");
  TF1 *fPullPx  = new TF1("fitPPx","gaus",-10,10);
  hPullPointPx->Fit(fPullPx,"qr");
  PullPx_mean[im][ip] = fPullPx->GetParameter(1);
  erPullPx_mean[im][ip] = fPullPx->GetParError(1) ;
  PullPx_sigma[im][ip] = fPullPx->GetParameter(2);
  erPullPx_sigma[im][ip] = fPullPx->GetParError(2) ;

  TH1F* hPullPointPy = (TH1F*)fileIN->Get("hPullPointPy");
  TF1 *fPullPy = new TF1("fitPPy","gaus",-10,10);
  hPullPointPy->Fit(fPullPy,"qr");
  PullPy_mean[im][ip] = fPullPy->GetParameter(1);
  PullPy_sigma[im][ip] = fPullPy->GetParameter(2);
  erPullPy_mean[im][ip] = fPullPy->GetParError(1);
  erPullPy_sigma[im][ip] = fPullPy->GetParError(2);
  
  TH1F* hPullPointPz = (TH1F*)fileIN->Get("hPullPointPz");
  TF1 *fPullPz  = new TF1("fitPPz","gaus",-10,10);
  hPullPointPz->Fit(fPullPz,"qr");
  PullPz_mean[im][ip] = fPullPz->GetParameter(1);
  PullPz_sigma[im][ip] = fPullPz->GetParameter(2);
  erPullPz_mean[im][ip] = fPullPz->GetParError(1);
  erPullPz_sigma[im][ip] = fPullPz->GetParError(2);

  TH1F* hPullPointX = (TH1F*)fileIN->Get("hPullPointX");
  TF1 *fPullX  = new TF1("fitPX","gaus",-10,10);
  hPullPointX->Fit(fPullX,"qr");
  PullX_mean[im][ip] = fPullX->GetParameter(1);
  PullX_sigma[im][ip] = fPullX->GetParameter(2);
  erPullX_mean[im][ip] = fPullX->GetParError(1);
  erPullX_sigma[im][ip] = fPullX->GetParError(2);

  TH1F* hPullPointY = (TH1F*)fileIN->Get("hPullPointY");
  TF1 *fPullY  = new TF1("fitPY","gaus",-10,10);
  hPullPointY->Fit(fPullY,"qr");
  PullY_mean[im][ip] = fPullY->GetParameter(1);
  PullY_sigma[im][ip] = fPullY->GetParameter(2);
  erPullY_mean[im][ip] = fPullY->GetParError(1);
  erPullY_sigma[im][ip] = fPullY->GetParError(2);
  
  TH1F* hPullPointZ = (TH1F*)fileIN->Get("hPullPointZ");
  TF1 *fPullZ  = new TF1("fitPZ","gaus",-100,100);
  hPullPointZ->Fit(fPullZ,"qr");
  PullZ_mean[im][ip] = fPullZ->GetParameter(1);
  PullZ_sigma[im][ip] = fPullZ->GetParameter(2);
  erPullZ_mean[im][ip] = fPullZ->GetParError(1);
  erPullZ_sigma[im][ip] = fPullZ->GetParError(2);

  TH1F* hPullTheta = (TH1F*)fileIN->Get("hPullTheta");
  TF1 *fPullTheta = new TF1("fitpth","gaus",-10,10);
  hPullTheta->Fit(fPullTheta,"qr");
  PullTheta_mean[im][ip] = fPullTheta->GetParameter(1);
  PullTheta_sigma[im][ip] = fPullTheta->GetParameter(2);
  erPullTheta_mean[im][ip] = fPullTheta->GetParError(1);
  erPullTheta_sigma[im][ip] = fPullTheta->GetParError(2);

  TH1F* hPullPhi = (TH1F*)fileIN->Get("hPullPhi");
  TF1 *fPullPhi = new TF1("fitpphi","gaus",-10,10);
  hPullPhi->Fit(fPullPhi,"qr");
  PullPhi_mean[im][ip] = fPullPhi->GetParameter(1);
  PullPhi_sigma[im][ip] = fPullPhi->GetParameter(2);
  erPullPhi_mean[im][ip] = fPullPhi->GetParError(1);
  erPullPhi_sigma[im][ip] = fPullPhi->GetParError(2);

  TH1F *hResLumiTrkPointXPull = (TH1F*)fileIN->Get("hResLumiTrkPointXPull");
  hResLumiTrkPointXPull->Fit(fPullX,"qr");
  PullX_mean_in[im][ip] = fPullX->GetParameter(1);
  PullX_sigma_in[im][ip] = fPullX->GetParameter(2);
  erPullX_mean_in[im][ip] = fPullX->GetParError(1);
  erPullX_sigma_in[im][ip] = fPullX->GetParError(2);
  TH1F *hResLumiTrkPointYPull = (TH1F*)fileIN->Get("hResLumiTrkPointYPull");
  hResLumiTrkPointYPull->Fit(fPullY,"qr");
  PullY_mean_in[im][ip] = fPullY->GetParameter(1);
  PullY_sigma_in[im][ip] = fPullY->GetParameter(2);
  erPullY_mean_in[im][ip] = fPullY->GetParError(1);
  erPullY_sigma_in[im][ip] = fPullY->GetParError(2);
  TH1F *hResLumiTrkPointZPull = (TH1F*)fileIN->Get("hResLumiTrkPointZPull");
  hResLumiTrkPointZPull->Fit(fPullZ,"qr");
  PullZ_mean_in[im][ip]=-1000;
  PullZ_sigma_in[im][ip]=-1000;
  if(hResLumiTrkPointZPull->GetRMS()>1e-6){
    PullZ_mean_in[im][ip] = fPullZ->GetParameter(1);
    PullZ_sigma_in[im][ip] = fPullZ->GetParameter(2);
    erPullZ_mean_in[im][ip] = fPullZ->GetParError(1);
    erPullZ_sigma_in[im][ip] = fPullZ->GetParError(2);
  }


  TH1F *hResLumiTrkPointPxPull = (TH1F*)fileIN->Get("hResLumiTrkPointPxPull");
  hResLumiTrkPointPxPull->Fit(fPullPx,"qr");
  PullPx_mean_in[im][ip] = fPullPx->GetParameter(1);
  PullPx_sigma_in[im][ip] = fPullPx->GetParameter(2);
  erPullPx_mean_in[im][ip] = fPullPx->GetParError(1);
  erPullPx_sigma_in[im][ip] = fPullPx->GetParError(2);

  TH1F *hResLumiTrkPointPyPull = (TH1F*)fileIN->Get("hResLumiTrkPointPyPull");
  hResLumiTrkPointPyPull->Fit(fPullPy,"qr");
  PullPy_mean_in[im][ip] = fPullPy->GetParameter(1);
  PullPy_sigma_in[im][ip] = fPullPy->GetParameter(2);
  erPullPy_mean_in[im][ip] = fPullPy->GetParError(1);
  erPullPy_sigma_in[im][ip] = fPullPy->GetParError(2);

  TH1F *hResLumiTrkPointPzPull = (TH1F*)fileIN->Get("hResLumiTrkPointPzPull");
  hResLumiTrkPointPzPull->Fit(fPullPz,"qr");
  PullPz_mean_in[im][ip] = fPullPz->GetParameter(1);
  PullPz_sigma_in[im][ip] = fPullPz->GetParameter(2);
  erPullPz_mean_in[im][ip] = fPullPz->GetParError(1);
  erPullPz_sigma_in[im][ip] = fPullPz->GetParError(2);

  //	TGraphErrors(Int_t n, const Double_t* x, const Double_t* y, const Double_t* ex = 0, const Double_t* ey = 0)


  }//methods
  }//pbeam

  //Creat and fill graphs with pbeam dependence
  TGraphErrors *gPullPx_mean[nMethods], *gPullPx_sigma[nMethods], *gPullPy_mean[nMethods], *gPullPy_sigma[nMethods], *gPullPz_mean[nMethods], *gPullPz_sigma[nMethods];
  TGraphErrors *gPullX_mean[nMethods], *gPullX_sigma[nMethods], *gPullY_mean[nMethods], *gPullY_sigma[nMethods], *gPullZ_mean[nMethods], *gPullZ_sigma[nMethods];
  TGraphErrors *gPullTheta_mean[nMethods], *gPullTheta_sigma[nMethods], *gPullPhi_mean[nMethods], *gPullPhi_sigma[nMethods];
  TGraphErrors *gPullX_mean_in[nMethods], *gPullX_sigma_in[nMethods], *gPullY_mean_in[nMethods], *gPullY_sigma_in[nMethods], *gPullZ_mean_in[nMethods], *gPullZ_sigma_in[nMethods];
  TGraphErrors *gPullPx_mean_in[nMethods], *gPullPx_sigma_in[nMethods], *gPullPy_mean_in[nMethods], *gPullPy_sigma_in[nMethods], *gPullPz_mean_in[nMethods], *gPullPz_sigma_in[nMethods];

  TMultiGraph *mgPullPx_mean = new TMultiGraph(); TMultiGraph *mgPullPx_sigma = new TMultiGraph();
  TMultiGraph *mgPullPy_mean = new TMultiGraph(); TMultiGraph *mgPullPy_sigma = new TMultiGraph();
  TMultiGraph *mgPullPz_mean = new TMultiGraph(); TMultiGraph *mgPullPz_sigma = new TMultiGraph();
  TMultiGraph *mgPullX_mean = new TMultiGraph(); TMultiGraph *mgPullX_sigma = new TMultiGraph();
  TMultiGraph *mgPullY_mean = new TMultiGraph(); TMultiGraph *mgPullY_sigma = new TMultiGraph();
  TMultiGraph *mgPullZ_mean = new TMultiGraph(); TMultiGraph *mgPullZ_sigma = new TMultiGraph();
  TMultiGraph *mgPullTheta_mean = new TMultiGraph(); TMultiGraph *mgPullTheta_sigma = new TMultiGraph();
  TMultiGraph *mgPullPhi_mean = new TMultiGraph(); TMultiGraph *mgPullPhi_sigma = new TMultiGraph();
  TMultiGraph *mgPullX_mean_in = new TMultiGraph(); TMultiGraph *mgPullX_sigma_in = new TMultiGraph();
  TMultiGraph *mgPullY_mean_in = new TMultiGraph(); TMultiGraph *mgPullY_sigma_in = new TMultiGraph();
  TMultiGraph *mgPullZ_mean_in = new TMultiGraph(); TMultiGraph *mgPullZ_sigma_in = new TMultiGraph();
  TMultiGraph *mgPullPx_mean_in = new TMultiGraph(); TMultiGraph *mgPullPx_sigma_in = new TMultiGraph();
  TMultiGraph *mgPullPy_mean_in = new TMultiGraph(); TMultiGraph *mgPullPy_sigma_in = new TMultiGraph();
  TMultiGraph *mgPullPz_mean_in = new TMultiGraph(); TMultiGraph *mgPullPz_sigma_in = new TMultiGraph();
  for(int im=0;im<nMethods;im++){
    gPullPx_mean[im] = new TGraphErrors(nBeamPoints,p_beam,PullPx_mean[im],0,erPullPx_mean[im]);
    gPullPx_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,PullPx_sigma[im],0,erPullPx_sigma[im]);
    gPullPx_mean[im]->SetMarkerStyle(20+im);
    gPullPx_sigma[im]->SetMarkerStyle(20+im);
    gPullPx_mean[im]->SetMarkerColor(im+1);
    gPullPx_sigma[im]->SetMarkerColor(im+1);
    gPullPx_mean[im]->SetMarkerSize(2.0);
    gPullPx_sigma[im]->SetMarkerSize(2.0);
    mgPullPx_mean->Add(gPullPx_mean[im],"LP");
    mgPullPx_sigma->Add(gPullPx_sigma[im],"LP");
  

    gPullPy_mean[im] = new TGraphErrors(nBeamPoints,p_beam,PullPy_mean[im],0,erPullPy_mean[im]);
    gPullPy_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,PullPy_sigma[im],0,erPullPy_sigma[im]);
    gPullPy_mean[im]->SetMarkerStyle(20+im);
    gPullPy_sigma[im]->SetMarkerStyle(20+im);
    gPullPy_mean[im]->SetMarkerColor(im+1);
    gPullPy_sigma[im]->SetMarkerColor(im+1);
    gPullPy_mean[im]->SetMarkerSize(2.0);
    gPullPy_sigma[im]->SetMarkerSize(2.0);
    mgPullPy_mean->Add(gPullPy_mean[im],"LP");
    mgPullPy_sigma->Add(gPullPy_sigma[im],"LP");
   

    gPullPz_mean[im] = new TGraphErrors(nBeamPoints,p_beam,PullPz_mean[im],0,erPullPz_mean[im]);
    gPullPz_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,PullPz_sigma[im],0,erPullPz_sigma[im]);
    gPullPz_mean[im]->SetMarkerStyle(20+im);
    gPullPz_sigma[im]->SetMarkerStyle(20+im);
    gPullPz_mean[im]->SetMarkerColor(im+1);
    gPullPz_sigma[im]->SetMarkerColor(im+1);
    gPullPz_mean[im]->SetMarkerSize(2.0);
    gPullPz_sigma[im]->SetMarkerSize(2.0);
    mgPullPz_mean->Add(gPullPz_mean[im],"LP");
    mgPullPz_sigma->Add(gPullPz_sigma[im],"LP");
  

    gPullX_mean[im] = new TGraphErrors(nBeamPoints,p_beam,PullX_mean[im],0,erPullX_mean[im]);
    gPullX_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,PullX_sigma[im],0,erPullX_sigma[im]);
    gPullX_mean[im]->SetMarkerStyle(20+im);
    gPullX_sigma[im]->SetMarkerStyle(20+im);
    gPullX_mean[im]->SetMarkerColor(im+1);
    gPullX_sigma[im]->SetMarkerColor(im+1);
    gPullX_mean[im]->SetMarkerSize(2.0);
    gPullX_sigma[im]->SetMarkerSize(2.0);
    mgPullX_mean->Add(gPullX_mean[im],"LP");
    mgPullX_sigma->Add(gPullX_sigma[im],"LP");
  
    gPullY_mean[im] = new TGraphErrors(nBeamPoints,p_beam,PullY_mean[im],0,erPullY_mean[im]);
    gPullY_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,PullY_sigma[im],0,erPullY_sigma[im]);
    gPullY_mean[im]->SetMarkerStyle(20+im);
    gPullY_sigma[im]->SetMarkerStyle(20+im);
    gPullY_mean[im]->SetMarkerColor(im+1);
    gPullY_sigma[im]->SetMarkerColor(im+1);
    gPullY_mean[im]->SetMarkerSize(2.0);
    gPullY_sigma[im]->SetMarkerSize(2.0);
    mgPullY_mean->Add(gPullY_mean[im],"LP");
    mgPullY_sigma->Add(gPullY_sigma[im],"LP");
  

    gPullZ_mean[im] = new TGraphErrors(nBeamPoints,p_beam,PullZ_mean[im],0,erPullZ_mean[im]);
    gPullZ_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,PullZ_sigma[im],0,erPullZ_sigma[im]);
    gPullZ_mean[im]->SetMarkerStyle(20+im);
    gPullZ_sigma[im]->SetMarkerStyle(20+im);
    gPullZ_mean[im]->SetMarkerColor(im+1);
    gPullZ_sigma[im]->SetMarkerColor(im+1);
    gPullZ_mean[im]->SetMarkerSize(2.0);
    gPullZ_sigma[im]->SetMarkerSize(2.0);
    mgPullZ_mean->Add(gPullZ_mean[im],"LP");
    mgPullZ_sigma->Add(gPullZ_sigma[im],"LP");
  

    gPullTheta_mean[im] = new TGraphErrors(nBeamPoints,p_beam,PullTheta_mean[im],0,erPullTheta_mean[im]);
    gPullTheta_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,PullTheta_sigma[im],0,erPullTheta_sigma[im]);
    gPullTheta_mean[im]->SetMarkerStyle(20+im);
    gPullTheta_sigma[im]->SetMarkerStyle(20+im);
    gPullTheta_mean[im]->SetMarkerColor(im+1);
    gPullTheta_sigma[im]->SetMarkerColor(im+1);
    gPullTheta_mean[im]->SetMarkerSize(2.0);
    gPullTheta_sigma[im]->SetMarkerSize(2.0);
    mgPullTheta_mean->Add(gPullTheta_mean[im],"LP");
    mgPullTheta_sigma->Add(gPullTheta_sigma[im],"LP");
    

    gPullPhi_mean[im] = new TGraphErrors(nBeamPoints,p_beam,PullPhi_mean[im],0,erPullPhi_mean[im]);
    gPullPhi_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,PullPhi_sigma[im],0,erPullPhi_sigma[im]);
    gPullPhi_mean[im]->SetMarkerStyle(20+im);
    gPullPhi_sigma[im]->SetMarkerStyle(20+im);
    gPullPhi_mean[im]->SetMarkerColor(im+1);
    gPullPhi_sigma[im]->SetMarkerColor(im+1);
    gPullPhi_mean[im]->SetMarkerSize(2.0);
    gPullPhi_sigma[im]->SetMarkerSize(2.0);
    mgPullPhi_mean->Add(gPullPhi_mean[im],"LP");
    mgPullPhi_sigma->Add(gPullPhi_sigma[im],"LP");

    gPullX_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullX_mean_in[im],0,erPullX_mean_in[im]);
    gPullX_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullX_sigma_in[im],0,erPullX_sigma_in[im]);
    gPullX_mean_in[im]->SetMarkerStyle(20+im);
    gPullX_sigma_in[im]->SetMarkerStyle(20+im);
    gPullX_mean_in[im]->SetMarkerColor(im+1);
    gPullX_sigma_in[im]->SetMarkerColor(im+1);
    gPullX_mean_in[im]->SetMarkerSize(2.0);
    gPullX_sigma_in[im]->SetMarkerSize(2.0); 
    mgPullX_mean_in->Add(gPullX_mean_in[im],"LP");
    mgPullX_sigma_in->Add(gPullX_sigma_in[im],"LP");
   

    gPullY_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullY_mean_in[im],0,erPullY_mean_in[im]);
    gPullY_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullY_sigma_in[im],0,erPullY_sigma_in[im]);
    gPullY_mean_in[im]->SetMarkerStyle(20+im);
    gPullY_sigma_in[im]->SetMarkerStyle(20+im);
    gPullY_mean_in[im]->SetMarkerColor(im+1);
    gPullY_sigma_in[im]->SetMarkerColor(im+1);
    gPullY_mean_in[im]->SetMarkerSize(2.0);
    gPullY_sigma_in[im]->SetMarkerSize(2.0);
    mgPullY_mean_in->Add(gPullY_mean_in[im],"LP");
    mgPullY_sigma_in->Add(gPullY_sigma_in[im],"LP");
   
    gPullZ_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullZ_mean_in[im],0,erPullZ_mean_in[im]);
    gPullZ_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullZ_sigma_in[im],0,erPullZ_sigma_in[im]);
    gPullZ_mean_in[im]->SetMarkerStyle(20+im);
    gPullZ_sigma_in[im]->SetMarkerStyle(20+im);
    gPullZ_mean_in[im]->SetMarkerColor(im+1);
    gPullZ_sigma_in[im]->SetMarkerColor(im+1);
    gPullZ_mean_in[im]->SetMarkerSize(2.0);
    gPullZ_sigma_in[im]->SetMarkerSize(2.0);
    mgPullZ_mean_in->Add(gPullZ_mean_in[im],"LP");
    mgPullZ_sigma_in->Add(gPullZ_sigma_in[im],"LP");
  
    gPullPx_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPx_mean_in[im],0,erPullPx_mean_in[im]);
    gPullPx_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPx_sigma_in[im],0,erPullPx_sigma_in[im]);
    gPullPx_mean_in[im]->SetMarkerStyle(20+im);
    gPullPx_sigma_in[im]->SetMarkerStyle(20+im);
    gPullPx_mean_in[im]->SetMarkerColor(im+1);
    gPullPx_sigma_in[im]->SetMarkerColor(im+1);
    gPullPx_mean_in[im]->SetMarkerSize(2.0);
    gPullPx_sigma_in[im]->SetMarkerSize(2.0);
    mgPullPx_mean_in->Add(gPullPx_mean_in[im],"LP");
    mgPullPx_sigma_in->Add(gPullPx_sigma_in[im],"LP");
    
    gPullPy_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPy_mean_in[im],0,erPullPy_mean_in[im]);
    gPullPy_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPy_sigma_in[im],0,erPullPy_sigma_in[im]);
    gPullPy_mean_in[im]->SetMarkerStyle(20+im);
    gPullPy_sigma_in[im]->SetMarkerStyle(20+im);
    gPullPy_mean_in[im]->SetMarkerColor(im+1);
    gPullPy_sigma_in[im]->SetMarkerColor(im+1);
    gPullPy_mean_in[im]->SetMarkerSize(2.0);
    gPullPy_sigma_in[im]->SetMarkerSize(2.0);
    mgPullPy_mean_in->Add(gPullPy_mean_in[im],"LP");
    mgPullPy_sigma_in->Add(gPullPy_sigma_in[im],"LP");
   
    gPullPz_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPz_mean_in[im],0,erPullPz_mean_in[im]);
    gPullPz_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPz_sigma_in[im],0,erPullPz_sigma_in[im]);
    gPullPz_mean_in[im]->SetMarkerStyle(20+im);
    gPullPz_sigma_in[im]->SetMarkerStyle(20+im);
    gPullPz_mean_in[im]->SetMarkerColor(im+1);
    gPullPz_sigma_in[im]->SetMarkerColor(im+1);
    gPullPz_mean_in[im]->SetMarkerSize(2.0);
    gPullPz_sigma_in[im]->SetMarkerSize(2.0);
    mgPullPz_mean_in->Add(gPullPz_mean_in[im],"LP");
    mgPullPz_sigma_in->Add(gPullPz_sigma_in[im],"LP");
  }

  //Draw & Save results
  TString resname= pathName+"/TrkFit_BackPropag_diffMethods_Results";
  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";
 
  TLegend *leg = new TLegend(0.7,0.75,0.98,0.95);
  leg->SetFillColor(0);
  //   leg->SetHeader("The Legend Title");
  for(int im=0;im<nMethods;im++){
    leg->AddEntry(gPullPx_mean[im],MethodName[im],"lep");
  }
  TLine *perfectmeanval = new TLine(0.75,0,15.6,0);
  perfectmeanval->SetLineColor(46);
  perfectmeanval->SetLineStyle(2);
  perfectmeanval->SetLineWidth(2);
  TLine *perfectsigmaval = new TLine(0.75,1,15.6,1);
  perfectsigmaval->SetLineColor(46);
  perfectsigmaval->SetLineStyle(2);
  perfectsigmaval->SetLineWidth(2);

  TCanvas c1;
  mgPullPx_mean->Draw("AP");  
  mgPullPx_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPx_mean->GetYaxis()->SetTitle("(P^{REC}_{x} - P^{MC}_{x})/#sigma_{Px}, mean [PCA]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPx_sigma->Draw("AP");
  mgPullPx_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPx_sigma->GetYaxis()->SetTitle("(P^{REC}_{x} - P^{MC}_{x})/#sigma_{Px}, sigma [PCA]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
 mgPullPy_mean->Draw("AP");  
 mgPullPy_mean->GetXaxis()->SetTitle("momentum, GeV/c");
 mgPullPy_mean->GetYaxis()->SetTitle("(P^{REC}_{y} - P^{MC}_{y})/#sigma_{Py}, mean [PCA]");
 leg->Draw();
 perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPy_sigma->Draw("AP");
  mgPullPy_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPy_sigma->GetYaxis()->SetTitle("(P^{REC}_{y} - P^{MC}_{y})/#sigma_{Py}, sigma [PCA]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPz_mean->Draw("AP");  
  mgPullPz_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPz_mean->GetYaxis()->SetTitle("(P^{REC}_{z} - P^{MC}_{z})/#sigma_{Pz}, mean [PCA]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPz_sigma->Draw("AP");
  mgPullPz_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPz_sigma->GetYaxis()->SetTitle("(P^{REC}_{z} - P^{MC}_{z})/#sigma_{Pz}, sigma [PCA]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullX_mean->Draw("AP");  
  mgPullX_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullX_mean->GetYaxis()->SetTitle("(X^{REC} - X^{MC})/#sigma_{X}, mean [PCA]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullX_sigma->Draw("AP");
  mgPullX_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullX_sigma->GetYaxis()->SetTitle("(X^{REC} - X^{MC})/#sigma_{X}, sigma [PCA]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullY_mean->Draw("AP");  
  mgPullY_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullY_mean->GetYaxis()->SetTitle("(Y^{REC} - Y^{MC})/#sigma_{Y}, mean [PCA]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullY_sigma->Draw("AP");
  mgPullY_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullY_sigma->GetYaxis()->SetTitle("(Y^{REC} - Y^{MC})/#sigma_{Y}, sigma [PCA]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullZ_mean->Draw("AP");  
  mgPullZ_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullZ_mean->GetYaxis()->SetTitle("(Z^{REC} - Z^{MC})/#sigma_{Z}, mean [PCA]");
  // leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullZ_sigma->Draw("AP");
  mgPullZ_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullZ_sigma->GetYaxis()->SetTitle("(Z^{REC} - Z^{MC})/#sigma_{Z}, sigma [PCA]");
  perfectsigmaval->Draw();
  leg->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullTheta_mean->Draw("AP");  
  mgPullTheta_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullTheta_mean->GetYaxis()->SetTitle("(#theta^{REC} - #theta^{MC})/#sigma_{#theta}, mean [PCA]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullTheta_sigma->Draw("AP");
  mgPullTheta_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullTheta_sigma->GetYaxis()->SetTitle("(#theta^{REC} - #theta^{MC})/#sigma_{#theta}, sigma [PCA]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPhi_mean->Draw("AP");  
 mgPullPhi_mean->GetXaxis()->SetTitle("momentum, GeV/c");
 mgPullPhi_mean->GetYaxis()->SetTitle("(#phi^{REC} - #phi^{MC})/#sigma_{#phi}, mean [PCA]");
 leg->Draw();
 perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPhi_sigma->Draw("AP");
 mgPullPhi_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
 mgPullPhi_sigma->GetYaxis()->SetTitle("(#phi^{REC} - #phi^{MC})/#sigma_{#phi}, sigma [PCA]");
 leg->Draw();
 perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullX_mean_in->Draw("AP");  
  mgPullX_mean_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullX_mean_in->GetYaxis()->SetTitle("(X^{REC}- X^{MC})/#sigma_{X}, mean [LMD]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullX_sigma_in->Draw("AP");
  mgPullX_sigma_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullX_sigma_in->GetYaxis()->SetTitle("(X^{REC} - X^{MC})/#sigma_{X}, sigma [LMD]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullY_mean_in->Draw("AP");  
  mgPullY_mean_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullY_mean_in->GetYaxis()->SetTitle("(Y^{REC} - Y^{MC})/#sigma_{Y}, mean [LMD]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullY_sigma_in->Draw("AP");
  mgPullY_sigma_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullY_sigma_in->GetYaxis()->SetTitle("(Y^{REC} - Y^{MC})/#sigma_{Y}, sigma [LMD]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  // mgPullZ_mean_in->Draw("AP");  
  // mgPullZ_mean_in->GetXaxis()->SetTitle("momentum, GeV/c");
  // mgPullZ_mean_in->GetYaxis()->SetTitle("(Z^{REC} - Z^{MC})/#sigma_{Z}, mean [LMD]");
  // leg->Draw();
  // c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  // c1.Clear();
  // mgPullZ_sigma_in->Draw("AP");
  // mgPullZ_sigma_in->GetXaxis()->SetTitle("momentum, GeV/c");
  // mgPullZ_sigma_in->GetYaxis()->SetTitle("(Z^{REC} - Z^{MC})/#sigma_{Z}, sigma [LMD]");
  // leg->Draw();
  // c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  // c1.Clear();
  mgPullPx_mean_in->Draw("AP");  
  mgPullPx_mean_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPx_mean_in->GetYaxis()->SetTitle("(P^{REC}_{x} - P^{MC}_{x})/#sigma_{Px}, mean [LMD]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPx_sigma_in->Draw("AP");
  mgPullPx_sigma_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPx_sigma_in->GetYaxis()->SetTitle("(P^{REC}_{x} - P^{MC}_{x})/#sigma_{Px}, sigma [LMD]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPy_mean_in->Draw("AP");  
  mgPullPy_mean_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPy_mean_in->GetYaxis()->SetTitle("(P^{REC}_{y} - P^{MC}_{y})/#sigma_{Py}, mean [LMD]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPy_sigma_in->Draw("AP");
  mgPullPy_sigma_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPy_sigma_in->GetYaxis()->SetTitle("(P^{REC}_{y} - P^{MC}_{y})/#sigma_{Py}, sigma [LMD]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPz_mean_in->Draw("AP");  
  mgPullPz_mean_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPz_mean_in->GetYaxis()->SetTitle("(P^{REC}_{z} - P^{MC}_{z})/#sigma_{Pz}, mean [LMD]");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPz_sigma_in->Draw("AP");
  mgPullPz_sigma_in->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPz_sigma_in->GetYaxis()->SetTitle("(P^{REC}_{z} - P^{MC}_{z})/#sigma_{Pz}, sigma [LMD]");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_c); //write canvas and close the pdf file open
  c1.Clear();


 TString resname_root = resname+".root";
  TFile *fout = new TFile(resname_root,"RECREATE");
  mgPullPx_sigma->Write();
  mgPullPy_mean->Write();  
  mgPullPy_sigma->Write();
  mgPullPz_mean->Write();  
  mgPullPz_sigma->Write();
  mgPullX_mean->Write();  
  mgPullX_sigma->Write();
  mgPullY_mean->Write();  
  mgPullY_sigma->Write();
  mgPullZ_mean->Write();  
  mgPullZ_sigma->Write();
  mgPullTheta_mean->Write();  
  mgPullTheta_sigma->Write();
  mgPullPhi_mean->Write();  
  mgPullPhi_sigma->Write();
  mgPullX_mean_in->Write();  
  mgPullX_sigma_in->Write();
  mgPullY_mean_in->Write();  
  mgPullY_sigma_in->Write();
  mgPullZ_mean_in->Write();  
  mgPullZ_sigma_in->Write();
  mgPullPx_mean_in->Write();  
  mgPullPx_sigma_in->Write();
  mgPullPy_mean_in->Write();  
  mgPullPy_sigma_in->Write();
  mgPullPz_mean_in->Write();  
  mgPullPz_sigma_in->Write();
  fout->Write();
}
