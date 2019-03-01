//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndRecoMultiKalmanTask
//      see PndRecoMultiKalmanTask.h for details
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
#include "PndRecoMultiKalmanTask.h"

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

PndRecoMultiKalmanTask::PndRecoMultiKalmanTask(const char* name, Int_t iVerbose, TString fithypo)
  : PndPersistencyTask(name, iVerbose), fTrackInBranchName(""),
    fTrackOutBranchName(""), fMvdBranchName(""), fCentralTrackerBranchName(""),
    fFitter(),  fPersistence(kTRUE),
    fUseGeane(kTRUE), fIdealHyp(kFALSE),
    fPropagateToIP(kFALSE), fPropagateDistance(2.f), fPerpPlane(kFALSE),fTrackRep(0),
    fNumIt(1), fBusyCut(20)
{
  fFitWithHypo = fithypo;
  fFitTrackArrayElectron = new TClonesArray("PndTrack");
  fFitTrackArrayMuon     = new TClonesArray("PndTrack");
  fFitTrackArrayPion     = new TClonesArray("PndTrack");
  fFitTrackArrayKaon     = new TClonesArray("PndTrack");
  fFitTrackArrayProton   = new TClonesArray("PndTrack");
  fFitter = new PndRecoKalmanFit();
  SetPersistency(kTRUE);
}


PndRecoMultiKalmanTask::~PndRecoMultiKalmanTask()
{
}

