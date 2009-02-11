//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class GenfitTask
//      see GenfitTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "GenfitTask.h"

// C/C++ Headers ----------------------
#include <map>
#include <algorithm>
#include <string>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndTpcPoint.h"
#include "CbmTstPoint.h"
#include "Track.h"

#include "AliceHelixTrackRep.h"
#include "PndTpcRecoHit.h"
#include "SltHit.h"
#include "Kalman.h"
#include "RecoHitFactory.h"
#include "FitterExceptions.h"

#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "MCHitGenerator.h"
#include "TCanvas.h"
#include "TView.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"

// Class Member definitions -----------


GenfitTask::GenfitTask()
  : FairTask("Genfit"), _persistence(kFALSE)
{
}


GenfitTask::~GenfitTask()
{
  delete theRecoHitFactory;
}


InitStatus
GenfitTask::Init()
{


  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("GenfitTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  

  _hitArray=(TClonesArray*) ioman->GetObject("PndTpcPoint");
  
  if(_hitArray==0)
    {
      Error("IdealPatternRecTask::Init","Hit-array not found!");
      return kERROR;
    }


  _digiArray=(TClonesArray*) ioman->GetObject("PndTpcDigi");
  
  if(_digiArray==0)
    {
      Error("IdealPatternRecTask::Init","Digi-array not found!");
      return kERROR;
    }


  _TSThitArray=(TClonesArray*) ioman->GetObject("TSTPoint");
  
  if(_TSThitArray==0)
    {
      Error("IdealPatternRecTask::Init","TSTHit-array not found!");
      return kERROR;
    }

  
  theRecoHitFactory = new RecoHitFactory();
  theRecoHitFactory->addProducer(3,new RecoHitProducer<PndTpcDigi,PndTpcRecoHit>(_digiArray));
  theRecoHitFactory->addProducer(5,new RecoHitProducer<PndTpcPoint,PndTpcRecoHit>(_hitArray));
  /*
    theRecoHitFactory->addProducer(9,new RecoHitProducer<CbmTstPoint,PndTpcRecoHit>(_TSThitArray));
    theRecoHitFactory->addProducer(11,new RecoHitProducer<CbmTstPoint,PndTpcRecoHit>(_TSThitArray));
    theRecoHitFactory->addProducer(13,new RecoHitProducer<CbmTstPoint,PndTpcRecoHit>(_TSThitArray));
    theRecoHitFactory->addProducer(14,new RecoHitProducer<CbmTstPoint,PndTpcRecoHit>(_TSThitArray));
    theRecoHitFactory->addProducer(18,new RecoHitProducer<CbmTstPoint,PndTpcRecoHit>(_TSThitArray));
    theRecoHitFactory->addProducer(22,new RecoHitProducer<CbmTstPoint,PndTpcRecoHit>(_TSThitArray));
    theRecoHitFactory->addProducer(26,new RecoHitProducer<CbmTstPoint,PndTpcRecoHit>(_TSThitArray));
  */
  

  // Get input collection
  _trackInputArray=(TClonesArray*) ioman->GetObject("TrackPreFit");
  
  if(_trackInputArray==0)
    {
      Error("GenfitTask::Init","TrackPreFit-array not found!");
      return kERROR;
    }
  

  // create and register output array
  // persistency to be implemented
  _trackOutputArray = new TClonesArray("Track"); 
  ioman->Register("TrackPostFit","GenFit",_trackOutputArray,_persistence);

  return kSUCCESS;
}


void
GenfitTask::Exec(Option_t* opt)
{

  // Reset output Array
  if(_trackOutputArray==0) Fatal("Genfit::Exec)","No TrackOutputArray");
  _trackOutputArray->Delete();

  

  // BUILD TRACKS
  std::map<int, Track*> trackMap;
  std::vector<TMatrixT<double> > mctruth;
  std::vector<Track*> trackvec;


  

  Int_t nTracks=_trackInputArray->GetEntriesFast();
  for(Int_t itrack=0;itrack<nTracks;++itrack){
    Track* inTrack=(Track*)_trackInputArray->At(itrack);



	// getting start parameters from MonteCarloTruth
	AliceHelixTrackRep* rep=new AliceHelixTrackRep();

	mctruth.push_back(rep->getStartState());
	
	Int_t size = _trackOutputArray->GetEntriesFast();
	//make copy of inTrack in map and TClonesArray


	trackMap[itrack]=new ((*_trackOutputArray)[size]) Track(*inTrack);
	
    // create a recoHit object from hit
	//    trackMap[MCid]->addHit(recoHit);
    trackMap[itrack]->addHitVector(theRecoHitFactory->createMany(inTrack->getCand()));
  }

  std::map<int, Track*>::iterator it=trackMap.begin();
  while(it!=trackMap.end()){
	trackvec.push_back(it->second);
	//    trackvec.push_back((Track*)_trackInputArray->At(0));
	//	trackvec.at(0)->addHitVector( theRecoHitFactory->createMany(
	//																trackvec.at(0)->getDetIDs(),
	//																trackvec.at(0)->getHitIndices()
	//																) );
    ++it;
  }


  std::for_each(trackMap.begin(),trackMap.end(),Kalman());

  std::cout<<_trackOutputArray->GetEntriesFast()
		   <<" Tracks created"<<std::endl;


  return;


}













ClassImp(GenfitTask)
