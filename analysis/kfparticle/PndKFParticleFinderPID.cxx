//-----------------------------------------------------------
//-----------------------------------------------------------

// Panda Headers ----------------------
#include "PndKFParticleFinderPID.h"
#include "PndPidCandidate.h"
#include "PndMCTrack.h"

#include "FairRunAna.h"

//ROOT headers
#include "TClonesArray.h"

//c++ and std headers
#include <iostream>

PndKFParticleFinderPID::PndKFParticleFinderPID(const char* name, Int_t iVerbose):
  FairTask(name, iVerbose), fChargedTrackBranchName(""), fNeutralTrackBranchName(""), fMCTracksBranchName(""),
  fChargedTrackArray(0), fNeutralTrackArray(0), fMCTrackArray(0),
  fPIDMode(0), fPID(0)
{
}

PndKFParticleFinderPID::~PndKFParticleFinderPID()
{
}

InitStatus PndKFParticleFinderPID::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
  {
    Error("PndKFParticleFinderPID::Init","RootManager not instantiated!");
    return kERROR;
  }
  
  // Get charged tracks
  fChargedTrackArray=(TClonesArray*) ioman->GetObject(fChargedTrackBranchName);
  if(fChargedTrackArray==0)
  {
    Error("PndKFParticleFinderPID::Init","Charged track array not found! Stop the execution.");
    return kERROR;
  }
  
  // Get neutral tracks
  fNeutralTrackArray=(TClonesArray*) ioman->GetObject(fNeutralTrackBranchName);
  if(fNeutralTrackArray==0)
  {
    Error("PndKFParticleFinderPID::Init","Neutral track array not found! Will continue without gamma reconstruction.");
  }
  
  if(fPIDMode==1)
  {
    //MC Tracks
    fMCTrackArray=(TClonesArray*) ioman->GetObject(fMCTracksBranchName);
    if(fMCTrackArray==0)
    {
      Error("StandaloneHitGenerator::Init","MC track array not found! Stop the execution.");
      return kERROR;
    }
  }
  
  return kSUCCESS;
}

void PndKFParticleFinderPID::Exec(Option_t*)
{ 
  fPID.clear();
  
  Int_t nTracks=fChargedTrackArray->GetEntriesFast();
  fPID.resize(nTracks, -1);

  if(fPIDMode==1)
    SetMCPID();
  if(fPIDMode==2)
    SetRecoPID();
}

void PndKFParticleFinderPID::Finish()
{
}

void PndKFParticleFinderPID::SetMCPID()
{ 
  Int_t nTracks=fChargedTrackArray->GetEntriesFast();
  for(int iTr=0; iTr<nTracks; iTr++)
  {
    PndPidCandidate *inTrack = (PndPidCandidate*)fChargedTrackArray->At(iTr);
    if(!inTrack) continue;
    Int_t iMC = inTrack->GetMcIndex();
    
    if(iMC >= 0)
    {
      PndMCTrack *pndMCTrack = (PndMCTrack*)fMCTrackArray->At(iMC);
      if(TMath::Abs(pndMCTrack->GetPdgCode()) > 2500)
        continue;
      fPID[iTr] = pndMCTrack->GetPdgCode();
    }
  }
}

void PndKFParticleFinderPID::SetRecoPID()
{
}

ClassImp(PndKFParticleFinderPID);
