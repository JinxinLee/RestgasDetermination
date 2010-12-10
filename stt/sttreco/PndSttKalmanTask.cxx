
// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndSttKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "GFRecoHitFactory.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "PndSttRecoHit.h"
#include "PndSttHit.h"
#include "GeaneTrackRep.h"
#include "GFException.h"
#include "GFKalman.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndSttTube.h"
#include "PndSttRecoHitProducer.h"
#include "PndSttMapCreator.h"


using namespace std;

// Class Member definitions -----------


PndSttKalmanTask::PndSttKalmanTask(){
  fVerbose = 0;}

PndSttKalmanTask::~PndSttKalmanTask(){}

InitStatus
PndSttKalmanTask::Init()
{
  cout << "PndSttKalmanTask::Init()" << endl;

  FairRootManager *ioman = FairRootManager::Instance();
  
  if (!ioman) 
  {
      cout << "-E- PndSttRecoHirProducer: "
	   << "RootManager not instantised!" << endl;
      return kFATAL;
  }

  // open STTTrack array
  fTrackArray=(TClonesArray*) ioman->GetObject("Track");
  if(fTrackArray==0){
    Error("PndSttKalmanTask::Init","track-array not found!");
    return kERROR;
  }

  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  TClonesArray *tubeAr = mapper->FillTubeArray();

  // Build hit factory -----------------------------
  _theRecoHitFactory = new GFRecoHitFactory();
  
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("STTHit");
    if(ar==0){
      Error("PndSttKalmanTask::Init","STTHit array not found!");
    }
    else{ 
      _theRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("STTHit"), new PndSttRecoHitProducer<PndSttHit,PndSttRecoHit>(ar, tubeAr));
    }
  
  GFException::quiet(true);

 return kSUCCESS;
}

// CHECK added 
void PndSttKalmanTask::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

void
PndSttKalmanTask::Exec(Option_t* opt)
{
//   std::cout<<"PndSttKalmanTask::Exec"<<  std::endl;
  if(fTrackArray)
    {

      Int_t ntracks=fTrackArray->GetEntriesFast();
      //      cout << "ntracks " << ntracks << endl;

      for(Int_t itr=0;itr<ntracks;++itr){
	GFTrack* trk = (GFTrack*) fTrackArray->At(itr);
	trk->addHitVector(_theRecoHitFactory->createMany(trk->getCand()));
	if(fVerbose >= 2) std::cout<<trk->getNumHits()<<" hits in track " <<itr<<std::endl;
	GFKalman k;
	//        k.setLazy(1);
	k.setNumIterations(1);
	k.processTrack(trk);
      }
    }
  return;
}

ClassImp(PndSttKalmanTask)
