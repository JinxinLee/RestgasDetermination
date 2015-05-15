/*
 * PndMCIdealTrackFinderNewLinkss.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#include "PndMCIdealTrackFinderNewLinks.h"

#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "FairHit.h"

#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndMCTrack.h"

ClassImp(PndMCIdealTrackFinderNewLinks);

PndMCIdealTrackFinderNewLinks::PndMCIdealTrackFinderNewLinks() {
	// TODO Auto-generated constructor stub

}

PndMCIdealTrackFinderNewLinks::~PndMCIdealTrackFinderNewLinks() {
	// TODO Auto-generated destructor stub
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCIdealTrackFinderNewLinks::Init()
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

	for (int i = 0; i < fBranchNames.size(); i++){
		if (ioman->GetObject(fBranchNames[i]) != 0){
			fBranchMap[fBranchNames[i]] = (TClonesArray*)ioman->GetObject(fBranchNames[i]);
		}
	}

  	fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

  	ioman->GetObject("MVDPoint");
	ioman->GetObject("STTPoint");
    ioman->GetObject("GEMPoint");
    ioman->GetObject("FTSPoint");

	fTrackCand = new TClonesArray("PndTrackCand");
 	ioman->Register("IdealTrackCand", "MC", fTrackCand, kTRUE);
	fTrack = new TClonesArray("PndTrack");
	ioman->Register("IdealTrack", "MC", fTrack, kTRUE);
 
	fPdg = new TDatabasePDG();

  	return kSUCCESS;
}

void PndMCIdealTrackFinderNewLinks::Exec(Option_t* opt)
{
	fTrackCand->Delete();
	fTrack->Delete();
	fTrackCandMap.clear();

	std::cout << "Event #" << FairRootManager::Instance()->GetEntryNr() << std::endl;
	CreateTrackCands();

	std::cout << "PndMCIdealTrackFinderNewLinks:Found Tracks:" << std::endl;	
	int trackcounter = 0;
	for (std::map<FairLink, PndTrackCand>::iterator iter = fTrackCandMap.begin(); iter != fTrackCandMap.end(); iter++){
		PndTrackCand* myTrackCand = new((*fTrackCand)[fTrackCand->GetEntriesFast()]) PndTrackCand(iter->second);
		myTrackCand->setMcTrackId(iter->first.GetIndex());
		myTrackCand->AddLink(iter->first);
		myTrackCand->SetTimeStamp(FairRootManager::Instance()->GetEventTime());
		//std::cout << myTrackCand->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")) << " : " << std::endl;
		myTrackCand->Print();
		//std::cout << *myTrackCand << std::endl;

		// ....... track
		PndMCTrack *mc = (PndMCTrack *) FairRootManager::Instance()->GetCloneOfLinkData(iter->first);
		int charge = 0;
		if (mc->GetPdgCode()<100000000) charge = (Int_t)TMath::Sign(1.0, ((TParticlePDG*) fPdg->GetParticle(mc->GetPdgCode()))->Charge());
		else charge = 1;

		// first
		FairMCPoint firstpoint = fFirstPointMap[iter->first];
		TVector3 firstpos(0, 0, 0), firstmom(0, 0, 0);
		firstpoint.Position(firstpos);
		firstpoint.Momentum(firstmom);
		FairTrackParP* firstPar=new FairTrackParP(firstpos, firstmom,
							  TVector3(0., 0., 0.), TVector3(0., 0., 0.), 
							  charge, firstpos,
							  TVector3(1.,0.,0.), TVector3(0.,1.,0.));					 
		
		// last
		FairMCPoint lastpoint = fLastPointMap[iter->first];
		TVector3 lastpos(0, 0, 0), lastmom(0, 0, 0);
		lastpoint.Position(lastpos);
		lastpoint.Momentum(lastmom);
		FairTrackParP* lastPar=new FairTrackParP(lastpos, lastmom,
							  TVector3(0., 0., 0.), TVector3(0., 0., 0.), 
							  charge, lastpos,
							  TVector3(1.,0.,0.), TVector3(0.,1.,0.));					 

		PndTrack* myTrack = new((*fTrack)[fTrack->GetEntriesFast()]) PndTrack(*firstPar, *lastPar, *myTrackCand, 0,0,1,mc->GetPdgCode(), trackcounter,FairRootManager::Instance()->GetBranchId("MCTrack")); // CHECK trackcounter is correct??

		trackcounter++;
		// .............
	}
}

void PndMCIdealTrackFinderNewLinks::CreateTrackCands()
{
	fHitCount = 0;
	for (std::map<TString, TClonesArray*>::iterator iter = fBranchMap.begin(); iter != fBranchMap.end(); iter++){
		//std::cout << "AddLinks from Branch: " << iter->first << std::endl;
		for (int i = 0; i < iter->second->GetEntriesFast(); i++){
			FairMultiLinkedData_Interface* links = (FairMultiLinkedData_Interface*)iter->second->At(i);
			//std::cout << *links << std::endl;

			// get the mc point(s) from each reco hit ......
			FairMultiLinkedData mvdpoints = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDPoint"));
			FairMultiLinkedData sttpoints = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTPoint"));
            FairMultiLinkedData gempoints = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId("GEMPoint"));
            FairMultiLinkedData ftspoints = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId("FTSPoint"));
			// std::cout << "hit " << i << " connected to points " << mvdpoints.GetNLinks() << " " << sttpoints.GetNLinks() << " " << gempoints.GetNLinks() << std::endl;

            // There seems to be a bug with ghost hits from the GEM stations. If more than one
            // MC point is associated to a hit, there is a good chance for false assignments
            // leading to wrong tracks. For the moment, skip hits with more than 1 GEM point.
            if (gempoints.GetNLinks() > 1) continue;
            if ((*iter).first == "MVDHitsStrip" && mvdpoints.GetNLinks() > 1) continue;

            FairMultiLinkedData array;
			if(mvdpoints.GetNLinks() > 0) array = mvdpoints;
			else if(sttpoints.GetNLinks() > 0) array = sttpoints;
            else if(gempoints.GetNLinks() > 0) array = gempoints;
            else if(ftspoints.GetNLinks() > 0) array = ftspoints;
			
			double tof = 0;
			FairMCPoint *firstpoint = NULL, *lastpoint = NULL;
			for (int ipnt = 0; ipnt < array.GetNLinks(); ipnt++){
			  FairMCPoint *point = (FairMCPoint *) FairRootManager::Instance()->GetCloneOfLinkData(array.GetLink(ipnt));
			  tof += point->GetTime();
			  //  std::cout << ipnt << " " << tof << std::endl;
			  if(firstpoint == NULL || point->GetTime() < firstpoint->GetTime()) firstpoint = point;
			  if(lastpoint == NULL || point->GetTime() > lastpoint->GetTime()) lastpoint = point;
			}
			tof /= array.GetNLinks();
			// std::cout << i << " " << tof << std::endl;
			// .............................................
			FairMultiLinkedData mctracks = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
			for (int trackIndex = 0; trackIndex < mctracks.GetNLinks(); trackIndex++){
				if (!fTrackCandMap.count(mctracks.GetLink(trackIndex))){
					fTrackCandMap[mctracks.GetLink(trackIndex)] = PndTrackCand();
					fTrackCandMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
					fFirstPointMap[mctracks.GetLink(trackIndex)] = *firstpoint;
					// fFirstPointMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
					fLastPointMap[mctracks.GetLink(trackIndex)] = *lastpoint;
					// fLastPointMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
				}
				else {
				  FairMCPoint tmpfirstpoint = fFirstPointMap[mctracks.GetLink(trackIndex)];
				  if(firstpoint->GetTime() < tmpfirstpoint.GetTime()) fFirstPointMap[mctracks.GetLink(trackIndex)] = *firstpoint;
				  FairMCPoint tmplastpoint = fLastPointMap[mctracks.GetLink(trackIndex)];
				  if(lastpoint->GetTime() > tmplastpoint.GetTime()) fLastPointMap[mctracks.GetLink(trackIndex)] = *lastpoint;
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
