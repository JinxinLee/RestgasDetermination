
// Panda Headers ----------------------

// This Class' Header ------------------
#include "SttKalmanTask.h"

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
#include "PndSttHit.h"
#include "GeaneTrackRep.h"
#include "FitterExceptions.h"
#include "Kalman.h"

using namespace std;

// Class Member definitions -----------


SttKalmanTask::SttKalmanTask(){}

SttKalmanTask::~SttKalmanTask(){}

InitStatus
SttKalmanTask::Init()
{
  cout << "SttKalmanTask::Init()" << endl;

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
    Error("SttKalmanTask::Init","track-array not found!");
    return kERROR;
  }
  
  // Build hit factory -----------------------------
  _theRecoHitFactory = new RecoHitFactory();
  
  std::map<unsigned int,TString>::iterator iter=_hitBranchMap.begin();

  while(iter!=_hitBranchMap.end()){
    TClonesArray* ar=(TClonesArray*) ioman->GetObject(iter->second);
    if(ar==0){
      Error("SttKalmanTask::Init","point-array %s not found!",iter->second.Data());
    }
    else{ 
      _theRecoHitFactory->addProducer(iter->first,new RecoHitProducer<PndSttHit,SttRecoHit>(ar));
    }
    ++iter;
  }//end loops over hit types
  
 return kSUCCESS;
}


void
SttKalmanTask::Exec(Option_t* opt)
{
//   std::cout<<"SttKalmanTask::Exec"<<  std::endl;
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
SttKalmanTask::AddHitBranch(unsigned int detId, const TString& m){
  
  std::cout << "hit branch " << m << " " << detId  << std::endl;
  _hitBranchMap[detId]=m;
}


ClassImp(SttKalmanTask)
