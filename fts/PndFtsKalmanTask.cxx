//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndFtsKalmanTask
//      see PndFtsKalmanTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert  TUM               (original author)
//      Stefano Spataro    UNI Torino        (PndRecoKalmanTask implementation)
//      Ralf Kliemt        Uni Bonn          (made it for Fts+Gem+Mvd)
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndFtsKalmanTask.h"

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

PndFtsKalmanTask::PndFtsKalmanTask(const char* name, Int_t iVerbose)
: FairTask(name, iVerbose), fPersistence(kFALSE), fPDGHyp(211)
{
  fTrackInBranchName  = "FTSTrkIdeal"; 
  fTrackOutBranchName = "FTSGenTrack";
  
  fFitTrackArray = new TClonesArray("PndTrack");
  fUseGeane = kTRUE;
  fNumIt = 1;
  fFitter = new PndFtsKalmanFit();
  fBusyCut=20;
}


PndFtsKalmanTask::~PndFtsKalmanTask()
{
}

InitStatus
PndFtsKalmanTask::Init()
{
  
  fFitter->SetGeane(fUseGeane);
  fFitter->SetNumIterations(fNumIt);
  fFitter->SetVerbose(fVerbose);
  if (!fFitter->Init()) return kFATAL;
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
  {
    Error("PndFtsKalmanTask::Init","RootManager not instantiated!");
    return kERROR;
  }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackInBranchName);
  if(fTrackArray==0)
  {
    Error("PndFtsKalmanTask::Init","track-array not found!");
    return kERROR;
  }
  
  FairRootManager::Instance()->
  Register(fTrackOutBranchName,"GenFw", fFitTrackArray, kTRUE);
  
  return kSUCCESS;
}

void PndFtsKalmanTask::SetParContainers() {
  //FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  //rtdb->getContainer("PndGeoSttPar");
}

void PndFtsKalmanTask::Exec(Option_t* opt)
{
  fFitTrackArray->Clear("C");
  
  if(0==fTrackArray) return;
  Int_t ntracks=fTrackArray->GetEntriesFast();
  
  // Detailed output
  if (fVerbose>1) Info("Exec","Input array %s contains %i tracks.",fTrackArray->GetName(), ntracks);
  
  if(ntracks>fBusyCut)
  {
    Warning("Exec","ntracks=%i -- Evil Event! skipping...",ntracks);
    return;
  }
  
  
  for(Int_t itr=0;itr<ntracks;++itr)
  {
    if (fVerbose>2) Info("Exec","starting track %i",itr);
    
    TClonesArray& trkRef = *fFitTrackArray;
    Int_t size = trkRef.GetEntriesFast();
    
    PndTrack *prefitTrack = (PndTrack*)fTrackArray->At(itr);
    Int_t  fCharge= prefitTrack->GetParamFirst().GetQ();
    Int_t PDGCode= fPDGHyp*fCharge;
    
    PndTrack *fitTrack = new PndTrack();
    if (fVerbose>2) Info("Exec","Calling fitter",itr);
    fitTrack = fFitter->Fit(prefitTrack, PDGCode);
    if (fVerbose>2) Info("Exec","Fitter done, store the track.",itr);
    PndTrack* pndTrack = new(trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
                                                    fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), itr, FairRootManager::Instance()->GetBranchId(fTrackInBranchName));
  }
  
  if (fVerbose>1)Info("Exec","Fitting done");
  
  return;
}

void PndFtsKalmanTask::SetParticleHypo(TString h)
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
    std::cout << "-I- PndFtsKalmanTask::SetParticleHypo: Not recognised PID set -> Using default MUON hypothesis" << std::endl;
    fPDGHyp=-13; // Muon is default.
  }
}

void PndFtsKalmanTask::SetParticleHypo(Int_t h)
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
      std::cout << "-I- PndFtsKalmanTask::SetParticleHypo: Not recognised PID set -> Using default MUON hypothesis" << std::endl;
      fPDGHyp = -13;
      break;
  }
}
  ClassImp(PndFtsKalmanTask);
