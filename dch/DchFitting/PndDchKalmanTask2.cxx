// -------------------------------------------------------------------------------
// -----                 PndDchKalmanTask2 source file                       -----
// -----            Created 30.09.2008  by A. Wronska                        -----
// -----  based on the stt/sttreco code by Susanna Costanza and Lia Lavezzi  -----
// -------------------------------------------------------------------------------

// Panda Headers ----------------------
#include "FairRootManager.h"
#include "PndDchKalmanTask2.h"
#include "GFTrack.h"
#include "PndDchRecoHit2.h"
#include "PndDchCylinderHit.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// ROOT Headers --------
#include "TClonesArray.h"
#include "TFile.h"
#include "TGeoManager.h"


PndDchKalmanTask2::PndDchKalmanTask2()
  : FairTask("Kalman Filter"), fPersistence(kFALSE), fEvtCount(0), fNumIt(1)
{
  fTrackBranchName = "FSTracks";

}


PndDchKalmanTask2::~PndDchKalmanTask2()
{;
}

InitStatus PndDchKalmanTask2::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

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
  fTheRecoHitFactory = new GFRecoHitFactory();
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndDchCylinderHit");
  if(ar==0){
    Error("PndDchKalmanTask2::Init","PndDchCylinderHit array not found!");
  }
  else{ 
    fTheRecoHitFactory->addProducer(1,new GFRecoHitProducer<PndDchCylinderHit,PndDchRecoHit2>(ar));
  } // "1" stands here for kind of detector
  
}


void
PndDchKalmanTask2::Exec(Option_t* opt)
{
  std::cout<<"\n\n-------------------------------------------------------\n";
  std::cout<<"      PndDchKalmanTask2::Exec                       "<<std::endl;
  std::cout<<"-------------------------------------------------------"<<std::endl;
  
  Int_t ntracks=fTrackArray->GetEntriesFast();
  
  GFKalman fitter;
  fitter.setLazy(0);
  fitter.setNumIterations(fNumIt);
  
  for(Int_t itr=0; itr<ntracks; ++itr){
    GFTrack* trk = (GFTrack*)fTrackArray->At(itr);
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
    catch(GFException& e) {
      std::cout<<"Creation of reco hits failed :-( "<<std::endl;
      std::cout << e.what() << std::endl;
      //throw e;
      continue;
    }
    
    // Start Fitter
    try{
      fitter.processTrack(trk);
    }
    catch (GFException e){
      std::cout<<"*** FITTER EXCEPTION ***"<<std::endl;
      std::cout<<e.what()<<std::endl;
    }
    ++fTrackCount;
  }
  return;
}

ClassImp(PndDchKalmanTask2)
  
