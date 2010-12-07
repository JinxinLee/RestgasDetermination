#include "PndTpcClustPlusRTFTask.h"

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
#include "PndSdsHit.h"
#include "PndTrackCand.h"
#include "GFTrackCand.h"
#include "PndRiemannTrack.h"
#include "PndRiemannHit.h"

#include "PndTpcCluster.h"
#include "PndDetectorList.h"


PndTpcClustPlusRTFTask::PndTpcClustPlusRTFTask() : FairTask("TPC Clustering))) + Riemann Track Finder")
{
	fMaxDist=2;

	fHitBranch = "MVDHitsPixel";
	fHitBranch2 = "MVDHitsStrip";

	fHitBranchTPC = "PndTpcCluster";
	fMVDTrackBranch="MVDRiemannTrackCand";
	fEventNr = 0;


}

PndTpcClustPlusRTFTask::~PndTpcClustPlusRTFTask()
{
}

bool Condition(std::pair<int,double> hit1, std::pair<int,double> hit2)
{
	return (hit1.second<hit2.second);
}

void PndTpcClustPlusRTFTask::SetParContainers()
{
}

InitStatus PndTpcClustPlusRTFTask::ReInit()
{

  InitStatus stat=kERROR;
  return stat;

}

// -----   Public method Init   --------------------------------------------
InitStatus PndTpcClustPlusRTFTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndTpcClustPlusRTFTask::Init: "
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


   fHitArrayTPC = (TClonesArray*) ioman->GetObject(fHitBranchTPC);
    if ( !fHitArrayTPC){
      std::cout << "-W- PndTpcClustPlusRTFTask::Init: " << "No hitArrayTPC!" << std::endl;
      return kERROR;
    }

    fMVDTrackCandArray = (TClonesArray*) ioman->GetObject(fMVDTrackBranch);
    if ( !fMVDTrackCandArray){
      std::cout << "-W- PndTpcClustPlusRTFTask::Init: " << "No MVDTrackCandArray!" << std::endl;
      return kERROR;
    }

    fTPCTrackCandArray = new TClonesArray("GFTrackCand");
    ioman->Register("TPCclustRTF", "MVD", fTPCTrackCandArray, kTRUE);

    fFinalTrackCand = new TClonesArray("GFTrackCand");
    ioman->Register("MvdTpcTrackCand", "MVD", fFinalTrackCand, kTRUE);



  std::cout << "-I- PndTpcClustPlusRTFTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndTpcClustPlusRTFTask::Exec(Option_t* opt)
{
	fTPCTrackCandArray->Delete();
	fFinalTrackCand->Delete();

	std::vector< std::pair< std::vector< std::pair<int,double> > , std::pair<double,double>  > > Blocks;
	TVector3 pos;
	for(int i=0;i<fHitArrayTPC->GetEntriesFast();i++){
		PndTpcCluster* cluster=(PndTpcCluster*)fHitArrayTPC->At(i);
		pos=cluster->pos();
		AddHit(i,pos,Blocks);
	}
	  for(unsigned int i=0;i<Blocks.size();i++){
		  GFTrackCand Cand;
		  for(unsigned int j=0;j<Blocks[i].first.size();j++){
			  Cand.addHit(kTpcCluster,Blocks[i].first[j].first,0);
		  }
		  int size=fTPCTrackCandArray->GetEntriesFast();
		  new ((*fTPCTrackCandArray)[size])GFTrackCand(Cand);
	  }

	std::cout<<"NumOfTpcBlocks= "<<Blocks.size()<<std::endl;
	std::cout<<"NumOfMvdCands= "<<fMVDTrackCandArray->GetEntriesFast() <<std::endl;

	MergeMVDandTPC();

  std::cout <<"----------------Event : "<<fEventNr++<<std::endl;
}

