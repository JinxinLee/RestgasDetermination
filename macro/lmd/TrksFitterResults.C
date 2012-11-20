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
void TrksFitterResults(TString fileName="")
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
  cout<<"***************************************"<<endl;
  cout<<" "<<endl;

  ///Resolution & Pull distributions
  TH1F* hResPointPx = (TH1F*)fileIN->Get("hResPointPx");
  TF1 *fResPx = (TF1*)hResPointPx->GetFunction("fitcoord");
  double ResPx = fResPx->GetParameter(2);

  TH1F* hResPointPy = (TH1F*)fileIN->Get("hResPointPy");
  TF1 *fResPy = (TF1*)hResPointPy->GetFunction("fitcoord");
  double ResPy = fResPy->GetParameter(2);

  TH1F* hResPointPz = (TH1F*)fileIN->Get("hResPointPz");
  TF1 *fResPz = (TF1*)hResPointPz->GetFunction("fitcoord");
  double ResPz = fResPz->GetParameter(2);

  TH1F* hPullPointPx = (TH1F*)fileIN->Get("hPullPointPx");
  TF1 *fPullPx = (TF1*)hPullPointPx->GetFunction("fitp");
  double PullPx_mean = fPullPx->GetParameter(1);
  double PullPx_sigma = fPullPx->GetParameter(2);

  TH1F* hPullPointPy = (TH1F*)fileIN->Get("hPullPointPy");
  TF1 *fPullPy = (TF1*)hPullPointPy->GetFunction("fitp");
  double PullPy_mean = fPullPy->GetParameter(1);
  double PullPy_sigma = fPullPy->GetParameter(2);
  
  TH1F* hPullPointPz = (TH1F*)fileIN->Get("hPullPointPz");
  TF1 *fPullPz = (TF1*)hPullPointPz->GetFunction("fitp");
  double PullPz_mean = fPullPz->GetParameter(1);
  double PullPz_sigma = fPullPz->GetParameter(2);


  TH1F* hResPointX = (TH1F*)fileIN->Get("hResPointX");
  TF1 *fResX = (TF1*)hResPointX->GetFunction("fitcoord");
  double ResX = fResX->GetParameter(2);

  TH1F* hResPointY = (TH1F*)fileIN->Get("hResPointY");
  TF1 *fResY = (TF1*)hResPointY->GetFunction("fitcoord");
  double ResY = fResY->GetParameter(2);

  TH1F* hResPointZ = (TH1F*)fileIN->Get("hResPointZ");
  TF1 *fResZ = (TF1*)hResPointZ->GetFunction("fitcoord");
  double ResZ = fResZ->GetParameter(2);

  TH1F* hPullPointX = (TH1F*)fileIN->Get("hPullPointX");
  TF1 *fPullX = (TF1*)hPullPointX->GetFunction("fitp");
  double PullX_mean = fPullX->GetParameter(1);
  double PullX_sigma = fPullX->GetParameter(2);

  TH1F* hPullPointY = (TH1F*)fileIN->Get("hPullPointY");
  TF1 *fPullY = (TF1*)hPullPointY->GetFunction("fitp");
  double PullY_mean = fPullY->GetParameter(1);
  double PullY_sigma = fPullY->GetParameter(2);
  
  TH1F* hPullPointZ = (TH1F*)fileIN->Get("hPullPointZ");
  double PullZ_mean=-100;
  double PullZ_sigma=-100;
  if(hPullPointZ->GetRMS()!=0){
    TF1 *fPullZ = (TF1*)hPullPointZ->GetFunction("fitp");
    PullZ_mean = fPullZ->GetParameter(1);
    PullZ_sigma = fPullZ->GetParameter(2);
  }

  TH2F* hiniResMCTheta = (TH2F*)fileIN->Get("hMCThetaResSeedTheta");
  TH1D* hiniResTheta = (TH1D*)hiniResMCTheta->ProjectionY();
  TF1 *funrth = new TF1("fitrth","gaus",-0.01,0.01);
  funrth->SetParameters(100,0,3e-3);
  funrth->SetParNames("Constant","Mean","Sigma");
  hiniResTheta->Fit(funrth,"r");
  double thetaResIni = funrth->GetParameter(2);

  TH1F* hResTheta = (TH1F*)fileIN->Get("hResTheta");
  TF1 *fResTheta = (TF1*)hResTheta->GetFunction("fitrth");
  double ResTheta = fResTheta->GetParameter(2);

  TH1F* hPullTheta = (TH1F*)fileIN->Get("hPullTheta");
  TF1 *fPullTheta = (TF1*)hPullTheta->GetFunction("fitp");
  double PullTheta_mean = fPullTheta->GetParameter(1);
  double PullTheta_sigma = fPullTheta->GetParameter(2);

  TH1F* hResPhi = (TH1F*)fileIN->Get("hResPhi");
  TF1 *fResPhi = (TF1*)hResPhi->GetFunction("fitrphi");
  double ResPhi = fResPhi->GetParameter(2);

  TH1F* hPullPhi = (TH1F*)fileIN->Get("hPullPhi");
  TF1 *fPullPhi = (TF1*)hPullPhi->GetFunction("fitp");
  double PullPhi_mean = fPullPhi->GetParameter(1);
  double PullPhi_sigma = fPullPhi->GetParameter(2);

  cout<<" Resolution:"<<endl;
  cout<<" initial res theta (fitted trk, near LMD) = "<<thetaResIni*1e6<<" mkrad"<<endl;
  cout<<" Res PCA (X, Y, Z) = ("<<ResX<<", "<<ResY<<", "<<ResZ<<") cm"<<endl;
  cout<<" Res Mom (Px, Py, Pz) = ("<<ResPx*1e3<<", "<<ResPy*1e3<<", "<<ResPz*1e3<<") MeV/c"<<endl;
  cout<<" Res Angles (Theta, Phi) = ("<<ResTheta*1e6<<", mkrad; "<<ResPhi*1e3<<", mrad)"<<endl;
  cout<<"***************************************"<<endl;
  cout<<" "<<endl;

  cout<<" Pulls:"<<endl;
  cout<<" Pull PCA (X, Y, Z) = ("<<PullX_mean<<"+/-"<<PullX_sigma<<", "<<PullY_mean<<"+/-"<<PullY_sigma<<", "<<PullZ_mean<<"+/-"<<PullZ_sigma<<")"<<endl;
  cout<<" Pull Mom (Px, Py, Pz) = ("<<PullPx_mean<<"+/-"<<PullPx_sigma<<", "<<PullPy_mean<<"+/-"<<PullPy_sigma<<", "<<PullPz_mean<<"+/-"<<PullPz_sigma<<")"<<endl;
  cout<<" Pull Angles (Theta, Phi) = ("<<PullTheta_mean<<"+/-"<<PullTheta_sigma<<"; "
      <<PullPhi_mean<<"+/-"<<PullTheta_sigma<<")"<<endl;
  cout<<"***************************************"<<endl;
  cout<<" "<<endl;

}
