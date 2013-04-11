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
void TrksFitBPPullsCompar_alltogether(TString pathName="/panda/pandaroot/macro/lmd/testPixel/09042013/")
{
  // const int nBeamPoints=5;
  // double p_beam[nBeamPoints]={1.5, 4.06, 8.9, 11.91, 15};
  const int nBeamPoints=2;
  double p_beam[nBeamPoints]={1.5, 15};
 //  const int nMethods=6;
 //  TString Method[nMethods]={"_MINUIT_BPGEANE","_MINUIT_BPRK","_KALMANGEANE_BPGEANE","_KALMANGEANE_BPRK","_KALMANRK_BPGEANE","_KALMANRK_BPRK"};
 // TString MethodName[nMethods]={"MINUIT, BP=GEANE","MINUIT, BP=RK","KALMAN=GEANE, BP=GEANE","KALMAN=GEANE, BP=RK","KALMAN=RK, BP=GEANE","KALMAN=RK, BP=RK"};
 // const int nMethods=2;
 //  TString Method[nMethods]={"_MINUIT_BPGEANE","_MINUIT_BPRK"};
 // TString MethodName[nMethods]={"MINUIT, BP=GEANE","MINUIT, BP=RK"};

const int nMethods=4;
 TString Method[nMethods]={"_KALMANGEANE_BPGEANE","_KALMANGEANE_BPRK","_KALMANRK_BPGEANE","_KALMANRK_BPRK"};
 TString MethodName[nMethods]={"KALMAN=GEANE, BP=GEANE","KALMAN=GEANE, BP=RK","KALMAN=RK, BP=GEANE","KALMAN=RK, BP=RK"};


  double PullPx_mean[nMethods][nBeamPoints], PullPx_sigma[nMethods][nBeamPoints], PullPy_mean[nMethods][nBeamPoints], PullPy_sigma[nMethods][nBeamPoints], PullPz_mean[nMethods][nBeamPoints], PullPz_sigma[nMethods][nBeamPoints];
  double PullX_mean[nMethods][nBeamPoints], PullX_sigma[nMethods][nBeamPoints],PullY_mean[nMethods][nBeamPoints], PullY_sigma[nMethods][nBeamPoints],PullZ_mean[nMethods][nBeamPoints], PullZ_sigma[nMethods][nBeamPoints];
  double PullTheta_mean[nMethods][nBeamPoints], PullTheta_sigma[nMethods][nBeamPoints], PullPhi_mean[nMethods][nBeamPoints], PullPhi_sigma[nMethods][nBeamPoints];
  double PullX_mean_in[nMethods][nBeamPoints], PullX_sigma_in[nMethods][nBeamPoints],PullY_mean_in[nMethods][nBeamPoints], PullY_sigma_in[nMethods][nBeamPoints],PullZ_mean_in[nMethods][nBeamPoints], PullZ_sigma_in[nMethods][nBeamPoints];
  double PullPx_mean_in[nMethods][nBeamPoints],PullPx_sigma_in[nMethods][nBeamPoints], PullPy_mean_in[nMethods][nBeamPoints],PullPy_sigma_in[nMethods][nBeamPoints], PullPz_mean_in[nMethods][nBeamPoints],PullPz_sigma_in[nMethods][nBeamPoints];
  double PullTheta_mean_in[nMethods][nBeamPoints], PullTheta_sigma_in[nMethods][nBeamPoints], PullPhi_mean_in[nMethods][nBeamPoints], PullPhi_sigma_in[nMethods][nBeamPoints];

  double erPullPx_mean[nMethods][nBeamPoints], erPullPx_sigma[nMethods][nBeamPoints], erPullPy_mean[nMethods][nBeamPoints], erPullPy_sigma[nMethods][nBeamPoints], erPullPz_mean[nMethods][nBeamPoints], erPullPz_sigma[nMethods][nBeamPoints];
  double erPullX_mean[nMethods][nBeamPoints], erPullX_sigma[nMethods][nBeamPoints], erPullY_mean[nMethods][nBeamPoints], erPullY_sigma[nMethods][nBeamPoints], erPullZ_mean[nMethods][nBeamPoints], erPullZ_sigma[nMethods][nBeamPoints];
  double erPullTheta_mean[nMethods][nBeamPoints], erPullTheta_sigma[nMethods][nBeamPoints], erPullPhi_mean[nMethods][nBeamPoints], erPullPhi_sigma[nMethods][nBeamPoints];
  double erPullX_mean_in[nMethods][nBeamPoints], erPullX_sigma_in[nMethods][nBeamPoints], erPullY_mean_in[nMethods][nBeamPoints], erPullY_sigma_in[nMethods][nBeamPoints], erPullZ_mean_in[nMethods][nBeamPoints], erPullZ_sigma_in[nMethods][nBeamPoints];
  double erPullPx_mean_in[nMethods][nBeamPoints],erPullPx_sigma_in[nMethods][nBeamPoints], erPullPy_mean_in[nMethods][nBeamPoints], erPullPy_sigma_in[nMethods][nBeamPoints], erPullPz_mean_in[nMethods][nBeamPoints], erPullPz_sigma_in[nMethods][nBeamPoints];
  double erPullTheta_mean_in[nMethods][nBeamPoints], erPullTheta_sigma_in[nMethods][nBeamPoints], erPullPhi_mean_in[nMethods][nBeamPoints], erPullPhi_sigma_in[nMethods][nBeamPoints];

double ResPx_mean[nMethods][nBeamPoints], ResPx_sigma[nMethods][nBeamPoints], ResPy_mean[nMethods][nBeamPoints], ResPy_sigma[nMethods][nBeamPoints], ResPz_mean[nMethods][nBeamPoints], ResPz_sigma[nMethods][nBeamPoints];
  double ResX_mean[nMethods][nBeamPoints], ResX_sigma[nMethods][nBeamPoints],ResY_mean[nMethods][nBeamPoints], ResY_sigma[nMethods][nBeamPoints],ResZ_mean[nMethods][nBeamPoints], ResZ_sigma[nMethods][nBeamPoints];
  double ResTheta_mean[nMethods][nBeamPoints], ResTheta_sigma[nMethods][nBeamPoints], ResPhi_mean[nMethods][nBeamPoints], ResPhi_sigma[nMethods][nBeamPoints];
  double ResX_mean_in[nMethods][nBeamPoints], ResX_sigma_in[nMethods][nBeamPoints],ResY_mean_in[nMethods][nBeamPoints], ResY_sigma_in[nMethods][nBeamPoints],ResZ_mean_in[nMethods][nBeamPoints], ResZ_sigma_in[nMethods][nBeamPoints];
  double ResPx_mean_in[nMethods][nBeamPoints],ResPx_sigma_in[nMethods][nBeamPoints], ResPy_mean_in[nMethods][nBeamPoints],ResPy_sigma_in[nMethods][nBeamPoints], ResPz_mean_in[nMethods][nBeamPoints],ResPz_sigma_in[nMethods][nBeamPoints];
  double ResTheta_mean_in[nMethods][nBeamPoints], ResTheta_sigma_in[nMethods][nBeamPoints], ResPhi_mean_in[nMethods][nBeamPoints], ResPhi_sigma_in[nMethods][nBeamPoints];

  double erResPx_mean[nMethods][nBeamPoints], erResPx_sigma[nMethods][nBeamPoints], erResPy_mean[nMethods][nBeamPoints], erResPy_sigma[nMethods][nBeamPoints], erResPz_mean[nMethods][nBeamPoints], erResPz_sigma[nMethods][nBeamPoints];
  double erResX_mean[nMethods][nBeamPoints], erResX_sigma[nMethods][nBeamPoints], erResY_mean[nMethods][nBeamPoints], erResY_sigma[nMethods][nBeamPoints], erResZ_mean[nMethods][nBeamPoints], erResZ_sigma[nMethods][nBeamPoints];
  double erResTheta_mean[nMethods][nBeamPoints], erResTheta_sigma[nMethods][nBeamPoints], erResPhi_mean[nMethods][nBeamPoints], erResPhi_sigma[nMethods][nBeamPoints];
  double erResX_mean_in[nMethods][nBeamPoints], erResX_sigma_in[nMethods][nBeamPoints], erResY_mean_in[nMethods][nBeamPoints], erResY_sigma_in[nMethods][nBeamPoints], erResZ_mean_in[nMethods][nBeamPoints], erResZ_sigma_in[nMethods][nBeamPoints];
  double erResPx_mean_in[nMethods][nBeamPoints],erResPx_sigma_in[nMethods][nBeamPoints], erResPy_mean_in[nMethods][nBeamPoints], erResPy_sigma_in[nMethods][nBeamPoints], erResPz_mean_in[nMethods][nBeamPoints], erResPz_sigma_in[nMethods][nBeamPoints];
 double erResTheta_mean_in[nMethods][nBeamPoints], erResTheta_sigma_in[nMethods][nBeamPoints], erResPhi_mean_in[nMethods][nBeamPoints], erResPhi_sigma_in[nMethods][nBeamPoints];


  for(int ip=0;ip<nBeamPoints;ip++){
    TString fileNameG = pathName+"/mom_";
    // switch (ip){
    // case 0: fileNameG+="1_5/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    // break;
    // case 1: fileNameG+="4_06/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    // break;
    // case 2: fileNameG+="8_9/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    // break;
    // case 3: fileNameG+="11_91/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    // break;
    // case 4: fileNameG+="15/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    // break;
    // default: cout << "?????";
    // break;
    // }
 switch (ip){
    case 0: fileNameG+="1_5/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    break;
    case 1: fileNameG+="15/Lumi_out_MC_and_REC_trks_matches_with_IDs0";
    break;
    default: cout << "?????";
    break;
    }

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

  TH1F *hResLumiTrkThetaPull = (TH1F*)fileIN->Get("hResLumiTrkThetaPull");
  hResLumiTrkThetaPull->Fit(fPullTheta,"qr");
  PullTheta_mean_in[im][ip] = fPullTheta->GetParameter(1);
  PullTheta_sigma_in[im][ip] = fPullTheta->GetParameter(2);
  erPullTheta_mean_in[im][ip] = fPullTheta->GetParError(1);
  erPullTheta_sigma_in[im][ip] = fPullTheta->GetParError(2);

TH1F *hResLumiTrkPhiPull = (TH1F*)fileIN->Get("hResLumiTrkPhiPull");
  hResLumiTrkPhiPull->Fit(fPullPhi,"qr");
  PullPhi_mean_in[im][ip] = fPullPhi->GetParameter(1);
  PullPhi_sigma_in[im][ip] = fPullPhi->GetParameter(2);
  erPullPhi_mean_in[im][ip] = fPullPhi->GetParError(1);
  erPullPhi_sigma_in[im][ip] = fPullPhi->GetParError(2);


  //Resolution

  TH1F* hResPointPx = (TH1F*)fileIN->Get("hResPointPx");
  TF1 *fResPx  = new TF1("fitPPx","gaus",-10,10);
  hResPointPx->Fit(fResPx,"qr");
  ResPx_mean[im][ip] = 1e3*fResPx->GetParameter(1);
  erResPx_mean[im][ip] = 1e3*fResPx->GetParError(1) ;
  ResPx_sigma[im][ip] = 1e3*fResPx->GetParameter(2);
  erResPx_sigma[im][ip] = 1e3*fResPx->GetParError(2) ;

  TH1F* hResPointPy = (TH1F*)fileIN->Get("hResPointPy");
  TF1 *fResPy = new TF1("fitPPy","gaus",-10,10);
  hResPointPy->Fit(fResPy,"qr");
  ResPy_mean[im][ip] = 1e3*fResPy->GetParameter(1);
  ResPy_sigma[im][ip] = 1e3*fResPy->GetParameter(2);
  erResPy_mean[im][ip] = 1e3*fResPy->GetParError(1);
  erResPy_sigma[im][ip] = 1e3*fResPy->GetParError(2);
  
  TH1F* hResPointPz = (TH1F*)fileIN->Get("hResPointPz");
  TF1 *fResPz  = new TF1("fitPPz","gaus",-10,10);
  hResPointPz->Fit(fResPz,"qr");
  ResPz_mean[im][ip] = 1e3*fResPz->GetParameter(1);
  ResPz_sigma[im][ip] = 1e3*fResPz->GetParameter(2);
  erResPz_mean[im][ip] = 1e3*fResPz->GetParError(1);
  erResPz_sigma[im][ip] = 1e3*fResPz->GetParError(2);

  TH1F* hResPointX = (TH1F*)fileIN->Get("hResPointX");
  TF1 *fResX  = new TF1("fitPX","gaus",-10,10);
  hResPointX->Fit(fResX,"qr");
  ResX_mean[im][ip] = fResX->GetParameter(1);
  ResX_sigma[im][ip] = fResX->GetParameter(2);
  erResX_mean[im][ip] = fResX->GetParError(1);
  erResX_sigma[im][ip] = fResX->GetParError(2);

  TH1F* hResPointY = (TH1F*)fileIN->Get("hResPointY");
  TF1 *fResY  = new TF1("fitPY","gaus",-10,10);
  hResPointY->Fit(fResY,"qr");
  ResY_mean[im][ip] = fResY->GetParameter(1);
  ResY_sigma[im][ip] = fResY->GetParameter(2);
  erResY_mean[im][ip] = fResY->GetParError(1);
  erResY_sigma[im][ip] = fResY->GetParError(2);
  
  TH1F* hResPointZ = (TH1F*)fileIN->Get("hResPointZ");
  TF1 *fResZ  = new TF1("fitPZ","gaus",-100,100);
  hResPointZ->Fit(fResZ,"qr");
  ResZ_mean[im][ip] = fResZ->GetParameter(1);
  ResZ_sigma[im][ip] = fResZ->GetParameter(2);
  erResZ_mean[im][ip] = fResZ->GetParError(1);
  erResZ_sigma[im][ip] = fResZ->GetParError(2);

  TH1F* hResTheta = (TH1F*)fileIN->Get("hResTheta");
  TF1 *fResTheta = new TF1("fitpth","gaus",-10,10);
  hResTheta->Fit(fResTheta,"qr");
  ResTheta_mean[im][ip] = fResTheta->GetParameter(1);
  ResTheta_sigma[im][ip] = fResTheta->GetParameter(2);
  erResTheta_mean[im][ip] = fResTheta->GetParError(1);
  erResTheta_sigma[im][ip] = fResTheta->GetParError(2);

  TH1F* hResPhi = (TH1F*)fileIN->Get("hResPhi");
  TF1 *fResPhi = new TF1("fitpphi","gaus",-10,10);
  hResPhi->Fit(fResPhi,"qr");
  ResPhi_mean[im][ip] = fResPhi->GetParameter(1);
  ResPhi_sigma[im][ip] = fResPhi->GetParameter(2);
  erResPhi_mean[im][ip] = fResPhi->GetParError(1);
  erResPhi_sigma[im][ip] = fResPhi->GetParError(2);

  TH1F *hResLumiTrkPointX = (TH1F*)fileIN->Get("hResLumiTrkPointX");
  hResLumiTrkPointX->Fit(fResX,"qr");
  ResX_mean_in[im][ip] = fResX->GetParameter(1);
  ResX_sigma_in[im][ip] = fResX->GetParameter(2);
  erResX_mean_in[im][ip] = fResX->GetParError(1);
  erResX_sigma_in[im][ip] = fResX->GetParError(2);
  TH1F *hResLumiTrkPointY = (TH1F*)fileIN->Get("hResLumiTrkPointY");
  hResLumiTrkPointY->Fit(fResY,"qr");
  ResY_mean_in[im][ip] = fResY->GetParameter(1);
  ResY_sigma_in[im][ip] = fResY->GetParameter(2);
  erResY_mean_in[im][ip] = fResY->GetParError(1);
  erResY_sigma_in[im][ip] = fResY->GetParError(2);
  TH1F *hResLumiTrkPointZ = (TH1F*)fileIN->Get("hResLumiTrkPointZ");
  hResLumiTrkPointZ->Fit(fResZ,"qr");
  ResZ_mean_in[im][ip]=-1000;
  ResZ_sigma_in[im][ip]=-1000;
  if(hResLumiTrkPointZ->GetRMS()>1e-6){
    ResZ_mean_in[im][ip] = fResZ->GetParameter(1);
    ResZ_sigma_in[im][ip] = fResZ->GetParameter(2);
    erResZ_mean_in[im][ip] = fResZ->GetParError(1);
    erResZ_sigma_in[im][ip] = fResZ->GetParError(2);
  }


  TH1F *hResLumiTrkPointPx = (TH1F*)fileIN->Get("hResLumiTrkPointPx");
  hResLumiTrkPointPx->Fit(fResPx,"qr");
  ResPx_mean_in[im][ip] = 1e3*fResPx->GetParameter(1);
  ResPx_sigma_in[im][ip] = 1e3*fResPx->GetParameter(2);
  erResPx_mean_in[im][ip] = 1e3*fResPx->GetParError(1);
  erResPx_sigma_in[im][ip] = 1e3*fResPx->GetParError(2);

  TH1F *hResLumiTrkPointPy = (TH1F*)fileIN->Get("hResLumiTrkPointPy");
  hResLumiTrkPointPy->Fit(fResPy,"qr");
  ResPy_mean_in[im][ip] = 1e3*fResPy->GetParameter(1);
  ResPy_sigma_in[im][ip] = 1e3*fResPy->GetParameter(2);
  erResPy_mean_in[im][ip] = 1e3*fResPy->GetParError(1);
  erResPy_sigma_in[im][ip] = 1e3*fResPy->GetParError(2);

  TH1F *hResLumiTrkPointPz = (TH1F*)fileIN->Get("hResLumiTrkPointPz");
  hResLumiTrkPointPz->Fit(fResPz,"qr");
  ResPz_mean_in[im][ip] = 1e3*fResPz->GetParameter(1);
  ResPz_sigma_in[im][ip] = 1e3*fResPz->GetParameter(2);
  erResPz_mean_in[im][ip] = 1e3*fResPz->GetParError(1);
  erResPz_sigma_in[im][ip] = 1e3*fResPz->GetParError(2);

  TH1F *hResLumiTrkTheta = (TH1F*)fileIN->Get("hResLumiTrkTheta");
  hResLumiTrkTheta->Fit(fResTheta,"qr");
  ResTheta_mean_in[im][ip] = fResTheta->GetParameter(1);
  ResTheta_sigma_in[im][ip] = fResTheta->GetParameter(2);
  erResTheta_mean_in[im][ip] = fResTheta->GetParError(1);
  erResTheta_sigma_in[im][ip] = fResTheta->GetParError(2);

  TH1F *hResLumiTrkPhi = (TH1F*)fileIN->Get("hResLumiTrkPhi");
  hResLumiTrkPhi->Fit(fResPhi,"qr");
  ResPhi_mean_in[im][ip] = fResPhi->GetParameter(1);
  ResPhi_sigma_in[im][ip] = fResPhi->GetParameter(2);
  erResPhi_mean_in[im][ip] = fResPhi->GetParError(1);
  erResPhi_sigma_in[im][ip] = fResPhi->GetParError(2);
  //****************************************

  }//methods
  }//pbeam

  //Creat and fill graphs with pbeam dependence
  TGraphErrors *gPullPx_mean[nMethods], *gPullPx_sigma[nMethods], *gPullPy_mean[nMethods], *gPullPy_sigma[nMethods], *gPullPz_mean[nMethods], *gPullPz_sigma[nMethods];
  TGraphErrors *gPullX_mean[nMethods], *gPullX_sigma[nMethods], *gPullY_mean[nMethods], *gPullY_sigma[nMethods], *gPullZ_mean[nMethods], *gPullZ_sigma[nMethods];
  TGraphErrors *gPullTheta_mean[nMethods], *gPullTheta_sigma[nMethods], *gPullPhi_mean[nMethods], *gPullPhi_sigma[nMethods];
 TGraphErrors *gPullTheta_mean_in[nMethods], *gPullTheta_sigma_in[nMethods], *gPullPhi_mean_in[nMethods], *gPullPhi_sigma_in[nMethods];
  TGraphErrors *gPullX_mean_in[nMethods], *gPullX_sigma_in[nMethods], *gPullY_mean_in[nMethods], *gPullY_sigma_in[nMethods], *gPullZ_mean_in[nMethods], *gPullZ_sigma_in[nMethods];
  TGraphErrors *gPullPx_mean_in[nMethods], *gPullPx_sigma_in[nMethods], *gPullPy_mean_in[nMethods], *gPullPy_sigma_in[nMethods], *gPullPz_mean_in[nMethods], *gPullPz_sigma_in[nMethods];

  TMultiGraph *mgPullPx_mean = new TMultiGraph("mgPullPx_mean",""); TMultiGraph *mgPullPx_sigma = new TMultiGraph("mgPullPx_sigma","");
  TMultiGraph *mgPullPy_mean = new TMultiGraph("mgPullPy_mean",""); TMultiGraph *mgPullPy_sigma = new TMultiGraph("mgPullPy_sigma","");
  TMultiGraph *mgPullPz_mean = new TMultiGraph("mgPullPz_mean",""); TMultiGraph *mgPullPz_sigma = new TMultiGraph("mgPullPz_sigma","");
  TMultiGraph *mgPullX_mean = new TMultiGraph("mgPullX_mean",""); TMultiGraph *mgPullX_sigma = new TMultiGraph("mgPullX_sigma","");
  TMultiGraph *mgPullY_mean = new TMultiGraph("mgPullY_mean",""); TMultiGraph *mgPullY_sigma = new TMultiGraph("mgPullY_sigma","");
  TMultiGraph *mgPullZ_mean = new TMultiGraph("mgPullZ_mean",""); TMultiGraph *mgPullZ_sigma = new TMultiGraph("mgPullZ_sigma","");
  TMultiGraph *mgPullTheta_mean = new TMultiGraph("mgPullTheta_mean",""); TMultiGraph *mgPullTheta_sigma = new TMultiGraph("mgPullTheta_sigma","");
  TMultiGraph *mgPullPhi_mean = new TMultiGraph("mgPullPhi_mean",""); TMultiGraph *mgPullPhi_sigma = new TMultiGraph("mgPullPhi_sigma","");

  TMultiGraph *mgPullX_mean_in = new TMultiGraph("mgPullX_mean_in",""); TMultiGraph *mgPullX_sigma_in = new TMultiGraph("mgPullX_sigma_in","");
  TMultiGraph *mgPullY_mean_in = new TMultiGraph("mgPullY_mean_in",""); TMultiGraph *mgPullY_sigma_in = new TMultiGraph("mgPullX_sigma_in","");
  TMultiGraph *mgPullZ_mean_in = new TMultiGraph("mgPullZ_mean_in",""); TMultiGraph *mgPullZ_sigma_in = new TMultiGraph("mgPullX_sigma_in","");
  TMultiGraph *mgPullPx_mean_in = new TMultiGraph("mgPullPx_mean_in",""); TMultiGraph *mgPullPx_sigma_in = new TMultiGraph("mgPullPx_sigma_in","");
  TMultiGraph *mgPullPy_mean_in = new TMultiGraph("mgPullPy_mean_in",""); TMultiGraph *mgPullPy_sigma_in = new TMultiGraph("mgPullPy_sigma_in","");
  TMultiGraph *mgPullPz_mean_in = new TMultiGraph("mgPullPz_mean_in",""); TMultiGraph *mgPullPz_sigma_in = new TMultiGraph("mgPullPz_sigma_in","");
  TMultiGraph *mgPullTheta_mean_in = new TMultiGraph("mgPullTheta_mean_in",""); TMultiGraph *mgPullTheta_sigma_in = new TMultiGraph("mgPullTheta_sigma_in","");
  TMultiGraph *mgPullPhi_mean_in = new TMultiGraph("mgPullPhi_mean_in",""); TMultiGraph *mgPullPhi_sigma_in = new TMultiGraph("mgPullPhi_sigma_in","");

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
    gPullPx_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPx_mean_in[im],0,erPullPx_mean_in[im]);
    gPullPx_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPx_sigma_in[im],0,erPullPx_sigma_in[im]);
    gPullPx_mean_in[im]->SetMarkerStyle(24+im);
    gPullPx_sigma_in[im]->SetMarkerStyle(24+im);
    gPullPx_mean_in[im]->SetMarkerColor(im+5);
    gPullPx_sigma_in[im]->SetMarkerColor(im+5);
    gPullPx_mean_in[im]->SetMarkerSize(2.0);
    gPullPx_sigma_in[im]->SetMarkerSize(2.0);
    mgPullPx_mean->Add(gPullPx_mean_in[im],"LP");
    mgPullPx_sigma->Add(gPullPx_sigma_in[im],"LP");

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
    gPullPy_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPy_mean_in[im],0,erPullPy_mean_in[im]);
    gPullPy_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPy_sigma_in[im],0,erPullPy_sigma_in[im]);
    gPullPy_mean_in[im]->SetMarkerStyle(24+im);
    gPullPy_sigma_in[im]->SetMarkerStyle(24+im);
    gPullPy_mean_in[im]->SetMarkerColor(im+5);
    gPullPy_sigma_in[im]->SetMarkerColor(im+5);
    gPullPy_mean_in[im]->SetMarkerSize(2.0);
    gPullPy_sigma_in[im]->SetMarkerSize(2.0);
    mgPullPy_mean->Add(gPullPy_mean_in[im],"LP");
    mgPullPy_sigma->Add(gPullPy_sigma_in[im],"LP");

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
    gPullPz_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPz_mean_in[im],0,erPullPz_mean_in[im]);
    gPullPz_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPz_sigma_in[im],0,erPullPz_sigma_in[im]);
    gPullPz_mean_in[im]->SetMarkerStyle(24+im);
    gPullPz_sigma_in[im]->SetMarkerStyle(24+im);
    gPullPz_mean_in[im]->SetMarkerColor(im+5);
    gPullPz_sigma_in[im]->SetMarkerColor(im+5);
    gPullPz_mean_in[im]->SetMarkerSize(2.0);
    gPullPz_sigma_in[im]->SetMarkerSize(2.0);
    mgPullPz_mean->Add(gPullPz_mean_in[im],"LP");
    mgPullPz_sigma->Add(gPullPz_sigma_in[im],"LP");

    gPullX_mean[im] = new TGraphErrors(nBeamPoints,p_beam,PullX_mean[im],0,erPullX_mean[im]);
    gPullX_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,PullX_sigma[im],0,erPullX_sigma[im]);
    gPullX_mean[im]->SetMarkerStyle(20+im);
    gPullX_sigma[im]->SetMarkerStyle(20+im);
    gPullX_mean[im]->SetMarkerColor(im+1);
    gPullX_sigma[im]->SetMarkerColor(im+1);
    gPullX_mean[im]->SetMarkerSize(2.0);
    gPullX_sigma[im]->SetMarkerSize(2.0);
    gPullX_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullX_mean_in[im],0,erPullX_mean_in[im]);
    gPullX_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullX_sigma_in[im],0,erPullX_sigma_in[im]);
    gPullX_mean_in[im]->SetMarkerStyle(24+im);
    gPullX_sigma_in[im]->SetMarkerStyle(24+im);
    gPullX_mean_in[im]->SetMarkerColor(im+5);
    gPullX_sigma_in[im]->SetMarkerColor(im+5);
    gPullX_mean_in[im]->SetMarkerSize(2.0);
    gPullX_sigma_in[im]->SetMarkerSize(2.0); 
    mgPullX_mean->Add(gPullX_mean_in[im],"LP");
    mgPullX_sigma->Add(gPullX_sigma_in[im],"LP");
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
    gPullY_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullY_mean_in[im],0,erPullY_mean_in[im]);
    gPullY_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullY_sigma_in[im],0,erPullY_sigma_in[im]);
    gPullY_mean_in[im]->SetMarkerStyle(24+im);
    gPullY_sigma_in[im]->SetMarkerStyle(24+im);
    gPullY_mean_in[im]->SetMarkerColor(im+5);
    gPullY_sigma_in[im]->SetMarkerColor(im+5);
    gPullY_mean_in[im]->SetMarkerSize(2.0);
    gPullY_sigma_in[im]->SetMarkerSize(2.0);
    mgPullY_mean->Add(gPullY_mean_in[im],"LP");
    mgPullY_sigma->Add(gPullY_sigma_in[im],"LP");


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

    gPullTheta_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullTheta_mean_in[im],0,erPullTheta_mean_in[im]);
    gPullTheta_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullTheta_sigma_in[im],0,erPullTheta_sigma_in[im]);
    gPullTheta_mean_in[im]->SetMarkerStyle(24+im);
    gPullTheta_sigma_in[im]->SetMarkerStyle(24+im);
    gPullTheta_mean_in[im]->SetMarkerColor(im+5);
    gPullTheta_sigma_in[im]->SetMarkerColor(im+5);
    gPullTheta_mean_in[im]->SetMarkerSize(2.0);
    gPullTheta_sigma_in[im]->SetMarkerSize(2.0);
    mgPullTheta_mean->Add(gPullTheta_mean_in[im],"LP");
    mgPullTheta_sigma->Add(gPullTheta_sigma_in[im],"LP");


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

    gPullPhi_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPhi_mean_in[im],0,erPullPhi_mean_in[im]);
    gPullPhi_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullPhi_sigma_in[im],0,erPullPhi_sigma_in[im]);
    gPullPhi_mean_in[im]->SetMarkerStyle(24+im);
    gPullPhi_sigma_in[im]->SetMarkerStyle(24+im);
    gPullPhi_mean_in[im]->SetMarkerColor(im+5);
    gPullPhi_sigma_in[im]->SetMarkerColor(im+5);
    gPullPhi_mean_in[im]->SetMarkerSize(2.0);
    gPullPhi_sigma_in[im]->SetMarkerSize(2.0);
    mgPullPhi_mean->Add(gPullPhi_mean_in[im],"LP");
    mgPullPhi_sigma->Add(gPullPhi_sigma_in[im],"LP");
   
    // gPullZ_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullZ_mean_in[im],0,erPullZ_mean_in[im]);
    // gPullZ_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,PullZ_sigma_in[im],0,erPullZ_sigma_in[im]);
    // gPullZ_mean_in[im]->SetMarkerStyle(20+im);
    // gPullZ_sigma_in[im]->SetMarkerStyle(20+im);
    // gPullZ_mean_in[im]->SetMarkerColor(im+1);
    // gPullZ_sigma_in[im]->SetMarkerColor(im+1);
    // gPullZ_mean_in[im]->SetMarkerSize(2.0);
    // gPullZ_sigma_in[im]->SetMarkerSize(2.0);
    // mgPullZ_mean_in->Add(gPullZ_mean_in[im],"LP");
    // mgPullZ_sigma_in->Add(gPullZ_sigma_in[im],"LP");
  }

  //Creat and fill graphs with pbeam dependence
  TGraphErrors *gResPx_mean[nMethods], *gResPx_sigma[nMethods], *gResPy_mean[nMethods], *gResPy_sigma[nMethods], *gResPz_mean[nMethods], *gResPz_sigma[nMethods];
  TGraphErrors *gResX_mean[nMethods], *gResX_sigma[nMethods], *gResY_mean[nMethods], *gResY_sigma[nMethods], *gResZ_mean[nMethods], *gResZ_sigma[nMethods];
  TGraphErrors *gResTheta_mean[nMethods], *gResTheta_sigma[nMethods], *gResPhi_mean[nMethods], *gResPhi_sigma[nMethods];
  TGraphErrors *gResX_mean_in[nMethods], *gResX_sigma_in[nMethods], *gResY_mean_in[nMethods], *gResY_sigma_in[nMethods], *gResZ_mean_in[nMethods], *gResZ_sigma_in[nMethods];
  TGraphErrors *gResPx_mean_in[nMethods], *gResPx_sigma_in[nMethods], *gResPy_mean_in[nMethods], *gResPy_sigma_in[nMethods], *gResPz_mean_in[nMethods], *gResPz_sigma_in[nMethods];
  TGraphErrors *gResTheta_mean_in[nMethods], *gResTheta_sigma_in[nMethods], *gResPhi_mean_in[nMethods], *gResPhi_sigma_in[nMethods];

  TMultiGraph *mgResPx_mean = new TMultiGraph("mgResPx_mean",""); TMultiGraph *mgResPx_sigma = new TMultiGraph("mgResPx_sigma","");
  TMultiGraph *mgResPy_mean = new TMultiGraph("mgResPy_mean",""); TMultiGraph *mgResPy_sigma = new TMultiGraph("mgResPy_sigma","");
  TMultiGraph *mgResPz_mean = new TMultiGraph("mgResPz_mean",""); TMultiGraph *mgResPz_sigma = new TMultiGraph("mgResPz_sigma","");
  TMultiGraph *mgResX_mean = new TMultiGraph("mgResX_mean",""); TMultiGraph *mgResX_sigma = new TMultiGraph("mgResX_sigma","");
  TMultiGraph *mgResY_mean = new TMultiGraph("mgResY_mean",""); TMultiGraph *mgResY_sigma = new TMultiGraph("mgResY_sigma","");
  TMultiGraph *mgResZ_mean = new TMultiGraph("mgResZ_mean",""); TMultiGraph *mgResZ_sigma = new TMultiGraph("mgResZ_sigma","");
  TMultiGraph *mgResTheta_mean = new TMultiGraph("mgResTheta_mean",""); TMultiGraph *mgResTheta_sigma = new TMultiGraph("mgResTheta_sigma","");
  TMultiGraph *mgResPhi_mean = new TMultiGraph("mgResPhi_mean",""); TMultiGraph *mgResPhi_sigma = new TMultiGraph("mgResPhi_sigma","");
  TMultiGraph *mgResX_mean_in = new TMultiGraph("mgResX_mean_in",""); TMultiGraph *mgResX_sigma_in = new TMultiGraph("mgResX_sigma_in","");
  TMultiGraph *mgResY_mean_in = new TMultiGraph("mgResY_mean_in",""); TMultiGraph *mgResY_sigma_in = new TMultiGraph("mgResX_sigma_in","");
  TMultiGraph *mgResZ_mean_in = new TMultiGraph("mgResZ_mean_in",""); TMultiGraph *mgResZ_sigma_in = new TMultiGraph("mgResX_sigma_in","");
  TMultiGraph *mgResPx_mean_in = new TMultiGraph("mgResPx_mean_in",""); TMultiGraph *mgResPx_sigma_in = new TMultiGraph("mgResPx_sigma_in","");
  TMultiGraph *mgResPy_mean_in = new TMultiGraph("mgResPy_mean_in",""); TMultiGraph *mgResPy_sigma_in = new TMultiGraph("mgResPy_sigma_in","");
  TMultiGraph *mgResPz_mean_in = new TMultiGraph("mgResPz_mean_in",""); TMultiGraph *mgResPz_sigma_in = new TMultiGraph("mgResPz_sigma_in","");

  for(int im=0;im<nMethods;im++){
    gResPx_mean[im] = new TGraphErrors(nBeamPoints,p_beam,ResPx_mean[im],0,erResPx_mean[im]);
    gResPx_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,ResPx_sigma[im],0,erResPx_sigma[im]);
    gResPx_mean[im]->SetMarkerStyle(20+im);
    gResPx_sigma[im]->SetMarkerStyle(20+im);
    gResPx_mean[im]->SetMarkerColor(im+1);
    gResPx_sigma[im]->SetMarkerColor(im+1);
    gResPx_mean[im]->SetMarkerSize(2.0);
    gResPx_sigma[im]->SetMarkerSize(2.0);
    mgResPx_mean->Add(gResPx_mean[im],"LP");
    mgResPx_sigma->Add(gResPx_sigma[im],"LP");
    gResPx_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResPx_mean_in[im],0,erResPx_mean_in[im]);
    gResPx_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResPx_sigma_in[im],0,erResPx_sigma_in[im]);
    gResPx_mean_in[im]->SetMarkerStyle(24+im);
    gResPx_sigma_in[im]->SetMarkerStyle(24+im);
    gResPx_mean_in[im]->SetMarkerColor(im+5);
    gResPx_sigma_in[im]->SetMarkerColor(im+5);
    gResPx_mean_in[im]->SetMarkerSize(2.0);
    gResPx_sigma_in[im]->SetMarkerSize(2.0);
    mgResPx_mean->Add(gResPx_mean_in[im],"LP");
    mgResPx_sigma->Add(gResPx_sigma_in[im],"LP");

    gResPy_mean[im] = new TGraphErrors(nBeamPoints,p_beam,ResPy_mean[im],0,erResPy_mean[im]);
    gResPy_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,ResPy_sigma[im],0,erResPy_sigma[im]);
    gResPy_mean[im]->SetMarkerStyle(20+im);
    gResPy_sigma[im]->SetMarkerStyle(20+im);
    gResPy_mean[im]->SetMarkerColor(im+1);
    gResPy_sigma[im]->SetMarkerColor(im+1);
    gResPy_mean[im]->SetMarkerSize(2.0);
    gResPy_sigma[im]->SetMarkerSize(2.0);
    mgResPy_mean->Add(gResPy_mean[im],"LP");
    mgResPy_sigma->Add(gResPy_sigma[im],"LP");
    gResPy_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResPy_mean_in[im],0,erResPy_mean_in[im]);
    gResPy_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResPy_sigma_in[im],0,erResPy_sigma_in[im]);
    gResPy_mean_in[im]->SetMarkerStyle(24+im);
    gResPy_sigma_in[im]->SetMarkerStyle(24+im);
    gResPy_mean_in[im]->SetMarkerColor(im+5);
    gResPy_sigma_in[im]->SetMarkerColor(im+5);
    gResPy_mean_in[im]->SetMarkerSize(2.0);
    gResPy_sigma_in[im]->SetMarkerSize(2.0);
    mgResPy_mean->Add(gResPy_mean_in[im],"LP");
    mgResPy_sigma->Add(gResPy_sigma_in[im],"LP");

    gResPz_mean[im] = new TGraphErrors(nBeamPoints,p_beam,ResPz_mean[im],0,erResPz_mean[im]);
    gResPz_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,ResPz_sigma[im],0,erResPz_sigma[im]);
    gResPz_mean[im]->SetMarkerStyle(20+im);
    gResPz_sigma[im]->SetMarkerStyle(20+im);
    gResPz_mean[im]->SetMarkerColor(im+1);
    gResPz_sigma[im]->SetMarkerColor(im+1);
    gResPz_mean[im]->SetMarkerSize(2.0);
    gResPz_sigma[im]->SetMarkerSize(2.0);
    mgResPz_mean->Add(gResPz_mean[im],"LP");
    mgResPz_sigma->Add(gResPz_sigma[im],"LP");
    gResPz_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResPz_mean_in[im],0,erResPz_mean_in[im]);
    gResPz_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResPz_sigma_in[im],0,erResPz_sigma_in[im]);
    gResPz_mean_in[im]->SetMarkerStyle(24+im);
    gResPz_sigma_in[im]->SetMarkerStyle(24+im);
    gResPz_mean_in[im]->SetMarkerColor(im+5);
    gResPz_sigma_in[im]->SetMarkerColor(im+5);
    gResPz_mean_in[im]->SetMarkerSize(2.0);
    gResPz_sigma_in[im]->SetMarkerSize(2.0);
    mgResPz_mean->Add(gResPz_mean_in[im],"LP");
    mgResPz_sigma->Add(gResPz_sigma_in[im],"LP");

    gResX_mean[im] = new TGraphErrors(nBeamPoints,p_beam,ResX_mean[im],0,erResX_mean[im]);
    gResX_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,ResX_sigma[im],0,erResX_sigma[im]);
    gResX_mean[im]->SetMarkerStyle(20+im);
    gResX_sigma[im]->SetMarkerStyle(20+im);
    gResX_mean[im]->SetMarkerColor(im+1);
    gResX_sigma[im]->SetMarkerColor(im+1);
    gResX_mean[im]->SetMarkerSize(2.0);
    gResX_sigma[im]->SetMarkerSize(2.0);
    gResX_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResX_mean_in[im],0,erResX_mean_in[im]);
    gResX_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResX_sigma_in[im],0,erResX_sigma_in[im]);
    gResX_mean_in[im]->SetMarkerStyle(24+im);
    gResX_sigma_in[im]->SetMarkerStyle(24+im);
    gResX_mean_in[im]->SetMarkerColor(im+5);
    gResX_sigma_in[im]->SetMarkerColor(im+5);
    gResX_mean_in[im]->SetMarkerSize(2.0);
    gResX_sigma_in[im]->SetMarkerSize(2.0); 
    mgResX_mean->Add(gResX_mean_in[im],"LP");
    mgResX_sigma->Add(gResX_sigma_in[im],"LP");
    mgResX_mean->Add(gResX_mean[im],"LP");
    mgResX_sigma->Add(gResX_sigma[im],"LP");
  
    gResY_mean[im] = new TGraphErrors(nBeamPoints,p_beam,ResY_mean[im],0,erResY_mean[im]);
    gResY_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,ResY_sigma[im],0,erResY_sigma[im]);
    gResY_mean[im]->SetMarkerStyle(20+im);
    gResY_sigma[im]->SetMarkerStyle(20+im);
    gResY_mean[im]->SetMarkerColor(im+1);
    gResY_sigma[im]->SetMarkerColor(im+1);
    gResY_mean[im]->SetMarkerSize(2.0);
    gResY_sigma[im]->SetMarkerSize(2.0);
    mgResY_mean->Add(gResY_mean[im],"LP");
    mgResY_sigma->Add(gResY_sigma[im],"LP");
    gResY_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResY_mean_in[im],0,erResY_mean_in[im]);
    gResY_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResY_sigma_in[im],0,erResY_sigma_in[im]);
    gResY_mean_in[im]->SetMarkerStyle(24+im);
    gResY_sigma_in[im]->SetMarkerStyle(24+im);
    gResY_mean_in[im]->SetMarkerColor(im+5);
    gResY_sigma_in[im]->SetMarkerColor(im+5);
    gResY_mean_in[im]->SetMarkerSize(2.0);
    gResY_sigma_in[im]->SetMarkerSize(2.0);
    mgResY_mean->Add(gResY_mean_in[im],"LP");
    mgResY_sigma->Add(gResY_sigma_in[im],"LP");


    gResZ_mean[im] = new TGraphErrors(nBeamPoints,p_beam,ResZ_mean[im],0,erResZ_mean[im]);
    gResZ_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,ResZ_sigma[im],0,erResZ_sigma[im]);
    gResZ_mean[im]->SetMarkerStyle(20+im);
    gResZ_sigma[im]->SetMarkerStyle(20+im);
    gResZ_mean[im]->SetMarkerColor(im+1);
    gResZ_sigma[im]->SetMarkerColor(im+1);
    gResZ_mean[im]->SetMarkerSize(2.0);
    gResZ_sigma[im]->SetMarkerSize(2.0);
    mgResZ_mean->Add(gResZ_mean[im],"LP");
    mgResZ_sigma->Add(gResZ_sigma[im],"LP");
  

    gResTheta_mean[im] = new TGraphErrors(nBeamPoints,p_beam,ResTheta_mean[im],0,erResTheta_mean[im]);
    gResTheta_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,ResTheta_sigma[im],0,erResTheta_sigma[im]);
    gResTheta_mean[im]->SetMarkerStyle(20+im);
    gResTheta_sigma[im]->SetMarkerStyle(20+im);
    gResTheta_mean[im]->SetMarkerColor(im+1);
    gResTheta_sigma[im]->SetMarkerColor(im+1);
    gResTheta_mean[im]->SetMarkerSize(2.0);
    gResTheta_sigma[im]->SetMarkerSize(2.0);
    mgResTheta_mean->Add(gResTheta_mean[im],"LP");
    mgResTheta_sigma->Add(gResTheta_sigma[im],"LP");
    gResTheta_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResTheta_mean_in[im],0,erResTheta_mean_in[im]);
    gResTheta_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResTheta_sigma_in[im],0,erResTheta_sigma_in[im]);
    gResTheta_mean_in[im]->SetMarkerStyle(24+im);
    gResTheta_sigma_in[im]->SetMarkerStyle(24+im);
    gResTheta_mean_in[im]->SetMarkerColor(im+5);
    gResTheta_sigma_in[im]->SetMarkerColor(im+5);
    gResTheta_mean_in[im]->SetMarkerSize(2.0);
    gResTheta_sigma_in[im]->SetMarkerSize(2.0);
    mgResTheta_mean->Add(gResTheta_mean_in[im],"LP");
    mgResTheta_sigma->Add(gResTheta_sigma_in[im],"LP");
    

    gResPhi_mean[im] = new TGraphErrors(nBeamPoints,p_beam,ResPhi_mean[im],0,erResPhi_mean[im]);
    gResPhi_sigma[im] = new TGraphErrors(nBeamPoints,p_beam,ResPhi_sigma[im],0,erResPhi_sigma[im]);
    gResPhi_mean[im]->SetMarkerStyle(20+im);
    gResPhi_sigma[im]->SetMarkerStyle(20+im);
    gResPhi_mean[im]->SetMarkerColor(im+1);
    gResPhi_sigma[im]->SetMarkerColor(im+1);
    gResPhi_mean[im]->SetMarkerSize(2.0);
    gResPhi_sigma[im]->SetMarkerSize(2.0);
    mgResPhi_mean->Add(gResPhi_mean[im],"LP");
    mgResPhi_sigma->Add(gResPhi_sigma[im],"LP");
    gResPhi_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResPhi_mean_in[im],0,erResPhi_mean_in[im]);
    gResPhi_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResPhi_sigma_in[im],0,erResPhi_sigma_in[im]);
    gResPhi_mean_in[im]->SetMarkerStyle(24+im);
    gResPhi_sigma_in[im]->SetMarkerStyle(24+im);
    gResPhi_mean_in[im]->SetMarkerColor(im+5);
    gResPhi_sigma_in[im]->SetMarkerColor(im+5);
    gResPhi_mean_in[im]->SetMarkerSize(2.0);
    gResPhi_sigma_in[im]->SetMarkerSize(2.0);
    mgResPhi_mean->Add(gResPhi_mean_in[im],"LP");
    mgResPhi_sigma->Add(gResPhi_sigma_in[im],"LP");
   
    // gResZ_mean_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResZ_mean_in[im],0,erResZ_mean_in[im]);
    // gResZ_sigma_in[im] = new TGraphErrors(nBeamPoints,p_beam,ResZ_sigma_in[im],0,erResZ_sigma_in[im]);
    // gResZ_mean_in[im]->SetMarkerStyle(20+im);
    // gResZ_sigma_in[im]->SetMarkerStyle(20+im);
    // gResZ_mean_in[im]->SetMarkerColor(im+1);
    // gResZ_sigma_in[im]->SetMarkerColor(im+1);
    // gResZ_mean_in[im]->SetMarkerSize(2.0);
    // gResZ_sigma_in[im]->SetMarkerSize(2.0);
    // mgResZ_mean_in->Add(gResZ_mean_in[im],"LP");
    // mgResZ_sigma_in->Add(gResZ_sigma_in[im],"LP");
  }
  //******************************************

  //Draw & Save results
  TString resname= pathName+"/TrkFit_BackPropag_diffMethods_Results";
  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";
 TString resname_root = resname+".root";
  TFile *fout = new TFile(resname_root,"RECREATE");

  TLegend *leg = new TLegend(0.7,0.65,0.98,0.97);
  leg->SetFillColor(0);
  //   leg->SetHeader("The Legend Title");
  for(int im=0;im<nMethods;im++){
    TString MethodName_ext1 = MethodName[im]+" [LMD]";
    TString MethodName_ext2 = MethodName[im]+" [PCA]";
    leg->AddEntry(gPullPx_mean_in[im],MethodName_ext1,"lep");
    leg->AddEntry(gPullPx_mean[im],MethodName_ext2,"lep");
  }
  TLegend *legsh = new TLegend(0.7,0.77,0.98,0.97);
  legsh->SetFillColor(0);
  //   leg->SetHeader("The Legend Title");
  for(int im=0;im<nMethods;im++){
    TString MethodName_ext2 = MethodName[im]+" [PCA]";
    legsh->AddEntry(gPullPx_mean[im],MethodName_ext2,"lep");
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
  mgPullPx_mean->GetYaxis()->SetTitle("(P^{REC}_{x} - P^{MC}_{x})/#sigma_{Px}, mean");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPx_sigma->Draw("AP");
  mgPullPx_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPx_sigma->GetYaxis()->SetTitle("(P^{REC}_{x} - P^{MC}_{x})/#sigma_{Px}, sigma");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
 mgPullPy_mean->Draw("AP");  
 mgPullPy_mean->GetXaxis()->SetTitle("momentum, GeV/c");
 mgPullPy_mean->GetYaxis()->SetTitle("(P^{REC}_{y} - P^{MC}_{y})/#sigma_{Py}, mean");
 leg->Draw();
 perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPy_sigma->Draw("AP");
  mgPullPy_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPy_sigma->GetYaxis()->SetTitle("(P^{REC}_{y} - P^{MC}_{y})/#sigma_{Py}, sigma");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPz_mean->Draw("AP");  
  mgPullPz_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPz_mean->GetYaxis()->SetTitle("(P^{REC}_{z} - P^{MC}_{z})/#sigma_{Pz}, mean");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPz_sigma->Draw("AP");
  mgPullPz_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullPz_sigma->GetYaxis()->SetTitle("(P^{REC}_{z} - P^{MC}_{z})/#sigma_{Pz}, sigma");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullX_mean->Draw("AP");  
  mgPullX_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullX_mean->GetYaxis()->SetTitle("(X^{REC} - X^{MC})/#sigma_{X}, mean");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullX_sigma->Draw("AP");
  mgPullX_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullX_sigma->GetYaxis()->SetTitle("(X^{REC} - X^{MC})/#sigma_{X}, sigma");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullY_mean->Draw("AP");  
  mgPullY_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullY_mean->GetYaxis()->SetTitle("(Y^{REC} - Y^{MC})/#sigma_{Y}, mean");
  leg->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullY_sigma->Draw("AP");
  mgPullY_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullY_sigma->GetYaxis()->SetTitle("(Y^{REC} - Y^{MC})/#sigma_{Y}, sigma");
  leg->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(1);// set log scale
  mgPullZ_mean->Draw("AP");  
  mgPullZ_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullZ_mean->GetYaxis()->SetTitle("(Z^{REC} - Z^{MC})/#sigma_{Z}, mean [PCA]");
  //  legsh->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(0);// set norm scale
  mgPullZ_sigma->Draw("AP");
  mgPullZ_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullZ_sigma->GetYaxis()->SetTitle("(Z^{REC} - Z^{MC})/#sigma_{Z}, sigma [PCA]");
  perfectsigmaval->Draw();
  legsh->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullTheta_mean->Draw("AP");  
  mgPullTheta_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullTheta_mean->GetYaxis()->SetTitle("(#theta^{REC} - #theta^{MC})/#sigma_{#theta}, mean [PCA]");
  legsh->Draw();
  perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullTheta_sigma->Draw("AP");
  mgPullTheta_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgPullTheta_sigma->GetYaxis()->SetTitle("(#theta^{REC} - #theta^{MC})/#sigma_{#theta}, sigma [PCA]");
  legsh->Draw();
  perfectsigmaval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPhi_mean->Draw("AP");  
 mgPullPhi_mean->GetXaxis()->SetTitle("momentum, GeV/c");
 mgPullPhi_mean->GetYaxis()->SetTitle("(#phi^{REC} - #phi^{MC})/#sigma_{#phi}, mean [PCA]");
 legsh->Draw();
 perfectmeanval->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgPullPhi_sigma->Draw("AP");
 mgPullPhi_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
 mgPullPhi_sigma->GetYaxis()->SetTitle("(#phi^{REC} - #phi^{MC})/#sigma_{#phi}, sigma [PCA]");
 legsh->Draw();
 perfectsigmaval->Draw();
 c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();

  //Draw resolution only
  mgResPx_mean->Draw("AP");  
  mgResPx_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResPx_mean->GetYaxis()->SetTitle("(P^{REC}_{x} - P^{MC}_{x}), mean, MeV/c");
  leg->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgResPx_sigma->Draw("AP");
  mgResPx_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResPx_sigma->GetYaxis()->SetTitle("(P^{REC}_{x} - P^{MC}_{x}), sigma, MeV/c");
  leg->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
 mgResPy_mean->Draw("AP");  
 mgResPy_mean->GetXaxis()->SetTitle("momentum, GeV/c");
 mgResPy_mean->GetYaxis()->SetTitle("(P^{REC}_{y} - P^{MC}_{y}), mean, MeV/c");
 leg->Draw();
  
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgResPy_sigma->Draw("AP");
  mgResPy_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResPy_sigma->GetYaxis()->SetTitle("(P^{REC}_{y} - P^{MC}_{y}), sigma, MeV/c");
  leg->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgResPz_mean->Draw("AP");  
  mgResPz_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResPz_mean->GetYaxis()->SetTitle("(P^{REC}_{z} - P^{MC}_{z}), mean, MeV/c");
  leg->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgResPz_sigma->Draw("AP");
  mgResPz_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResPz_sigma->GetYaxis()->SetTitle("(P^{REC}_{z} - P^{MC}_{z}), sigma, MeV/c");
  leg->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  mgResX_mean->Draw("AP");  
  mgResX_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResX_mean->GetYaxis()->SetTitle("(X^{REC} - X^{MC}), mean, cm");
  leg->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(1);// set log scale
  mgResX_sigma->Draw("AP");
  mgResX_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResX_sigma->GetYaxis()->SetTitle("(X^{REC} - X^{MC}), sigma, cm");
  leg->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(0);// set norm scale
  mgResY_mean->Draw("AP");  
  mgResY_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResY_mean->GetYaxis()->SetTitle("(Y^{REC} - Y^{MC}), mean, cm");
  leg->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(1);// set log scale
  mgResY_sigma->Draw("AP");
  mgResY_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResY_sigma->GetYaxis()->SetTitle("(Y^{REC} - Y^{MC}), sigma, cm");
  leg->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(1);// set log scale
  mgResZ_mean->Draw("AP");  
  mgResZ_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResZ_mean->GetYaxis()->SetTitle("(Z^{REC} - Z^{MC}), mean [PCA], cm");
  //  legsh->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(0);// set normal scale
  mgResZ_sigma->Draw("AP");
  mgResZ_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResZ_sigma->GetYaxis()->SetTitle("(Z^{REC} - Z^{MC}), sigma [PCA], cm");
   
  legsh->Draw();
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(0);// set norm scale
  mgResTheta_mean->Draw("AP");  
  mgResTheta_mean->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResTheta_mean->GetYaxis()->SetTitle("(#theta^{REC} - #theta^{MC}), mean [PCA], rad");
  legsh->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(1);// set log scale
  mgResTheta_sigma->Draw("AP");
  mgResTheta_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
  mgResTheta_sigma->GetYaxis()->SetTitle("(#theta^{REC} - #theta^{MC}), sigma [PCA], rad");
  legsh->Draw();
   
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(0);// set norm scale
  mgResPhi_mean->Draw("AP");  
 mgResPhi_mean->GetXaxis()->SetTitle("momentum, GeV/c");
 mgResPhi_mean->GetYaxis()->SetTitle("(#phi^{REC} - #phi^{MC}), mean [PCA], rad");
 legsh->Draw();
  
  c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
  c1.Clear();
  c1.SetLogy(1);// set log scale
  mgResPhi_sigma->Draw("AP");
 mgResPhi_sigma->GetXaxis()->SetTitle("momentum, GeV/c");
 mgResPhi_sigma->GetYaxis()->SetTitle("(#phi^{REC} - #phi^{MC}), sigma [PCA], rad");
 legsh->Draw();
  
 // c1.Print(resname_pdf_o); //write canvas and keep the pdf file open
 // c1.Clear();
  //*************************************************************

  c1.Print(resname_pdf_c); //write canvas and close the pdf file open
  c1.Clear();

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


 mgResPx_sigma->Write();
  mgResPy_mean->Write();  
  mgResPy_sigma->Write();
  mgResPz_mean->Write();  
  mgResPz_sigma->Write();
  mgResX_mean->Write();  
  mgResX_sigma->Write();
  mgResY_mean->Write();  
  mgResY_sigma->Write();
  mgResZ_mean->Write();  
  mgResZ_sigma->Write();
  mgResTheta_mean->Write();  
  mgResTheta_sigma->Write();
  mgResPhi_mean->Write();  
  mgResPhi_sigma->Write();
  mgResX_mean_in->Write();  
  mgResX_sigma_in->Write();
  mgResY_mean_in->Write();  
  mgResY_sigma_in->Write();
  mgResZ_mean_in->Write();  
  mgResZ_sigma_in->Write();
  mgResPx_mean_in->Write();  
  mgResPx_sigma_in->Write();
  mgResPy_mean_in->Write();  
  mgResPy_sigma_in->Write();
  mgResPz_mean_in->Write();  
  mgResPz_sigma_in->Write();
  //  legsh->Write();
  leg->Write();
  fout->Write();
}
