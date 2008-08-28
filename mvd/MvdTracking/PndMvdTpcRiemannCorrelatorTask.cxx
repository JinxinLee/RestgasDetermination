#include "PndMvdTpcRiemannCorrelatorTask.h"

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
#include "PndMvdTpcRiemannCorrelatorTask.h"
#include "PndTpcHit.h"
#include "PndTpcCluster.h"
#include "PndRiemannHit.h"

PndMvdTpcRiemannCorrelatorTask::PndMvdTpcRiemannCorrelatorTask() : CbmTask("MVD TPC Riemann Track Correlator"),
fMaxDist(1), fMaxSZ(1), fMaxSZChi2(1)
{
	fHitBranchMVDPixel = "MVDHitsPixel";
	fHitBranchMVDStrip = "MVDHitsStrip";
	fHitBranchTPC = "PndTpcCluster";
	fTrackBranch = "MVDRiemannTrackCand";
	
	fEventNr = 0;
	//fTrackBranch = "MCTrack";
}

PndMvdTpcRiemannCorrelatorTask::~PndMvdTpcRiemannCorrelatorTask()
{
}

void PndMvdTpcRiemannCorrelatorTask::SetParContainers()
{
  // Get Base Container
/*  
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
*/
}

InitStatus PndMvdTpcRiemannCorrelatorTask::ReInit()
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
InitStatus PndMvdTpcRiemannCorrelatorTask::Init() 
{
  
  CbmRootManager* ioman = CbmRootManager::Instance();

  if ( ! ioman ) 
    {
      std::cout << "-E- PndMvdIdealTrackFinderTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }
    
  // Get input array
  fHitArrayMVDPixel = (TClonesArray*) ioman->GetObject(fHitBranchMVDPixel);
  if ( !fHitArrayMVDPixel){
    std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No hitArray Pixel!" << std::endl;
    return kERROR;
  }
  
  fHitArrayMVDStrip = (TClonesArray*) ioman->GetObject(fHitBranchMVDStrip);
   if ( !fHitArrayMVDStrip){
     std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No hitArray Strip!" << std::endl;
     return kERROR;
   }

  fHitArrayTPC = (TClonesArray*) ioman->GetObject(fHitBranchTPC);
    if ( !fHitArrayTPC){
      std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No hitArray TPC!" << std::endl;
      return kERROR;
    } 
    
   fTrackCandArray = (TClonesArray*) ioman->GetObject(fTrackBranch);
     if ( !fTrackCandArray){
       std::cout << "-W- PndMvdIdealTrackFinderTask::Init: " << "No hitArray2!" << std::endl;
       return kERROR;
     }
   
  fCombinedArray = new TClonesArray("TrackCand");
  ioman->Register("MVD_TPC_CombinedRiemannTrack", "MVD", fCombinedArray, kTRUE);
  
  std::cout << "-I- PndMvdTpcRiemannCorrelator: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdTpcRiemannCorrelatorTask::Exec(Option_t* opt) 
{

  // Reset output array
  if ( ! fCombinedArray )
    Fatal("Exec", "No CombinedTrackCandArray");
  fCombinedArray->Clear();
  
  std::vector<TH1*> distHistos;
  std::vector<TH1*> szDistHistos;
  std::vector<TH1*> szChi2Histos;
  
  TString distHName("distHisto_");
  TString szDistHName("szdistHisto_");
  TString szChi2HName("szChi2Histo_");
  
  if (fVerbose > 0)
 	std::cout << "Analysing Event Nr: " << fEventNr << std::endl;
    
  int foundTracks = 0;
  if (fTrackCandArray->GetEntriesFast() > 100)
	  return;
  for (int trackInd = 0; trackInd < fTrackCandArray->GetEntriesFast(); trackInd++){				//run through all tracks;
	  TrackCand* myMvdTrackCand = (TrackCand*)fTrackCandArray->At(trackInd);
	 // PndRiemannTrack myTrack = GetRiemannTrack(myMvdTrackCand);
	 // PndRiemannTrack newTrack = GetRiemannTrack(myMvdTrackCand);
	  TString name = distHName;
/*	  name+= fEventNr;
	  name+= "_";
	  name+= trackInd;
	  TH1D* myDistHisto = new TH1D(name.Data(), "Riemann Distance to Track", 200, -10,10);
	  name = szDistHName;
	  name+= fEventNr;
	  name+= "_";
	  name+= trackInd;
	  TH1D* mySZDistHisto = new TH1D(name.Data(), "Riemann SZDistance", 200, -10,10);
	  name = szChi2HName;
	  name+= fEventNr;
	  name+= "_";
	  name+= trackInd;
	  TH1D* mySZChi2Histo = new TH1D(name.Data(), "Riemann SZ Chi2", 200, 0,10);
	*/
	  if (fVerbose > 0)
		  std::cout << "TrackNr: " << trackInd << std::endl;
	  if (fVerbose > 0)
		  std::cout << "First iteration - Candidates: ";
	  TrackCand newCand = AddTPCHits(myMvdTrackCand); //first iteration
	  if (fVerbose > 0){
		  std::cout << newCand.getNHits() << std::endl;
		  std::cout << "Second iteration - Candidates: ";
	  }
	  
//	  TrackCand finalCand = newCand;
	  TrackCand finalCand = AddTPCHits(&newCand);
	  if (fVerbose > 0)
		  std::cout << finalCand.getNHits() << std::endl;
	/*  for (int tpcCluster = 0; tpcCluster < fHitArrayTPC->GetEntriesFast(); tpcCluster++){
		  PndTpcCluster* myCluster = (PndTpcCluster*) fHitArrayTPC->At(tpcCluster);
		  PndTpcHit myTpcHit(*myCluster);
		  PndRiemannHit myRHit((CbmHit*)&myTpcHit);
		  double trackDist = myTrack.dist(&myRHit);
		  double szDist = myTrack.szDist(&myRHit);
		  double szChi2 = myTrack.calcSZChi2(&myRHit);
		  
		  myDistHisto->Fill(trackDist);
		  mySZDistHisto->Fill(szDist);
		  mySZChi2Histo->Fill(szChi2);
		  
		  if (fabs(szDist) < fMaxSZ && fabs(trackDist) < fMaxDist && fabs(szChi2) < fMaxSZChi2){
			  if (fVerbose > 1){
				  std::cout << "TPCCluster " << tpcCluster << " added to track " << trackInd << std::endl;
				  std::cout << "trackDist: " << trackDist << " szDist: " << szDist << " szChi2: " << szChi2 << std::endl;
			  }
			  myMvdTrackCand->addHit(3, tpcCluster);
			  newTrack.addHit(&myRHit);
		  }
			  
	  }*/
//	  distHistos.push_back(myDistHisto);
//	  szDistHistos.push_back(mySZDistHisto);/
//	  szChi2Histos.push_back(mySZChi2Histo);
//	  newTrack.refit();
//	  newTrack.szFit();
  	PndRiemannTrack finalTrack = GetRiemannTrack(&finalCand);
	  
	  finalCand.setCurv(1/finalTrack.r());
	  finalCand.setDip(finalTrack.dip());
	  
	  if (finalCand.getNHits() > 10){
		  new ((*fCombinedArray)[foundTracks])TrackCand(finalCand);
		  foundTracks++;
	  }
  }
//  fHRiemannDistVector.push_back(distHistos);
//  fHSZDistVector.push_back(szDistHistos);
//  fHSZChi2Vector.push_back(szChi2Histos);
  fEventNr++;
  

//  for (int i = 0; i < trackFinder.NTracks(); i++){
//	  new ((*fTrackCandArray)[i])TrackCand(trackFinder.GetTrackCand(i));    
//  }
}

PndRiemannTrack PndMvdTpcRiemannCorrelatorTask::GetRiemannTrack(TrackCand* cand)
{
	PndRiemannTrack result;
	for (int i = 0; i < cand->getNHits(); i++){
		unsigned int detId, hitId;
		cand->getHit(i, detId, hitId);
		CbmHit* myHit = 0;
		if (detId == 1){
			myHit = (CbmHit*)fHitArrayMVDPixel->At(hitId);
		}
		else if (detId == 2){
			myHit = (CbmHit*)fHitArrayMVDStrip->At(hitId);
		}
		if (myHit != 0)
			result.addHit(new PndRiemannHit(myHit));
	}
	result.refit();
	result.szFit();
	return result;
}

TrackCand PndMvdTpcRiemannCorrelatorTask::AddTPCHits(TrackCand* myCand)
{
	PndRiemannTrack myTrack = GetRiemannTrack(myCand);
	TrackCand newCand = *myCand;
	for (int tpcCluster = 0; tpcCluster < fHitArrayTPC->GetEntriesFast(); tpcCluster++){
		if (myCand->HitInTrack(3, tpcCluster)) continue;
		PndTpcCluster* myCluster = (PndTpcCluster*) fHitArrayTPC->At(tpcCluster);
		PndTpcHit myTpcHit(*myCluster);
		PndRiemannHit myRHit((CbmHit*)&myTpcHit);
		double trackDist = myTrack.dist(&myRHit);
		double szDist = myTrack.szDist(&myRHit);
		double szChi2 = myTrack.calcSZChi2(&myRHit);
		  
//		  myDistHisto->Fill(trackDist);
//		  mySZDistHisto->Fill(szDist);
//		  mySZChi2Histo->Fill(szChi2);
		  
		if (fabs(szDist) < fMaxSZ && fabs(trackDist) < fMaxDist && fabs(szChi2) < fMaxSZChi2){
			if (fVerbose > 1){
				std::cout << "TPCCluster " << tpcCluster << " added" <<  std::endl;
				std::cout << "trackDist: " << trackDist << " szDist: " << szDist << " szChi2: " << szChi2 << std::endl;
			}
			newCand.addHit(3, tpcCluster);
			//newTrack.addHit(&myRHit);
		}
	}
	return newCand;
}

void PndMvdTpcRiemannCorrelatorTask::DrawDistHistos(TCanvas* can, int event)
{
	DrawHistos(can, fHRiemannDistVector.at(event));
}

void PndMvdTpcRiemannCorrelatorTask::DrawSZHistos(TCanvas* can, int event)
{
	DrawHistos(can, fHSZDistVector.at(event));
}

void PndMvdTpcRiemannCorrelatorTask::DrawSZChi2Histos(TCanvas* can, int event)
{
	DrawHistos(can, fHSZChi2Vector.at(event));
}

void PndMvdTpcRiemannCorrelatorTask::DrawHistos(TCanvas* can, std::vector<TH1*> histos)
{
	can->Divide(3,3);
	
	for (int i = 0; i < histos.size() && i < 9; i++){
		can->cd(i+1);
		can->Update();
		histos.at(i)->DrawClone();
	}	
}


ClassImp(PndMvdTpcRiemannCorrelatorTask);

