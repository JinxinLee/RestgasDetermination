#include <iostream.h>
#include <iomanip>
#include <set>
#include <vector.h>
#include <iterator>
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "PndEmcDigi.h"
#include "PndTofPoint.h"
#include "PndMvdMCPoint.h"
#include "PndTpcPoint.h"
#include "PndTofHit.h"
#include "PndEmcCluster.h"
#include "PndEmcMapper.h"
#include "PndEmcTwoCoordIndex.h"
#include "TROOT.h"
#include "TStyle.h"
#include <memory>
#include "TSystem.h"
#include "CbmMCTrack.h"
#include "CbmMCPoint.h"
#include "CbmTrackParH.h"
#include "TNtuple.h"
#include "TFile.h"
#include "PndPidCand.h"
#include "TGraph.h"
#include "TH3F.h"
#include "TColor.h"
using namespace std;
void histFill(TString const inf1, TString const inf2,TH2F &pvss,TH2F &pvslam,
              TH2F &pvstpcDEdx,TH2F &pvsmvdDEdx,TH2F &pvsMass,TH2F &pvsSinv, TH2F &pvsmvdP, Int_t const pdg_code)
{
 TFile *fsim1 = new TFile(inf1);
  TTree *tsim1 = ( TTree* )fsim1->Get("cbmsim");

  TFile *freco1 = new TFile(inf2);
  TTree *treco1 = ( TTree* )freco1->Get("cbmsim");

 TClonesArray *ArrMCTrack1 = new TClonesArray("CbmMCTrack");
 tsim1->SetBranchAddress("MCTrack",&ArrMCTrack1);

 TClonesArray *ArrTof1 = new TClonesArray("PndTofPoint");
 tsim1->SetBranchAddress("TofPoint",&ArrTof1);

 TClonesArray *ArrTpc1 = new TClonesArray("PndTpcPoint");
 tsim1->SetBranchAddress("PndTpcPoint",&ArrTpc1);

 TClonesArray *ArrMvd1 = new TClonesArray("PndMvdMCPoint");
 tsim1->SetBranchAddress("MVDPoint",&ArrMvd1);

 TClonesArray *ArrTofHit1 = new TClonesArray("PndTofHit");
 treco1->SetBranchAddress("TofHit",&ArrTofHit1);

 TClonesArray *ArrTrackPar1 = new TClonesArray("CbmTrackParH");
 treco1->SetBranchAddress("GeaneTrackPar",&ArrTrackPar1);

 TClonesArray *ArrPndPidCand1 = new TClonesArray("PndPidCand");
 treco1->SetBranchAddress("PndPidCand",&ArrPndPidCand1);
/*
 for (Int_t i =0; i < tsim1->GetEntries();i++)  {   
     tsim1->GetEntry(i);
     treco1->GetEntry(i);
cout<<inf1<<inf2<<setw(10)<<ArrMvd1->GetEntriesFast()<<setw(10)<<ArrTpc1->GetEntriesFast()<<endl;
}
*/


 for (Int_t i =0; i < tsim1->GetEntries();i++)     
     {
     tsim1->GetEntry(i);
     treco1->GetEntry(i);
     PndTofPoint *tof = (PndTofPoint *) ArrTof1->At(0);
     PndTofHit *tofHit = (PndTofHit *) ArrTofHit1->At(0);
     PndPidCand *track = (PndPidCand *) ArrPndPidCand1->At(0);
     CbmTrackParH *trackpar = (CbmTrackParH *) ArrTrackPar1->At(0);
     
     Double_t de_tpc=0;
     Double_t dx_tpc=0;
     for (Int_t j = 0; j < ArrTpc1->GetEntriesFast(); j++ )
     {
       PndTpcPoint *tpc = (PndTpcPoint *) ArrTpc1->At(j);
       if(tpc == 0)continue;
       Int_t trkID = tpc->GetTrackID();
//cout<<trkID<<"  "<<ArrMCTrack1->GetEntriesFast()<<endl;
       CbmMCTrack *mctrack_g = (CbmMCTrack *) ArrMCTrack1->At(trkID);
       Int_t pdg = mctrack_g->GetPdgCode();
       if (pdg == pdg_code )
        {
       de_tpc += tpc->GetEnergyLoss();
       dx_tpc += tpc->GetLength();
        }
     }


     Double_t de_mvd=0;
     Double_t dx_mvd=0;
     for (Int_t j = 0; j < ArrMvd1->GetEntriesFast(); j++ )
     {
       PndMvdMCPoint *mvd = (PndMvdMCPoint *) ArrMvd1->At(j);
       if(mvd == 0)continue;
       Int_t trkID = mvd->GetTrackID();
       CbmMCTrack *mctrack_g = (CbmMCTrack *) ArrMCTrack1->At(trkID);
       Int_t pdg = mctrack_g->GetPdgCode();
       if (pdg == pdg_code )
        {
        de_mvd += mvd->GetEnergyLoss();
//cout<<de_mvd<<endl;
        dx_mvd += mvd->GetLength();
        }
     }
if (track == NULL) continue;
if (tofHit == NULL) continue;
if (tof == NULL) continue;
if (trackpar == NULL) continue;

Double_t s = track->Get("speed");
Double_t p = track->Get("PMag");
Double_t gama2 = 1.0/(1-s*s);
Double_t m2 = p*p/(s*s*gama2);
Double_t tpc = de_tpc/dx_tpc*1000000;
Double_t mvd = de_mvd/dx_mvd*1000000;
//cout<<m<<setw(12)<<gama<<setw(12)<<s<<setw(12)<<p<<setw(12)<<de_mvd<<setw(12)<<dx_mvd<<setw(12)<<de_tpc<<setw(12)<<dx_tpc<<endl; 
//if ( track->Get("PDG") == 11 )
pvss.Fill(p,track->Get("speed"));
pvsSinv.Fill(p,log(1/s)*s);
pvsmvdP.Fill(p,mvd*p*p);
//pvslam.Fill(track->Get("PMag"),trackpar->GetLambda()/track->GetP());
pvstpcDEdx.Fill(p,tpc);
pvsmvdDEdx.Fill(p,mvd);             //  Kev/cm
//if ( track->Get("PDG") == 11 )
pvsMass.Fill(p,m2);
   }

}

