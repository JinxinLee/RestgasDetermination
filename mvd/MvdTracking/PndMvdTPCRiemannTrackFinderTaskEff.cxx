#include "PndMvdTPCRiemannTrackFinderTaskEff.h"

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TFile.h"
#include "TVector.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"


// PndMvd includes
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "PndTrackCand.h"
#include "PndMvdHit.h"
#include "PndMCTrack.h"
#include "PndRiemannTrack.h"
#include "PndDetectorList.h"
#include "PndTpcCluster.h"

#include "PndRiemannHit.h"
#include "PndRiemannTrack.h"

PndMvdTPCRiemannTrackFinderTaskEff::PndMvdTPCRiemannTrackFinderTaskEff() : FairTask("MVD Riemann Track Finder")
{
	fHitBranch = "MVDHitsPixel";
	fHitBranch2 = "MVDHitsStrip";
	fMCTrackBranch = "MCTrack";
	fIdealTrackBranch = "MVDIdealTrackCand";
//	fFTrackBranch = "MVDRiemannTrackCand";
	fFTrackBranch = "MvdTpcTrackCand";
	fHitBranchTPC = "PndTpcCluster";
	fIdealTPCTrackBranch = "TrackPreFit";

	fMaxSZChi2 = 1;
	fMaxSZDist = 10;
	fMinPointDist = 1;
	fMaxDist = 1;

	fEventNr = 0;
}

PndMvdTPCRiemannTrackFinderTaskEff::~PndMvdTPCRiemannTrackFinderTaskEff()
{
}

void PndMvdTPCRiemannTrackFinderTaskEff::SetParContainers()
{
}

InitStatus PndMvdTPCRiemannTrackFinderTaskEff::ReInit()
{

  InitStatus stat=kERROR;
  return stat;

}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdTPCRiemannTrackFinderTaskEff::Init()
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

   fMCTracksArray = (TClonesArray*) ioman->GetObject(fMCTrackBranch);
   if ( !fMCTracksArray){
 	std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: " << "No MCtracks!" << std::endl;
 	return kERROR;
   }

  fIdealTrackCandArray = (TClonesArray*) ioman->GetObject(fIdealTrackBranch);
  if ( !fIdealTrackCandArray){
     std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: " << "No IdealTrackCands!" << std::endl;
     return kERROR;
  }
  fTrackCandArray = (TClonesArray*) ioman->GetObject(fFTrackBranch);
  if ( !fTrackCandArray){
     std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: " << "No FTrackCands!" << std::endl;
     return kERROR;
  }

  fIdealTPCTrackArray = (TClonesArray*) ioman->GetObject(fIdealTPCTrackBranch);
  if ( !fIdealTPCTrackArray){
    std::cout << "-W- PndMvdTPCRiemannTrackFinderTaskCutPar::Init: " << "No IdealTPCTrack!" << std::endl;
    return kERROR;
  }
  fHitArrayTPC = (TClonesArray*) ioman->GetObject(fHitBranchTPC);
   if ( !fHitArrayTPC){
     std::cout << "-W- PndMvdTPCRiemannTrackFinderTaskCutPar::Init: " << "No hitArrayTPC!" << std::endl;
     return kERROR;
   }

    int Nbin=10;
    eff0H = new TH2F("eff0H","eff0H",Nbin,0.1,1,Nbin,15,150);
    effH = new TH2F("effH","effH",Nbin,0.1,1,Nbin,15,150);

    GhH = new TH2F("GhH","GhH",Nbin,0.1,1,Nbin,15,150);

    TPCinclTrue = new TH2F("TPCinclTrue","TPCinclTrue",Nbin,0.1,1,Nbin,15,150);
    TPCinclFalse = new TH2F("TPCinclFalse","TPCinclFalse",Nbin,0.1,1,Nbin,15,150);
    TPCtotal= new TH2F("TPCtotal","TPCtotal",Nbin,0.1,1,Nbin,15,150);

  std::cout << "-I- PndMvdRiemannTrackFinderTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdTPCRiemannTrackFinderTaskEff::Exec(Option_t* opt)
{

  std::vector<PndTrackCand*>  RecoT;

	  for(int i=0;i<fIdealTrackCandArray->GetEntriesFast();i++){
		 PndTrackCand *cand=(PndTrackCand*)fIdealTrackCandArray->At(i);
		 PndMCTrack* myTrack = (PndMCTrack*)fMCTracksArray->At(cand->getMcTrackId());
		 if (CheckRecoTrack(cand,myTrack)){
			if (myTrack->GetMotherID()==-1) cand->AddHit(0,0,0);
			int NumTpc=-1;
			 GFTrackCand CandTPC;
			 for(int k=0;k<fIdealTPCTrackArray->GetEntriesFast();k++){
				 CandTPC=((GFTrack*)fIdealTPCTrackArray->At(k))->getCand();
				 if (CandTPC.getMcTrackId()==cand->getMcTrackId()){
					 NumTpc=k;
					 break;
				 }
			 }

			 RecoT.push_back(cand);
			 TVector3 Pvec=myTrack->GetMomentum();
			 double Theta=180.0*Pvec.Theta()/TMath::Pi();
			 double P=Pvec.Mag();
			 eff0H->Fill(P,Theta);
			 if (NumTpc>-1) {
				 for(unsigned int j=0;j<((GFTrack*)fIdealTPCTrackArray->At(NumTpc))->getCand().getNHits();j++){
				 TPCtotal->Fill(P,Theta);
				 }
			 }
	     }
	  }
	  int count=0;
	  for(int i=0;i<fMCTracksArray->GetEntriesFast();i++){
		  PndMCTrack* myTrack = (PndMCTrack*)fMCTracksArray->At(i);
		  if ((myTrack->GetMotherID()==-1)){
			 TVector3 Pvec=myTrack->GetMomentum();
			 //double Theta=180.0*Pvec.Theta()/TMath::Pi();
			 //double P=Pvec.Mag();
//		  eff0H->Fill(P,Theta);
		  count++;
		  }
	  }
//	  std::cout<<count<<std::endl;


	  ComparingFandR(RecoT);

}

