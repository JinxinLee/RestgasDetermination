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
: FairTask(name, iVerbose), fPersistence(kFALSE), fPDGHyp(-13)
{
  fTrackInBranchName  = "LheTrack"; 
  fTrackOutBranchName = "LheGenTrack"; 
  fMvdBranchName = "";
  fCentralTrackerBranchName = "";
  fFitTrackArray = new TClonesArray("PndTrack");  
  fUseGeane = kTRUE;
  fNumIt = 1;
  fFitter = new PndRecoKalmanFit();
  fBusyCut=20;
}


PndRecoKalmanTask::~PndRecoKalmanTask()
{
}

InitStatus
PndRecoKalmanTask::Init()
{
  
  fFitter->SetGeane(fUseGeane);
  fFitter->SetNumIterations(fNumIt); 
  fFitter->SetMvdBranchName(fMvdBranchName);
  fFitter->SetCentralTrackerBranchName(fCentralTrackerBranchName);
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
  
  ioman->Register(fTrackOutBranchName,"Gen", fFitTrackArray, kTRUE);
  
  return kSUCCESS;
}

void PndRecoKalmanTask::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  rtdb->getContainer("PndGeoSttPar");
}

void PndRecoKalmanTask::Exec(Option_t* opt)
{
  if (fVerbose>0) std::cout<<"PndRecoKalmanTask::Exec"<<std::endl;
  
  fFitTrackArray->Delete();
  
  Int_t ntracks=fTrackArray->GetEntriesFast();
  
  // Detailed output
  if (fVerbose>1) std::cout << " -I- PndRecoKalmanTask: contains " << ntracks << " Tracks."<< std::endl;
  
  // Cut too busy events TODO
  if(ntracks>fBusyCut)
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
    Int_t PDGCode= fPDGHyp*fCharge;
    
    PndTrack *fitTrack = new PndTrack();
    fitTrack = fFitter->Fit(prefitTrack, PDGCode);
    
    PndTrack* pndTrack = new(trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
                                                    fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), itr, FairRootManager::Instance()->GetBranchId(fTrackInBranchName));
  }
  
  if (fVerbose>0) std::cout<<"Fitting done"<<std::endl;
  
  return;
}

void PndRecoKalmanTask::SetParticleHypo(TString h)
{
  // Set the hypothesis for the fit, charge will be applied later
  if(h.BeginsWith("e") || h.BeginsWith("E")){
    fPDGHyp=-11; //electrons
  }else if(h.BeginsWith("m") || h.BeginsWith("M")){
    fPDGHyp=-13; //muons
  }else if(h.BeginsWith("pi") || h.BeginsWith("Pi") || h.BeginsWith("PI")){
    fPDGHyp=211; //pions
  }else if(h.BeginsWith("K") || h.BeginsWith("K")){
    fPDGHyp=321; //kaons
  }else if(h.BeginsWith("p") || h.BeginsWith("P") || h.BeginsWith("antip")){
    fPDGHyp=2212; //protons/antiprotons
  }else{
    std::cout << "-I- PndRecoKalmanTask::SetParticleHypo: Not recognised PID set -> Using default MUON hypothesis" << std::endl;
    fPDGHyp=-13; // Muon is default.
  }
}

void PndRecoKalmanTask::SetParticleHypo(Int_t h)
{  
  switch (abs(h))
  {
    case 11:
      fPDGHyp = -11;
      break;
    case 13:
      fPDGHyp = -13;
      break;
    case 211:
      fPDGHyp = 211;
      break;
    case 321:
      fPDGHyp = 321;
      break;
    case 2212:
      fPDGHyp = 2212;
      break;
    default:
      std::cout << "-I- PndRecoKalmanTask::SetParticleHypo: Not recognised PID set -> Using default MUON hypothesis" << std::endl;
      fPDGHyp = -13;
      break;
  }
}
  ClassImp(PndRecoKalmanTask);