void PndTpcClustPlusRTFTask::AddHit(int index,TVector3 pos,std::vector< std::pair< std::vector< std::pair<int,double> > , std::pair<double,double>  > >& Blocks )
{
	std::vector<int> TooCloseBlocks;
	TVector3 testPos;
	int count=0;
	for(unsigned int block=0;block<Blocks.size();block++){
		int begin=0,end=Blocks[block].first.size()-1;
		int hit=begin;
		if ( end>0 && !CheckAngle(pos.XYvector().Phi(), Blocks[block].second )) continue;
//		std::cout<<pos.XYvector().Phi()<<"   "<<Blocks[block].second.first<<"    "<<Blocks[block].second.second<<std::endl;
		if (((PndTpcCluster*)fHitArrayTPC->At(Blocks[block].first[hit].first))->pos().Z()*pos.Z()<0) continue;
		bool flag=true;
		while (begin<=end){
			if (flag){
				hit=begin;
				begin++;
				flag=!flag;
			}
			else{
				hit=end;
				end--;
				flag=!flag;
			}
			testPos=((PndTpcCluster*)fHitArrayTPC->At(Blocks[block].first[hit].first))->pos();
			if (dist(pos,testPos)<fMaxDist){
				TooCloseBlocks.push_back(block);
				count++;
				break;
			}
		}
		if (count==2) break;
	}

	if (TooCloseBlocks.size()>0){
		int KeptBlock=TooCloseBlocks[0];
		for(int i=TooCloseBlocks.size()-1;i>0;i--){
			Blocks[KeptBlock].first.insert(Blocks[KeptBlock].first.end(),Blocks[TooCloseBlocks[i]].first.begin(),Blocks[TooCloseBlocks[i]].first.end());
			Blocks.erase(Blocks.begin()+TooCloseBlocks[i]);
		}
		std::pair<int,double> New(index,pos.Z());
		Blocks[KeptBlock].first.insert(Blocks[KeptBlock].first.end(),New);
		sort(Blocks[KeptBlock].first.begin(),Blocks[KeptBlock].first.end(),Condition);
		int begin=0,end=Blocks[KeptBlock].first.size()-1;
		Blocks[KeptBlock].second=CutAngles(((PndTpcCluster*)fHitArrayTPC->At(Blocks[KeptBlock].first[begin].first))->pos().XYvector(),
				((PndTpcCluster*)fHitArrayTPC->At(Blocks[KeptBlock].first[end].first))->pos().XYvector());

	}
	else{
		std::vector< std::pair<int,double> > NewOne;
		std::pair<int,double> New(index,pos.Z());
		NewOne.push_back(New);
		std::pair<double,double> cutAng(0,0);
		std::pair< std::vector< std::pair<int,double> > , std::pair<double,double>  > NewBlock(NewOne, cutAng);
		Blocks.push_back(NewBlock);
	}

}


double PndTpcClustPlusRTFTask::dist(TVector3 pos1, TVector3 pos2)
{
	return (pos1-pos2).Mag();
}

std::pair<double,double>  PndTpcClustPlusRTFTask::CutAngles(TVector2 begin, TVector2 end)
{
	double PhiBegin=begin.Phi();
	double PhiEnd=end.Phi();
	TVector2 First,Second;
	double dEB=PhiEnd-PhiBegin;
		if (dEB>TMath::Pi()){
			First=end;
			Second=begin;
		}
		else if(dEB>0){
			First=begin;
			Second=end;
		}
		else if (dEB>-TMath::Pi()){
			First=end;
			Second=begin;
		}
		else{
			First=begin;
			Second=end;
		}
		First=First.Rotate(-fMaxDist/First.Mod());
		Second=Second.Rotate(fMaxDist/Second.Mod());

		std::pair<double,double> result(First.Phi(),Second.Phi());
//		std::cout<<"-------"<<std::endl;
//		std::cout<<fMaxDist/First.Mod()<<std::endl;
//		std::cout<<begin.Phi()<<"     "<<end.Phi()<<std::endl;
//		std::cout<<First.Phi()<<"     "<<Second.Phi()<<std::endl;
//		std::cout<<"-------"<<std::endl;

	return result;
}

bool PndTpcClustPlusRTFTask::CheckAngle(double testAngle,std::pair<double,double> cutAngles)
{
	if (cutAngles.first<cutAngles.second){
		if (testAngle>cutAngles.first && testAngle<cutAngles.second)
			return true;
		else
			return false;
	}
	else{
		if (testAngle>cutAngles.first or testAngle<cutAngles.second)
			return true;
		else
			return false;
	}
}

