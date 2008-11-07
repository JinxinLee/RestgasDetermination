// -------------------------------------------------------------------------------
// -----                 PndDchKalmanTask2 source file                       -----
// -----            Created 30.09.2008  by A. Wronska                        -----
// -----  based on the stt/sttreco code by Susanna Costanza and Lia Lavezzi  -----
// -------------------------------------------------------------------------------

// Panda Headers ----------------------
#include "CbmRootManager.h"
#include "PndDchKalmanTask2.h"
#include "Track.h"
#include "PndDchRecoHit2.h"
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
#include "TFile.h"
#include "TGeoManager.h"


PndDchKalmanTask2::PndDchKalmanTask2()
  : CbmTask("Kalman Filter"), fPersistence(kFALSE),fSmooth(kFALSE), fEvtCount(0), fNumIt(1)
{
  fTrackBranchName = "Track";

}


PndDchKalmanTask2::~PndDchKalmanTask2()
{;
}

InitStatus PndDchKalmanTask2::Init()
{
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndDchKalmanTask2::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);
  
  if(fTrackArray==0) {
    Error("PndDchKalmanTask2::Init","Track array not found!");
    return kERROR;
  }
  
  // Build hit factory -----------------------------
  fTheRecoHitFactory = new RecoHitFactory();
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndDchCylinderHit");
  if(ar==0){
    Error("PndDchKalmanTask2::Init","PndDchCylinderHit array not found!");
  }
  else{ 
    fTheRecoHitFactory->addProducer(1,new RecoHitProducer<PndDchCylinderHit,PndDchRecoHit2>(ar));
  } // "1" stands here for kind of detector
  
}


void
PndDchKalmanTask2::Exec(Option_t* opt)
{
  std::cout<<"\n\n-------------------------------------------------------\n";
  std::cout<<"      PndDchKalmanTask2::Exec                       "<<std::endl;
  std::cout<<"-------------------------------------------------------"<<std::endl;
  
  Int_t ntracks=fTrackArray->GetEntriesFast();
  
  Kalman fitter;
  fitter.setLazy(1);
  fitter.setNumIterations(fNumIt);
  
  for(Int_t itr=0; itr<ntracks; ++itr){
    Track* trk = (Track*)fTrackArray->At(itr);
    // Load RecoHits 
    try {
      if(fVerbose>0){
	std::cout<<"Creating reco hits..."<<std::endl;
	std::cout<<"the candidate had "<<trk->getCand().getNHits()<<" hits"<<std::endl;
      }
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      if(fVerbose>0)
	std::cout<<"\t"<<trk->getNumHits()<<" reco hits were created for track "
		 <<itr<<std::endl;
    }
    catch(FitterException& e) {
      std::cout<<"Creation of reco hits failed :-( "<<std::endl;
      std::cout << e.what() << std::endl;
      //throw e;
      continue;
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
    ++fTrackCount;
  }
  return;
}

ClassImp(PndDchKalmanTask2)
