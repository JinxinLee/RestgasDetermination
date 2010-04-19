//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndRecoKalmanTask
//      see PndRecoKalmanTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Stefano Spataro, UNI Torino
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndRecoKalmanTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <cmath>

// Collaborating Class Headers --------
#include "TClonesArray.h"
#include "PndTrack.h"
#include "FairRootManager.h"
#include "FairGeanePro.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

PndRecoKalmanTask::PndRecoKalmanTask(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose), fPersistence(kFALSE)
{
  fTrackInBranchName  = "LheTrack"; 
  fTrackOutBranchName = "LheGenTrack";
  
  fFitTrackArray = new TClonesArray("PndTrack");
  fUseGeane = kTRUE;
  fNumIt = 1;
  fFitter = new PndRecoKalmanFit();
}


PndRecoKalmanTask::~PndRecoKalmanTask()
{
}

InitStatus
PndRecoKalmanTask::Init()
{
 
  fFitter->SetGeane(fUseGeane);
  fFitter->SetNumIterations(fNumIt);
  if (!fFitter->Init()) return kFATAL;
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndRecoKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackInBranchName);
  if(fTrackArray==0)
    {
      Error("PndRecoKalmanTask::Init","track-array not found!");
      return kERROR;
    }
  
  FairRootManager::Instance()->
    Register(fTrackOutBranchName,"Gen", fFitTrackArray, kTRUE);
  
  return kSUCCESS;
}

void PndRecoKalmanTask::SetParContainers() 
{
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

void PndRecoKalmanTask::Exec(Option_t* opt)
{
  if (fVerbose>0) std::cout<<"PndRecoKalmanTask::Exec"<<std::endl;
  
  fFitTrackArray->Clear();
  
  Int_t ntracks=fTrackArray->GetEntriesFast();

  // Detailed output
  if (fVerbose>1) std::cout << " -I- PndRecoKalmanTask: contains " << ntracks << " Tracks."<< std::endl;
  
  // Cut too busy events TODO
  if(ntracks>20)
    {
      std::cout<<" -I- PndRecoKalmanTask::Exec: ntracks=" << ntracks << " Evil Event! skipping" << std::endl;
      return;
    }
  
  
  for(Int_t itr=0;itr<ntracks;++itr)
    {
      if (fVerbose>1) std::cout<<"starting track"<<itr<<std::endl;

      TClonesArray& trkRef = *fFitTrackArray;
      Int_t size = trkRef.GetEntriesFast();
      
      PndTrack *prefitTrack = (PndTrack*)fTrackArray->At(itr);
      Int_t  fCharge= prefitTrack->GetParamFirst().GetQ();
      Int_t PDGCode= -13*fCharge;
      
      PndTrack *fitTrack = new PndTrack();
      fitTrack = fFitter->Fit(prefitTrack, PDGCode);
      
      PndTrack* pndTrack = new(trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
						      fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), itr, kLheTrack);
    }
  
  if (fVerbose>0) std::cout<<"Fitting done"<<std::endl;
  
  return;
}

ClassImp(PndRecoKalmanTask);