void PndTpcClustPlusRTFTask::MergeMVDandTPC()
{
	std::vector<PndRiemannTrack> MVD;
	std::vector<PndRiemannTrack> TPC;

	std::vector<int> SkipMVDCands; /// skip after merging
	std::vector<int> SkipTPCCands; /// skip after merging

//////////////////////////////// refit of Riemann Mvd and Tpc track cands////////////////////////////////////
	for(int i=0;i<fMVDTrackCandArray->GetEntriesFast();i++){
		PndTrackCand* cand = (PndTrackCand*)fMVDTrackCandArray->At(i);
		PndRiemannTrack track;
		int detId,hitId;
		for(unsigned int j=0;j<cand->GetNHits();j++){
			detId=cand->GetSortedHit(j).GetDetId();
			hitId=cand->GetSortedHit(j).GetHitId();
			PndSdsHit *point;
			if (detId == kMVDHitsPixel)
				 point = (PndSdsHit*)fHitArray->At(hitId);
			else if (detId == kMVDHitsStrip)
				 point = (PndSdsHit*)fHitArray2->At(hitId);
			else point = 0;
			if (point!=0){
				PndRiemannHit hit(point->GetX(),point->GetY(),point->GetZ(),point->GetDx(),point->GetDy(),point->GetDz());
				track.addHit(hit);
			}
		}
		track.refit();
		track.szFit();
		MVD.push_back(track);
	}

	for(int i=0;i<fTPCTrackCandArray->GetEntriesFast();i++){
		GFTrackCand* cand = (GFTrackCand*)fTPCTrackCandArray->At(i);
		PndRiemannTrack track;
		unsigned int detId,hitId;
		for(unsigned int j=0;j<cand->getNHits();j++){
			cand->getHit(j,detId,hitId);
			PndTpcCluster* cluster;
			if (detId == kTpcCluster)
				cluster=(PndTpcCluster*)fHitArrayTPC->At(hitId);
			else cluster = 0;
			if (cluster!=0){
				TVector3 pos=cluster->pos();
				TVector3 err=cluster->sig();
				PndRiemannHit hit(pos.X(),pos.Y(),pos.Z(),err.X(),err.Y(),err.Z());
				track.addHit(hit);
			}
		}
		track.refit();
		track.szFit();
//		std::cout<<track.szChi2()<<std::endl;
		TPC.push_back(track);
	}
/////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////<Merge MVD cands and TPC cands//////////////////////////////////////
	for(unsigned int i=0;i<MVD.size();i++){
		double mvdRadius=MVD[i].r();
		TVector2 mvdOrig(MVD[i].orig()[0],MVD[i].orig()[1]);
		double mvdDip=MVD[i].dip();
		bool sign;
		if(MVD[i].getLastHit()->z()>0)
			sign=true;
		else sign=false;
		double cutR=GetMaxRadius(mvdRadius, mvdDip,sign);
		double cutD=GetMaxDip(mvdRadius, mvdDip,sign);
		double cutO=GetMaxOrigin(mvdRadius, mvdDip,sign);
//		std::cout<<"r="<<mvdRadius<<"   "<<"dip="<<mvdDip<<std::endl;
		std::vector<int> tpcAdd;
		for(unsigned int j=0;j<TPC.size();j++){
			double tpcRadius=TPC[j].r();
			TVector2 tpcOrig(TPC[j].orig()[0],TPC[j].orig()[1]);
			double tpcDip=TPC[j].dip();

//			std::cout<<"r="<<mvdRadius<<"   "<<"dip="<<mvdDip<<std::endl;
//			std::cout<<"r="<<tpcRadius<<"   "<<"dip="<<tpcDip<<std::endl;
//			if ((TPC[j].szChi2()<5) &&  (fabs(2*(mvdRadius-tpcRadius)/(mvdRadius+tpcRadius))<cutR)  && (fabs(2*(mvdDip-tpcDip)/(mvdDip+tpcDip))<cutD) && (fabs(2*(mvdOrig-tpcOrig).Mod()/(mvdOrig+tpcOrig).Mod())<cutO)){
			if((TPC[j].szChi2()<10) &&  (fabs((mvdRadius-tpcRadius)/mvdRadius)<cutR)  && (fabs((mvdDip-tpcDip)/mvdDip)<cutD) && (fabs((mvdOrig-tpcOrig).Mod()/mvdOrig.Mod())<cutO)  ){
				std::cout<<"checking ok:"<<i<<"  "<<j<<std::endl;
//				std::cout<<"r="<<mvdRadius<<"   "<<"dip="<<mvdDip<<std::endl;
//				std::cout<<"r="<<tpcRadius<<"   "<<"dip="<<tpcDip<<std::endl;
//				std::cout<<"/////////////////////"<<std::endl;
				tpcAdd.push_back(j);
			}
//			std::cout<<"/////////////////////"<<std::endl;
		}
		if (tpcAdd.size()>0){
			CreateTrackCand(i,tpcAdd);
			SkipMVDCands.push_back(i);
			for(unsigned int k=0;k<tpcAdd.size();k++){
				SkipTPCCands.push_back(tpcAdd[k]);
			}
		}
//		CreateTrackCand(i,tpcAdd);
	}
////////////////////////////////////////////////////////////////////////////////

	std::vector<int> restTPCcands;
	std::vector<std::pair<unsigned int,unsigned int> > restMVDhits;

	///////////////////////////////< filling  restTPCcands
	for(unsigned int i=0;i<TPC.size();i++){
		bool flag=true;
		for(unsigned int j=0;j<SkipTPCCands.size();j++){
			if((int)i==SkipTPCCands[j]){
				flag=false;
				break;
			}
		}
		if (flag) restTPCcands.push_back(i);
	}
////////////////////////////////< filling restMVDhits
	for(int i=0;i<fHitArray->GetEntriesFast();i++){
		std::pair<unsigned int,unsigned int> hit(kMVDHitsPixel,i);
		restMVDhits.push_back(hit);
	}
	for(int i=0;i<fHitArray2->GetEntriesFast();i++){
		std::pair<unsigned int,unsigned int> hit(kMVDHitsStrip,i);
		restMVDhits.push_back(hit);
	}
	for(unsigned int i=0;i<SkipMVDCands.size();i++){
		PndTrackCand* cand = (PndTrackCand*)fMVDTrackCandArray->At(SkipMVDCands[i]);
		unsigned int detId,hitId;
		for(unsigned int j=0;j<cand->GetNHits();j++){
			detId=cand->GetSortedHit(j).GetDetId();
			hitId=cand->GetSortedHit(j).GetHitId();
			for(unsigned int k=0;k<restMVDhits.size();k++){
				if ( detId==restMVDhits[k].first &&  hitId==restMVDhits[k].second ){
					restMVDhits.erase(restMVDhits.begin()+k);
					break;
				}

			}
		}
	}

	std::vector<PndRiemannHit> MvdHits;
	for(unsigned int j=0;j<restMVDhits.size();j++){
		PndSdsHit* point;
		if (restMVDhits[j].first == kMVDHitsPixel)
			 point = (PndSdsHit*)fHitArray->At(restMVDhits[j].second);
		else if ( restMVDhits[j].first == kMVDHitsStrip)
			 point = (PndSdsHit*)fHitArray2->At(restMVDhits[j].second);
		else point = 0;
		if (point!=0){
			PndRiemannHit hit(point->GetX(),point->GetY(),point->GetZ(),point->GetDx(),point->GetDy(),point->GetDz());
			MvdHits.push_back(hit);
		}
		else std::cout<<"ERROR in input hitArray"<<std::endl;
	}

//////////////////////////////////////////////

///////////////////////////////////////////// check restMvdHits using restTPCcands as seed

	for(unsigned int i=0;i<restTPCcands.size();i++){
		std::vector<int> FoundMvdHits;
		if (TPC[restTPCcands[i]].szChi2()<5){
			double radius=TPC[restTPCcands[i]].r();
			double dip=TPC[restTPCcands[i]].dip();
			bool sign;
			if (TPC[restTPCcands[i]].getLastHit()->z()>0)
				sign=true;
			else sign=false;
			for(unsigned int j=0;j<restMVDhits.size();j++){
				if(TPC[restTPCcands[i]].calcSZChi2(&MvdHits[j])<GetMaxPlaneDist(radius,dip,sign) && fabs(TPC[restTPCcands[i]].dist(&MvdHits[j]))<GetMaxSZChi2(radius,dip,sign)){
					std::cout<<"YES!!!!!!!!!!!!!!!!"<<std::endl;
					FoundMvdHits.push_back(j);
				}
			}
		}
		if(FoundMvdHits.size()>0){
			GFTrackCand cand=*(GFTrackCand*)fTPCTrackCandArray->At(restTPCcands[i]);
			for(unsigned int k=0;k<FoundMvdHits.size();k++){
				cand.addHit(restMVDhits[FoundMvdHits[k]].first,restMVDhits[FoundMvdHits[k]].second,0);
			}
			int size=fFinalTrackCand->GetEntriesFast();
			new ((*fFinalTrackCand)[size])GFTrackCand(cand);
		}
	}



}

