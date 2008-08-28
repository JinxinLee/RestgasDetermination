#include "PndMvdRiemannTrackFinderTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmMCTrack.h"


// PndMvd includes
#include "PndMvdRecoHit.h"
// #include "PndMvdTrackCand.h"
#include "Track.h"
#include "TrackCand.h"
#include "PndMvdHit.h"
#include "PndMvdMCPoint.h"
#include "PndMvdCluster.h"
#include "PndMvdDigi.h"
#include "PndRiemannTrackFinder.h"

PndMvdRiemannTrackFinderTask::PndMvdRiemannTrackFinderTask() : CbmTask("MVD Riemann Track Finder"),
	fMaxDist(0.1), fMinPointDist(0.5), fMaxSZChi2(1)
{
	fHitBranch = "MVDHitsPixel";
	fHitBranch2 = "MVDHitsStrip";
	fMaxSZChi2 = 1;
	fMaxSZDist = 10;
	fMinPointDist = 1;
	fMaxDist = 1;
	
	fEventNr = 0;
	//fTrackBranch = "MCTrack";
}

PndMvdRiemannTrackFinderTask::~PndMvdRiemannTrackFinderTask()
{
}

void PndMvdRiemannTrackFinderTask::SetParContainers()
{
  // Get Base Container
/*  
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
*/
}

InitStatus PndMvdRiemannTrackFinderTask::ReInit()
{
  
  InitStatus stat=kERROR;
  return stat;
  
  /*
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
  
  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdRiemannTrackFinderTask::Init() 
{
  
  CbmRootManager* ioman = CbmRootManager::Instance();

  if ( ! ioman ) 
    {
      std::cout << "-E- PndMvdIdealTrackFinderTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }
    
  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject(fHitBranch);
  if ( !fHitArray){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No hitArray!" << std::endl;
    return kERROR;
  }
  
  fHitArray2 = (TClonesArray*) ioman->GetObject(fHitBranch2);
   if ( !fHitArray2){
     std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No hitArray2!" << std::endl;
     return kERROR;
   }
   
  fTrackCandArray = new TClonesArray("TrackCand");
  ioman->Register("MVDRiemannTrackCand", "MVD", fTrackCandArray, kTRUE);
  
  std::cout << "-I- PndMvdIdealTrackFinderTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdRiemannTrackFinderTask::Exec(Option_t* opt) 
{

  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Clear();

  PndRiemannTrackFinder trackFinder;
  trackFinder.SetVerbose(fVerbose);
  
  trackFinder.AddHits(fHitArray);
  trackFinder.AddHits(fHitArray2);
  trackFinder.SetMaxSZChi2(fMaxSZChi2);
  trackFinder.SetMinPointDist(fMinPointDist);
  trackFinder.SetMaxPlaneDistance(fMaxDist);
  trackFinder.SetMaxSZDist(fMaxSZDist);
  trackFinder.SetVerbose(fVerbose);
  trackFinder.FindTracks();
  
  std::cout << "Found Tracks: " << trackFinder.NTracks() << " in event no. " << fEventNr++ << std::endl;
  std::cout << "----------------" << std::endl;
 // std::vector<TrackCand> myCand = trackFinder.GetTrackCand();
  for (int i = 0; i < trackFinder.NTracks(); i++){
	  TrackCand myCand = trackFinder.GetTrackCand(i);
// 	  std::cout << myCand;
	  new ((*fTrackCandArray)[i])TrackCand(trackFinder.GetTrackCand(i));    
  }
}

ClassImp(PndMvdRiemannTrackFinderTask);

