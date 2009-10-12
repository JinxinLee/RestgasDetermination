// =========================================================================
// To be used in single particle events
// cut1 : on the pdg code
// cut2 : (optional) on the number of hits
// cut3 : eliminate tracks whose covPred is full of 0
//
// the track is back extrapolated to the plane O(0,0,0) U(1,0,0) V(0,1,0) and 
// there the pull are made
// 
// in the MC track array the primary generated particle is taken
// in the gen (lhe) track array all the particles with correct pdg are taken
//
// The track is extrapolated to the xy plane in order to compare it in SD
// directly with MC track in MARS
// =========================================================================

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndDchPullTask.h"

#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "TH1F.h"
#include "TFile.h"

#include "PndMCTrack.h"

#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "GeaneTrackRep.h"

#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TStyle.h"


PndDchPullTask::PndDchPullTask(){
}

PndDchPullTask::~PndDchPullTask(){}

InitStatus PndDchPullTask::Init()
{

   hhrecop = new TH1F("hhrecop", "lhe reco p: mc - reco", 100, 0.5, 1.5);
  hgrecop = new TH1F("hgrecop", "gen reco p: mc - reco", 100, 0.5, 1.5);
  // on plane - pull
  hp = new TH1F("hp", "p: pull((mc-reco)/#sigma_{q/p})", 50,-10., 10.);
  htv = new TH1F("htv", "tv: pull((mc-reco)/#sigma_{tv})", 50,-10.,10.);
  htw = new TH1F("htw", "tw: pull((mc - reco)/#sigma_{tw})", 50,-10.,10.);
  hv  = new TH1F("hv",  "v: pull((mc - reco)/#sigma_{v})", 50,-10.,10.);
  hw  = new TH1F("hw",  "w: pull((mc - reco)/#sigma_{w})", 50,-10.,10.);
  // on plane - residuals
  Double_t range = 0.3;
  hrpx = new TH1F("hrpx", "px: mc - reco", 50,-1.,1.);
  hrpy = new TH1F("hrpy", "py: mc - reco", 50,-1.,1.);
  hrpz = new TH1F("hrpz", "pz: mc - reco", 50,-1.,1.);
  hrp  = new TH1F("hrp", "p: mc - reco", 100,-0.2, 0.2); //-range,range
  hrtv = new TH1F("hrtv", "tv: mc - reco", 100,-0.2,0.2);
  hrtw = new TH1F("hrtw", "tw: mc - reco", 100,-0.2,0.2);
  hrv  = new TH1F("hrv",  "v: mc - reco", 100,-10.,10.);
  hrw  = new TH1F("hrw",  "w: mc - reco", 100,-10.,10.);

  //  hpreco = new TH1F("hpreco","h reco momentum",100,-2,2);
  gaussRes = new TF1("gaussRes","gaus",-0.1,0.1); //gaussian for residual distribution
  gaussPull = new TF1("gaussPull","gaus",-10,10); //gaussian for pull distridution
  gaussPull->SetLineColor(kBlue);
  gaussRes->SetLineColor(kRed);

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndDchPullTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection FSTracks
  fTrackArray = (TClonesArray*) ioman->GetObject("FSTracks");
  if(fTrackArray == 0)
    {
      Error("PndDchPullTask::Init","FSTrack-array not found!");
      return kERROR;
    }
  
  // Get input collection MCTracks
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if(fMCTrackArray == 0)
    {
      Error("PndDchPullTask::Init","MCTrack-array not found!");
      return kERROR;
    }
  

  evtcounter = 0;
  return kSUCCESS;
}