InitStatus
PndRecoMultiKalmanTask::Init()
{

  fFitter->SetGeane(fUseGeane);
  fFitter->SetPropagateToIP(fPropagateToIP);
  fFitter->SetPropagateDistance(fPropagateDistance);
  fFitter->SetPerpPlane(fPerpPlane);
  fFitter->SetNumIterations(fNumIt);
  fFitter->SetMvdBranchName(fMvdBranchName);
  fFitter->SetCentralTrackerBranchName(fCentralTrackerBranchName);
  fFitter->SetVerbose(fVerbose);
  fFitter->SetTrackRep(fTrackRep);
  if (!fFitter->Init()) return kFATAL;

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
  {
    Error("PndRecoMultiKalmanTask::Init","RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackInBranchName);
  if(fTrackArray==0)
  {
    Error("PndRecoMultiKalmanTask::Init","track-array not found!");
    return kERROR;
  }
  unsigned int nfits = 0;
  fHypoFlag[0] = false;
  fHypoFlag[1] = false;
  fHypoFlag[2] = false;
  fHypoFlag[3] = false;
  fHypoFlag[4] = false;
  std::cout<<fFitWithHypo<<std::endl;
  if (fFitWithHypo.Contains("electron")) {
    nfits++;
    fHypoFlag[0] = true;
    ioman->Register(fTrackOutBranchName+"Electron","Gen", fFitTrackArrayElectron, GetPersistency());
  }
  if (fFitWithHypo.Contains("muon")) {
    nfits++;
    fHypoFlag[1] = true;
    ioman->Register(fTrackOutBranchName+"Muon",    "Gen", fFitTrackArrayMuon,     GetPersistency());
  }
  if (fFitWithHypo.Contains("pion")) {
    nfits++;
    fHypoFlag[2] = true;
    ioman->Register(fTrackOutBranchName+"Pion",    "Gen", fFitTrackArrayPion,     GetPersistency());
  }
  if (fFitWithHypo.Contains("kaon")) {
    nfits++;
    fHypoFlag[3] = true;
    ioman->Register(fTrackOutBranchName+"Kaon",    "Gen", fFitTrackArrayKaon,     GetPersistency());
  }
  if (fFitWithHypo.Contains("proton")) {
    nfits++;
    fHypoFlag[4] = true;
    ioman->Register(fTrackOutBranchName+"Proton",  "Gen", fFitTrackArrayProton,   GetPersistency());
  }
  if (nfits == 0) {
    std::cout<<" -I- PndRecoMultiKalmanTask::Init: No hypotheses given, running kalman filter with all 5 hypothesis" << std::endl;
    fHypoFlag[0] = true;
    fHypoFlag[1] = true;
    fHypoFlag[2] = true;
    fHypoFlag[3] = true;
    fHypoFlag[4] = true;
    ioman->Register(fTrackOutBranchName+"Electron","Gen", fFitTrackArrayElectron, GetPersistency());
    ioman->Register(fTrackOutBranchName+"Muon",    "Gen", fFitTrackArrayMuon,     GetPersistency());
    ioman->Register(fTrackOutBranchName+"Pion",    "Gen", fFitTrackArrayPion,     GetPersistency());
    ioman->Register(fTrackOutBranchName+"Kaon",    "Gen", fFitTrackArrayKaon,     GetPersistency());
    ioman->Register(fTrackOutBranchName+"Proton",  "Gen", fFitTrackArrayProton,   GetPersistency());
  }
  return kSUCCESS;
}

void PndRecoMultiKalmanTask::SetParContainers()
{
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
  fFtsParameters = (PndGeoFtsPar*) rtdb->getContainer("PndGeoFtsPar");
}

void PndRecoMultiKalmanTask::Exec(Option_t*)
{
  if (fVerbose>0) std::cout<<"PndRecoMultiKalmanTask::Exec"<<std::endl;

  fFitTrackArrayElectron->Clear();
  fFitTrackArrayMuon->Clear();
  fFitTrackArrayPion->Clear();
  fFitTrackArrayKaon->Clear();
  fFitTrackArrayProton->Clear();

  Int_t ntracks=fTrackArray->GetEntriesFast();

  // Detailed output
  if (fVerbose>1) std::cout << " -I- PndRecoMultiKalmanTask: contains " << ntracks << " Tracks."<< std::endl;

  // Cut too busy events TODO
  if(ntracks>fBusyCut)
  {
    std::cout<<" -I- PndRecoMultiKalmanTask::Exec: ntracks=" << ntracks << " Evil Event! skipping" << std::endl;
    return;
  }


  for(Int_t itr=0; itr<ntracks; ++itr)
  {
    if (fVerbose>1) std::cout<<"starting track"<<itr<<std::endl;
    PndTrack *prefitTrack = (PndTrack*)fTrackArray->At(itr);
    Int_t  fCharge= prefitTrack->GetParamFirst().GetQ();

    if (fHypoFlag[0]) { // Electron
      Int_t PDGCode = -11*fCharge;
      PndTrack *fitTrack = new PndTrack();
      fitTrack = fFitter->Fit(prefitTrack, PDGCode);

      TClonesArray& trkRef = *fFitTrackArrayElectron;
      Int_t size = trkRef.GetEntriesFast();
      new(trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
                                 fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), itr, kLheTrack);//PndTrack* pndTrack =  //[R.K.03/2017] unused variable
    } // end of electron

    if (fHypoFlag[1]) { // Muon
      Int_t PDGCode = -13*fCharge;
      PndTrack *fitTrack = new PndTrack();
      fitTrack = fFitter->Fit(prefitTrack, PDGCode);

      TClonesArray& trkRef = *fFitTrackArrayMuon;
      Int_t size = trkRef.GetEntriesFast();
      new(trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
                                 fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), itr, kLheTrack);//PndTrack* pndTrack =  //[R.K.03/2017] unused variable
    } // end of Muon

    if (fHypoFlag[2]) { // Pion
      Int_t PDGCode = 211*fCharge;
      PndTrack *fitTrack = new PndTrack();
      fitTrack = fFitter->Fit(prefitTrack, PDGCode);

      TClonesArray& trkRef = *fFitTrackArrayPion;
      Int_t size = trkRef.GetEntriesFast();
      new(trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
                                 fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), itr, kLheTrack);//PndTrack* pndTrack =  //[R.K.03/2017] unused variable
    } // end of Pion

    if (fHypoFlag[3]) { // Kaon
      Int_t PDGCode = 321*fCharge;
      PndTrack *fitTrack = new PndTrack();
      fitTrack = fFitter->Fit(prefitTrack, PDGCode);

      TClonesArray& trkRef = *fFitTrackArrayKaon;
      Int_t size = trkRef.GetEntriesFast();
      new(trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
                                 fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), itr, kLheTrack);//PndTrack* pndTrack =  //[R.K.03/2017] unused variable
    } // end of Kaon

    if (fHypoFlag[4]) { // Proton
      Int_t PDGCode = 2212*fCharge;
      PndTrack *fitTrack = new PndTrack();
      fitTrack = fFitter->Fit(prefitTrack, PDGCode);

      TClonesArray& trkRef = *fFitTrackArrayProton;
      Int_t size = trkRef.GetEntriesFast();
      new(trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(),
                                 fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(), fitTrack->GetPidHypo(), itr, kLheTrack);//PndTrack* pndTrack =  //[R.K.03/2017] unused variable
    } // end of Proton

  } // end of track loop

  if (fVerbose>0) std::cout<<"Fitting done"<<std::endl;

  return;
}

ClassImp(PndRecoMultiKalmanTask);