void PndMvdTPCRiemannTrackFinderTaskEff::FinishEvent()
{
}
bool PndMvdTPCRiemannTrackFinderTaskEff::CheckRecoTrack(PndTrackCand *cand,PndMCTrack* myTrack)
{
	if (/*myTrack->GetPdgCode()==211 &&*/ (cand->GetNHits()>2) && (myTrack->GetMotherID()==-1)){
		int count=0;
		unsigned int detIDi, hitIDi;
		unsigned int detIDj, hitIDj;
		for(unsigned int i=0;i<cand->GetNHits();i++){
			detIDi=cand->GetSortedHit(i).GetDetId();
			hitIDi=cand->GetSortedHit(i).GetHitId();
			PndMvdHit *pointI;
			if (detIDi ==kMVDHitsPixel )
				 pointI = (PndMvdHit*)fHitArray->At(hitIDi);
			else if (detIDi ==kMVDHitsStrip )
				 pointI = (PndMvdHit*)fHitArray2->At(hitIDi);
			else pointI = 0;

				for(unsigned int j=0;j<cand->GetNHits();j++){
					detIDj=cand->GetSortedHit(j).GetDetId();
					hitIDj=cand->GetSortedHit(j).GetHitId();
					PndMvdHit *pointJ;
					if (detIDj ==kMVDHitsPixel )
						 pointJ = (PndMvdHit*)fHitArray->At(hitIDj);
					else if (detIDj == kMVDHitsStrip)
						 pointJ = (PndMvdHit*)fHitArray2->At(hitIDj);
					else pointJ = 0;

					if ((pointI!=0) && (pointJ!=0) && (i!=j)){
						TVector3 a=pointI->GetPosition() - pointJ->GetPosition();
						if (a.Mag()<fMinPointDist){
							count++;
						}
					}
				}
		}
		if ((cand->GetNHits()-count/2)<3){
			std::cout <<"Less then 3 base points in RecoTrack"<<"   "<<cand->GetNHits()<<"   "<<count <<std::endl;
			return false;
		}
		else return true;
	}
	else
		return false;
}
void PndMvdTPCRiemannTrackFinderTaskEff::ComparingFandR(std::vector<PndTrackCand*>  RecoT)
{
	  unsigned int detidRC, hitidRC;
	  unsigned int detidF, hitidF;
		for(int trackF=0;trackF<fTrackCandArray->GetEntriesFast();trackF++){
			for(unsigned int trackRC=0;trackRC<RecoT.size();trackRC++){
				int cSame=0;
			  /*for(int iF=0;iF<((PndTrackCand*)fTrackCandArray->At(trackF))->GetNHits();iF++){
				  detidF=((PndTrackCand*)fTrackCandArray->At(trackF))->GetSortedHit(iF).GetDetId();
				  hitidF=((PndTrackCand*)fTrackCandArray->At(trackF))->GetSortedHit(iF).GetHitId();*/
				for(unsigned int iF=0;iF<((GFTrackCand*)fTrackCandArray->At(trackF))->getNHits();iF++){
					((GFTrackCand*)fTrackCandArray->At(trackF))->getHit(iF,detidF, hitidF);

					for(unsigned int iRC=0;iRC<RecoT[trackRC]->GetNHits();iRC++){
						detidRC=RecoT[trackRC]->GetSortedHit(iRC).GetDetId();
						hitidRC=RecoT[trackRC]->GetSortedHit(iRC).GetHitId();
						if (detidRC==detidF && hitidRC==hitidF){
							cSame++;
						}
					}
				}

			      PndMCTrack* myTrack = (PndMCTrack*)fMCTracksArray->At(RecoT[trackRC]->getMcTrackId());

//				  if (  (cSame>=4) && ((int)(((PndTrackCand*)fTrackCandArray->At(trackF))->GetNHits())==cSame+NumOfTpcHits(trackF)) ){
					  if (  (cSame>=4) && ((int)(((GFTrackCand*)fTrackCandArray->At(trackF))->getNHits())==cSame+NumOfTpcHits(trackF)) ){
					  std::cout<<"TRUE FOUND"<<std::endl;
					     TVector3 Pvec=myTrack->GetMomentum();
						 double Theta=180.0*Pvec.Theta()/TMath::Pi();
						 double P=Pvec.Mag();
						 effH->Fill(P,Theta);
						 FillTPCHits(trackF,trackRC,RecoT);
						 RecoT.erase(RecoT.begin()+trackRC);
					  break;
				  }
				  else{
					  if ((trackRC)==RecoT.size()-1){
							std::cout<<"        FALSE FOUND"<<std::endl;
							AddGhostTrack(trackF);
					  }
				  }

			 }
		}
}


