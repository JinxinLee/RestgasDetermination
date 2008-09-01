// USE THIS to use helixhits

// Panda Headers ----------------------

// This Class' Header ------------------
#include "SttKalmanTask2.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
#include "RecoHitFactory.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "SttRecoHit.h"
#include "PndSttHelixHit.h"
#include "GeaneTrackRep.h"
#include "FitterExceptions.h"
#include "Kalman.h"

using namespace std;

// Class Member definitions -----------


SttKalmanTask2::SttKalmanTask2(){}

SttKalmanTask2::~SttKalmanTask2(){}

InitStatus
SttKalmanTask2::Init()
{
  cout << "SttKalmanTask2::Init()" << endl;

  CbmRootManager *ioman = CbmRootManager::Instance();
  
  if (!ioman) 
  {
      cout << "-E- SttRecoHirProducer: "
	   << "RootManager not instantised!" << endl;
      return kFATAL;
  }

  // open STTTrack array
  fTrackArray=(TClonesArray*) ioman->GetObject("Track");
  if(fTrackArray==0){
    Error("SttKalmanTask2::Init","track-array not found!");
    return kERROR;
  }

//   // open SttHelixHit array
//   fSttHelixHitArray=(TClonesArray*) ioman->GetObject("SttHelixHit");
//   if(fSttHelixHitArray==0){
//     Error("SttKalmanTask2::Init","stt helixhit-array not found!");
//     return kERROR;
//   }
  
  // Build hit factory -----------------------------
  _theRecoHitFactory = new RecoHitFactory();
  
  std::map<unsigned int,TString>::iterator iter=_hitBranchMap.begin();
     
  while(iter!=_hitBranchMap.end()){
    TClonesArray* ar=(TClonesArray*) ioman->GetObject(iter->second);
    if(ar==0){
      Error("SttKalmanTask2::Init","point-array %s not found!",iter->second.Data());
    }
    else{ 
      // use helix hit
      _theRecoHitFactory->addProducer(iter->first,new RecoHitProducer<PndSttHelixHit,SttRecoHit>(ar));
    }
    ++iter;
  }//end loops over hit types
  
 return kSUCCESS;
}


void
SttKalmanTask2::Exec(Option_t* opt)
{
//   std::cout<<"SttKalmanTask2::Exec"<<  std::endl;
  if(fTrackArray)
    {

      Int_t ntracks=fTrackArray->GetEntriesFast();
      //      cout << "ntracks " << ntracks << endl;

      for(Int_t itr=0;itr<ntracks;++itr){
	Track* trk = (Track*) fTrackArray->At(itr);
	trk->addHitVector(_theRecoHitFactory->createMany(trk->getCand()));
	std::cout<<trk->getNumHits()<<" hits in track " <<itr<<std::endl;
	Kalman k;
        k.setLazy(1);
	//	k.setNumIterations(1);
	k.processTrack(trk);
      }
    }
  return;
}

void 
SttKalmanTask2::AddHitBranch(unsigned int detId, const TString& m){
  
  std::cout << "hit branch " << m << " " << detId  << std::endl;
  _hitBranchMap[detId]=m;
}


ClassImp(SttKalmanTask2)
