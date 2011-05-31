#include "PndMvdTPCRiemannTrackFinderTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TVector.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

// PndMvd includes
#include "PndTrackCand.h"
#include "PndSdsHit.h"
#include "PndMvdRiemannTrackFinder.h"
#include "PndRiemannTrack.h"
#include "PndRiemannHit.h"

#include "PndTpcCluster.h"
#include "PndDetectorList.h"


PndMvdTPCRiemannTrackFinderTask::PndMvdTPCRiemannTrackFinderTask() : FairTask("MVD-TPC Riemann Track Finder")
{
	fHitBranch = "MVDHitsPixel";
	fHitBranch2 = "MVDHitsStrip";
	fHitBranchTPC = "PndTpcCluster";
	fMaxSZChi2 = 1;
	fMaxSZDist = 10;
	fMinPointDist = 1;
	fMaxDist = 1;

	fEventNr = 0;

}

PndMvdTPCRiemannTrackFinderTask::~PndMvdTPCRiemannTrackFinderTask()
{
}

void PndMvdTPCRiemannTrackFinderTask::SetParContainers()
{
}

InitStatus PndMvdTPCRiemannTrackFinderTask::ReInit()
{

  InitStatus stat=kERROR;
  return stat;

}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdTPCRiemannTrackFinderTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdTPCRiemannTrackFinderTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject(fHitBranch);
  if ( !fHitArray){
    std::cout << "-W- PndMvdTPCRiemannTrackFinderTask::Init: " << "No hitArray!" << std::endl;
    return kERROR;
  }

  fHitArray2 = (TClonesArray*) ioman->GetObject(fHitBranch2);
   if ( !fHitArray2){
     std::cout << "-W- PndMvdTPCRiemannTrackFinderTask::Init: " << "No hitArray2!" << std::endl;
     return kERROR;
   }

   fHitArrayTPC = (TClonesArray*) ioman->GetObject(fHitBranchTPC);
    if ( !fHitArrayTPC){
      std::cout << "-W- PndMvdTPCRiemannTrackFinderTask::Init: " << "No hitArrayTPC!" << std::endl;
      return kERROR;
    }
  fRiemannTracks= new TClonesArray("PndRiemannTrack");

  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("MVDRiemannTrackCand", "MVD", fTrackCandArray, kTRUE);

  fMVDTPCTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("MVDTPCRiemannTrackCand", "MVD", fMVDTPCTrackCandArray, kTRUE);


  std::cout << "-I- PndMvdTPCRiemannTrackFinderTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdTPCRiemannTrackFinderTask::Exec(Option_t* opt)
{

  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");

  fTrackCandArray->Clear();
  fRiemannTracks->Clear();
  fMVDTPCTrackCandArray->Clear();

  FairRootManager* ioman = FairRootManager::Instance();

  PndMvdRiemannTrackFinder trackFinder;
  trackFinder.SetVerbose(fVerbose);
  trackFinder.AddHits(fHitArray, ioman->GetBranchId(fHitBranch));
  trackFinder.AddHits(fHitArray2, ioman->GetBranchId(fHitBranch2));
  trackFinder.SetMaxSZChi2(fMaxSZChi2);
  trackFinder.SetMinPointDist(fMinPointDist);
  trackFinder.SetMaxPlaneDistance(fMaxDist);
  trackFinder.SetMaxSZDist(fMaxSZDist);
  trackFinder.SetVerbose(fVerbose);
  trackFinder.SetMinNumberOfHits(4);
 // trackFinder.SetGeoH(fGeoH);

  trackFinder.SetCutDistH(fCutDistH);
  trackFinder.SetCutChi2H(fCutChi2H);

  trackFinder.FindTracks();

  std::cout << "Found Tracks MVD: " << trackFinder.NTracks() << " in event no. " << fEventNr++ << std::endl;


  for (int i = 0; i < trackFinder.NTracks(); i++){
	  new ((*fTrackCandArray)[i])PndTrackCand(trackFinder.GetTrackCand(i));
	  PndRiemannTrack track;
	  for(unsigned int j=0;j<trackFinder.GetTrackCand(i).GetNHits();j++){
		  int detId,hitId;
		  PndRiemannHit hit;
		  detId=trackFinder.GetTrackCand(i).GetSortedHit(j).GetDetId();
		  hitId=trackFinder.GetTrackCand(i).GetSortedHit(j).GetHitId();
			  if (detId==FairRootManager::Instance()->GetBranchId(fHitBranch)){
				  hit.setXYZ(((PndSdsHit*)fHitArray->At(hitId))->GetPosition().X(),((PndSdsHit*)fHitArray->At(hitId))->GetPosition().Y(),((PndSdsHit*)fHitArray->At(hitId))->GetPosition().Z());
				  hit.setDXYZ(((PndSdsHit*)fHitArray->At(hitId))->GetDx(),((PndSdsHit*)fHitArray->At(hitId))->GetDy(),((PndSdsHit*)fHitArray->At(hitId))->GetDz());
			  }
			  else if (detId==FairRootManager::Instance()->GetBranchId(fHitBranch2)){
				  hit.setXYZ(((PndSdsHit*)fHitArray2->At(hitId))->GetPosition().X(),((PndSdsHit*)fHitArray2->At(hitId))->GetPosition().Y(),((PndSdsHit*)fHitArray2->At(hitId))->GetPosition().Z());
				  hit.setDXYZ(((PndSdsHit*)fHitArray2->At(hitId))->GetDx(),((PndSdsHit*)fHitArray2->At(hitId))->GetDy(),((PndSdsHit*)fHitArray2->At(hitId))->GetDz());
			  }
			  else if (detId==0){
				  hit.setXYZ(0,0,0);
				  hit.setDXYZ(0.1,0.1,0.1);
			  }
			  else  {std::cout<<"ERROR"<<"  "<<"det="<<detId<<"  hit="<<hitId<<std::endl; continue;};
			  track.addHit(hit);
	  }
	  track.refit(true);
	  track.szFit(true);
	  new((*fRiemannTracks)[i])PndRiemannTrack(track);
  }
  CheckTPCHitsForMvdSeeds();
  std::cout << "Found Tracks MVD+TPC: " <<fMVDTPCTrackCandArray->GetEntriesFast() <<std::endl;
  std::cout << "----------------" << std::endl;
}

