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
void TrksFitterResults(TString fileName="/panda/pandaroot/macro/lmd/testPixel/09042013/mom_15/Lumi_out_MC_and_REC_trks_matches_with_IDs0_MINUIT_BPGEANE.root")
{
  int Ntrks = 1;
  TFile *fileIN = new TFile(fileName,"READ");
  TH1F* hResTheta = (TH1F*)fileIN->Get("hResTheta");
  int nrec = hResTheta->GetEntries();

  //missed\ghost from definition I: compare dPhi & dTheta for MC and REC trks
  TH1F* hntrkmissed_I = (TH1F*)fileIN->Get("hntrkmissed_I");
  int nmiss_I = hntrkmissed_I->GetEntries();
  int nmiss_I_under = hntrkmissed_I->GetBinContent(0);
  nmiss_I-=nmiss_I_under;
  double missed_I_mean = hntrkmissed_I->GetMean();
  double missed_I_rms = hntrkmissed_I->GetMeanError();
  
  TH1F* hntrkghost_I = (TH1F*)fileIN->Get("hntrkghost_I");
  int nghost_I = hntrkghost_I->GetEntries();
  int nghost_I_under = hntrkghost_I->GetBinContent(0);
  nghost_I-=nghost_I_under;
  double ghost_I_mean = hntrkghost_I->GetMean();
  double ghost_I_rms = hntrkghost_I->GetMeanError();

  //missed\ghost from definition II: based on hits info
  TH1F* hntrkmissed_II = (TH1F*)fileIN->Get("hntrkmissed_II");
  int nmiss_II = hntrkmissed_II->GetEntries();
  int nmiss_II_under = hntrkmissed_II->GetBinContent(0);
  nmiss_II-=nmiss_II_under;
  double missed_II_mean = hntrkmissed_II->GetMean();
  double missed_II_rms = hntrkmissed_II->GetMeanError();  
  
  TH1F* hntrkghost_II = (TH1F*)fileIN->Get("hntrkghost_II");
  int nghost_II = hntrkghost_II->GetEntries();
  int nghost_II_under = hntrkghost_II->GetBinContent(0);
  nghost_II-=nghost_II_under;
  double ghost_II_mean = hntrkghost_II->GetMean();
  double ghost_II_rms = hntrkghost_II->GetMeanError();
  
  TH2F* hnRecnMC = (TH2F*)fileIN->Get("hnRecnMC");
  TH1D* hnRecnMC_rec = hnRecnMC->ProjectionY();
  int nsimTrk = (hnRecnMC_rec->GetEntries()); 
  
  double ghost_I=100*double(nghost_I*ghost_I_mean)/(nsimTrk*Ntrks);
  double missed_I=100*double(nmiss_I*missed_I_mean)/(nsimTrk*Ntrks);
  double ghost_II=100*double(nghost_II*ghost_II_mean)/(nsimTrk*Ntrks);
  double missed_II=100*double(nmiss_II*missed_II_mean)/(nsimTrk*Ntrks);
  cout<<"nsimTrk = "<<nsimTrk<<" nrec = "<<nrec<<endl;
  cout<<"(I:based on phi&theta diff between MC and REC), (II:based on hit info)"<<endl;
  cout<<" nmiss_I = "<<nmiss_I<<" "<<missed_I<<" %"<<endl;
  cout<<" nmiss_II = "<<nmiss_II<<" "<<missed_II<<" %"<<endl;
  cout<<" nghost_I = "<<nghost_I<<" "<<ghost_I<<" %"<<endl;
  cout<<" nghost_II = "<<nghost_II<<" "<<ghost_II<<" %"<<endl;
  //  cout<<"***************************************"<<endl;
  cout<<" "<<endl;

  ///Resolution & Pull distributions
  //  cout<<"&*********** and now we'll refit hists! *************** &"<<endl;
  TH1F* hResPointPx = (TH1F*)fileIN->Get("hResPointPx");
  //  TF1 *fResPx = (TF1*)hResPointPx->GetFunction("fitcoord");
  TF1 *fResPx = new TF1("fitPx","gaus");
  hResPointPx->Fit(fResPx,"qr");
  double MeanPx = fResPx->GetParameter(1);
  double ResPx = fResPx->GetParameter(2);
  
  TH1F* hResPointPy = (TH1F*)fileIN->Get("hResPointPy");
  //  TF1 *fResPy = (TF1*)hResPointPy->GetFunction("fitcoord");
  TF1 *fResPy = new TF1("fitPy","gaus");
  hResPointPy->Fit(fResPy,"qr");
  double MeanPy = fResPy->GetParameter(1);
  double ResPy = fResPy->GetParameter(2);

  TH1F* hResPointPz = (TH1F*)fileIN->Get("hResPointPz");
  //  TF1 *fResPz = (TF1*)hResPointPz->GetFunction("fitcoord");
  TF1 *fResPz = new TF1("fitPz","gaus");
  hResPointPz->Fit(fResPz,"qr");
  double MeanPz = fResPz->GetParameter(1);
  double ResPz = fResPz->GetParameter(2);

  TH1F* hPullPointPx = (TH1F*)fileIN->Get("hPullPointPx");
  TF1 *fPullPx  = new TF1("fitPPx","gaus",-10,10);
  hPullPointPx->Fit(fPullPx,"qr");
  double PullPx_mean = fPullPx->GetParameter(1);
  double PullPx_sigma = fPullPx->GetParameter(2);

  TH1F* hPullPointPy = (TH1F*)fileIN->Get("hPullPointPy");
  TF1 *fPullPy = new TF1("fitPPy","gaus",-10,10);
  hPullPointPy->Fit(fPullPy,"qr");
  double PullPy_mean = fPullPy->GetParameter(1);
  double PullPy_sigma = fPullPy->GetParameter(2);
  
  TH1F* hPullPointPz = (TH1F*)fileIN->Get("hPullPointPz");
  TF1 *fPullPz  = new TF1("fitPPz","gaus",-10,10);
  hPullPointPz->Fit(fPullPz,"qr");
  double PullPz_mean = fPullPz->GetParameter(1);
  double PullPz_sigma = fPullPz->GetParameter(2);


  TH1F* hResPointX = (TH1F*)fileIN->Get("hResPointX");
  TF1 *fResX  = new TF1("fitX","gaus",-10,10);
  hResPointX->Fit(fResX,"qr");
  double MeanX = fResX->GetParameter(1);
  double ResX = fResX->GetParameter(2);

  TH1F* hResPointY = (TH1F*)fileIN->Get("hResPointY");
  TF1 *fResY  = new TF1("fitY","gaus");
  hResPointY->Fit(fResY,"qr");
  double MeanY = fResY->GetParameter(1);
  double ResY = fResY->GetParameter(2);

  TH1F* hResPointZ = (TH1F*)fileIN->Get("hResPointZ");
  TF1 *fResZ  = new TF1("fitZ","gaus");
  hResPointZ->Fit(fResZ,"qr");
  double MeanZ = fResZ->GetParameter(1);
  double ResZ = fResZ->GetParameter(2);

  TH1F* hPullPointX = (TH1F*)fileIN->Get("hPullPointX");
  TF1 *fPullX  = new TF1("fitPX","gaus",-10,10);
  hPullPointX->Fit(fPullX,"qr");
  double PullX_mean = fPullX->GetParameter(1);
  double PullX_sigma = fPullX->GetParameter(2);

  TH1F* hPullPointY = (TH1F*)fileIN->Get("hPullPointY");
  TF1 *fPullY  = new TF1("fitPY","gaus",-10,10);
  hPullPointY->Fit(fPullY,"qr");
  double PullY_mean = fPullY->GetParameter(1);
  double PullY_sigma = fPullY->GetParameter(2);
  
  TH1F* hPullPointZ = (TH1F*)fileIN->Get("hPullPointZ");
  // double PullZ_mean=-100;
  // double PullZ_sigma=-100;
  //  if(hPullPointZ->GetRMS()!=0){
  TF1 *fPullZ  = new TF1("fitPZ","gaus",-100,100);
    hPullPointZ->Fit(fPullZ,"qr");
    double PullZ_mean = fPullZ->GetParameter(1);
    double PullZ_sigma = fPullZ->GetParameter(2);
    // }

  TH1F* hResTheta = (TH1F*)fileIN->Get("hResTheta");
  TF1 *fResTheta = new TF1("fitfrth","gaus",-0.01,0.01);
  hResTheta->Fit(fResTheta,"qr");
  double MeanTheta = fResTheta->GetParameter(1);
  double ResTheta = fResTheta->GetParameter(2);

  TH1F* hPullTheta = (TH1F*)fileIN->Get("hPullTheta");
  TF1 *fPullTheta = new TF1("fitpth","gaus",-10,10);
  hPullTheta->Fit(fPullTheta,"qr");
  double PullTheta_mean = fPullTheta->GetParameter(1);
  double PullTheta_sigma = fPullTheta->GetParameter(2);

  TH1F* hResPhi = (TH1F*)fileIN->Get("hResPhi");
  TF1 *fResPhi = new TF1("fitrphi","gaus",-1,1);
  hResPhi->Fit(fResPhi,"qr");
  double MeanPhi = fResPhi->GetParameter(1);
  double ResPhi = fResPhi->GetParameter(2);

  TH1F* hPullPhi = (TH1F*)fileIN->Get("hPullPhi");
  TF1 *fPullPhi = new TF1("fitpphi","gaus",-10,10);
  hPullPhi->Fit(fPullPhi,"qr");
  double PullPhi_mean = fPullPhi->GetParameter(1);
  double PullPhi_sigma = fPullPhi->GetParameter(2);


  //After track fit (before back-propagation)
 TH1F*  hResLumiTrkPointX = (TH1F*)fileIN->Get("hResLumiTrkPointX");
  hResLumiTrkPointX->Fit(fResX,"qr");
  double MeanXin = fResX->GetParameter(1);
  double ResXin = fResX->GetParameter(2);
  TH1F*  hResLumiTrkPointY = (TH1F*)fileIN->Get("hResLumiTrkPointY");
  hResLumiTrkPointY->Fit(fResY,"qr");
  double MeanYin = fResY->GetParameter(1);
  double ResYin = fResY->GetParameter(2);
  TH1F*  hResLumiTrkPointZ = (TH1F*)fileIN->Get("hResLumiTrkPointZ");
  double MeanZin=-1000;
  double ResZin =-1000;
  if(hResLumiTrkPointZ->GetRMS()>1e-6){
  hResLumiTrkPointZ->Fit(fResZ,"qr");
  MeanZin = fResZ->GetParameter(1);
  ResZin = fResZ->GetParameter(2);
  }

  TH1F* hResLumiTrkPointPx = (TH1F*)fileIN->Get("hResLumiTrkPointPx");
  hResLumiTrkPointPx->Fit(fResPx,"qr");
  double MeanPxin = fResPx->GetParameter(1);
  double ResPxin = fResPx->GetParameter(2);
  TH1F* hResLumiTrkPointPy = (TH1F*)fileIN->Get("hResLumiTrkPointPy");
  hResLumiTrkPointPy->Fit(fResPy,"qr");
  double MeanPyin = fResPy->GetParameter(1);
  double ResPyin = fResPy->GetParameter(2);
  TH1F* hResLumiTrkPointPz = (TH1F*)fileIN->Get("hResLumiTrkPointPz");
  hResLumiTrkPointPz->Fit(fResPz,"qr");
  double MeanPzin = fResPz->GetParameter(1);
  double ResPzin = fResPz->GetParameter(2);

  TH1F *hResLumiTrkTheta = (TH1F*)fileIN->Get("hResLumiTrkTheta");
  TF1 *funrth = new TF1("fitrth","gaus",-0.01,0.01);
  funrth->SetParameters(100,0,3e-3);
  funrth->SetParNames("Constant","Mean","Sigma");
  hResLumiTrkTheta->Fit(funrth,"qr");
  double MeanThetain = funrth->GetParameter(1);
  double ResThetain = funrth->GetParameter(2);

  TH1F *hResLumiTrkPhi = (TH1F*)fileIN->Get("hResLumiTrkPhi");
  TF1 *funrth = new TF1("fitrth","gaus",-0.01,0.01);
  funrth->SetParameters(100,0,3e-3);
  funrth->SetParNames("Constant","Mean","Sigma");
  hResLumiTrkPhi->Fit(funrth,"qr");
  double MeanPhiin = funrth->GetParameter(1);
  double ResPhiin = funrth->GetParameter(2);

  TH1F *hResLumiTrkPointXPull = (TH1F*)fileIN->Get("hResLumiTrkPointXPull");
  hResLumiTrkPointXPull->Fit(fPullX,"qr");
  double PullX_mean_in = fPullX->GetParameter(1);
  double PullX_sigma_in = fPullX->GetParameter(2);
  TH1F *hResLumiTrkPointYPull = (TH1F*)fileIN->Get("hResLumiTrkPointYPull");
  hResLumiTrkPointYPull->Fit(fPullY,"qr");
  double PullY_mean_in = fPullY->GetParameter(1);
  double PullY_sigma_in = fPullY->GetParameter(2);
  TH1F *hResLumiTrkPointZPull = (TH1F*)fileIN->Get("hResLumiTrkPointZPull");
  hResLumiTrkPointZPull->Fit(fPullZ,"qr");
  double PullZ_mean_in=-1000;
  double PullZ_sigma_in=-1000;
  if(hResLumiTrkPointZPull->GetRMS()>1e-6){
    PullZ_mean_in = fPullZ->GetParameter(1);
    PullZ_sigma_in = fPullZ->GetParameter(2);
  }

  TH1F *hResLumiTrkPointPxPull = (TH1F*)fileIN->Get("hResLumiTrkPointPxPull");
  hResLumiTrkPointPxPull->Fit(fPullPx,"qr");
  double PullPx_mean_in = fPullPx->GetParameter(1);
  double PullPx_sigma_in = fPullPx->GetParameter(2);

  TH1F *hResLumiTrkPointPyPull = (TH1F*)fileIN->Get("hResLumiTrkPointPyPull");
  hResLumiTrkPointPyPull->Fit(fPullPy,"qr");
  double PullPy_mean_in = fPullPy->GetParameter(1);
  double PullPy_sigma_in = fPullPy->GetParameter(2);

  TH1F *hResLumiTrkPointPzPull = (TH1F*)fileIN->Get("hResLumiTrkPointPzPull");
  hResLumiTrkPointPzPull->Fit(fPullPz,"qr");
  double PullPz_mean_in = fPullPz->GetParameter(1);
  double PullPz_sigma_in = fPullPz->GetParameter(2);

  cout<<" Resolution:"<<endl;
  cout<<"After Trk-Fit (before back propagation):"<<endl;
  cout<<" Trk (X, Y, Z) = ("<<1e1*MeanXin<<" +\- "<<1e1*ResXin<<"; "<<1e1*MeanYin<<" +\- "<<1e1*ResYin<<"; "<<1e1*MeanZin<<" +\- "<<1e1*ResZin<<") mm"<<endl;
  cout<<" Mom Trk (Px, Py, Pz) = ("<<MeanPxin*1e3<<" +\- "<<ResPxin*1e3<<"; "<<MeanPyin*1e3<<" +\- "<<ResPyin*1e3<<"; "<<MeanPzin*1e3<<" +\- "<<ResPzin*1e3<<") MeV/c"<<endl;
  cout<<" Angles Trk (Theta, Phi) = ("<<MeanThetain*1e6<<" +\- "<<ResThetain*1e6<<", mkrad; "<<MeanPhiin*1e3<<" +\- "<<ResPhiin*1e3<<", mrad)"<<endl;
  cout<<" "<<endl;
  cout<<" Pull PCA (X, Y, Z) = ("<<PullX_mean_in<<" +/- "<<PullX_sigma_in<<"; "<<PullY_mean_in<<" +/- "<<PullY_sigma_in<<"; "<<PullZ_mean_in<<" +/- "<<PullZ_sigma_in<<")"<<endl;
  cout<<" Pull Mom (Px, Py, Pz) = ("<<PullPx_mean_in<<" +/- "<<PullPx_sigma_in<<"; "<<PullPy_mean_in<<" +/- "<<PullPy_sigma_in<<"; "<<PullPz_mean_in<<" +/- "<<PullPz_sigma_in<<")"<<endl;
  cout<<"....."<<endl;
  cout<<"After back propagation:"<<endl;
  cout<<" PCA (X, Y, Z) = ("<<1e1*MeanX<<" +\- "<<1e1*ResX<<"; "<<1e1*MeanY<<" +\- "<<1e1*ResY<<"; "<<1e1*MeanZ<<" +\- "<<1e1*ResZ<<") mm"<<endl;
  cout<<" Mom (Px, Py, Pz) = ("<<MeanPx*1e3<<" +\- "<<ResPx*1e3<<"; "<<MeanPy*1e3<<" +\- "<<ResPy*1e3<<"; "<<MeanPz*1e3<<" +\- "<<ResPz*1e3<<") MeV/c"<<endl;
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
