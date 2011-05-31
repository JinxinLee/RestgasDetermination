#include "PndMvdTPCRiemannTrackFinderTaskCutPar.h"

#include <iostream>
// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TVector.h"
#include "TH2F.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"


// PndMvd includes
#include "PndSdsRecoHit.h"
 #include "PndTrackCand.h"
#include "GFTrack.h"
#include "PndSdsHit.h"
//#include "PndSdsMCPoint.h"
//#include "PndSdsCluster.h"
//#include "PndSdsDigi.h"
//#include "PndMvdRiemannTrackFinder.h"
#include "PndRiemannTrack.h"
#include "PndRiemannHit.h"

#include "PndTpcCluster.h"
#include "PndDetectorList.h"


PndMvdTPCRiemannTrackFinderTaskCutPar::PndMvdTPCRiemannTrackFinderTaskCutPar() : FairTask("MVD-TPC Riemann Track Finder")
{
	fHitBranch = "MVDHitsPixel";
	fHitBranch2 = "MVDHitsStrip";
	fHitBranchTPC = "PndTpcCluster";
	fMCTrackBranch = "MCTrack";
	fTrackBranch = "MVDIdealTrackCand";
	fIdealTPCTrackBranch = "TrackPreFit";
	fMaxSZChi2 = 1;
	fMaxSZDist = 10;
	fMinPointDist = 1;
	fMaxDist = 1;

	fEventNr = 0;

	fNCut=10;

	//fTrackBranch = "MCTrack";
	fNbin=100000;
	frangeDist=2;
	frangeChi2=1;
	fPtS=0.1;
	fPtF=1.0;
	fThetaS=15;
	fThetaF=150;

    for(int i=0;i<fNPt;i++){
    	for(int j=0;j<fNTh;j++){
    		TH1F *Hist1 = new TH1F("histDist","h",fNbin,-frangeDist,frangeDist);
    		fhistsDist[i][j]=Hist1;
    		TH1F *Hist2 = new TH1F("histChi2","h",fNbin,0,frangeChi2);
    		fhistsChi2[i][j]=Hist2;
    	}
    }

    fCutDistH = new TH2F("CutDist","CutDist",fNPt,fPtS,fPtF,fNTh,fThetaS,fThetaF);
    fCutChi2H = new TH2F("CutChi2","CutChi2",fNPt,fPtS,fPtF,fNTh,fThetaS,fThetaF);

    fCutDistH->GetXaxis()->SetTitle("Pt, GeV/c");
    fCutDistH->GetYaxis()->SetTitle("Theta");
    fCutChi2H->GetXaxis()->SetTitle("Pt, GeV/c");
	fCutChi2H->GetYaxis()->SetTitle("Theta");

	fTEMP = new TH1F("","",10000,0,1000);

}

PndMvdTPCRiemannTrackFinderTaskCutPar::~PndMvdTPCRiemannTrackFinderTaskCutPar()
{
}

void PndMvdTPCRiemannTrackFinderTaskCutPar::SetParContainers()
{
}

