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
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
//#include "TDatabasePDG.h"


#include "PndMvdRecoHit.h"
#include "PndGemRecoHit.h"
#include "PndTpcSPHit.h"
#include "PndSttRecoHit.h"
#include "PndGenfitAdapters.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndDetectorList.h"

#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "GFDetPlane.h"
#include "FairTrackParH.h"

#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"

// Class Member definitions -----------


PndLheKalmanTask::PndLheKalmanTask(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose), fPersistence(kFALSE)
{
  fTrackBranchName = "LheTrack";
  fFitTrackArray = new TClonesArray("PndTrack");
  fUseGeane = kTRUE;
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
  FairRootManager* ioman= FairRootManager::Instance();

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
  fTheRecoHitFactory = new GFRecoHitFactory();
  if (fVerbose<2) GFException::quiet(true);

  TClonesArray* stripar=(TClonesArray*) ioman->GetObject("MVDHitsStrip");
  if(stripar==0)
    { //TODO Convention on detector number needed
      Error("PndLheKalmanTask::Init","MVDHitsStrip array not found");
    } 
  else
    {
      fTheRecoHitFactory->addProducer(kMVDHitsStrip,new GFRecoHitProducer<PndMvdHit,PndMvdRecoHit>(stripar));
    }
  
  TClonesArray* pixelar=(TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if(pixelar==0)
    {
      Error("PndLheKalmanTask::Init","MVDHitsPixel array not found");
    } 
  else 
    { //TODO Convention on detector number needed
      fTheRecoHitFactory->addProducer(kMVDHitsPixel,new GFRecoHitProducer<PndMvdHit,PndMvdRecoHit>(pixelar));
    }
  
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  if(ar==0)
    {
      Error("PndLheKalmanTask::Init","PndTpcCluster array not found");
    }
  else
    {
      fTheRecoHitFactory->addProducer(kTpcCluster,new GFRecoHitProducer<PndTpcCluster,PndTpcSPHit>(ar));
    }
  
  TClonesArray* sttr=(TClonesArray*) ioman->GetObject("SttHelixHit");
  if(sttr==0)
    {
      Error("PndLheKalmanTask::Init","SttHelixHit array not found");
    }
  else
    {
      fTheRecoHitFactory->addProducer(kSttHelixHit,new GFRecoHitProducer<PndSttHelixHit,PndSttRecoHit>(sttr));
    }
  
  TClonesArray* gemar=(TClonesArray*) ioman->GetObject("GEMHit");
  if(gemar==0)
    {
      Error("PndLheKalmanTask::Init","GEMHit array not found");
    }
  else
    {
      fTheRecoHitFactory->addProducer(kGemHit,new GFRecoHitProducer<PndGemHit,PndGemRecoHit>(gemar));
    }
  
  if (fUseGeane)  fPro = new FairGeanePro();

  if (fSmoothing)  Error("PndLheKalmanTask::Init","Smoothing not available. Please switch the flag OFF");
     
  FairRootManager::Instance()->
    Register("LheGenTrack","Lhe", fFitTrackArray, kTRUE);
 
  return kSUCCESS;
}


void PndLheKalmanTask::Exec(Option_t* opt)
{
  if (fVerbose>0) std::cout<<"PndLheKalmanTask::Exec"<<std::endl;
  // Reset output Array
  //if(fTrackArray==0) Fatal("Kalman::Exec)","No TrackArray");
  
  fFitTrackArray->Clear();
  
  Int_t ntracks=fTrackArray->GetEntriesFast();

  // Detailed output
  if (fVerbose>1) std::cout << " -I- PndLheKalmanTask: contains " << ntracks << " Tracks."<< std::endl;
    
  // Cut too busy events TODO
  if(ntracks>20)
    {
      std::cout<<" -I- PndLheKalmanTask::Exec: ntracks=" << ntracks << " Evil Event! skipping" << std::endl;
      return;
    }
  
  std::vector<TLorentzVector*> particles;
  std::vector<Int_t> signs;

  // Fitting ---------------- can go to another task!
  GFKalman fitter;
  fitter.setNumIterations(fNumIt);

  for(Int_t itr=0;itr<ntracks;++itr)
    {
      if (fVerbose>1) std::cout<<"starting track"<<itr<<std::endl;
      PndTrack *lheTrack = (PndTrack*)fTrackArray->At(itr);
      Double_t  fCharge= lheTrack->GetParamFirst().GetQ();
      Int_t PDGCode= -13*(Int_t)TMath::Sign(1.,fCharge);
      
      TVector3 StartPos(lheTrack->GetParamFirst().GetX(),lheTrack->GetParamFirst().GetY(),lheTrack->GetParamFirst().GetZ()); 
      TVector3 StartMom(lheTrack->GetParamFirst().GetPx(),lheTrack->GetParamFirst().GetPy(),lheTrack->GetParamFirst().GetPz());
      TVector3 StartPosErr(lheTrack->GetParamFirst().GetDX(),lheTrack->GetParamFirst().GetDY(),lheTrack->GetParamFirst().GetDZ()); 
      TVector3 StartMomErr(lheTrack->GetParamFirst().GetDPx(),lheTrack->GetParamFirst().GetDPy(),lheTrack->GetParamFirst().GetDPz());
      
      GFAbsTrackRep* rep = 0;
      if (fUseGeane)
	{
	  // Calculating params at PCA to Origin
	  FairTrackParP par = lheTrack->GetParamFirst();
	  Int_t ierr = 0;
	  FairTrackParH *helix = new FairTrackParH(&par, ierr);
	  FairGeanePro *fPro0 = new FairGeanePro();
	  FairTrackParH *fRes= new FairTrackParH();
	  fPro0->SetPoint(TVector3(0,0,0));
	  fPro0->PropagateToPCA(1, -1);
	  Bool_t rc =  fPro0->Propagate(helix, fRes, PDGCode);
	  if (rc)
	    {
	      StartPos.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
	      StartMom.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());
	      StartPosErr.SetXYZ(fRes->GetDX(), fRes->GetDY(), fRes->GetDZ());
	      StartMomErr.SetXYZ(fRes->GetDPx(), fRes->GetDPy(), fRes->GetDPz());
	    }
	  
	  GFDetPlane start_pl(StartPos, TVector3(1.,0.,0.), TVector3(0.,1.,0.));
	  GeaneTrackRep *grep = new GeaneTrackRep(fPro,
						  start_pl,StartMom,
						  StartPosErr,StartMomErr,
						  fCharge,PDGCode);
	  grep->setPropDir(1);
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
      
      GFTrack* trk= new GFTrack(rep);
      PndTrackCand trackCand = lheTrack->GetTrackCand();
      trk->setCandidate(*PndTrackCand2GenfitTrackCand(&trackCand));
      
      // Load RecoHits
      try 
	{
	  trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
	  if (fVerbose>0) std::cout<<trk->getNumHits()<<" hits in track "
				   <<itr<<std::endl;
	}
      catch(GFException& e)
	{
	  std::cout << e.what() << std::endl;
	  throw e;
	}
      
      // Start Fitter
      try
	{
	  fitter.processTrack(trk);
	}
      catch (GFException e)
	{
	  std::cout<<"*** FITTER EXCEPTION ***"<<std::endl;
	  std::cout<<e.what()<<std::endl;
	}
      if (fVerbose>0) std::cout<<"SUCESSFULL FIT!"<<std::endl;
      
      PndTrack *fitTrack = 0;
      try
	{ 
	  fitTrack = (PndTrack*)GenfitTrack2PndTrack(trk);
	}
      catch (GFException e)
	{
	  std::cout<<"*** PndGenfitAdapters EXCEPTION ***"<<std::endl;
	  std::cout<<e.what()<<std::endl;
	  continue;
	}
      
      TClonesArray& trkRef = *fFitTrackArray;
      Int_t size = trkRef.GetEntriesFast();
      PndTrack* pndTrack = new(trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
						      fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), itr);
    }
  
  if (fVerbose>0) std::cout<<"Fitting done"<<std::endl;
  
  return;
}

ClassImp(PndLheKalmanTask);
