#include "PndMvdRiemannTrackFinderTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

// PndMvd includes
#include "PndTrackCand.h"
#include "PndMvdRiemannTrackFinder.h"


PndMvdRiemannTrackFinderTask::PndMvdRiemannTrackFinderTask() : FairTask("MVD Riemann Track Finder")
{
	fHitBranch = "MVDHitsPixel";
	fHitBranch2 = "MVDHitsStrip";
	fMaxSZChi2 = 1;
	fMaxSZDist = 10;
	fMinPointDist = 1;
	fMaxDist = 1;

	fEventNr = 0;
}

PndMvdRiemannTrackFinderTask::~PndMvdRiemannTrackFinderTask()
{
}

void PndMvdRiemannTrackFinderTask::SetParContainers()
{
//	fGeoH = PndGeoHandling::Instance();
}

InitStatus PndMvdRiemannTrackFinderTask::ReInit()
{
  InitStatus stat=kERROR;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdRiemannTrackFinderTask::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdRiemannTrackFinderTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject(fHitBranch);
  if ( !fHitArray){
    std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: " << "No hitArray!" << std::endl;
    return kERROR;
  }

  fHitArray2 = (TClonesArray*) ioman->GetObject(fHitBranch2);
   if ( !fHitArray2){
     std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: " << "No hitArray2!" << std::endl;
     return kERROR;
   }

  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("MVDRiemannTrackCand", "MVD", fTrackCandArray, kTRUE);

  std::cout << "-I- PndMvdRiemannTrackFinderTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdRiemannTrackFinderTask::Exec(Option_t* opt)
{

  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");

  fTrackCandArray->Clear();

  PndMvdRiemannTrackFinder trackFinder;
  trackFinder.SetVerbose(fVerbose);

  trackFinder.AddHits(fHitArray);
  trackFinder.AddHits(fHitArray2);
  trackFinder.SetMaxSZChi2(fMaxSZChi2);
  trackFinder.SetMinPointDist(fMinPointDist);
  trackFinder.SetMaxPlaneDistance(fMaxDist);
  trackFinder.SetMaxSZDist(fMaxSZDist);
  trackFinder.SetVerbose(fVerbose);
  trackFinder.SetMinNumberOfHits(4);
  //trackFinder.SetGeoH(fGeoH);

  trackFinder.SetCutDistH(fCutDistH);
  trackFinder.SetCutChi2H(fCutChi2H);

  trackFinder.FindTracks();


  std::cout << "Found Tracks: " << trackFinder.NTracks() << " in event no. " << fEventNr++ << std::endl;
  std::cout << "----------------" << std::endl;

  for (int i = 0; i < trackFinder.NTracks(); i++){
	  new ((*fTrackCandArray)[i])PndTrackCand(trackFinder.GetTrackCand(i));

  }
}

void PndMvdRiemannTrackFinderTask::FinishEvent()
{
	fTrackCandArray->Clear();
}

ClassImp(PndMvdRiemannTrackFinderTask);

