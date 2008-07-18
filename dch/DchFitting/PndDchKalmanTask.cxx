
// Panda Headers ----------------------
#include "CbmRootManager.h"
#include "PndDchKalmanTask.h"
#include "Track.h"
#include "PndDchRecoHit.h"
#include "PndDchCylinderHit.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// ROOT Headers --------
#include "TClonesArray.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"

// Class Member definitions -----------


PndDchKalmanTask::PndDchKalmanTask()
  : CbmTask("Kalman Filter"), fPersistence(kFALSE),fSmooth(kFALSE), fEvtCount(0)
{
  fTrackBranchName = "Track";
}


PndDchKalmanTask::~PndDchKalmanTask()
{
  if(fhP!=NULL) delete fhP;
  if(fhChi2!=NULL) delete fhChi2;
}

InitStatus PndDchKalmanTask::Init()
{
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndDchKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);
  
  if(fTrackArray==0)
    {
      Error("PndDchKalmanTask::Init","Track array not found!");
      return kERROR;
    }
  
  // Build hit factory -----------------------------
  fTheRecoHitFactory = new RecoHitFactory();
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndDchCylinderHit");
  if(ar==0){
    Error("PndDchKalmanTask::Init","PndDchCylinderHit array not found!");
  }
  else{ 
    fTheRecoHitFactory->addProducer(1,new RecoHitProducer<PndDchCylinderHit,PndDchRecoHit>(ar));
  } // "1" stands here for kind of detector number
  
  // setup histograms
  fhP    = new TH1D("preco","preco",200,0.9,1.1);
  fhChi2 = new TH1D("chi2","chi2",100,0,20);

  return kSUCCESS;
}


void
PndDchKalmanTask::Exec(Option_t* opt)
{
  std::cout<<"\n\n-------------------------------------------------------"<<std::endl;
  std::cout<<"      PndDchKalmanTask::Exec                       "<<std::endl;
  std::cout<<"-------------------------------------------------------"<<std::endl;
  
  Int_t ntracks=fTrackArray->GetEntriesFast();
  
  // Fitting ---------------- can go to another task!
  Kalman fitter;
  
  for(Int_t itr=0; itr<ntracks; ++itr){
    Track* trk = (Track*)fTrackArray->At(itr);
    // Load RecoHits 
    try {
      std::cout<<"Creating reco hits..."<<std::endl;
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      std::cout<<"\t"<<trk->getNumHits()<<" hits in track "
	       <<itr<<std::endl;
    }
    catch(FitterException& e) {
      std::cout<<"Creation of reco hits failed :-( "<<std::endl;
      std::cout << e.what() << std::endl;
      throw e;
    }
    
    // Start Fitter
    try{
      fitter.processTrack(trk);
      if(fSmooth)fitter.smoothing(trk);
    }
    catch (FitterException e){
      std::cout<<"*** FITTER EXCEPTION ***"<<std::endl;
      std::cout<<e.what()<<std::endl;
      
    }

    // Fill some histos after fit
    if(trk->getTrackRep(0)->getStatusFlag()==0){
      trk->getCardinalRep()->Print();
      fhP->Fill(trk->getMom().Mag());
      double chi2 = trk->getChiSqu();
      fhChi2->Fill(trk->getChiSqu());
      ++fTrackCount;
    }
  }
  
  return;
}

void PndDchKalmanTask::WriteHistograms(){
  TFile* file = CbmRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("DchKalman");
  file->cd("DchKalman");

  fhP->Write();
  delete fhP;
  fhP = NULL;

  fhChi2->Write();
  delete fhChi2;
  fhChi2 = NULL;

}

void PndDchKalmanTask::Finish(){
  WriteHistograms();
}


ClassImp(PndDchKalmanTask)
