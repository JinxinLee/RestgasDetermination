/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                PndTimeStructureAnaTaskT source file             -----
// -------------------------------------------------------------------------

#include "PndTimeStructureAnaTask.h"

#include "FairMultiLinkedData.h"                   // for FairLink
#include "FairRootManager.h"            // for FairRootManager
#include "FairTimeStamp.h"              // for FairTimeStamp
#include "FairRunAna.h"

#include "PndSdsHit.h"

#include "Riosfwd.h"                    // for ostream
#include "TClass.h"                     // for TClass
#include "TClonesArray.h"               // for TClonesArray
#include "PndMCTrack.h"

#include <iostream>                     // for operator<<, cout, ostream, etc
#include <iomanip>
#include <vector>                       // for vector

InitStatus PndTimeStructureAnaTask::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndTimeStructureAnaTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- PndTimeStructureAnaTaskT::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  if(fData.size() == 0){

  	fData.push_back(DataObject("MVDSortedPixelDigis"));
  	fData.push_back(DataObject("MVDSortedStripDigis"));
  	fData.push_back(DataObject("STTSortedHits"));
  	fData.push_back(DataObject("GEMDigiSorted"));
  	fData.push_back(DataObject("SciTSortedHit"));
  	fDataPrim.push_back(DataObject("MVDSortedPixelDigisPrim"));
  	fDataPrim.push_back(DataObject("MVDSortedStripDigisPrim"));
  	fDataPrim.push_back(DataObject("STTSortedHitsPrim"));
  	fDataPrim.push_back(DataObject("GEMDigiSortedPrim"));
  	fDataPrim.push_back(DataObject("SciTSortedHitPrim"));
  }

  for (int i = 0; i < fData.size(); i++){
	  fData[i].fBranch = (TClonesArray*)ioman->GetObject(fData[i].fBranchName);
  }

  fHistoMixedEvents = new TH1D("hMixedEvents","hMixedEvents", fData.size(), 0, fData.size());
  fHistoMixedEventsPrim = new TH1D("hMixedEventsPrim","hMixedEventsPrim", fDataPrim.size(), 0, fDataPrim.size());

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndTimeStructureAnaTask::Exec(Option_t* opt)
{
	std::cout << " ------------- Event " << FairRootManager::Instance()->GetEntryNr() << " ----------------" << std::endl;
	bool primaryParticle = false;
	PndMCTrack* mcTrack = 0;
	for (int i = 0; i < fData.size(); i++){
		for (int j = 0; j < fData[i].fBranch->GetEntries(); j++){
			FairTimeStamp* data = (FairTimeStamp*)fData[i].fBranch->At(j);
			FairMultiLinkedData links = data->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
//			std::cout << fData[i].fBranchName.Data() << " " << j <<  " : " << *data << std::endl;
			Int_t currentEventNr = -1;
			if (links.GetNLinks() > 0){
				currentEventNr = links.GetLink(0).GetEntry();
				mcTrack = (PndMCTrack*)FairRootManager::Instance()->GetCloneOfLinkData(links.GetLink(0));
				if (mcTrack != 0)
					if (mcTrack->GetMotherID() < 0){
						primaryParticle = true;
//						std::cout << "Primary Particle found!" << std::endl;
					}
			}
			if (currentEventNr < 0) continue;
//			if(links.GetNLinks() > 1)
//				std::cout << fData[i].fBranchName.Data() << " : " << links << std::endl;
//			std::cout << "CurrentEventNr: " << currentEventNr << " : OldEventNr " << fData[i].fOldEventNr << std::endl;
			if (fData[i].fEventStartStopMap.count(currentEventNr) == 0){
				fData[i].fEventStartStopMap[currentEventNr] = std::pair<Double_t, Double_t>(data->GetTimeStamp(), data->GetTimeStamp());
			}
			else{
				fData[i].fEventStartStopMap[currentEventNr].second = data->GetTimeStamp();
			}
			if (currentEventNr < fData[i].fOldEventNr){
				if (fData[i].fEventMixture == kFALSE){
					fHistoMixedEvents->Fill(i);
					fData[i].fEventMixture = kTRUE;
					std::cout << "Event Mixture!" << std::endl;
				}
			}
			else if (currentEventNr == fData[i].fOldEventNr){
				fData[i].fEventHisto->Fill(data->GetTimeStamp() - fData[i].fOldTimeStamp);
				fData[i].fOldTimeStamp = data->GetTimeStamp();

			} else {
//				std::cout << "HistoFilled: " << data->GetTimeStamp()  << " - " << fData[i].fOldTimeStamp << data->GetTimeStamp() - fData[i].fOldTimeStamp << std::endl;
				fData[i].fTimeHisto->Fill(data->GetTimeStamp() - fData[i].fOldTimeStamp);
				fData[i].fOldTimeStamp = data->GetTimeStamp();
				fData[i].fOldEventNr = currentEventNr;
				fData[i].fEventMixture = kFALSE;

			}

			if (primaryParticle){
				if (fDataPrim[i].fEventStartStopMap.count(currentEventNr) == 0){
					fDataPrim[i].fEventStartStopMap[currentEventNr] = std::pair<Double_t, Double_t>(data->GetTimeStamp(), data->GetTimeStamp());
				}
				else{
					fDataPrim[i].fEventStartStopMap[currentEventNr].second = data->GetTimeStamp();
				}
				if (currentEventNr < fDataPrim[i].fOldEventNr){
					if (fDataPrim[i].fEventMixture == kFALSE){
						fHistoMixedEventsPrim->Fill(i);
						fDataPrim[i].fEventMixture = kTRUE;
						std::cout << "Event Mixture!" << std::endl;
					}
				}
				else if (currentEventNr == fDataPrim[i].fOldEventNr){
					fDataPrim[i].fEventHisto->Fill(data->GetTimeStamp() - fDataPrim[i].fOldTimeStamp);
					fDataPrim[i].fOldTimeStamp = data->GetTimeStamp();

				} else {
//					std::cout << "PrimHistoFilled: " << data->GetTimeStamp()  << " - " << fDataPrim[i].fOldTimeStamp << data->GetTimeStamp() - fDataPrim[i].fOldTimeStamp << std::endl;
					fDataPrim[i].fTimeHisto->Fill(data->GetTimeStamp() - fDataPrim[i].fOldTimeStamp);
					fDataPrim[i].fOldTimeStamp = data->GetTimeStamp();
					fDataPrim[i].fOldEventNr = currentEventNr;
					fDataPrim[i].fEventMixture = kFALSE;

				}
			}
		}
	}
}

