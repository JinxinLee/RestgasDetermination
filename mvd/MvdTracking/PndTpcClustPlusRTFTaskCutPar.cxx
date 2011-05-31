#include "PndTpcClustPlusRTFTaskCutPar.h"

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
#include "GFTrackCand.h"
#include "PndSdsHit.h"
//#include "PndSdsMCPoint.h"
//#include "PndSdsCluster.h"
//#include "PndSdsDigi.h"
//#include "PndMvdRiemannTrackFinder.h"
#include "PndRiemannTrack.h"
#include "PndRiemannHit.h"

#include "PndTpcCluster.h"
#include "PndDetectorList.h"


PndTpcClustPlusRTFTaskCutPar::PndTpcClustPlusRTFTaskCutPar() : FairTask("MVD-TPC Riemann Track Finder")
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
	frangeR=1;
	frangeD=1;
	frangeO=1;
	frangeDist=3;
	frangeChi2=3;
	fPtS=0.1;
	fPtF=1.0;
	fThetaS=15;
	fThetaF=150;

    for(int i=0;i<fNPt;i++){
    	for(int j=0;j<fNTh;j++){
    		TH1F *Hist1 = new TH1F("histR","R",fNbin,0,frangeR);
    		fhistsR[i][j]=Hist1;
    		TH1F *Hist2 = new TH1F("histD","D",fNbin,0,frangeD);
    		fhistsD[i][j]=Hist2;
    		TH1F *Hist3 = new TH1F("histO","O",fNbin,0,frangeO);
    		fhistsO[i][j]=Hist3;
    		TH1F *Hist4 = new TH1F("histDist","O",fNbin,0,frangeDist);
    		fhistsDist[i][j]=Hist4;
    		TH1F *Hist5 = new TH1F("histChi2","O",fNbin,0,frangeChi2);
    		fhistsChi2[i][j]=Hist5;
    	}
    }

    fCutRadiusH = new TH2F("CutR","CutR",fNPt,fPtS,fPtF,fNTh,fThetaS,fThetaF);
    fCutDipH = new TH2F("CutD","CutD",fNPt,fPtS,fPtF,fNTh,fThetaS,fThetaF);
    fCutOriginH = new TH2F("CutO","CutO",fNPt,fPtS,fPtF,fNTh,fThetaS,fThetaF);
    fCutDistH = new TH2F("CutDist","CutDist",fNPt,fPtS,fPtF,fNTh,fThetaS,fThetaF);
    fCutChi2H = new TH2F("CutChi2","CutChi2",fNPt,fPtS,fPtF,fNTh,fThetaS,fThetaF);

    fCutRadiusH->GetXaxis()->SetTitle("Pt, GeV/c");
    fCutRadiusH->GetYaxis()->SetTitle("Theta");
    fCutDipH->GetXaxis()->SetTitle("Pt, GeV/c");
    fCutDipH->GetYaxis()->SetTitle("Theta");
    fCutOriginH->GetXaxis()->SetTitle("Pt, GeV/c");
    fCutOriginH->GetYaxis()->SetTitle("Theta");

    fCutDistH->GetXaxis()->SetTitle("Pt, GeV/c");
    fCutDistH ->GetYaxis()->SetTitle("Theta");

    fCutChi2H->GetXaxis()->SetTitle("Pt, GeV/c");
    fCutChi2H->GetYaxis()->SetTitle("Theta");

}

PndTpcClustPlusRTFTaskCutPar::~PndTpcClustPlusRTFTaskCutPar()
{
}

void PndTpcClustPlusRTFTaskCutPar::SetParContainers()
{
}