int PndMvdTPCRiemannTrackFinderTaskEff::NumOfTpcHits(int NumOfTrack)
{
	int count=0;
	unsigned int detid, hitid;
	/*for(int i=0;i<((PndTrackCand*)fTrackCandArray->At(NumOfTrack))->GetNHits();i++){
		detid=((PndTrackCand*)fTrackCandArray->At(NumOfTrack))->GetSortedHit(i).GetDetId();
		hitid=((PndTrackCand*)fTrackCandArray->At(NumOfTrack))->GetSortedHit(i).GetHitId();
		if (detid==kTpcCluster) count++;
	}*/
	for(unsigned int i=0;i<((GFTrackCand*)fTrackCandArray->At(NumOfTrack))->getNHits();i++){
		((GFTrackCand*)fTrackCandArray->At(NumOfTrack))->getHit(i,detid, hitid);
			if (detid==kTpcCluster) count++;
		}
	return count;
}
void  PndMvdTPCRiemannTrackFinderTaskEff::FillTPCHits(int NF, int NR,std::vector<PndTrackCand*>  RecoT)
{
	int NumTpc=-1;
	 for(int k=0;k<fIdealTPCTrackArray->GetEntriesFast();k++){
		 GFTrackCand CandTPC=((GFTrack*)fIdealTPCTrackArray->At(k))->getCand();
		 if (CandTPC.getMcTrackId()==RecoT[NR]->getMcTrackId()){
			 NumTpc=k;
			 break;
		 }
	 }
	 if (NumTpc>-1){
		 GFTrackCand CandTPC=((GFTrack*)fIdealTPCTrackArray->At(NumTpc))->getCand();

		  unsigned int detidRC, hitidRC;
		  unsigned int detidF, hitidF;

		  int cSame=0;
		  for(unsigned int iF=0;iF<CandTPC.getNHits();iF++){
			  CandTPC.getHit(iF,detidF,hitidF);
/*				for(int iRC=0;iRC<((PndTrackCand*)fTrackCandArray->At(NF))->GetNHits();iRC++){
					detidRC=((PndTrackCand*)fTrackCandArray->At(NF))->GetSortedHit(iRC).GetDetId();
					hitidRC=((PndTrackCand*)fTrackCandArray->At(NF))->GetSortedHit(iRC).GetHitId();
					if (detidRC==detidF && hitidRC==hitidF){
						cSame++;
					}
				}*/
				for(unsigned int iRC=0;iRC<((GFTrackCand*)fTrackCandArray->At(NF))->getNHits();iRC++){
					((GFTrackCand*)fTrackCandArray->At(NF))->getHit(iRC,detidRC, hitidRC);
					if (detidRC==detidF && hitidRC==hitidF){
						cSame++;
					}
				}
			}
		  PndMCTrack* myTrack = (PndMCTrack*)fMCTracksArray->At(CandTPC.getMcTrackId());
			 TVector3 Pvec=myTrack->GetMomentum();
			 double Theta=180.0*Pvec.Theta()/TMath::Pi();
			 double P=Pvec.Mag();
			 for(int i =0;i<cSame;i++){
				 TPCinclTrue->Fill(P,Theta);
			 }
			 for(int i=0;i<NumOfTpcHits(NF)-cSame;i++){
				 TPCinclFalse->Fill(P,Theta);
			 }
	 }


}

