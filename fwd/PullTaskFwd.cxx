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
#include "PullTaskFwd.h"

#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "TClonesArray.h"
#include "Track.h"
#include "TH1F.h"
#include "TFile.h"

#include "PndGemTrack.h"
#include "PndMCTrack.h"

#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "GeaneTrackRep.h"


PullTaskFwd::PullTaskFwd(){
}

PullTaskFwd::~PullTaskFwd(){}

InitStatus PullTaskFwd::Init()
{

   hhrecop = new TH1F("hhrecop", "lhe reco p: mc - reco", 100, 0.5, 1.5);
  hgrecop = new TH1F("hgrecop", "gen reco p: mc - reco", 100, 0.5, 1.5);
  // on plane - pull
  hp = new TH1F("hp", "p: mc - reco", 50,-10., 10.);
  htv = new TH1F("htv", "tv: mc - reco", 50,-10.,10.);
  htw = new TH1F("htw", "tw: mc - reco", 50,-10.,10.);
  hv  = new TH1F("hv",  "v: mc - reco", 50,-10.,10.);
  hw  = new TH1F("hw",  "w: mc - reco", 50,-10.,10.);
  // on plane - residuals
  hrpx = new TH1F("hrpx", "px: mc - reco", 50,-1.,1.);
  hrpy = new TH1F("hrpy", "py: mc - reco", 50,-1.,1.);
  hrpz = new TH1F("hrpz", "pz: mc - reco", 50,-1.,1.);
  hrp  = new TH1F("hrp", "p: mc - reco", 300,-0.3, 0.3);
  hrtv = new TH1F("hrtv", "tv: mc - reco", 300,-0.3,0.3);
  hrtw = new TH1F("hrtw", "tw: mc - reco", 300,-0.3,0.3);
  hrv  = new TH1F("hrv",  "v: mc - reco", 50,-1.,1.);
  hrw  = new TH1F("hrw",  "w: mc - reco", 50,-1.,1.);

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PullTaskFwd::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection FSTracks
  fTrackArray = (TClonesArray*) ioman->GetObject("FSTracks");
  if(fTrackArray == 0)
    {
      Error("PullTaskFwd::Init","FSTrack-array not found!");
      return kERROR;
    }
  
  // Get input collection MCTracks
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if(fMCTrackArray == 0)
    {
      Error("PullTaskFwd::Init","MCTrack-array not found!");
      return kERROR;
    }
  

  // open PndGemTrack array
  fGemTrackArray = (TClonesArray*) ioman->GetObject("PndGemTrack");
  if(fGemTrackArray == 0){
    Error("PullTaskFwd::Init","PndGemTrack-array not found!");
    return kERROR;
  }
  evtcounter = 0;
  return kSUCCESS;
}


void PullTaskFwd::Exec(Option_t* opt)
{
  if(evtcounter%100 == 0) std::cout << evtcounter << std::endl;
  evtcounter++;

  Int_t ntracks = fTrackArray->GetEntriesFast();
  if(ntracks>1) std::cout << "exec "  << ntracks << " tracks " << std::endl;

  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
  Int_t nGemTracks = fGemTrackArray->GetEntriesFast();

  if(nGemTracks != nMCTracks){
    std::cout<<"PullTaskFwd: I do not know which Tracks corresponds to which MCTrack"<<std::endl;
    //    return;
  }

  for(Int_t itr=0;itr < ntracks;++itr){

    Track *genTrack = (Track*) fTrackArray->At(itr);
    if(!genTrack || genTrack->getTrackRep(0)->getStatusFlag()!=0 ){
      std::cout<<"Pull Task: Track not existing or fitting unsuccessful"<<std::endl;
      continue;
    }

    //get corresponding MCTrack
    PndGemTrack* gemTrack = (PndGemTrack*) fGemTrackArray->At(itr);
    Int_t mcidx = (Int_t)(gemTrack->GetParamLast()->GetZ()-1e6);
    PndMCTrack* MCTrack = (PndMCTrack*) fMCTrackArray->At(mcidx);

  
   // fill pull histo
    DetPlane O_pl(TVector3(0,0,0),TVector3(1,0,0),TVector3(0,1,0));
    GeaneTrackRep* grep = dynamic_cast<GeaneTrackRep*>(genTrack->getCardinalRep());
    
    DetPlane refPlane = grep->getReferencePlane();

    FairGeanePro *geane = new FairGeanePro();
    grep->setPropagator(geane);
    
    grep->setPropDir(-1);
    TMatrixT<double> statePred(5,1), covPred(5,5);
    TVector3 O_mom;
    
    grep->extrapolate(O_pl, 
		      statePred,
		      covPred);

    // update grep to pca of IP
    grep->setState(statePred);
    grep->setCov(covPred);
    grep->setReferencePlane(O_pl);

    O_mom = grep->getMom(grep->getReferencePlane());
    // fill this histo with the reco momentum
    hgrecop->Fill(O_mom.Mag());

    Double_t err_qp = sqrt(covPred[0][0]);
    Double_t err_tv = sqrt(covPred[1][1]);
    Double_t err_tw = sqrt(covPred[2][2]);
    Double_t err_v  = sqrt(covPred[3][3]);
    Double_t err_w  = sqrt(covPred[4][4]);
    
    // momentum
    if(err_qp!=0)  hp->Fill((MCTrack->GetMomentum().Mag() - O_mom.Mag())/err_qp);
    hrp->Fill((MCTrack->GetMomentum().Mag() - O_mom.Mag()));

    hrpx->Fill(MCTrack->GetMomentum().X()  - O_mom.X());
    hrpy->Fill(MCTrack->GetMomentum().Y()  - O_mom.Y());
    hrpz->Fill(MCTrack->GetMomentum().Z()  - O_mom.Z()); 
    
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


//_________________________________________________________________
void PullTaskFwd::WriteHistograms(){


  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("PullTaskFwd");
  file->cd("PullTaskFwd");
  
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

ClassImp(PullTaskFwd);