void PndMvdTPCRiemannTrackFinderTask::CheckTPCHitsForMvdSeeds()
{
	TVector3 pos;
	for(int i=0;i<fRiemannTracks->GetEntriesFast();i++){
		int count=0;
		PndTrackCand TpcCand;
		bool sign;
		if (((PndRiemannTrack*)fRiemannTracks->At(i))->getHit(((PndRiemannTrack*)fRiemannTracks->At(i))->getNumHits()-1)->z()>0){
			sign=true;
		}
		else sign=false;
		double CutDist=GetMaxPlaneDist(((PndRiemannTrack*)fRiemannTracks->At(i))->r(),((PndRiemannTrack*)fRiemannTracks->At(i))->dip(),sign);
		double CutSZ=GetMaxSZChi2(((PndRiemannTrack*)fRiemannTracks->At(i))->r(),((PndRiemannTrack*)fRiemannTracks->At(i))->dip(),sign);
	  for(int j=0;j<fHitArrayTPC->GetEntriesFast();j++){
		  pos=((PndTpcCluster*)fHitArrayTPC->At(j))->pos();
		  PndRiemannHit hit;
		  hit.setXYZ(pos.X(),pos.Y(),pos.Z());
		  if ((fabs(((PndRiemannTrack*)fRiemannTracks->At(i))->dist(&hit))<CutDist) &&(((PndRiemannTrack*)fRiemannTracks->At(i))->calcSZChi2(&hit)<CutSZ)){
			  count++;
			  TpcCand.AddHit("PndTpcCluster",j,0);
		  }
	  }
	  if (count>3){
		  int size=fMVDTPCTrackCandArray->GetEntriesFast();
		  new ((*fMVDTPCTrackCandArray)[size])PndTrackCand(*(PndTrackCand*)fTrackCandArray->At(i));
			  for(unsigned int k=0;k<TpcCand.GetNHits();k++){
			  ((PndTrackCand*)fMVDTPCTrackCandArray->At(size))->AddHit(TpcCand.GetSortedHit(k).GetDetId(),TpcCand.GetSortedHit(k).GetHitId(),TpcCand.GetSortedHit(k).GetRho());
			  }
	  }

	}

}