void PndTpcClustPlusRTFTask::CreateTrackCand(int mvdSeed , std::vector<int> tpcAdd)
{
	GFTrackCand FinalCand;
	PndTrackCand* mvd=(PndTrackCand*)fMVDTrackCandArray->At(mvdSeed);
	for(unsigned int i=0;i<mvd->GetNHits();i++){
		unsigned int detId=mvd->GetSortedHit(i).GetDetId();
		unsigned int hitId=mvd->GetSortedHit(i).GetHitId();
		FinalCand.addHit(detId,hitId,0);
	}

	for(unsigned int i=0;i<tpcAdd.size();i++){
		FinalCand.append(*(GFTrackCand*)fTPCTrackCandArray->At(tpcAdd[i]));
	}
	int size=fFinalTrackCand->GetEntriesFast();
	new ((*fFinalTrackCand)[size])GFTrackCand(FinalCand);
}


double PndTpcClustPlusRTFTask::GetMaxRadius(double radius, double dip, bool sign)
{
	double Pt=((radius/100)*2*3*1E8)/1E9;
	double Theta;
	  if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	  else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
    if(fCutRadiusH!=NULL){
    	double minPt=fCutRadiusH->GetXaxis()->GetXmin();
    	double maxPt=fCutRadiusH->GetXaxis()->GetXmax();
    	double minTh=fCutRadiusH->GetYaxis()->GetXmin();
    	double maxTh=fCutRadiusH->GetYaxis()->GetXmax();

    	int binPt=TMath::FloorNint((Pt-minPt)*fCutRadiusH->GetXaxis()->GetNbins()/(maxPt-minPt))+1;
    	int binTh=TMath::FloorNint((Theta-minTh)*fCutRadiusH->GetYaxis()->GetNbins()/(maxTh-minTh))+1;

    	if (binPt<1) binPt=1;
    	if (binPt>fCutRadiusH->GetXaxis()->GetNbins()) binPt=fCutRadiusH->GetXaxis()->GetNbins();
    	if (binTh<1) binTh=1;
    	if (binTh>fCutRadiusH->GetYaxis()->GetNbins()) binTh=fCutRadiusH->GetYaxis()->GetNbins();
    	return fCutRadiusH->GetBinContent(binPt,binTh);

    }
    else return 1;
}