void PndDchPullTask::Exec(Option_t* opt)
{
  if(evtcounter%100 == 0) std::cout << evtcounter << std::endl;
  evtcounter++;

  Int_t ntracks = fTrackArray->GetEntriesFast();
  if(ntracks>1) std::cout << "exec "  << ntracks << " tracks " << std::endl;

  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
  //std::cout<<"nTraks and MC"<<ntracks<<"-"<<nMCTracks<<std::endl;/////////////

  int pdgnum = 0;
  PndMCTrack *MCTrack, *iMCTrack;
  for(Int_t mcitr = 0; mcitr < nMCTracks; mcitr++)  {
    // There is ONLY ONE muon generated ==> fix the MC track with the right pdg!
    iMCTrack = (PndMCTrack*) fMCTrackArray->At(mcitr);
    if(!iMCTrack) continue;
    if(iMCTrack->GetPdgCode() != 13) continue; //13=muon, 2212=proton
    MCTrack = (PndMCTrack*) fMCTrackArray->At(mcitr);
    pdgnum++;
  }
  if(pdgnum!=1){
    std::cout<<"More than one muons in MC - ambiguity"<<std::endl;
    return;
  }

  for(Int_t itr=0;itr < ntracks;++itr){

    GFTrack *genTrack = (GFTrack*) fTrackArray->At(itr);
    if(!genTrack || genTrack->getTrackRep(0)->getStatusFlag()!=0 ){
      std::cout<<"Pull Task: Track not existing or fitting unsuccessful"<<std::endl;
      continue;
    }

   // fill pull histo
    GFDetPlane pl(TVector3(0,0,0),TVector3(1,0,0),TVector3(0,1,0));
    GFAbsTrackRep* rep = genTrack->getCardinalRep()->clone();
    //GeaneTrackRep* grep = dynamic_cast<GeaneTrackRep*>(genTrack->getCardinalRep());
    GeaneTrackRep* grep = dynamic_cast<GeaneTrackRep*>(rep);
    
    GFDetPlane refPlane = grep->getReferencePlane();

    FairGeanePro *geane = new FairGeanePro();
    grep->setPropagator(geane);
    
    grep->setPropDir(-1);
    TMatrixT<double> statePred(5,1), covPred(5,5);
    //    TVector3 O_mom;
    TVector3 momtest;
    grep->extrapolate(pl, 
 		      statePred,
 		      covPred);

    // update grep to pca of IP
    grep->setState(statePred);
    grep->setCov(covPred);
    grep->setReferencePlane(pl);

    momtest = grep->getMom(grep->getReferencePlane());
    // fill this histo with the reco momentum
    hgrecop->Fill(momtest.Mag());

    //std::cout<<"momtest------->"<<momtest.Mag()<<std::endl;

    Double_t err_qp = sqrt(covPred[0][0]);
    Double_t err_tv = sqrt(covPred[1][1]);
    Double_t err_tw = sqrt(covPred[2][2]);
    Double_t err_v  = sqrt(covPred[3][3]);
    Double_t err_w  = sqrt(covPred[4][4]);
    
    // momentum
    if(err_qp!=0)  hp->Fill((MCTrack->GetMomentum().Mag() - momtest.Mag())/err_qp);
    hrp->Fill((MCTrack->GetMomentum().Mag() - momtest.Mag()));

    hrpx->Fill(MCTrack->GetMomentum().X()  - momtest.X());
    hrpy->Fill(MCTrack->GetMomentum().Y()  - momtest.Y());
    hrpz->Fill(MCTrack->GetMomentum().Z()  - momtest.Z()); 
    
    // v'/w'
    Double_t mc_tv = (MCTrack->GetMomentum().X()/MCTrack->GetMomentum().Z());
    Double_t mc_tw = (MCTrack->GetMomentum().Y()/MCTrack->GetMomentum().Z());
    if(err_tv!=0)   htv->Fill((mc_tv - statePred[1][0])/err_tv);
    hrtv->Fill((mc_tv - statePred[1][0]));
    if(err_tw!=0)   htw->Fill((mc_tw - statePred[2][0])/err_tw);
    hrtw->Fill((mc_tw - statePred[2][0]));
    
    // v, w
    if(err_v!=0)    hv->Fill((MCTrack->GetStartVertex().X() - statePred[3][0])/err_v);
    hrv->Fill((MCTrack->GetStartVertex().X() - statePred[3][0]));
    if(err_w!=0)    hw->Fill((MCTrack->GetStartVertex().Y() - statePred[4][0])/err_w);
    hrw->Fill((MCTrack->GetStartVertex().Y() - statePred[4][0]));
    
}

 
  return;
}

void PndDchPullTask::PlotHistograms(){
  gStyle->SetOptStat(111111);
  gStyle->SetOptFit(0101);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.15);
  gStyle->SetStatColor(kWhite);
  fCanvas = new TCanvas("pullCanvas","pull GeaneTrackRep",1200,800);
  fCanvasRes = new TCanvas("resCanvas","residual GeaneTrackRep", 1200,800);

  fCanvas->SetFillColor(kWhite);
  fCanvas->Divide(3,2);

  fCanvas->cd(1);
  hp->SetFillColor(kRed);
  hp->Draw();
  hp->Fit(gaussPull,"RQ");

  fCanvas->cd(2);
  htv->SetFillColor(kRed);
  htv->Draw();
  htv->Fit(gaussPull,"RQ");

  fCanvas->cd(3);
  htw->SetFillColor(kRed);
  htw->Draw();
  htw->Fit(gaussPull,"RQ");

  fCanvas->cd(4);
  hv->SetFillColor(kRed);
  hv->Draw();
  hv->Fit(gaussPull,"RQ");

  fCanvas->cd(5);
  hw->SetFillColor(kRed);
  hw->Draw();
  hw->Fit(gaussPull,"RQ");
  
  fCanvasRes->SetFillColor(kWhite);
  fCanvasRes->Divide(3,2);
  fCanvasRes->cd(1);
  hrp->SetFillColor(kBlue);
  hrp->Draw();
  hrp->Fit(gaussRes,"RQ");

  fCanvasRes->cd(2);
  hrtv->SetFillColor(kBlue);
  hrtv->Draw();
  hrtv->Fit(gaussRes,"RQ");

  fCanvasRes->cd(3);
  hrtw->SetFillColor(kBlue);
  hrtw->Draw();
  hrtw->Fit(gaussRes,"RQ");

  fCanvasRes->cd(4);
  hrv->SetFillColor(kBlue);
  hrv->Draw();

  fCanvasRes->cd(5);
  hrw->SetFillColor(kBlue);
  hrw->Draw();
  

}
//_________________________________________________________________
void PndDchPullTask::WriteHistograms(){


  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("PullTask");
  file->cd("PullTask");
  
  hhrecop->Write();
  delete hhrecop;
  hgrecop->Write();
  delete hgrecop;
  hp->Write();
  delete hp;
  hrpx->Write();
  delete hrpx;
  hrpy->Write();
  delete hrpy;
  hrpz->Write();
  delete hrpz;
  htv->Write();
  delete htv;
  htw->Write();
  delete htw;
  hv->Write();
  delete hv;
  hw->Write();
  delete hw;

   hrp->Write();
  delete hrp;
  hrtv->Write();
  delete hrtv;
  hrtw->Write();
  delete hrtw;
  hrv->Write();
  delete hrv;
  hrw->Write();
  delete hrw;

}

ClassImp(PndDchPullTask);