InitStatus PndMvdTPCRiemannTrackFinderTaskCutPar::ReInit()
{

  InitStatus stat=kERROR;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdTPCRiemannTrackFinderTaskCutPar::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdTPCRiemannTrackFinderTaskCutPar::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject(fHitBranch);
  if ( !fHitArray){
    std::cout << "-W- PndMvdTPCRiemannTrackFinderTaskCutPar::Init: " << "No hitArray!" << std::endl;
    return kERROR;
  }

  fHitArray2 = (TClonesArray*) ioman->GetObject(fHitBranch2);
   if ( !fHitArray2){
     std::cout << "-W- PndMvdTPCRiemannTrackFinderTaskCutPar::Init: " << "No hitArray2!" << std::endl;
     return kERROR;
   }

   fHitArrayTPC = (TClonesArray*) ioman->GetObject(fHitBranchTPC);
    if ( !fHitArrayTPC){
      std::cout << "-W- PndMvdTPCRiemannTrackFinderTaskCutPar::Init: " << "No hitArrayTPC!" << std::endl;
      return kERROR;
    }

  fRiemannTracks = new TClonesArray("PndRiemannTrack");

  fTrackCandArray = (TClonesArray*) ioman->GetObject(fTrackBranch);
  if ( !fTrackCandArray){
    std::cout << "-W- PndMvdTPCRiemannTrackFinderTaskCutPar::Init: " << "No IdealTrackCands!" << std::endl;
    return kERROR;
  }

  fMCTrackArray = (TClonesArray*) ioman->GetObject(fMCTrackBranch);
  if ( !fMCTrackArray){
    std::cout << "-W- PndMvdTPCRiemannTrackFinderTaskCutPar::Init: " << "No MCTrack!" << std::endl;
    return kERROR;
  }
  fIdealTPCTrackArray = (TClonesArray*) ioman->GetObject(fIdealTPCTrackBranch);
  if ( !fIdealTPCTrackArray){
    std::cout << "-W- PndMvdTPCRiemannTrackFinderTaskCutPar::Init: " << "No IdealTPCTrack!" << std::endl;
    return kERROR;
  }

  std::cout << "-I- PndMvdTPCRiemannTrackFinderTaskCutPar: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdTPCRiemannTrackFinderTaskCutPar::Exec(Option_t* opt)
{

  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");
  fRiemannTracks->Clear();
  CalcPlanes();
  CalcParHists();
  std::cout << "Done event no. " << fEventNr++ << std::endl;
}

void PndMvdTPCRiemannTrackFinderTaskCutPar::CalcPlanes()
{
	for(int j=0;j<fTrackCandArray->GetEntriesFast();j++){
		  PndRiemannHit hit[20];
		  PndRiemannTrack track;
		  PndRiemannTrack track0;
		  if (((PndTrackCand*)fTrackCandArray->At(j))->GetNHits()>fNCut){
			  new ((*fRiemannTracks)[j])PndRiemannTrack(track0);
			  continue;
		  }
		  PndMCTrack *myTrack = (PndMCTrack*)fMCTrackArray->At(((PndTrackCand*)fTrackCandArray->At(j))->getMcTrackId());
		  unsigned int shift=0;
		  if (myTrack->GetMotherID()==-1){
			  hit[0].setXYZ(0,0,0);
			  hit[0].setDXYZ(0.1,0.1,0.1);
			  shift=1;
		  }
		  int count=0;
		  unsigned int i=0;
		  while ((i<((PndTrackCand*)fTrackCandArray->At(j))->GetNHits())){
			  unsigned int detId,hitId;
			  detId=((PndTrackCand*)fTrackCandArray->At(j))->GetSortedHit(i).GetDetId();
			  hitId=((PndTrackCand*)fTrackCandArray->At(j))->GetSortedHit(i).GetHitId();
				  if (detId==FairRootManager::Instance()->GetBranchId(fHitBranch)){
					  hit[i+shift].setXYZ(((PndSdsHit*)fHitArray->At(hitId))->GetPosition().X(),((PndSdsHit*)fHitArray->At(hitId))->GetPosition().Y(),((PndSdsHit*)fHitArray->At(hitId))->GetPosition().Z());
					  hit[i+shift].setDXYZ(((PndSdsHit*)fHitArray->At(hitId))->GetDx(),((PndSdsHit*)fHitArray->At(hitId))->GetDy(),((PndSdsHit*)fHitArray->At(hitId))->GetDz());
					  i++;
				  }
				  else if (detId==FairRootManager::Instance()->GetBranchId(fHitBranch2)){
					  hit[i+shift].setXYZ(((PndSdsHit*)fHitArray2->At(hitId))->GetPosition().X(),((PndSdsHit*)fHitArray2->At(hitId))->GetPosition().Y(),((PndSdsHit*)fHitArray2->At(hitId))->GetPosition().Z());
					  hit[i+shift].setDXYZ(((PndSdsHit*)fHitArray2->At(hitId))->GetDx(),((PndSdsHit*)fHitArray2->At(hitId))->GetDy(),((PndSdsHit*)fHitArray2->At(hitId))->GetDz());
					  i++;
				  }
				  else {i++;std::cout<<"ERROR"<<std::endl; continue;};

				  if (i>1){
					  if (!CheckTooCloseHits(hit[i-2+shift],hit[i-1+shift]))
						count++;
				  }
		  }
		  if (i>1){
		  if (!CheckTooCloseHits(hit[((PndTrackCand*)fTrackCandArray->At(j))->GetNHits()-2+shift],hit[((PndTrackCand*)fTrackCandArray->At(j))->GetNHits()-1+shift]))
		 	count++;
		  }
		  if ((((PndTrackCand*)fTrackCandArray->At(j))->GetNHits()-count)>3-shift){
			  for(unsigned int k=0;k<((PndTrackCand*)fTrackCandArray->At(j))->GetNHits();k++){
				  track.addHit(hit[k+shift]);
			  }
			  track.refit(true);
			  track.szFit(true);
			  new ((*fRiemannTracks)[j])PndRiemannTrack(track);
		  }
		  else {
			  new ((*fRiemannTracks)[j])PndRiemannTrack(track0);
		  }
	}
}
void  PndMvdTPCRiemannTrackFinderTaskCutPar::CalcParHists()
{
	for(int i=0;i<fIdealTPCTrackArray->GetEntriesFast();i++){
			  GFTrackCand Cand=((GFTrack*)fIdealTPCTrackArray->At(i))->getCand();
			  fTEMP->Fill(Cand.getNHits());
			  int NumOfPlane=-1;
			  for(int k=0;k<fTrackCandArray->GetEntriesFast();k++){
				  if (Cand.getMcTrackId()==((PndTrackCand*)fTrackCandArray->At(k))->getMcTrackId()){
					  NumOfPlane=k;
					  break;
				  }
			  }
			  if(NumOfPlane>-1){
					if(((PndRiemannTrack*)fRiemannTracks->At(NumOfPlane))->r()>0 && (((PndTrackCand*)fTrackCandArray->At(NumOfPlane))->GetNHits()<=fNCut)){
					  //unsigned int detId,hitId;
					  PndMCTrack* myTrack= (PndMCTrack*)fMCTrackArray->At(Cand.getMcTrackId());
					  double Pt=myTrack->GetPt();
					  double Theta=(myTrack->GetMomentum().Theta())*180/TMath::Pi();
					  if ((Pt>=fPtS) && (Pt<=fPtF) &&(Theta>=fThetaS) && (Theta<=fThetaF)){
					  std::vector<TVector3> TPCHits=GetNearestTPCHits(Cand);
					  PndRiemannHit hit0;
						  for(unsigned int j=0;j<TPCHits.size()/* && j<20*/;j++){
							hit0.setXYZ(TPCHits[j].X(),TPCHits[j].Y(),TPCHits[j].Z());
							fhistsDist[TMath::FloorNint(((Pt-fPtS)*fNPt/(fPtF-fPtS)))][TMath::FloorNint(((Theta-fThetaS)*fNTh/(fThetaF-fThetaS)))]->Fill(((PndRiemannTrack*)fRiemannTracks->At(NumOfPlane))->dist(&hit0));
							fhistsChi2[TMath::FloorNint(((Pt-fPtS)*fNPt/(fPtF-fPtS)))][TMath::FloorNint(((Theta-fThetaS)*fNTh/(fThetaF-fThetaS)))]->Fill(((PndRiemannTrack*)fRiemannTracks->At(NumOfPlane))->calcSZChi2(&hit0));
						  }
					  }
					}
			  }
	}
}
bool PndMvdTPCRiemannTrackFinderTaskCutPar::CheckTooCloseHits(PndRiemannHit hit1,PndRiemannHit hit2)
{
  //int count=0;
  TVector3 delta;
  delta=hit1.x()-hit2.x();
	if (delta.Mag()<fMinPointDist){
		return false;
	}
	else return true;

}

void PndMvdTPCRiemannTrackFinderTaskCutPar::CalcCuts()
{
	 for(int i=0;i<fNPt;i++){
	 	for(int j=0;j<fNTh;j++){
	 	     double Th1=0.90;
	 	     double CutDist=0;
	 	     double CutChi2=0;
	 	     double S01=fhistsDist[i][j]->Integral();
				 double S1=0;
				 if(S01>0){
					 S1+=fhistsDist[i][j]->GetBinContent(fNbin/2);
					 for(int k=1;k<(fNbin/2);k++){
						 S1+=fhistsDist[i][j]->GetBinContent(fNbin/2-k)+fhistsDist[i][j]->GetBinContent(fNbin/2+k);
						 if ((S1/S01)>=Th1){
							 CutDist=((double)k*frangeDist)/((double)fNbin/2.0);
							 break;
						 }
					 }
				 }

				 double Th2=Th1;
			  double S02=fhistsChi2[i][j]->Integral();
			     double S2=0;
			     if(S02>0){
					 for(int k=0;k<fNbin;k++){
						 S2+=fhistsChi2[i][j]->GetBinContent(k);
						 if ((S2/S02)>=Th2){
							 CutChi2=((double)k*frangeChi2)/((double)fNbin);
							 break;
						 }
					 }
			     }

			     fCutDistH->SetBinContent(i+1,j+1,CutDist);
			     fCutChi2H->SetBinContent(i+1,j+1,CutChi2);

	 	}
	 }

	 TFile fout("TPCCutHists.root","recreate");
	 fCutDistH->Write();
	 fCutChi2H->Write();
	 fout.Close();

}

std::vector<TVector3> PndMvdTPCRiemannTrackFinderTaskCutPar::GetNearestTPCHits(GFTrackCand Cand)
{
	std::vector<TVector3> result;
	  unsigned int detId,hitId;
	  TVector3 Origin=Cand.getPosSeed();
	  TVector3 pos;
	  std::vector<TVector3> Sort;
	  for(unsigned int j=0;j<Cand.getNHits();j++){
		  Cand.getHit(j,detId,hitId);
		  if (detId==kTpcCluster){
			pos=((PndTpcCluster*)fHitArrayTPC->At(hitId))->pos();
				bool flag=true;
				for(unsigned int i=0;i<Sort.size();i++){
					if (fabs(pos.Z()-Origin.Z())<fabs(Sort[i].Z()-Origin.Z())){
						Sort.insert(Sort.begin()+i,pos);
						flag=false;
						break;
					}
				}
				if (flag) {
					Sort.insert(Sort.end(),pos);
				}
		  }
	  }

	  for(unsigned int i=1;i<Sort.size();i++){
		  if (fabs(fabs(Sort[i-1].Z())-fabs(Sort[i].Z()))<1){
			  result.push_back(Sort[i-1]);
//			  std::cout<<Sort[i-1].Z()<<std::endl;
		  }
		  else break;
	  }
	  return result;
}


void PndMvdTPCRiemannTrackFinderTaskCutPar::FinishEvent()
{
	fRiemannTracks->Clear();
}

ClassImp(PndMvdTPCRiemannTrackFinderTaskCutPar);