double PndTpcClustPlusRTFTask::GetMaxDip(double radius, double dip, bool sign)
{
	double Pt=((radius/100)*2*3*1E8)/1E9;
	double Theta;
	  if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	  else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
    if(fCutDipH!=NULL){
    	double minPt=fCutDipH->GetXaxis()->GetXmin();
    	double maxPt=fCutDipH->GetXaxis()->GetXmax();
    	double minTh=fCutDipH->GetYaxis()->GetXmin();
    	double maxTh=fCutDipH->GetYaxis()->GetXmax();

    	int binPt=TMath::FloorNint((Pt-minPt)*fCutDipH->GetXaxis()->GetNbins()/(maxPt-minPt))+1;
    	int binTh=TMath::FloorNint((Theta-minTh)*fCutDipH->GetYaxis()->GetNbins()/(maxTh-minTh))+1;

    	if (binPt<1) binPt=1;
    	if (binPt>fCutDipH->GetXaxis()->GetNbins()) binPt=fCutDipH->GetXaxis()->GetNbins();
    	if (binTh<1) binTh=1;
    	if (binTh>fCutDipH->GetYaxis()->GetNbins()) binTh=fCutDipH->GetYaxis()->GetNbins();
    	return fCutDipH->GetBinContent(binPt,binTh);
    }
    else return 1;
}