// -------------------------------------------------------------------------

void PndTimeStructureAnaTask::FinishEvent()
{
 // fOutputArray->Delete();
}

void PndTimeStructureAnaTask::FinishTask()
{
	for (int i = 0; i < fData.size(); i++){
		fData[i].fTimeHisto->Write();
		fData[i].fEventHisto->Write();
		for (std::map<Int_t, std::pair<Double_t, Double_t> >::iterator iter = fData[i].fEventStartStopMap.begin(); iter != fData[i].fEventStartStopMap.end(); iter++) {
			fData[i].fEventDiffHisto->Fill(iter->second.second - iter->second.first);
		}
		fData[i].fEventDiffHisto->Write();
		fData[i].FillGraphs();
		fData[i].fEventGap->Write();
		fData[i].fOverlap->Write();

		fData[i].fEventGapPerc->Write();
		fData[i].fOverlapPerc->Write();

		fDataPrim[i].fTimeHisto->Write();
		fDataPrim[i].fEventHisto->Write();
		for (std::map<Int_t, std::pair<Double_t, Double_t> >::iterator iter = fDataPrim[i].fEventStartStopMap.begin(); iter != fDataPrim[i].fEventStartStopMap.end(); iter++) {
			fDataPrim[i].fEventDiffHisto->Fill(iter->second.second - iter->second.first);
		}
		fDataPrim[i].fEventDiffHisto->Write();
		fDataPrim[i].FillGraphs();
		fDataPrim[i].fEventGap->Write();
		fDataPrim[i].fOverlap->Write();

		fDataPrim[i].fEventGapPerc->Write();
		fDataPrim[i].fOverlapPerc->Write();
	}
	fHistoMixedEvents->Write();
	fHistoMixedEventsPrim->Write();
}

ClassImp(PndTimeStructureAnaTask);
