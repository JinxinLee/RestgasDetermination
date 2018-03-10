/*
 * PndIdealTrackFinders.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#include <PndIdealTrackFinder.h>
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "FairHit.h"

#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndMCTrack.h"
#include "PndGemMCPoint.h"
#include "PndGemHit.h"

#include "TRandom.h"
ClassImp(PndIdealTrackFinder);

PndIdealTrackFinder::PndIdealTrackFinder() :
  fOutBranchName("IdealTrack"), fTrackCand(0), fTrack(0), fMCTrack(0), fTrackSelector(0), fPdg(0), fHitCount(0), fMomSigma(0,0,0), fDPoP(0.), fRelative (kFALSE), fVtxSigma(0,0,0), fEfficiency(1.)
{
	fPointBranchMap["MVDHitsPixel"] = "MVDPoint";
	fPointBranchMap["MVDHitsStrip"] = "MVDPoint";
	fPointBranchMap["STTHit"] = "STTPoint";
	fPointBranchMap["GEMHit"] = "GEMPoint";
	fPointBranchMap["FTSHit"] = "FTSPoint";

	fPointBranchMap["SciTHit"] = "SciTPoint";
	fPointBranchMap["MdtHit"] = "MdtPoint";

	fPointBranchMap["DircHit"] = "";	 // no FairLinks Provided!
	fPointBranchMap["FTofHit"] = "";	 // no FairLinks Provided!
	fPointBranchMap["RichHit"] = "";	 // no FairLinks Provided!

	SetPersistency(kTRUE);
}

PndIdealTrackFinder::~PndIdealTrackFinder() {
	if (fTrackSelector != 0)
		delete fTrackSelector;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndIdealTrackFinder::Init()
{

	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		std::cout << "-E- PndMCTestHitCompare::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	if (fBranchNames.size() == 0){
	    // Use hits of all tracking subsystems if nothing is given
		AddBranchName("MVDHitsPixel");
		AddBranchName("MVDHitsStrip");
		AddBranchName("STTHit");
        AddBranchName("GEMHit");
        AddBranchName("FTSHit");
	}

	for (size_t i = 0; i < fBranchNames.size(); i++){
		if (ioman->GetObject(fBranchNames[i]) != 0){
			fBranchMap[fBranchNames[i]] = (TClonesArray*)ioman->GetObject(fBranchNames[i]);
		 	ioman->GetObject(fPointBranchMap[fBranchNames[i]]);  // initialise the used FairMcPoint Branches
		}
	}
  	fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");


	fTrackCand = new TClonesArray("PndTrackCand");
 	ioman->Register(fOutBranchName + "Cand", "MC", fTrackCand, GetPersistency());
	fTrack = new TClonesArray("PndTrack");
	ioman->Register(fOutBranchName, "MC", fTrack, GetPersistency());

	if (fTrackSelector == 0){
		std::cout << "-W- PndIdealTrackFinder::Init() no fTrackSelector set! All possible tracks will be taken!" << std::endl;
	}
 
	fPdg = new TDatabasePDG();

  	return kSUCCESS;
}

void PndIdealTrackFinder::Exec(Option_t*)
{
	fTrackCand->Delete();
	fTrack->Delete();
	fTrackCandMap.clear();

//	std::cout << "Event #" << FairRootManager::Instance()->GetEntryNr() << std::endl;
	CreateTrackCands();
	FilterTrackCands();

//	std::cout << "PndIdealTrackFinder:Found Tracks:" << std::endl;
	CreateTracks();
}

void PndIdealTrackFinder::CreateTrackCands()
{
	fHitCount = 0;
	for (std::map<TString, TClonesArray*>::iterator iter = fBranchMap.begin(); iter != fBranchMap.end(); iter++){
		//std::cout << "AddLinks from Branch: " << iter->first << std::endl;
		for (int i = 0; i < iter->second->GetEntriesFast(); i++){
			
			FairMultiLinkedData array;
			FairMultiLinkedData_Interface* links = (FairMultiLinkedData_Interface*)iter->second->At(i);
			TString hitBranch = iter->first;

			FairMCPoint *point = GetFairMCPoint(hitBranch, links, array);
			if (point == 0) {
				continue;
			}
			FairMCPoint firstpoint = *point;
			FairMCPoint lastpoint = *point;

			double tof = point->GetTime();
			delete(point);

			for (int ipnt = 1; ipnt < array.GetNLinks(); ipnt++){
			  point = (FairMCPoint *) FairRootManager::Instance()->GetCloneOfLinkData(array.GetLink(ipnt));
			  tof += point->GetTime();
			  //  std::cout << ipnt << " " << tof << std::endl;
			  if( point->GetTime() < firstpoint.GetTime()) firstpoint = *point;
			  if( point->GetTime() > lastpoint.GetTime()) lastpoint = *point;
			  delete(point);
			}
			tof /= array.GetNLinks();
			// std::cout << i << " " << tof << std::endl;
			// .............................................
			FairMultiLinkedData mctracks = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
			for (int trackIndex = 0; trackIndex < mctracks.GetNLinks(); trackIndex++){
				if (!fTrackCandMap.count(mctracks.GetLink(trackIndex))){
					fTrackCandMap[mctracks.GetLink(trackIndex)] = PndTrackCand();
					fTrackCandMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
					fFirstPointMap[mctracks.GetLink(trackIndex)] = firstpoint;
					// fFirstPointMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
					fLastPointMap[mctracks.GetLink(trackIndex)] = lastpoint;
					// fLastPointMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
				}
				else {
				  FairMCPoint tmpfirstpoint = fFirstPointMap[mctracks.GetLink(trackIndex)];
				  if(firstpoint.GetTime() < tmpfirstpoint.GetTime()) fFirstPointMap[mctracks.GetLink(trackIndex)] = firstpoint;
				  FairMCPoint tmplastpoint = fLastPointMap[mctracks.GetLink(trackIndex)];
				  if(lastpoint.GetTime() > tmplastpoint.GetTime()) fLastPointMap[mctracks.GetLink(trackIndex)] = lastpoint;
				}
				FairLink link(-1, FairRootManager::Instance()->GetEntryNr(),FairRootManager::Instance()->GetBranchId(iter->first),i);
				//std::cout << "CreateTrackCands " << mctracks.GetLink(trackIndex) << " : " << link << std::endl;
				fTrackCandMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
				// fTrackCandMap[mctracks.GetLink(trackIndex)].AddHit(link, fHitCount++);			//todo Rho is not properly calculated!
				fTrackCandMap[mctracks.GetLink(trackIndex)].AddHit(link, tof);
			}
		}
	}
}

void PndIdealTrackFinder::FilterTrackCands()
{
	if (fTrackSelector == 0){
		return;
	}
	for (std::map<FairLink, PndTrackCand>::iterator iter = fTrackCandMap.begin(); iter != fTrackCandMap.end();){
		if (!(*fTrackSelector)(iter->second.GetPointerToLinks(), true)){
			fTrackCandMap.erase(iter++);
		} else {
			++iter;
		}
	}
}

void PndIdealTrackFinder::CreateTracks()
{
	for (std::map<FairLink, PndTrackCand>::iterator iter = fTrackCandMap.begin(); iter != fTrackCandMap.end(); iter++){
		PndTrackCand* myTrackCand = new((*fTrackCand)[fTrackCand->GetEntriesFast()]) PndTrackCand(iter->second);
		myTrackCand->setMcTrackId(iter->first.GetIndex());
		myTrackCand->AddLink(iter->first);
		myTrackCand->SetTimeStamp(FairRootManager::Instance()->GetEventTime());
		//std::cout << myTrackCand->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")) << " : " << std::endl;
		//myTrackCand->Print();
		//std::cout << *myTrackCand << std::endl;

		// ....... track
		PndMCTrack *mc = (PndMCTrack *) FairRootManager::Instance()->GetCloneOfLinkData(iter->first);
		int charge = 0;
		if (mc->GetPdgCode()<100000000) charge = (Int_t)TMath::Sign(1.0, ((TParticlePDG*) fPdg->GetParticle(mc->GetPdgCode()))->Charge());
		else charge = 1;


		if(0 < fEfficiency && fEfficiency < 1){
		  if(gRandom->Rndm() > fEfficiency) continue;
		}

		// first
		FairMCPoint firstpoint = fFirstPointMap[iter->first];
		TVector3 firstpos(0, 0, 0), firstmom(0, 0, 0);
		if(myTrackCand->GetSortedHit(0).GetDetId() == FairRootManager::Instance()->GetBranchId("GEMHit")) {
		  TClonesArray *gemhitarray = fBranchMap["GEMHit"];
		  Int_t hitid = myTrackCand->GetSortedHit(0).GetHitId();
		  PndGemHit *gemhit = (PndGemHit*) gemhitarray->At(hitid);
		  FairMultiLinkedData gemhitlink = gemhit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("GEMPoint"));
		  PndGemMCPoint *gempoint = (PndGemMCPoint *) FairRootManager::Instance()->GetCloneOfLinkData(gemhitlink.GetLink(0));

		  TVector3 posin(0, 0, 0), posout(0, 0, 0);
		  gempoint->Position(posin);
		  gempoint->PositionOut(posout);
		  firstpos = 0.5 * (posin + posout);
		}
		else firstpoint.Position(firstpos);

		SmearVector(firstpos, fVtxSigma);
		firstpoint.Momentum(firstmom);

		if (fRelative) fMomSigma.SetXYZ(fDPoP*firstmom.Mag(),fDPoP*firstmom.Mag(),fDPoP*firstmom.Mag());
		SmearVector(firstmom, fMomSigma);
		FairTrackParP firstPar(firstpos, firstmom,
							  fVtxSigma, fMomSigma,
							  charge, firstpos,
							  TVector3(1.,0.,0.), TVector3(0.,1.,0.));
		// last
		FairMCPoint lastpoint = fLastPointMap[iter->first];
		TVector3 lastpos(0, 0, 0), lastmom(0, 0, 0);

		if(myTrackCand->GetSortedHit(myTrackCand->GetNHits() - 1).GetDetId() == FairRootManager::Instance()->GetBranchId("GEMHit")) {
		  TClonesArray *gemhitarray = fBranchMap["GEMHit"];
		  Int_t hitid = myTrackCand->GetSortedHit(myTrackCand->GetNHits() - 1).GetHitId();
		  PndGemHit *gemhit = (PndGemHit*) gemhitarray->At(hitid);
		  FairMultiLinkedData gemhitlink = gemhit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("GEMPoint"));
		  PndGemMCPoint *gempoint = (PndGemMCPoint *) FairRootManager::Instance()->GetCloneOfLinkData(gemhitlink.GetLink(0));

		  TVector3 posin(0, 0, 0), posout(0, 0, 0);
		  gempoint->Position(posin);
		  gempoint->PositionOut(posout);
		  lastpos = 0.5 * (posin + posout);
		}
		else lastpoint.Position(lastpos);

		SmearVector(lastpos, fVtxSigma);
		lastpoint.Momentum(lastmom);
		SmearVector(lastmom, fMomSigma);
		FairTrackParP lastPar(lastpos, lastmom,
							 fVtxSigma, fMomSigma,
							 charge, lastpos,
							 TVector3(1.,0.,0.), TVector3(0.,1.,0.));



		new((*fTrack)[fTrack->GetEntriesFast()]) PndTrack(firstPar, lastPar, *myTrackCand, 0,0,1,mc->GetPdgCode(), -1,-1);
	}
}

FairMCPoint* PndIdealTrackFinder::GetFairMCPoint(TString hitBranch, FairMultiLinkedData_Interface* links, FairMultiLinkedData& array)
{
	// get the mc point(s) from each reco hit ......
	FairMultiLinkedData mcpoints = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fPointBranchMap[hitBranch]));
	// std::cout << "hit " << i << " connected to points " << mvdpoints.GetNLinks() << " " << sttpoints.GetNLinks() << " " << gempoints.GetNLinks() << std::endl;

	// There seems to be a bug with ghost hits from the GEM stations. If more than one
	// MC point is associated to a hit, there is a good chance for false assignments
	// leading to wrong tracks. For the moment, skip hits with more than 1 GEM point.

	if (hitBranch == "GEMHit" && mcpoints.GetNLinks() > 1) return 0;
//	if ((*iter).first == "MVDHitsStrip" && mvdpoints.GetNLinks() > 1) return 0;

	array = mcpoints;


	if (array.GetNLinks() == 0){

		return 0;
	}
	return (FairMCPoint *) FairRootManager::Instance()->GetCloneOfLinkData(array.GetLink(0));
}

void PndIdealTrackFinder::SmearVector(TVector3 &vec, const TVector3 &sigma)
{
  // gaussian smearing
  Double_t rannn=0.;
  rannn = gRandom->Gaus(vec.X(),sigma.X());
  vec.SetX(rannn);
  
  rannn = gRandom->Gaus(vec.Y(),sigma.Y());
  vec.SetY(rannn);
  
  rannn = gRandom->Gaus(vec.Z(),sigma.Z());
  vec.SetZ(rannn);
  
  return;
}