double PndTpcClustPlusRTFTask::GetMaxOrigin(double radius, double dip, bool sign)
{
	double Pt=((radius/100)*2*3*1E8)/1E9;
	double Theta;
	  if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	  else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
    if(fCutOriginH!=NULL){
    	double minPt=fCutOriginH->GetXaxis()->GetXmin();
    	double maxPt=fCutOriginH->GetXaxis()->GetXmax();
    	double minTh=fCutOriginH->GetYaxis()->GetXmin();
    	double maxTh=fCutOriginH->GetYaxis()->GetXmax();

    	int binPt=TMath::FloorNint((Pt-minPt)*fCutOriginH->GetXaxis()->GetNbins()/(maxPt-minPt))+1;
    	int binTh=TMath::FloorNint((Theta-minTh)*fCutOriginH->GetYaxis()->GetNbins()/(maxTh-minTh))+1;

    	if (binPt<1) binPt=1;
    	if (binPt>fCutOriginH->GetXaxis()->GetNbins()) binPt=fCutOriginH->GetXaxis()->GetNbins();
    	if (binTh<1) binTh=1;
    	if (binTh>fCutOriginH->GetYaxis()->GetNbins()) binTh=fCutOriginH->GetYaxis()->GetNbins();
    	return fCutOriginH->GetBinContent(binPt,binTh);
    }
    else return 1;
}


double PndTpcClustPlusRTFTask::GetMaxPlaneDist(double radius, double dip, bool sign)
{
	double Pt=((radius/100)*2*3*1E8)/1E9;
	double Theta;
	  if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	  else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
    if(fCutDistH!=NULL){
    	double minPt=fCutDistH->GetXaxis()->GetXmin();
    	double maxPt=fCutDistH->GetXaxis()->GetXmax();
    	double minTh=fCutDistH->GetYaxis()->GetXmin();
    	double maxTh=fCutDistH->GetYaxis()->GetXmax();

    	int binPt=TMath::FloorNint((Pt-minPt)*fCutDistH->GetXaxis()->GetNbins()/(maxPt-minPt))+1;
    	int binTh=TMath::FloorNint((Theta-minTh)*fCutDistH->GetYaxis()->GetNbins()/(maxTh-minTh))+1;

    	if (binPt<1) binPt=1;
    	if (binPt>fCutDistH->GetXaxis()->GetNbins()) binPt=fCutDistH->GetXaxis()->GetNbins();
    	if (binTh<1) binTh=1;
    	if (binTh>fCutDistH->GetYaxis()->GetNbins()) binTh=fCutDistH->GetYaxis()->GetNbins();
    	return fCutDistH->GetBinContent(binPt,binTh);

    }
    else return 1;
}
double PndTpcClustPlusRTFTask::GetMaxSZChi2(double radius, double dip, bool sign)
{
	double Pt=((radius/100)*2*3*1E8)/1E9;
	double Theta;
	  if (sign) Theta=(TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
	  else Theta=(TMath::Pi()-TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)),-1)))*180/TMath::Pi();
    if(fCutChi2H!=NULL){
    	double minPt=fCutChi2H->GetXaxis()->GetXmin();
    	double maxPt=fCutChi2H->GetXaxis()->GetXmax();
    	double minTh=fCutChi2H->GetYaxis()->GetXmin();
    	double maxTh=fCutChi2H->GetYaxis()->GetXmax();

    	int binPt=TMath::FloorNint((Pt-minPt)*fCutChi2H->GetXaxis()->GetNbins()/(maxPt-minPt))+1;
    	int binTh=TMath::FloorNint((Theta-minTh)*fCutChi2H->GetYaxis()->GetNbins()/(maxTh-minTh))+1;

    	if (binPt<1) binPt=1;
    	if (binPt>fCutChi2H->GetXaxis()->GetNbins()) binPt=fCutChi2H->GetXaxis()->GetNbins();
    	if (binTh<1) binTh=1;
    	if (binTh>fCutChi2H->GetYaxis()->GetNbins()) binTh=fCutChi2H->GetYaxis()->GetNbins();
    	return fCutChi2H->GetBinContent(binPt,binTh);
    }
    else return 1;
}



void PndTpcClustPlusRTFTask::FinishEvent()
{
	fTPCTrackCandArray->Delete();
	fFinalTrackCand->Delete();
}

ClassImp(PndTpcClustPlusRTFTask);

