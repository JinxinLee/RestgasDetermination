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
void TrksFitterResults(TString fileName="/panda/pandaroot/macro/lmd/testPixel_TrkFitTest_KalmanGEANE_FullLMD/mom_15/Lumi_QA_0.root")
{
  int Ntrks = 1;
  TFile *fileIN = new TFile(fileName,"READ");
  //  fileIN->cd("NearIP");
  //  fileIN->Print();
  TH1F* hResTheta = (TH1F*)fileIN->Get("NearIP/hResTheta");
  int nrec = hResTheta->GetEntries();

  // //missed\ghost from definition I: compare dPhi & dTheta for MC and REC trks
  // TH1F* hntrkmissed_I = (TH1F*)fileIN->Get("hntrkmissed_I");
  // int nmiss_I = hntrkmissed_I->GetEntries();
  // int nmiss_I_under = hntrkmissed_I->GetBinContent(0);
  // nmiss_I-=nmiss_I_under;
  // double missed_I_mean = hntrkmissed_I->GetMean();
  // double missed_I_rms = hntrkmissed_I->GetMeanError();
  
  // TH1F* hntrkghost_I = (TH1F*)fileIN->Get("hntrkghost_I");
  // int nghost_I = hntrkghost_I->GetEntries();
  // int nghost_I_under = hntrkghost_I->GetBinContent(0);
  // nghost_I-=nghost_I_under;
  // double ghost_I_mean = hntrkghost_I->GetMean();
  // double ghost_I_rms = hntrkghost_I->GetMeanError();

  // //missed\ghost from definition II: based on hits info
  // TH1F* hntrkmissed_II = (TH1F*)fileIN->Get("hntrkmissed_II");
  // int nmiss_II = hntrkmissed_II->GetEntries();
  // int nmiss_II_under = hntrkmissed_II->GetBinContent(0);
  // nmiss_II-=nmiss_II_under;
  // double missed_II_mean = hntrkmissed_II->GetMean();
  // double missed_II_rms = hntrkmissed_II->GetMeanError();  
  
  // TH1F* hntrkghost_II = (TH1F*)fileIN->Get("hntrkghost_II");
  // int nghost_II = hntrkghost_II->GetEntries();
  // int nghost_II_under = hntrkghost_II->GetBinContent(0);
  // nghost_II-=nghost_II_under;
  // double ghost_II_mean = hntrkghost_II->GetMean();
  // double ghost_II_rms = hntrkghost_II->GetMeanError();
  
  // TH2F* hnRecnMC = (TH2F*)fileIN->Get("hnRecnMC");
  // TH1D* hnRecnMC_rec = hnRecnMC->ProjectionY();
  // int nsimTrk = (hnRecnMC_rec->GetEntries()); 
  
  // double ghost_I=100*double(nghost_I*ghost_I_mean)/(nsimTrk*Ntrks);
  // double missed_I=100*double(nmiss_I*missed_I_mean)/(nsimTrk*Ntrks);
  // double ghost_II=100*double(nghost_II*ghost_II_mean)/(nsimTrk*Ntrks);
  // double missed_II=100*double(nmiss_II*missed_II_mean)/(nsimTrk*Ntrks);
  // cout<<"nsimTrk = "<<nsimTrk<<" nrec = "<<nrec<<endl;
  // cout<<"(I:based on phi&theta diff between MC and REC), (II:based on hit info)"<<endl;
  // cout<<" nmiss_I = "<<nmiss_I<<" "<<missed_I<<" %"<<endl;
  // cout<<" nmiss_II = "<<nmiss_II<<" "<<missed_II<<" %"<<endl;
  // cout<<" nghost_I = "<<nghost_I<<" "<<ghost_I<<" %"<<endl;
  // cout<<" nghost_II = "<<nghost_II<<" "<<ghost_II<<" %"<<endl;
  // //  cout<<"***************************************"<<endl;
  // cout<<" "<<endl;



  ///Resolution & Pull distributions



  //After track fit (before back-propagation)
  //  fileIN->cd("NearLMD");
 TH1F*  hResLumiTrkPointX = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointX");
 // fResX->Update();
  TF1 *fResX = new TF1("fitX","gaus",-100,100);
  hResLumiTrkPointX->Fit(fResX,"qr");
  double MeanXin = fResX->GetParameter(1);
  double errMeanXin = fResX->GetParError(1);
  double ResXin = fResX->GetParameter(2);
  double errResXin = fResX->GetParError(2);
  TH1F*  hResLumiTrkPointY = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointY");
  // fResY->Update();
  TF1 *fResY = new TF1("fitY","gaus",-100,100);
  hResLumiTrkPointY->Fit(fResY,"qr");
  double MeanYin = fResY->GetParameter(1);
  double errMeanYin = fResY->GetParError(1);
  double ResYin = fResY->GetParameter(2);
  double errResYin = fResY->GetParError(2);
  TH1F*  hResLumiTrkPointZ = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointZ");
  double MeanZin=-1000;
  double ResZin =-1000;
  if(hResLumiTrkPointZ->GetRMS()>1e-6){
  hResLumiTrkPointZ->Fit(fResZ,"qr");
  MeanZin = fResZ->GetParameter(1);
  ResZin = fResZ->GetParameter(2);
  }

  TH1F* hResLumiTrkPointPx = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointPx");
  //  fResPx->Update();
  TF1 *fResPx = new TF1("fitPx","gaus",-100,100);
  hResLumiTrkPointPx->Fit(fResPx,"qr");
  double MeanPxin = fResPx->GetParameter(1);
  double errMeanPxin = fResPx->GetParError(1);
  double ResPxin = fResPx->GetParameter(2);
  double errResPxin = fResPx->GetParError(2);
  TH1F* hResLumiTrkPointPy = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointPy");
  //  fResPy->Update();
  TF1 *fResPy = new TF1("fitPy","gaus",-100,100);
  hResLumiTrkPointPy->Fit(fResPy,"qr");
  double MeanPyin = fResPy->GetParameter(1);
  double errMeanPyin = fResPy->GetParError(1);
  double ResPyin = fResPy->GetParameter(2);
  double errResPyin = fResPy->GetParError(2);
  TH1F* hResLumiTrkPointPz = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointPz");
  //  fResPz->Update();
  TF1 *fResPz = new TF1("fitPz","gaus",-100,100);
  hResLumiTrkPointPz->Fit(fResPz,"qr");
  double MeanPzin = fResPz->GetParameter(1);
  double errMeanPzin = fResPz->GetParError(1);
  double ResPzin = fResPz->GetParameter(2);
  double errResPzin = fResPz->GetParError(2);
  TH1F *hResLumiTrkTheta = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkTheta");
  TF1 *funrth = new TF1("fitrth","gaus",-0.01,0.01);
  funrth->SetParameters(100,0,3e-3);
  funrth->SetParNames("Constant","Mean","Sigma");
  hResLumiTrkTheta->Fit(funrth,"qr");
  double MeanThetain = funrth->GetParameter(1);
  double errMeanThetain = funrth->GetParError(1);
  double ResThetain = funrth->GetParameter(2);
  double errResThetain = funrth->GetParError(2);

  TH1F *hResLumiTrkPhi = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPhi");
  TF1 *funrth = new TF1("fitrth","gaus",-0.01,0.01);
  funrth->SetParameters(100,0,3e-3);
  funrth->SetParNames("Constant","Mean","Sigma");
  hResLumiTrkPhi->Fit(funrth,"qr");
  double MeanPhiin = funrth->GetParameter(1);
  double errMeanPhiin = funrth->GetParError(1);
  double ResPhiin = funrth->GetParameter(2);
  double errResPhiin = funrth->GetParError(2);

  TH1F *hResLumiTrkPointXPull = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointXPull");
  TF1 *fPullX  = new TF1("fitPullX","gaus",-10,10);
  //  fPullX->Update();
  hResLumiTrkPointXPull->Fit(fPullX,"qr");
  double PullX_mean_in = fPullX->GetParameter(1);
  double PullX_sigma_in = fPullX->GetParameter(2);
  TH1F *hResLumiTrkPointYPull = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointYPull");
  //  fPullY->Update();
  TF1 *fPullY  = new TF1("fitPullY","gaus",-10,10);
  hResLumiTrkPointYPull->Fit(fPullY,"qr");
  double PullY_mean_in = fPullY->GetParameter(1);
  double PullY_sigma_in = fPullY->GetParameter(2);
  TH1F *hResLumiTrkPointZPull = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointZPull");
  //  fPullZ->Update();
  TF1 *fPullZ  = new TF1("fitPullZ","gaus",-10,10);
  hResLumiTrkPointZPull->Fit(fPullZ,"qr");
  double PullZ_mean_in=-1000;
  double PullZ_sigma_in=-1000;
  if(hResLumiTrkPointZPull->GetRMS()>1e-6){
    PullZ_mean_in = fPullZ->GetParameter(1);
    PullZ_sigma_in = fPullZ->GetParameter(2);
  }

  TH1F *hResLumiTrkPointPxPull = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointPxPull");
  //fPullPx->Update();
  TF1 *fPullPx  = new TF1("fitPullPx","gaus",-10,10);
  hResLumiTrkPointPxPull->Fit(fPullPx,"qr");
  double PullPx_mean_in = fPullPx->GetParameter(1);
  double PullPx_sigma_in = fPullPx->GetParameter(2);

  TH1F *hResLumiTrkPointPyPull = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointPyPull");
  //  fPullPy->Update();
  TF1 *fPullPy  = new TF1("fitPullPy","gaus",-10,10);
  hResLumiTrkPointPyPull->Fit(fPullPy,"qr");
  double PullPy_mean_in = fPullPy->GetParameter(1);
  double PullPy_sigma_in = fPullPy->GetParameter(2);

  TH1F *hResLumiTrkPointPzPull = (TH1F*)fileIN->Get("NearLMD/hResLumiTrkPointPzPull");
  //  fPullPz->Update();
  TF1 *fPullPz  = new TF1("fitPullPz","gaus",-10,10);
  hResLumiTrkPointPzPull->Fit(fPullPz,"qr");
  double PullPz_mean_in = fPullPz->GetParameter(1);
  double PullPz_sigma_in = fPullPz->GetParameter(2);

  //  cout<<"&*********** and now we'll refit hists! *************** &"<<endl;
  TH1F* hResPointPx = (TH1F*)fileIN->Get("NearIP/hResPointPx");
  //  TF1 *fResPx = (TF1*)hResPointPx->GetFunction("fitcoord");
  TF1 *fResPx = new TF1("fitPx","gaus",-100,100);
  hResPointPx->Fit(fResPx,"qr");
  double MeanPx = fResPx->GetParameter(1);
  double ResPx = fResPx->GetParameter(2);
  
  TH1F* hResPointPy = (TH1F*)fileIN->Get("NearIP/hResPointPy");
  //  TF1 *fResPy = (TF1*)hResPointPy->GetFunction("fitcoord");
  TF1 *fResPy = new TF1("fitPy","gaus",-100,100);
  hResPointPy->Fit(fResPy,"qr");
  double MeanPy = fResPy->GetParameter(1);
  double ResPy = fResPy->GetParameter(2);

  TH1F* hResPointPz = (TH1F*)fileIN->Get("NearIP/hResPointPz");
  //  TF1 *fResPz = (TF1*)hResPointPz->GetFunction("fitcoord");
  TF1 *fResPz = new TF1("fitPz","gaus",-100,100);
  hResPointPz->Fit(fResPz,"qr");
  double MeanPz = fResPz->GetParameter(1);
  double ResPz = fResPz->GetParameter(2);

  TH1F* hPullPointPx = (TH1F*)fileIN->Get("NearIP/hPullPointPx");
  TF1 *fPullPx  = new TF1("fitPPx","gaus",-10,10);
  hPullPointPx->Fit(fPullPx,"qr");
  double PullPx_mean = fPullPx->GetParameter(1);
  double PullPx_sigma = fPullPx->GetParameter(2);

  TH1F* hPullPointPy = (TH1F*)fileIN->Get("NearIP/hPullPointPy");
  TF1 *fPullPy = new TF1("fitPPy","gaus",-10,10);
  hPullPointPy->Fit(fPullPy,"qr");
  double PullPy_mean = fPullPy->GetParameter(1);
  double PullPy_sigma = fPullPy->GetParameter(2);
  
  TH1F* hPullPointPz = (TH1F*)fileIN->Get("NearIP/hPullPointPz");
  TF1 *fPullPz  = new TF1("fitPPz","gaus",-10,10);
  hPullPointPz->Fit(fPullPz,"qr");
  double PullPz_mean = fPullPz->GetParameter(1);
  double PullPz_sigma = fPullPz->GetParameter(2);


  TH1F* hResPointX = (TH1F*)fileIN->Get("NearIP/hResPointX");
  TF1 *fResX  = new TF1("fitX","gaus",-10,10);
  hResPointX->Fit(fResX,"qr");
  double MeanX = fResX->GetParameter(1);
  double ResX = fResX->GetParameter(2);

  TH1F* hResPointY = (TH1F*)fileIN->Get("NearIP/hResPointY");
  TF1 *fResY  = new TF1("fitY","gaus",-100,100);
  hResPointY->Fit(fResY,"qr");
  double MeanY = fResY->GetParameter(1);
  double ResY = fResY->GetParameter(2);

  TH1F* hResPointZ = (TH1F*)fileIN->Get("NearIP/hResPointZ");
  TF1 *fResZ  = new TF1("fitZ","gaus",-100,100);
  hResPointZ->Fit(fResZ,"qr");
  double MeanZ = fResZ->GetParameter(1);
  double ResZ = fResZ->GetParameter(2);

  TH1F* hPullPointX = (TH1F*)fileIN->Get("NearIP/hPullPointX");
  TF1 *fPullX  = new TF1("fitPX","gaus",-10,10);
  hPullPointX->Fit(fPullX,"qr");
  double PullX_mean = fPullX->GetParameter(1);
  double PullX_sigma = fPullX->GetParameter(2);

  TH1F* hPullPointY = (TH1F*)fileIN->Get("NearIP/hPullPointY");
  TF1 *fPullY  = new TF1("fitPY","gaus",-10,10);
  hPullPointY->Fit(fPullY,"qr");
  double PullY_mean = fPullY->GetParameter(1);
  double PullY_sigma = fPullY->GetParameter(2);
  
  TH1F* hPullPointZ = (TH1F*)fileIN->Get("NearIP/hPullPointZ");
  // double PullZ_mean=-100;
  // double PullZ_sigma=-100;
  //  if(hPullPointZ->GetRMS()!=0){
  TF1 *fPullZ  = new TF1("fitPZ","gaus",-100,100);
    hPullPointZ->Fit(fPullZ,"qr");
    double PullZ_mean = fPullZ->GetParameter(1);
    double PullZ_sigma = fPullZ->GetParameter(2);
    // }

  TH1F* hResTheta = (TH1F*)fileIN->Get("NearIP/hResTheta");
  TF1 *fResTheta = new TF1("fitfrth","gaus",-0.01,0.01);
  hResTheta->Fit(fResTheta,"qr");
  double MeanTheta = fResTheta->GetParameter(1);
  double ResTheta = fResTheta->GetParameter(2);

  TH1F* hPullTheta = (TH1F*)fileIN->Get("NearIP/hPullTheta");
  TF1 *fPullTheta = new TF1("fitpth","gaus",-10,10);
  hPullTheta->Fit(fPullTheta,"qr");
  double PullTheta_mean = fPullTheta->GetParameter(1);
  double PullTheta_sigma = fPullTheta->GetParameter(2);

  TH1F* hResPhi = (TH1F*)fileIN->Get("NearIP/hResPhi");
  TF1 *fResPhi = new TF1("fitrphi","gaus",-1,1);
  hResPhi->Fit(fResPhi,"qr");
  double MeanPhi = fResPhi->GetParameter(1);
  double ResPhi = fResPhi->GetParameter(2);

  TH1F* hPullPhi = (TH1F*)fileIN->Get("NearIP/hPullPhi");
  TF1 *fPullPhi = new TF1("fitpphi","gaus",-10,10);
  hPullPhi->Fit(fPullPhi,"qr");
  double PullPhi_mean = fPullPhi->GetParameter(1);
  double PullPhi_sigma = fPullPhi->GetParameter(2);



  cout<<" Resolution:"<<endl;
  cout<<"After Trk-Fit (before back propagation):"<<endl;
  cout<<" Trk (X, Y) = ("<<"["<<1e4*MeanXin<<" +\- "<<1e4*errMeanXin<<"; "<<1e4*ResXin<<" +\- "<<1e4*errResXin<<"]; "
      <<"["<<1e4*MeanYin<<" +\- "<<1e4*errMeanYin<<"; "<<1e4*ResYin<<" +\- "<<1e4*errResYin<<"]; "
      <<") mkm"<<endl;
  cout<<" MomTrk (Px, Py, Pz) = ("<<"["<<MeanPxin*1e6<<" +\- "<<errMeanPxin*1e6<<"; "<<ResPxin*1e6<<" +\- "<<errResPxin*1e6<<"]; "
      <<"["<<MeanPyin*1e6<<" +\- "<<errMeanPyin*1e6<<"; "<<ResPyin*1e6<<" +\- "<<errResPyin*1e6<<"]; "
      <<"["<<MeanPzin*1e6<<" +\- "<<errMeanPzin*1e6<<"; "<<ResPzin*1e6<<" +\- "<<errResPzin*1e6<<"]; "
      <<") keV/c"<<endl;
  cout<<" Angles Trk (Theta, Phi) = ( ["<<MeanThetain*1e6<<" +\- "<<errMeanThetain*1e6<<"; "
      <<ResThetain*1e6<<" +\- "<<errResThetain*1e6<<"], mkrad; ["
      <<MeanPhiin*1e3<<" +\- "<<errMeanPhiin*1e3<<"; "<<ResPhiin*1e3<<" +\-"<<errResPhiin*1e3<<"], mrad)"<<endl;
  cout<<" "<<endl;
  cout<<" Pull PCA (X, Y, Z) = ("<<PullX_mean_in<<" +/- "<<PullX_sigma_in<<"; "<<PullY_mean_in<<" +/- "<<PullY_sigma_in<<"; "<<PullZ_mean_in<<" +/- "<<PullZ_sigma_in<<")"<<endl;
  cout<<" Pull Mom (Px, Py, Pz) = ("<<PullPx_mean_in<<" +/- "<<PullPx_sigma_in<<"; "<<PullPy_mean_in<<" +/- "<<PullPy_sigma_in<<"; "<<PullPz_mean_in<<" +/- "<<PullPz_sigma_in<<")"<<endl;
  cout<<"....."<<endl;
  cout<<"After back propagation:"<<endl;
  cout<<" PCA (X, Y, Z) = ("<<1e4*MeanX<<" +\- "<<1e4*ResX<<"; "<<1e4*MeanY<<" +\- "<<1e4*ResY<<"; "<<1e4*MeanZ<<" +\- "<<1e4*ResZ<<") mkm"<<endl;
  cout<<" Mom (Px, Py, Pz) = ("<<MeanPx*1e6<<" +\- "<<ResPx*1e6<<"; "<<MeanPy*1e6<<" +\- "<<ResPy*1e6<<"; "<<MeanPz*1e6<<" +\- "<<ResPz*1e6<<") keV/c"<<endl;
  cout<<" Angles (Theta, Phi) = ("<<MeanTheta*1e6<<" +\- "<<ResTheta*1e6<<", mkrad; "<<MeanPhi*1e3<<" +\- "<<ResPhi*1e3<<", mrad)"<<endl;
  //cout<<"***************************************"<<endl;
  cout<<" "<<endl;

  cout<<" Pulls:"<<endl;
  cout<<" Pull PCA (X, Y, Z) = ("<<PullX_mean<<" +/- "<<PullX_sigma<<"; "<<PullY_mean<<" +/- "<<PullY_sigma<<"; "<<PullZ_mean<<" +/- "<<PullZ_sigma<<")"<<endl;
  cout<<" Pull Mom (Px, Py, Pz) = ("<<PullPx_mean<<" +/- "<<PullPx_sigma<<"; "<<PullPy_mean<<" +/- "<<PullPy_sigma<<"; "<<PullPz_mean<<" +/- "<<PullPz_sigma<<")"<<endl;
  cout<<" Pull Angles (Theta, Phi) = ("<<PullTheta_mean<<" +/- "<<PullTheta_sigma<<"; "
      <<PullPhi_mean<<" +/- "<<PullTheta_sigma<<")"<<endl;
  cout<<"***************************************"<<endl;
  cout<<" "<<endl;

}
