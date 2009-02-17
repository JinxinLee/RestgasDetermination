/*

this macro is plot the speed and 
de/dx from mvd and tpc of two particle types
 
 */
#include <iostream.h>
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
#include "PndTrack.h"
#include "TGraph.h"
#include "TH3F.h"
#include "TColor.h"

using namespace std;
void ana()
{
// TChain *t2 = new TChain("cbmsim");
// t2->Add("eminus_sim.root");

 TH2F  *h1 = new TH2F("h1p","speed",11000,0,1.5,60,0,200);
 TH2F  *h2 = new TH2F("h2p","lambda",11000,-1,1,60,0,200);
 TH2F  *h3 = new TH2F("h3p","tpc->DEdx",11000,0,10,60,0,200);
 TH2F  *h4 = new TH2F("h4p","mvd->DEdx",11000,0,20,60,0,200);

  TFile *fsim = new TFile("points_tpcmvdemc_muplus.root");
  TTree *tsim = ( TTree* )fsim->Get("cbmsim");

  TFile *freco = new TFile("tracks_tpcmvdemc_muplus.root");
  TTree *treco = ( TTree* )freco->Get("cbmsim");

 TClonesArray *ArrMCTrack = new TClonesArray("CbmMCTrack");
 tsim->SetBranchAddress("MCTrack",&ArrMCTrack);

 TClonesArray *ArrTof = new TClonesArray("PndTofPoint");
 tsim->SetBranchAddress("TofPoint",&ArrTof);

 TClonesArray *ArrTpc = new TClonesArray("PndTpcPoint");
 tsim->SetBranchAddress("PndTpcPoint",&ArrTpc);

 TClonesArray *ArrMvd = new TClonesArray("PndMvdMCPoint");
 tsim->SetBranchAddress("MVDPoint",&ArrMvd);

 TClonesArray *ArrTofHit = new TClonesArray("PndTofHit");
 treco->SetBranchAddress("TofHit",&ArrTofHit);

 TClonesArray *ArrTrackPar = new TClonesArray("CbmTrackParH");
 treco->SetBranchAddress("GeaneTrackPar",&ArrTrackPar);

 TClonesArray *ArrPndTrack = new TClonesArray("PndTrack");
 treco->SetBranchAddress("PndTrack",&ArrPndTrack);

 TH2F *pvss = new TH2F("h21","momentum vs Speed",50,0,5,60,0,1.5);
 TH2F *pvslam = new TH2F("h22","momentum vs lambda",50,0,5,100,-1,1);
 TH2F *pvstpcDEdx = new TH2F("h23","momentum vs tpcDeDx",50,0,5,100,0.1,10);
 TH2F *pvsmvdDEdx = new TH2F("h24","momentum vs mvdDeDx",50,0,5,100,0.1,10);

 TH1F *pvss0 = new TH1F("h1","momentum vs Speed",60,0,1.5);
 TH1F *pvslam0 = new TH1F("h2","momentum vs lambda",100,-1,1);
 TH1F *pvstpcDEdx0 = new TH1F("h3","momentum vs tpcDeDx",100,0.1,10);
 TH1F *pvsmvdDEdx0 = new TH1F("h4","momentum vs mvdDeDx",100,0.1,10);
 pvss0->SetLineColor(4);
 pvslam0->SetLineColor(4);
 pvstpcDEdx0->SetLineColor(4);
 pvsmvdDEdx0->SetLineColor(4);
  cout<<"no of events = "<<tsim->GetEntries()<<endl;

     for (Int_t i =0; i < tsim->GetEntries();i++)     
     {
     tsim->GetEntry(i);
     treco->GetEntry(i);
/*
cout<<" "<<ArrMCTrack->GetEntriesFast()
    <<" "<<ArrTof->GetEntriesFast()
    <<" "<<ArrTofHit->GetEntriesFast()
    <<" "<<ArrTrackPar->GetEntriesFast()
    <<" "<<ArrPndTrack->GetEntriesFast()<<endl;
*/

     
  //   CbmMCTrack *mctrack_g = (CbmMCTrack *) ArrMCTrack->At(0);
     PndTofPoint *tof = (PndTofPoint *) ArrTof->At(0);
     PndTofHit *tofHit = (PndTofHit *) ArrTofHit->At(0);
     PndTrack *track = (PndTrack *) ArrPndTrack->At(0);
     CbmTrackParH *trackpar = (CbmTrackParH *) ArrTrackPar->At(0);
     Double_t de_tpc=0;
     Double_t dx_tpc=0;
     for (Int_t j = 0; j < ArrTpc->GetEntriesFast(); j++ )
     {
       PndTpcPoint *tpc = (PndTpcPoint *) ArrTpc->At(j);
       Int_t trkID = tpc->GetTrackID();
       CbmMCTrack *mctrack_g = (CbmMCTrack *) ArrMCTrack->At(trkID);
       Int_t pdg = mctrack_g->GetPdgCode();
//cout<<pdg<<endl;
       if (pdg == -13 )
        {
       de_tpc += tpc->GetEnergyLoss();
       dx_tpc += tpc->GetLength();
        }
     }

     Double_t de_mvd=0;
     Double_t dx_mvd=0;
     for (Int_t j = 0; j < ArrMvd->GetEntriesFast(); j++ )
     {
       PndMvdMCPoint *mvd = (PndMvdMCPoint *) ArrTpc->At(j);
       if(mvd == 0)continue;
       Int_t trkID = mvd->GetTrackID();
       CbmMCTrack *mctrack_g = (CbmMCTrack *) ArrMCTrack->At(trkID);
       Int_t pdg = mctrack_g->GetPdgCode();
       if (pdg == -13 )
        {
        de_mvd += mvd->GetEnergyLoss();
        dx_mvd += mvd->GetLength();
        }
     }

if (track == NULL) continue;
if (tofHit == NULL) continue;
if (tof == NULL) continue;
if (trackpar == NULL) continue;
// cout<<tof->GetTime()<<"  "<<track->GetSpeed()<<"  "<<i<<"  "<<de_tpc/dx_tpc<<"  "<<de_mvd/dx_mvd<<endl;

pvss->Fill(track->GetP(),track->GetSpeed());
pvslam->Fill(track->GetP(),trackpar->GetLambda()/track->GetP());
pvstpcDEdx->Fill(track->GetP(),de_tpc/dx_tpc*1000000);
pvsmvdDEdx->Fill(track->GetP(),de_mvd/dx_mvd*1000000);

pvss0->Fill(track->GetSpeed());
pvslam0->Fill(trackpar->GetLambda()/track->GetP());
pvstpcDEdx0->Fill(de_tpc/dx_tpc*1000000);
pvsmvdDEdx0->Fill(de_mvd/dx_mvd*1000000);
     }


/*
 this part for the anotherparticle
*/
 TH1F *pvss1 = new TH1F("h5","momentum vs Speed",60,0,1.5);
 TH1F *pvslam1 = new TH1F("h6","momentum vs lambda",100,-1,1);
 TH1F *pvstpcDEdx1 = new TH1F("h7","momentum vs tpcDeDx",100,0.1,10);
 TH1F *pvsmvdDEdx1 = new TH1F("h8","momentum vs mvdDeDx",100,0.1,20);
 pvss1->SetLineColor(2);
 pvslam1->SetLineColor(2);
 pvstpcDEdx1->SetLineColor(2);
 pvsmvdDEdx1->SetLineColor(2);
{
 TFile *fsim1 = new TFile("points_tpcmvdemc_proton.root");
  TTree *tsim1 = ( TTree* )fsim1->Get("cbmsim");

  TFile *freco1 = new TFile("tracks_tpcmvdemc_proton.root");
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

 TClonesArray *ArrPndTrack1 = new TClonesArray("PndTrack");
 treco1->SetBranchAddress("PndTrack",&ArrPndTrack1);



 for (Int_t i =0; i < tsim1->GetEntries();i++)     
     {
     tsim1->GetEntry(i);
     treco1->GetEntry(i);
     PndTofPoint *tof = (PndTofPoint *) ArrTof1->At(0);
     PndTofHit *tofHit = (PndTofHit *) ArrTofHit1->At(0);
     PndTrack *track = (PndTrack *) ArrPndTrack1->At(0);
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
       if (pdg == 2212 )
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
       if (pdg == 2212 )
        {
        de_mvd += mvd->GetEnergyLoss();
        dx_mvd += mvd->GetLength();
        }
     }
if (track == NULL) continue;
if (tofHit == NULL) continue;
if (tof == NULL) continue;
if (trackpar == NULL) continue;

pvss1->Fill(track->GetSpeed());
pvslam1->Fill(trackpar->GetLambda()/track->GetP());
pvstpcDEdx1->Fill(de_tpc/dx_tpc*1000000);
pvsmvdDEdx1->Fill(de_mvd/dx_mvd*1000000);
   }
  }



  TCanvas* c1 = new TCanvas("c1", "canvas", 100, 100, 1100, 1100);
  c1->Divide(2,2);
  c1->cd(1);
  h1->Draw();
  pvss1->Draw("same");
  pvss0->Draw("same");
  c1->cd(2);
  h2->Draw();
  pvslam0->Draw("same");
  pvslam1->Draw("same");
  c1->cd(3);
  h3->Draw();
  pvstpcDEdx0->Draw("same");
  pvstpcDEdx1->Draw("same");
  c1->cd(4);
  h4->Draw();
  pvsmvdDEdx1->Draw("same");
  pvsmvdDEdx0->Draw("same");
  c1->Update();


  TCanvas* c2 = new TCanvas("c2", "canvas", 100, 100, 1100, 1100);
  c2->Divide(2,2);
  c2->cd(1);
  pvss->Draw("colz");
  c2->cd(2);
  pvslam->Draw("colz");
  c2->cd(3);
  pvstpcDEdx->Draw("colz");
  c2->cd(4);
  pvsmvdDEdx->Draw("colz");
  c2->Update();


}

