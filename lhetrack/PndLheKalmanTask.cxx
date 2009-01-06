//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndLheKalmanTask
//      see PndLheKalmanTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Stefano Spataro, JLU Gießen         (Copied for LHE use)
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndLheKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
//#include "TDatabasePDG.h"


#include "PndMvdRecoHit.h"
#include "PndLheTpcPlanarRecoHit.h"
#include "PndSttRecoHit.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheHit.h"

#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "DetPlane.h"
#include "CbmTrackParH.h"

#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "CbmGeanePro.h"

// Class Member definitions -----------


PndLheKalmanTask::PndLheKalmanTask(const char* name, Int_t iVerbose)
  : CbmTask(name, iVerbose), fPersistence(kFALSE)
{
  fTrackBranchName = "PndTpcLheTrack";
  fFitTrackArray = new TClonesArray("Track");
  fUseGeane = kFALSE;
  fSmoothing = kFALSE;
  fNumIt = 1;
}


PndLheKalmanTask::~PndLheKalmanTask()
{
}

InitStatus
PndLheKalmanTask::Init()
{
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndLheKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);
  
  if(fTrackArray==0)
    {
      Error("PndLheKalmanTask::Init","track-array not found!");
      return kERROR;
    }
  

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new RecoHitFactory();

  TClonesArray* stripar=(TClonesArray*) ioman->GetObject("MVDHitsStrip");
  if(stripar==0)
    { //TODO Convention on detector number needed
      Error("PndLheKalmanTask::Init","MVDHitsStrip array not found");
    } 
  else
    {
      fTheRecoHitFactory->addProducer(4,new RecoHitProducer<PndMvdHit,PndMvdRecoHit>(stripar));
    }
  
  TClonesArray* pixelar=(TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if(pixelar==0)
    {
      Error("PndLheKalmanTask::Init","MVDHitsPixel array not found");
    } 
  else 
    { //TODO Convention on detector number needed
      fTheRecoHitFactory->addProducer(3,new RecoHitProducer<PndMvdHit,PndMvdRecoHit>(pixelar));
    }
  
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  if(ar==0)
    {
      Error("PndLheKalmanTask::Init","PndTpcCluster array not found");
    }
  else
    {
      fTheRecoHitFactory->addProducer(2,new RecoHitProducer<PndTpcCluster,PndLheTpcPlanarRecoHit>(ar));
    }
  
  TClonesArray* sttr=(TClonesArray*) ioman->GetObject("SttHelixHit");
  if(sttr==0)
    {
      Error("PndLheKalmanTask::Init","SttHelixHit array not found");
    }
  else
    {
      fTheRecoHitFactory->addProducer(5,new RecoHitProducer<PndSttHelixHit,PndSttRecoHit>(sttr));
    }
  
  if (fUseGeane)  fPro = new CbmGeanePro();
  
  CbmRootManager::Instance()->
    Register("LheGenTrack","Lhe", fFitTrackArray, kTRUE);
 
  return kSUCCESS;
}


void PndLheKalmanTask::Exec(Option_t* opt)
{
  if (fVerbose>0) std::cout<<"PndLheKalmanTask::Exec"<<std::endl;
  // Reset output Array
  //if(fTrackArray==0) Fatal("Kalman::Exec)","No TrackArray");
  
  //fFitTrackArray->Delete();
  fFitTrackArray->Clear();
  
  Int_t ntracks=fTrackArray->GetEntriesFast();

  // Detailed output
  if (fVerbose>1) std::cout << " -I- PndLheKalmanTask: contains " << ntracks << " Tracks."<< std::endl;
    
  // Cut too busy events TODO
  if(ntracks>20){
    std::cout<<" -I- PndLheKalmanTask::Exec: ntracks=" << ntracks << " Evil Event! skipping" << std::endl;
    return;
  }
  
  std::vector<TLorentzVector*> particles;
  std::vector<Int_t> signs;

  // Fitting ---------------- can go to another task!
  Kalman fitter;
  fitter.setLazy(1); // tell the fitter to skip hits if error occurs
  fitter.setNumIterations(fNumIt);

  for(Int_t itr=0;itr<ntracks;++itr){
    if (fVerbose>1) std::cout<<"starting track"<<itr<<std::endl;
    PndTpcLheTrack *lheTrack = (PndTpcLheTrack*)fTrackArray->At(itr);
    
    TVector3 StartPos(0., 0., 0.); 
    //TVector3 StartPos    = lheTrack->GetFirstHit().GetCoord();
    //TVector3 StartPosErr = lheTrack->GetFirstHit().GetError();
    TVector3 StartPosErr(0.5, 0.5, 0.5);
    TVector3 StartMom(0., 0., 0.);
    //TVector3 StartMom = lheTrack->GetMomentum();
    TVector3 StartMomErr = TVector3(0.1*StartMom);
    Float_t phi = lheTrack->ExtrapolateToZ(&StartMom, &StartPos, 0.);
    if (phi==-100000)
      {
	std::cout<<" -I- PndLheKalmanTask::Exec: Evil Track (lam==0)! skipping" << std::endl;
	continue; // not valid track
      }
    
    // Starting values for guessing
    Double_t  fCharge= lheTrack->GetCharge();
    Int_t PDGCode= 211*(Int_t)fCharge;
  //  TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
  //  TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
   
    // what to guess here?
    TVector3 U(1.,0.,0.);
    TVector3 V(0.,1.,0.);
    DetPlane start_pl(StartPos,U,V);

    AbsTrackRep* rep = 0;
    
    if (fUseGeane)
      {
	GeaneTrackRep *grep = new GeaneTrackRep(fPro,
			   	start_pl,StartMom,
				StartPosErr,StartMomErr,
				fCharge,PDGCode);
	//grep->setPropDir(1);
        rep = grep;
      }
    else
      {
	
	TVector3 dir=StartMom.Unit();
	Double_t dxdz=dir.X()/dir.Z();
	Double_t dydz=dir.Y()/dir.Z();
	Double_t qp=fCharge/StartMom.Mag();
	rep = new LSLTrackRep(StartPos.Z(),StartPos.X(),StartPos.Y(),dxdz,dydz,qp,
			      StartPosErr.X(),StartPosErr.Y(),0.1,0.1,0.1,NULL);
      }
    
    Track* trk= new Track(rep);
    trk->setCandidate(*(TrackCand*)lheTrack->GetTrackCand());
         
    // Load RecoHits
    try {
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      if (fVerbose>0) std::cout<<trk->getNumHits()<<" hits in track "
			       <<itr<<std::endl;
    }
    catch(FitterException& e) {
      std::cout << e.what() << std::endl;
      throw e;
    }

        
    // Start Fitter
    try{
      fitter.processTrack(trk);
      if(fSmoothing)fitter.smoothing(trk);
    }
    catch (FitterException e){
      std::cout<<"*** FITTER EXCEPTION ***"<<std::endl;
      std::cout<<e.what()<<std::endl;

    }

    if (fVerbose>0) std::cout<<"SUCESSFULL FIT!"<<std::endl;

    AddTrack(trk);
    lheTrack->SetFitTrackIndex(fFitTrackArray->GetEntriesFast()-1);
    
  }

  if (fVerbose>0) std::cout<<"Fitting done"<<std::endl;

  return;
}

//_________________________________________________________________
Track* PndLheKalmanTask::AddTrack(Track* track) {
  // Creates a new hit in the TClonesArray.
  
  TClonesArray& trkRef = *fFitTrackArray;
  Int_t size = trkRef.GetEntriesFast();
  return new(trkRef[size]) Track(*track);
}

ClassImp(PndLheKalmanTask);