double momentum(Double_t *v, Double_t *m)
{
  Double_t gama;
  gama = 1.0/sqrt(1-v[0]*v[0]);
  return gama*m[0]*v[0];
}


void ana2()
{

 TH2F  *h1 = new TH2F("h1p","P vs Speed",11000,0,3,60,0,2);
 TH2F  *h2 = new TH2F("h2p","P vs lambda",11000,0,3,60,-1,1);
 TH2F  *h3 = new TH2F("h3p","P vs tpc DEdx",11000,0,3,60,0,10);
 TH2F  *h4 = new TH2F("h4p","p vs mvd DEdx",11000,0,3,60,0,100);
 TH2F  *h5 = new TH2F("h5p","P vs m^2",11000,0,3,60,-2,3);
 TH2F  *h6 = new TH2F("h6p","P vs Speed inv",11000,0,3,60,0,10.0);
 TH2F  *h7 = new TH2F("h7p","p vs mvd P",11000,0,3,60,0,100);

 //cout<<"hai"<<endl;
 TH2F* pvss = new TH2F("h21","momentum vs Speed",50,0,3,60,0,2);
 TH2F *pvslam = new TH2F("h22","momentum vs lambda",50,0,5,100,-1,1);
 TH2F *pvstpcDEdx = new TH2F("h23","momentum vs tpcDeDx",50,0,5,100,0.1,10);
 TH2F *pvsmvdDEdx = new TH2F("h24","momentum vs mvdDeDx",50,0,5,300,0,100);
 TH2F *pvsMass = new TH2F("h25","momentum vs Mass",60,-3,3,100,-3,3);
 TH2F* pvsSinv = new TH2F("h26","momentum vs Speed inv ",50,0,3,600,0,10.0);
 TH2F *pvsmvdP = new TH2F("h27","momentum vs mvd P",50,0,5,300,0,100);

 TH2F* pvss1 = new TH2F("h31","momentum vs Speed",50,0,3,60,0,2);
 TH2F *pvslam1 = new TH2F("h32","momentum vs lambda",50,0,5,100,-1,1);
 TH2F *pvstpcDEdx1 = new TH2F("h33","momentum vs tpcDeDx",50,0,5,100,0.1,10);
 TH2F *pvsmvdDEdx1 = new TH2F("h34","momentum vs mvdDeDx",50,0,5,300,0,100);
 TH2F *pvsMass1 = new TH2F("h35","momentum vs Mass",60,-3,3,100,-3,3);
 TH2F* pvsSinv1 = new TH2F("h36","momentum vs Speed inv ",50,0,3,600,0,10.0);
 TH2F *pvsmvdP1 = new TH2F("h37","momentum vs mvd P",50,0,5,300,0,100);

 TH2F* pvss2 = new TH2F("h41","momentum vs Speed",50,0,3,60,0,2);
 TH2F *pvslam2 = new TH2F("h42","momentum vs lambda",50,0,5,100,-1,1);
 TH2F *pvstpcDEdx2 = new TH2F("h43","momentum vs tpcDeDx",50,0,5,100,0.1,10);
 TH2F *pvsmvdDEdx2 = new TH2F("h44","momentum vs mvdDeDx",50,0,5,300,0,100);
 TH2F *pvsMass2 = new TH2F("h45","momentum vs Mass",60,-3,3,100,-3,3);
 TH2F* pvsSinv2 = new TH2F("h46","momentum vs Speed inv ",50,0,3,600,0,10.0);
 TH2F *pvsmvdP2 = new TH2F("h47","momentum vs mvd P",50,0,5,300,0,100);

 TH2F* pvss3 = new TH2F("h51","momentum vs Speed",50,0,3,60,0,2);
 TH2F *pvslam3 = new TH2F("h52","momentum vs lambda",50,0,5,100,-1,1);
 TH2F *pvstpcDEdx3 = new TH2F("h53","momentum vs tpcDeDx",50,0,5,100,0.1,10);
 TH2F *pvsmvdDEdx3 = new TH2F("h54","momentum vs mvdDeDx",50,0,5,300,0,100);
 TH2F *pvsMass3 = new TH2F("h55","momentum vs Mass",60,-3,3,100,-3,3);
 TH2F* pvsSinv3 = new TH2F("h56","momentum vs Speed inv",50,0,3,600,0,10.0);
 TH2F *pvsmvdP3 = new TH2F("h57","momentum vs mvd P",50,0,5,300,0,100);


 TH2F* pvss4 = new TH2F("h61","momentum vs Speed",50,0,3,60,0,2);
 TH2F *pvslam4 = new TH2F("h62","momentum vs lambda",50,0,5,100,-1,1);
 TH2F *pvstpcDEdx4 = new TH2F("h63","momentum vs tpcDeDx",50,0,5,100,0.1,10);
 TH2F *pvsmvdDEdx4 = new TH2F("h64","momentum vs mvdDeDx",50,0,5,300,0,100);
 TH2F *pvsMass4 = new TH2F("h65","momentum vs Mass",60,-3,3,100,-3,3);
 TH2F* pvsSinv4 = new TH2F("h66","momentum vs Speed inv ",50,0,3,600,0,10.0);
 TH2F *pvsmvdP4 = new TH2F("h67","momentum vs mvd P",50,0,5,300,0,100);


pvss->SetMarkerColor(4);
pvslam->SetMarkerColor(4);
pvstpcDEdx->SetMarkerColor(4);
pvsmvdDEdx->SetMarkerColor(4);
pvsMass->SetMarkerColor(4);
pvsSinv->SetMarkerColor(4);
pvsmvdP->SetMarkerColor(4);

pvss1->SetMarkerColor(2);
pvslam1->SetMarkerColor(2);
pvstpcDEdx1->SetMarkerColor(2);
pvsmvdDEdx1->SetMarkerColor(2);
pvsMass1->SetMarkerColor(2);
pvsSinv1->SetMarkerColor(2);
pvsmvdP1->SetMarkerColor(2);

pvss2->SetMarkerColor(kGreen+2);
pvslam2->SetMarkerColor(kGreen+2);
pvstpcDEdx2->SetMarkerColor(kGreen+2);
pvsmvdDEdx2->SetMarkerColor(kGreen+2);
pvsMass2->SetMarkerColor(kGreen+2);
pvsSinv2->SetMarkerColor(kGreen+2);
pvsmvdP2->SetMarkerColor(kGreen+2);

pvss3->SetMarkerColor(kMagenta+1);
pvslam3->SetMarkerColor(kMagenta+1);
pvstpcDEdx3->SetMarkerColor(kMagenta+1);
pvsmvdDEdx3->SetMarkerColor(kMagenta+1);
pvsMass3->SetMarkerColor(kMagenta+1);
pvsSinv3->SetMarkerColor(kMagenta+1);
pvsmvdP3->SetMarkerColor(kMagenta+1);

pvss4->SetMarkerColor(12);
pvslam4->SetMarkerColor(12);
pvstpcDEdx4->SetMarkerColor(12);
pvsmvdDEdx4->SetMarkerColor(12);
pvsMass4->SetMarkerColor(12);
pvsSinv4->SetMarkerColor(12);
pvsmvdP4->SetMarkerColor(12);

 TString simFile,recoFile;

/*
 simFile="points_tpcmvdemc_piplus.root";
 recoFile="tracks_tpcmvdemc_piplus.root";
 histFill(simFile,recoFile,*pvss,*pvslam,*pvstpcDEdx,*pvsmvdDEdx,*pvsMass,211);
 cout<<" piplus "<<endl;



 simFile="points_tpcmvdemc_muminus.root";
 recoFile="tracks_tpcmvdemc_muminus.root";
 histFill(simFile,recoFile,*pvss1,*pvslam1,*pvstpcDEdx1,*pvsmvdDEdx1,*pvsMass1,13);
 cout<<" muminus "<<endl;

*/

 simFile="points_tpcmvdemc_eminus.root";
 recoFile="tracks_tpcmvdemc_eminus.root";
 histFill(simFile,recoFile,*pvss2,*pvslam2,*pvstpcDEdx2,*pvsmvdDEdx2,*pvsMass2,*pvsSinv2,*pvsmvdP2,11);
 cout<<" eminus "<<endl;


 simFile="points_tpcmvdemc_kplus.root";
 recoFile="tracks_tpcmvdemc_kplus.root";
 histFill(simFile,recoFile,*pvss3,*pvslam3,*pvstpcDEdx3,*pvsmvdDEdx3,*pvsMass3,*pvsSinv3,*pvsmvdP3,321);
 cout<<" kplus "<<endl;


 simFile="points_tpcmvdemc_proton.root";
 recoFile="tracks_tpcmvdemc_proton.root";
 histFill(simFile,recoFile,*pvss4,*pvslam4,*pvstpcDEdx4,*pvsmvdDEdx4,*pvsMass4,*pvsSinv4,*pvsmvdP4,2212);
 cout<<" proton "<<endl;

  TCanvas* c1 = new TCanvas("c1", "canvas", 100, 100, 1100, 1100);
  c1->Divide(2,2);
  c1->cd(1);
  h1->Draw();
  pvss->Draw("sameP");
  pvss1->Draw("sameP");
  pvss2->Draw("sameP");
  pvss3->Draw("sameP");
  pvss4->Draw("sameP");
//  c1->cd(2);
//  h2->Draw();
//  pvslam->Draw("sameP");
//  pvslam1->Draw("sameP");
//  pvslam2->Draw("sameP");
  c1->cd(2);
  h3->Draw();
  pvstpcDEdx->Draw("sameP");
  pvstpcDEdx1->Draw("sameP");
  pvstpcDEdx2->Draw("sameP");
  pvstpcDEdx3->Draw("sameP");
  pvstpcDEdx4->Draw("sameP");
  c1->cd(3);
  h4->Draw();
  pvsmvdDEdx->Draw("sameP");
  pvsmvdDEdx1->Draw("sameP");
  pvsmvdDEdx2->Draw("sameP");
  pvsmvdDEdx3->Draw("sameP");
  pvsmvdDEdx4->Draw("sameP");
  c1->cd(4);
  h5->Draw();
  pvsMass->Draw("sameP");
  pvsMass1->Draw("sameP");
  pvsMass2->Draw("sameP");
  pvsMass3->Draw("sameP");
  pvsMass4->Draw("sameP");
  c1->Update();

/*
Double_t par[1]={0.938};
TF1 *proton = new TF1("f",momentum,0,0.99,1);
 proton->SetParameters(par);
 proton->SetLineColor(4);

  Double_t par1[1]={0.10565};
  TF1 *muon= new TF1("f1",momentum,0,0.999999999999,1);
  muon->SetParameters(par1);
  muon->SetLineColor(2);

  Double_t par2[1]={0.13957};
  TF1 *piplus= new TF1("f2",momentum,0,0.999999999,1);
  piplus->SetParameters(par2);
  piplus->SetLineColor(kGreen+2);
*/

  TCanvas* c2 = new TCanvas("c2", "canvas", 100, 100, 1100, 1100);
  c2->Divide(2,2);
  c2->cd(1);
  h6->Draw();
  pvsSinv->Draw("sameP");
  pvsSinv1->Draw("sameP");
  pvsSinv2->Draw("sameP");
  pvsSinv3->Draw("sameP");
  pvsSinv4->Draw("sameP");

  c2->cd(2);
  h7->Draw();
  pvsmvdP->Draw("sameP");
  pvsmvdP1->Draw("sameP");
  pvsmvdP2->Draw("sameP");
  pvsmvdP3->Draw("sameP");
  pvsmvdP4->Draw("sameP");

}
