#include "PndMvdSttGemRiemannTrackFinderTask.h"

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
#include "FairRunAna.h"

// PndMvd includes
#include "PndTrackCand.h"
#include "PndMvdSttGemRiemannTrackFinder.h"

#include "PndSttMapCreator.h"


PndMvdSttGemRiemannTrackFinderTask::PndMvdSttGemRiemannTrackFinderTask() :
	FairTask("MVD Riemann Track Finder"), fMaxSZChi2(1), fMaxSZDist(10), fMinPointDist(1), fMaxDist(1), fEventNr(0), fB(2.0), fInitDone(kFALSE), fCutChi2H(0), fCutDistH(0), fPersistence(kTRUE)
{
  PndGeoHandling::Instance();
}

PndMvdSttGemRiemannTrackFinderTask::~PndMvdSttGemRiemannTrackFinderTask()
{
}

void PndMvdSttGemRiemannTrackFinderTask::SetParContainers()
{
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndMvdSttGemRiemannTrackFinderTask::ReInit()
{
  InitStatus stat=kSUCCESS;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdSttGemRiemannTrackFinderTask::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdSttGemRiemannTrackFinderTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
//  if (FairRunAna::Instance()->IsTimeStamp()){
//	  if (fTimeBasedHitBranch.size() == 0){
//		  std::cout << "-W- PndMvdSttGemRiemannTrackFinderTask::Init: " << "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!" << std::endl;
//		  fTimeBasedHitBranch.push_back(FairTSBufferParameters("MVDHitsPixel", new TimeGap(), 10));
//	  }
//  }
  if (fHitBranch.size() == 0){
	  std::cout << "-W- PndMvdSttGemRiemannTrackFinderTask::Init: " << "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!" << std::endl;
	  fHitBranch.push_back("MVDHitsPixel");
	  fHitBranch.push_back("MVDHitsStrip");
	  fHitBranch.push_back("STTHit");
	  fHitBranch.push_back("GEMHit");
  }

  for (int i = 0; i < (int)fHitBranch.size(); i++){
	  InitHitArray(fHitBranch[i]);
  }



  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("MVDRiemannTrackCand", "MVD", fTrackCandArray, fPersistence);

  fTrackArray = new TClonesArray("PndTrack");
  ioman->Register("MVDTrack", "MVD", fTrackArray, fPersistence);

  fRiemannTrackArray = new TClonesArray("PndRiemannTrack");
//  ioman->Register("MVDRiemannTrack", "MVD", fRiemannTrackArray, kTRUE);


  fStopFunctor= new StopTime();
  fTimeGapFunctor = new TimeGap();

  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();

  std::cout << "-I- PndMvdSttGemRiemannTrackFinderTask: Initialisation successfull" << std::endl;
  fInitDone = kTRUE;
  return kSUCCESS;
}

void PndMvdSttGemRiemannTrackFinderTask::AddHitBranch(TString branchName)
{
	if (fInitDone == kFALSE)
		fHitBranch.push_back(branchName);
	else
		std::cout << "-W- AddHitBranch has to be called before the Init() of the task!" << std::endl;
}

//void PndMvdSttGemRiemannTrackFinderTask::AddHitBranch(FairTSBufferParameters param)
//{
//	if (fInitDone == kFALSE)
//		fTimeBasedHitBranch.push_back(param);
//	else
//		std::cout << "-W- AddHitBranch has to be called before the Init() of the task!" << std::endl;
//}

// -----   Public method Exec   --------------------------------------------
void PndMvdSttGemRiemannTrackFinderTask::Exec(Option_t* opt)
{

	SetVerbose(3);
  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");

  fTrackCandArray->Delete();
  fTrackArray->Delete();
 // fRiemannTrackArray->Delete();

  PndMvdSttGemRiemannTrackFinder trackFinder;
  trackFinder.SetVerbose(fVerbose);
  trackFinder.SetSttTubeArray(fTubeArray);

  FillHitArray();

  FairRootManager *ioman = FairRootManager::Instance();

  std::cout << std::endl;
  std::cout << "------------- event " << fEventNr << "----------------" << std::endl;

  for (int i = 0; i < (int)fHitBranch.size(); i++){
	  trackFinder.AddHits(fHitArray[i], ioman->GetBranchId(fHitBranch[i]));
	  std::cout << "TrackFinder.AddHits: " << ioman->GetBranchId(fHitBranch[i]) << std::endl;
  }
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


  if (fVerbose > 0) std::cout << " -I- PndMvdSttGemRiemannTrackFinderTask::Exec : Found Tracks: " << trackFinder.NTracks() << " in event no. " << fEventNr++ << std::endl;
  if (fVerbose > 0) std::cout << " -I- PndMvdSttGemRiemannTrackFinderTask::Exec : ----------------" << std::endl;


  for (int i = 0; i < trackFinder.NTracks(); i++){
	  PndTrackCand* myCand = new ((*fTrackCandArray)[i])PndTrackCand(trackFinder.GetTrackCand(i));
//	  if (fVerbose > 1)
//	  {
//		  std::cout << "Track " << i << std::endl;
//		  std::cout << "Links: ";
//		  ((FairMultiLinkedData*) myCand)->Print();
//		  std::cout << std::endl;
//	  }

	  myCand->CalcTimeStamp();
//	  if (fVerbose > 1)
//		  trackFinder.GetTrack(i).Print();
	  //PndRiemannTrack myTrack = trackFinder.GetTrack(i);

	  PndTrack* myTrack = new ((*fTrackArray)[i])PndTrack(trackFinder.GetPndTrack(i, fB));
	  if (myCand->GetTimeStamp() == 0){
		  myTrack->SetTimeStamp(0.0001 * (i+1));
		  myCand->SetTimeStamp(0.0001 * (i+1));

	  } else {
		  myTrack->SetTimeStamp(myCand->GetTimeStamp());
		  myTrack->SetTimeStampError(myCand->GetTimeStampError());
	  }
	  myTrack->SetLink(FairLink("MVDRiemannTrackCand", i));
	  myTrack->SetTrackCandRef(myCand);

	  if (fVerbose > 1) {
		  std::cout << i << ": ";
		  myTrack->Print();
	  }
//	  PndRiemannTrack* newTrack = new ((*fRiemannTrackArray)[i])PndRiemannTrack();
//	  TVectorD origin = myTrack.orig();
//	  newTrack->init(origin[0], origin[1], myTrack.r(), myTrack.dip(), 0);
//	  for (int j = 0; j < myTrack.getNumHits(); j++){
//		  newTrack->addHit(*myTrack.getHit(j));
//	  }
  }
  fTrackCandArray->Sort();
  fTrackArray->Sort();

  for (int i = 0; i < fTrackCandArray->GetEntriesFast(); i++){
	  PndTrackCand* myCand =(PndTrackCand*)fTrackCandArray->At(i);
	  if (fVerbose > 1)
	  {
		  std::cout << "Track " << i << std::endl;
		  std::cout << "Links: ";
		  ((FairMultiLinkedData*) myCand)->Print();
		  std::cout << std::endl;
	  }
  }
}

void PndMvdSttGemRiemannTrackFinderTask::FinishEvent()
{
	fTrackCandArray->Delete();
	fTrackArray->Delete();
//	fRiemannTrackArray->Delete();
}

void PndMvdSttGemRiemannTrackFinderTask::InitHitArray(TString branchName)
{
	  TClonesArray* tempArray = (TClonesArray*) FairRootManager::Instance()->GetObject(branchName);
	  if (tempArray == 0){
		  std::cout << "-W- PndMvdSttGemRiemannTrackFinderTask::Init: " << "No hitArray for BranchName " << branchName.Data() << std::endl;
	  }
	  fHitArray.push_back(tempArray);
}

void PndMvdSttGemRiemannTrackFinderTask::FillHitArray()
{
	Double_t eventTime = -1;
	if (FairRunAna::Instance()->IsTimeStamp()){
		fHitArray[0]->Delete();
		fHitArray[1]->Delete();

		fHitArray[0] = FairRootManager::Instance()->GetData("MVDHitsPixel", fTimeGapFunctor, 10); //FairRootManager::Instance()->GetEventTime() +
		std::cout << "PixelHits: " << fHitArray[0]->GetEntriesFast() << std::endl;
		if (fHitArray[0]->GetEntriesFast() > 1){
			FairTimeStamp* data = (FairTimeStamp*)fHitArray[0]->At(1);
			eventTime = data->GetTimeStamp();
			std::cout << "EventTime: " << eventTime << std::endl;
			fHitArray[1] = FairRootManager::Instance()->GetData("MVDHitsStrip", fStopFunctor, eventTime - 10, fStopFunctor, eventTime + 10);
			std::cout << "StripHits: " << fHitArray[1]->GetEntriesFast() << std::endl;
		}
	}
}

ClassImp(PndMvdSttGemRiemannTrackFinderTask);