void PndMvdTPCRiemannTrackFinderTaskEff::AddGhostTrack(int trackF)
{
	PndRiemannTrack track;
	unsigned int detID, hitID;
	bool sign=true;
	for(UInt_t i=0;i<((GFTrackCand*)fTrackCandArray->At(trackF))->getNHits();i++){
//			detID=((PndTrackCand*)fTrackCandArray->At(trackF))->GetSortedHit(i).GetDetId();
//			hitID=((PndTrackCand*)fTrackCandArray->At(trackF))->GetSortedHit(i).GetHitId();
			((GFTrackCand*)fTrackCandArray->At(trackF))->getHit(i,detID, hitID);
			PndMvdHit *point;
			if (detID == kMVDHitsPixel)
				 point = (PndMvdHit*)fHitArray->At(hitID);
			else if (detID == kMVDHitsStrip)
				 point = (PndMvdHit*)fHitArray2->At(hitID);
			else point = 0;
			if (point!=0){
				PndRiemannHit hit;
				hit.setXYZ(point->GetX(),point->GetY(),point->GetZ());
				hit.setDXYZ(point->GetDx(),point->GetDy(),point->GetDz());
				track.addHit(hit);
			}
			if ((i==((GFTrackCand*)fTrackCandArray->At(trackF))->getNHits()-1) && (point!=0)){
				if (point->GetZ()>0){
					sign=true;
				}
				else sign=false;
			}
	}
	track.refit();
	track.szFit();
	Double_t R=track.r();
	Double_t Pt=R*(2*3*1E8)/(1E9*100);
	Double_t dip=track.dip();
	Double_t Theta;
	 if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
     else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	Double_t P=Pt/TMath::Cos(Theta);
	GhH->Fill(P,Theta);

}



ClassImp(PndMvdTPCRiemannTrackFinderTaskEff);