double PndMvdTPCRiemannTrackFinderTask::GetMaxPlaneDist(double radius, double dip, bool sign)
{
	double Pt=((radius/100)*2*3*1E8)/1E9;
	double Theta;
	  if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	  else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
    if(fTPCCutDistH!=NULL){
    	double minPt=fTPCCutDistH->GetXaxis()->GetXmin();
    	double maxPt=fTPCCutDistH->GetXaxis()->GetXmax();
    	double minTh=fTPCCutDistH->GetYaxis()->GetXmin();
    	double maxTh=fTPCCutDistH->GetYaxis()->GetXmax();

    	int binPt=TMath::FloorNint((Pt-minPt)*fTPCCutDistH->GetXaxis()->GetNbins()/(maxPt-minPt))+1;
    	int binTh=TMath::FloorNint((Theta-minTh)*fTPCCutDistH->GetYaxis()->GetNbins()/(maxTh-minTh))+1;

    	if (binPt<1) binPt=1;
    	if (binPt>fTPCCutDistH->GetXaxis()->GetNbins()) binPt=fTPCCutDistH->GetXaxis()->GetNbins();
    	if (binTh<1) binTh=1;
    	if (binTh>fTPCCutDistH->GetYaxis()->GetNbins()) binTh=fTPCCutDistH->GetYaxis()->GetNbins();
    	return fTPCCutDistH->GetBinContent(binPt,binTh);

    }
    else return 1;
}
double PndMvdTPCRiemannTrackFinderTask::GetMaxSZChi2(double radius, double dip, bool sign)
{
	double Pt=((radius/100)*2*3*1E8)/1E9;
	double Theta;
	  if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	  else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
    if(fTPCCutChi2H!=NULL){
    	double minPt=fTPCCutChi2H->GetXaxis()->GetXmin();
    	double maxPt=fTPCCutChi2H->GetXaxis()->GetXmax();
    	double minTh=fTPCCutChi2H->GetYaxis()->GetXmin();
    	double maxTh=fTPCCutChi2H->GetYaxis()->GetXmax();

    	int binPt=TMath::FloorNint((Pt-minPt)*fTPCCutChi2H->GetXaxis()->GetNbins()/(maxPt-minPt))+1;
    	int binTh=TMath::FloorNint((Theta-minTh)*fTPCCutChi2H->GetYaxis()->GetNbins()/(maxTh-minTh))+1;

    	if (binPt<1) binPt=1;
    	if (binPt>fTPCCutChi2H->GetXaxis()->GetNbins()) binPt=fTPCCutChi2H->GetXaxis()->GetNbins();
    	if (binTh<1) binTh=1;
    	if (binTh>fTPCCutChi2H->GetYaxis()->GetNbins()) binTh=fTPCCutChi2H->GetYaxis()->GetNbins();
    	return fTPCCutChi2H->GetBinContent(binPt,binTh);
    }
    else return 1;
}



void PndMvdTPCRiemannTrackFinderTask::FinishEvent()
{
	fTrackCandArray->Clear();
	fMVDTPCTrackCandArray->Clear();
	fRiemannTracks->Clear();
}

ClassImp(PndMvdTPCRiemannTrackFinderTask);

