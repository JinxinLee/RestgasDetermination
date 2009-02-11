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
#include "IdealPatternRecTask.h"

// C/C++ Headers ----------------------
#include <map>
#include <algorithm>
#include <string>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "CbmMCTrack.h"
#include "PndTpcPoint.h"
#include "CbmTstPoint.h"
#include "Track.h"

#include "AliceHelixTrackRep.h"
#include "PndTpcRecoHit.h"
#include "SltHit.h"
#include "Kalman.h"
#include "RecoHitFactory.h"

#include "TApplication.h"
#include "TMath.h"
#include "TROOT.h"
#include "TSystem.h"
#include "MCHitGenerator.h"
#include "TCanvas.h"
#include "TView.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"

// Class Member definitions -----------



IdealPatternRecTask::IdealPatternRecTask()
  : FairTask("IdealPatternRec"), _persistence(kFALSE)
{
  _hitBranchName = "PndTpcPoint";
}


IdealPatternRecTask::~IdealPatternRecTask()
{
  //delete theRecoHitFactory;
}


InitStatus
IdealPatternRecTask::Init()
{


  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("IdealPatternRecTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _hitArray=(TClonesArray*) ioman->GetObject(_hitBranchName);
  
  if(_hitArray==0)
    {
      Error("IdealPatternRecTask::Init","Hit-array not found!");
      return kERROR;
    }


  // Get input collection
  _digiArray=(TClonesArray*) ioman->GetObject("PndTpcDigi");
  
  if(_digiArray==0)
    {
      Error("IdealPatternRecTask::Init","Digi-array not found!");
      return kERROR;
    }


  // Get input collection
  _TSThitArray=(TClonesArray*) ioman->GetObject("TSTPoint");
  
  if(_TSThitArray==0)
    {
      Error("IdealPatternRecTask::Init","TSTHit-array not found!");
      return kERROR;
    }

  
  // Get MCTruth collection
  _mctruthArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(_mctruthArray==0)
    {
      Error("IdealPatternRecTask::Init","McTruth-array not found!");
      return kERROR;
    }


  // create and register output array
  // persistency to be implemented
  _trackOutputArray = new TClonesArray("Track"); 
  ioman->Register("TrackPreFit","GenFit",_trackOutputArray,_persistence);

  //  theRecoHitFactory = new RecoHitFactory();
  //  theRecoHitFactory->addProducer(0,new RecoHitProducer<PndTpcPoint,PndTpcRecoHit>(_hitArray));

  return kSUCCESS;
}


void
IdealPatternRecTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(_trackOutputArray==0) Fatal("IdealPatternRec::Exec)","No TrackArray");
  _trackOutputArray->Delete();
  // CREATE MONTECARLO MAP
  std::map<int, CbmMCTrack*> mcmap;
  Int_t nmc=_mctruthArray->GetEntriesFast();
  for(int imc=0;imc<nmc;++imc){
    CbmMCTrack* mc=(CbmMCTrack*)_mctruthArray->At(imc);
    mcmap[imc]=mc;
  }
  // BUILD TRACKS
  std::map<int, Track*> trackMap;

  Int_t TSTnhit=_TSThitArray->GetEntriesFast();
  for(Int_t TSTihit=0;TSTihit<TSTnhit;++TSTihit){
    CbmTstPoint* TSThit=(CbmTstPoint*)_TSThitArray->At(TSTihit);
    int MCid=TSThit->GetTrackID();


    // cut on secondaries (deltas) etc
    if(MCid<0)continue;

	//    Warning("IdealPatternRecTask::Exec","MCTrackId of hit=%i",MCid);
    TVector3 pos;
    TSThit->Position(pos);
    // build tracks here:
    // check if track already candidated:
    if(trackMap[MCid]==0){
      //Warning("IdealPatternRecTask::Exec","starting new track!");
      // getting start parameters from MonteCarloTruth
      AliceHelixTrackRep* rep=new AliceHelixTrackRep();
	  //mcmap[MCid]->GetStartVertex().Print();
	  //rep->init(pos,
	  //	  TVector3 v(mcmap[MCid]->GetMomentum().X()*1.e-7,
	  //				 mcmap[MCid]->GetMomentum().Y()*1.e-7,
	  //				 0.);

	  rep->init(mcmap[MCid]->GetStartVertex(),
	  //rep->init(pos,
				mcmap[MCid]->GetMomentum(),
				TVector3(0,0,2),
				-1);//hardcoded charge for mu-

	  Int_t size = _trackOutputArray->GetEntriesFast();
      trackMap[MCid]=new ((*_trackOutputArray)[size]) Track(rep);
	  //	  rep->getState().Print();
    }
    //trackMap[MCid]->addHitIndices(TSThit->GetDetectorID(),TSTihit);
  }

  int nhit=_digiArray->GetEntriesFast();
  double rARR[nhit];
  int indARR[nhit];
  for(Int_t ihit=0;ihit<nhit;++ihit){
    PndTpcDigi* digi=(PndTpcDigi*)_digiArray->At(ihit);
	TVector3 pos;
	PndTpcDigiMapper::getInstance()->map(digi,pos);
	rARR[ihit] = pos.Perp();
  }

  //hits have to be sorted by radius
  TMath::Sort(nhit,rARR,indARR,false);

  for(Int_t i=0;i<nhit;++i){

	if(trackMap[0] != NULL) {
	  //trackMap[0]->addHitIndices(3,indARR[i]);
	}
  }
  
  /*
  Int_t nhit=_hitArray->GetEntriesFast();
  for(Int_t ihit=0;ihit<nhit;++ihit){
    PndTpcPoint* hit=(PndTpcPoint*)_hitArray->At(ihit);
    int MCid=hit->GetTrackID();

    // cut on secondaries (deltas) etc
    if(MCid<0)continue;


    std::cout << "XYZ" <<hit->GetDetectorID() << std::endl;

	std::cout << ihit << " " << nhit << std::endl;
	trackMap[MCid]->addHitIndices(5,ihit);
  }
  */


  std::cout<<_trackOutputArray->GetEntriesFast()
		   <<" Tracks created"<<std::endl;

  return;
}













ClassImp(IdealPatternRecTask)