InitStatus PndTpcClustPlusRTFTaskCutPar::ReInit()
{

  InitStatus stat=kERROR;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndTpcClustPlusRTFTaskCutPar::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndTpcClustPlusRTFTaskCutPar::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject(fHitBranch);
  if ( !fHitArray){
    std::cout << "-W- PndTpcClustPlusRTFTaskCutPar::Init: " << "No hitArray!" << std::endl;
    return kERROR;
  }

  fHitArray2 = (TClonesArray*) ioman->GetObject(fHitBranch2);
   if ( !fHitArray2){
     std::cout << "-W- PndTpcClustPlusRTFTaskCutPar::Init: " << "No hitArray2!" << std::endl;
     return kERROR;
   }

   fHitArrayTPC = (TClonesArray*) ioman->GetObject(fHitBranchTPC);
    if ( !fHitArrayTPC){
      std::cout << "-W- PndTpcClustPlusRTFTaskCutPar::Init: " << "No hitArrayTPC!" << std::endl;
      return kERROR;
    }

  fMVDRiemannTracks = new TClonesArray("PndRiemannTrack");
  fTPCRiemannTracks = new TClonesArray("PndRiemannTrack");

  fTrackCandArray = (TClonesArray*) ioman->GetObject(fTrackBranch);
  if ( !fTrackCandArray){
    std::cout << "-W- PndTpcClustPlusRTFTaskCutPar::Init: " << "No IdealTrackCands!" << std::endl;
    return kERROR;
  }

  fMCTrackArray = (TClonesArray*) ioman->GetObject(fMCTrackBranch);
  if ( !fMCTrackArray){
    std::cout << "-W- PndTpcClustPlusRTFTaskCutPar::Init: " << "No MCTrack!" << std::endl;
    return kERROR;
  }
  fIdealTPCTrackArray = (TClonesArray*) ioman->GetObject(fIdealTPCTrackBranch);
  if ( !fIdealTPCTrackArray){
    std::cout << "-W- PndTpcClustPlusRTFTaskCutPar::Init: " << "No IdealTPCTrack!" << std::endl;
    return kERROR;
  }

  std::cout << "-I- PndTpcClustPlusRTFTaskCutPar: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndTpcClustPlusRTFTaskCutPar::Exec(Option_t* opt)
{

  // Reset output array
  if ( ! fTrackCandArray )
    Fatal("Exec", "No trackCandArray");
  fMVDRiemannTracks->Delete();
  fTPCRiemannTracks->Delete();
  CalcPlanes();
  CalcParHists();
  std::cout << "Done event no. " << fEventNr++ << std::endl;
}

void PndTpcClustPlusRTFTaskCutPar::CalcPlanes()
{
	for(int j=0;j<fTrackCandArray->GetEntriesFast();j++){
		  PndRiemannHit hit[20];
		  PndRiemannTrack track;
		  PndRiemannTrack track0;
		  if (((PndTrackCand*)fTrackCandArray->At(j))->GetNHits()>fNCut){
			  new ((*fMVDRiemannTracks)[j])PndRiemannTrack(track0);
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
			  track.refit();
			  track.szFit();
			  new ((*fMVDRiemannTracks)[j])PndRiemannTrack(track);
		  }
		  else {
			  new ((*fMVDRiemannTracks)[j])PndRiemannTrack(track0);
		  }
	}

	for(int i=0;i<fIdealTPCTrackArray->GetEntriesFast();i++){
		GFTrackCand Cand=((GFTrack*)fIdealTPCTrackArray->At(i))->getCand();

		  PndRiemannTrack track;
		  PndRiemannTrack track0;
		  if (Cand.getNHits()<3){
			  new ((*fTPCRiemannTracks)[i])PndRiemannTrack(track0);
			  continue;
		  }

		unsigned int detId,hitId;
		TVector3 pos,err;
		  for(unsigned int j=0;j<Cand.getNHits();j++){
			  Cand.getHit(j,detId,hitId);
			  if (detId==kTpcCluster){
				pos=((PndTpcCluster*)fHitArrayTPC->At(hitId))->pos();
				err=((PndTpcCluster*)fHitArrayTPC->At(hitId))->sig();
				PndRiemannHit hit(pos.X(),pos.Y(),pos.Z(),err.X(),err.Y(),err.Z());
				track.addHit(hit);
			  }
		  }
		  track.refit();
		  track.szFit();
		  if (track.szChi2()<10){
		  new ((*fTPCRiemannTracks)[i])PndRiemannTrack(track);
		  }
		  else new ((*fTPCRiemannTracks)[i])PndRiemannTrack(track0);
	}




}
void  PndTpcClustPlusRTFTaskCutPar::CalcParHists()
{
	for(int i=0;i<fIdealTPCTrackArray->GetEntriesFast();i++){
			  GFTrackCand Cand=((GFTrack*)fIdealTPCTrackArray->At(i))->getCand();
			  int NumOfPlane=-1;
			  for(int k=0;k<fTrackCandArray->GetEntriesFast();k++){
				  if (Cand.getMcTrackId()==((PndTrackCand*)fTrackCandArray->At(k))->getMcTrackId()){
					  NumOfPlane=k;
					  break;
				  }
			  }
			  if(NumOfPlane>-1){
					if(((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->r()>0 && (((PndTrackCand*)fTrackCandArray->At(NumOfPlane))->GetNHits()<=fNCut) &&  ((PndRiemannTrack*)fTPCRiemannTracks->At(i))->r()>0 ){
					  PndMCTrack* myTrack= (PndMCTrack*)fMCTrackArray->At(Cand.getMcTrackId());
					  double Pt=myTrack->GetPt();
					  double Theta=(myTrack->GetMomentum().Theta())*180/TMath::Pi();
					  if ((Pt>=fPtS) && (Pt<=fPtF) &&(Theta>=fThetaS) && (Theta<=fThetaF)){
						  double dR=((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->r()-((PndRiemannTrack*)fTPCRiemannTracks->At(i))->r();
						  double dDip=((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->dip()-((PndRiemannTrack*)fTPCRiemannTracks->At(i))->dip();
						  TVector2 O1(((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->orig()[0],((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->orig()[1]);
						  TVector2 O2(((PndRiemannTrack*)fTPCRiemannTracks->At(i))->orig()[0],((PndRiemannTrack*)fTPCRiemannTracks->At(i))->orig()[1]);
						  double dOrig=(O1-O2).Mod();
							fhistsR[TMath::FloorNint(((Pt-fPtS)*fNPt/(fPtF-fPtS)))][TMath::FloorNint(((Theta-fThetaS)*fNTh/(fThetaF-fThetaS)))]->Fill(fabs(dR/((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->r()));
							fhistsD[TMath::FloorNint(((Pt-fPtS)*fNPt/(fPtF-fPtS)))][TMath::FloorNint(((Theta-fThetaS)*fNTh/(fThetaF-fThetaS)))]->Fill(fabs(dDip/((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->dip()));
							fhistsO[TMath::FloorNint(((Pt-fPtS)*fNPt/(fPtF-fPtS)))][TMath::FloorNint(((Theta-fThetaS)*fNTh/(fThetaF-fThetaS)))]->Fill(fabs(dOrig/O1.Mod()));

						for(unsigned int mvdHit=0;mvdHit<((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->getNumHits();mvdHit++){
							fhistsDist[TMath::FloorNint(((Pt-fPtS)*fNPt/(fPtF-fPtS)))][TMath::FloorNint(((Theta-fThetaS)*fNTh/(fThetaF-fThetaS)))]->Fill(fabs(((PndRiemannTrack*)fTPCRiemannTracks->At(i))->dist(((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->getHit(mvdHit))));
							fhistsChi2[TMath::FloorNint(((Pt-fPtS)*fNPt/(fPtF-fPtS)))][TMath::FloorNint(((Theta-fThetaS)*fNTh/(fThetaF-fThetaS)))]->Fill(((PndRiemannTrack*)fTPCRiemannTracks->At(i))->calcSZChi2(((PndRiemannTrack*)fMVDRiemannTracks->At(NumOfPlane))->getHit(mvdHit)));
						}


					  }
					}
			  }
	}
}
bool PndTpcClustPlusRTFTaskCutPar::CheckTooCloseHits(PndRiemannHit hit1,PndRiemannHit hit2)
{
  //int count=0;
  TVector3 delta;
  delta=hit1.x()-hit2.x();
	if (delta.Mag()<fMinPointDist){
		return false;
	}
	else return true;

}

void PndTpcClustPlusRTFTaskCutPar::CalcCuts()
{
	 for(int i=0;i<fNPt;i++){
	 	for(int j=0;j<fNTh;j++){
	 	     double Th=0.90;
	 	     double CutR=0;
	 	     double CutD=0;
	 	     double CutO=0;
	 	     double CutDist=0;
	 	     double CutChi2=0;

	 	     double S0=fhistsR[i][j]->Integral();
			     double S=0;
			     if(S0>0){
					 for(int k=0;k<fNbin;k++){
						 S+=fhistsR[i][j]->GetBinContent(k);
						 if ((S/S0)>=Th){
							 CutR=((double)k*frangeR)/((double)fNbin);
							 break;
						 }
					 }
			     }
			     S0=fhistsD[i][j]->Integral();
				 S=0;
				 if(S0>0){
					 for(int k=0;k<fNbin;k++){
						 S+=fhistsD[i][j]->GetBinContent(k);
						 if ((S/S0)>=Th){
							 CutD=((double)k*frangeD)/((double)fNbin);
							 break;
						 }
					 }
				 }


			     S0=fhistsO[i][j]->Integral();
				 S=0;
				 if(S0>0){
					 for(int k=0;k<fNbin;k++){
						 S+=fhistsO[i][j]->GetBinContent(k);
						 if ((S/S0)>=Th){
							 CutO=((double)k*frangeO)/((double)fNbin);
							 break;
						 }
					 }
				 }

			     S0=fhistsDist[i][j]->Integral();
				 S=0;
				 if(S0>0){
					 for(int k=0;k<fNbin;k++){
						 S+=fhistsDist[i][j]->GetBinContent(k);
						 if ((S/S0)>=Th){
							 CutDist=((double)k*frangeDist)/((double)fNbin);
							 break;
						 }
					 }
				 }

			     S0=fhistsChi2[i][j]->Integral();
				 S=0;
				 if(S0>0){
					 for(int k=0;k<fNbin;k++){
						 S+=fhistsChi2[i][j]->GetBinContent(k);
						 if ((S/S0)>=Th){
							 CutChi2=((double)k*frangeChi2)/((double)fNbin);
							 break;
						 }
					 }
				 }

			     fCutRadiusH->SetBinContent(i+1,j+1,CutR);
			     fCutDipH->SetBinContent(i+1,j+1,CutD);
			     fCutOriginH->SetBinContent(i+1,j+1,CutO);
			     fCutDistH->SetBinContent(i+1,j+1,CutDist);
			     fCutChi2H->SetBinContent(i+1,j+1,CutChi2);
	 	}
	 }

	 TFile fout("MergeCutHists.root","recreate");
	 fCutRadiusH ->Write();
	 fCutDipH->Write();
	 fCutOriginH->Write();
	 fout.Close();

	 TFile fout2("TPCtoMVDCutHists.root","recreate");
	 fCutDistH->Write();
	 fCutChi2H->Write();
	 fout2.Close();
}


void PndTpcClustPlusRTFTaskCutPar::FinishEvent()
{
	fMVDRiemannTracks->Delete();
	fTPCRiemannTracks->Delete();
}

ClassImp(PndTpcClustPlusRTFTaskCutPar);

