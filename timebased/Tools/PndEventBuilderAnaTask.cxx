#include "PndEventBuilderAnaTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

#include "PndMCTrack.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairTimeStamp.h"
#include "FairMultiLinkedData.h"


void EventInfo::CalculateBranchInfo(){
    for (auto mcevent : fMCEvents){
        int eventNr = mcevent.first;
        for (auto pointInfo : mcevent.second.fMCPoints){
            int sum = 0;
//            std::cout << pointInfo.first << std::endl;
//            for (auto pointsFound : pointInfo.second)
//                std::cout << pointsFound << " ";
//            std::cout << std::endl;
            std::for_each(pointInfo.second.begin(), pointInfo.second.end(), [&](MCPoint point){sum += point.isFound;});
            fBranchInfo[pointInfo.first].fPercentageMCPoints[eventNr]=((double)sum)/pointInfo.second.size();
            fBranchInfo[pointInfo.first].fNumberOfFoundMCPoints[eventNr]=sum;
            fBranchInfo[pointInfo.first].fTotalNumberOfPoints[eventNr] = pointInfo.second.size();

            int totalPrim = 0;
            int sumPrim = 0;

            std::for_each(pointInfo.second.begin(), pointInfo.second.end(), [&](MCPoint point){totalPrim += point.isPrimary;});
            std::for_each(pointInfo.second.begin(), pointInfo.second.end(), [&](MCPoint point){if (point.isPrimary){sumPrim += point.isFound;}});
            fBranchInfo[pointInfo.first].fPercentagePrimaryMCPoints[eventNr]=((double)sumPrim)/totalPrim;
            fBranchInfo[pointInfo.first].fNumberOfFoundPrimaryMCPoints[eventNr]=sumPrim;
            fBranchInfo[pointInfo.first].fTotalNumberOfPrimaryPoints[eventNr] = totalPrim;

        }
    }
}

PndEventBuilderAnaTask::PndEventBuilderAnaTask() :
	PndPersistencyTask("Pnd Gap Event Builder"), fMainBranchName(), fEntryNr(0)
{
}

PndEventBuilderAnaTask::~PndEventBuilderAnaTask()
{
}

void PndEventBuilderAnaTask::SetParContainers()
{
  //FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb(); //[R.K. 01/2017] unused variable?
//  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndEventBuilderAnaTask::ReInit()
{
  InitStatus stat=kSUCCESS;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndEventBuilderAnaTask::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndEventBuilderAnaTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  fMCArray = (TClonesArray*) FairRootManager::Instance()->GetObject("MCTrack");

//  fMainHitArray = (TClonesArray*) FairRootManager::Instance()->GetObject(fMainBranchName);
//  std::cout << "ClassName of HitArray: " << fMainHitArray->ClassName() << std::endl;
 // fBranchHistos[fMainBranchName] = new BranchHistos(fMainBranchName, fMainHitArray->GetEntries(), fMCArray->GetEntries());

  for (auto & branch :  fAddHitArray){

	  TString outputName = branch.first;
	  branch.second = ((TClonesArray*)ioman->GetObject(outputName.Data()));
	  if (fBranchHistos.count(GetPointBranch(outputName)) == 0){                            //necessary for MVD Strips and Pixel because they share the same point array
	      fBranchHistos[GetPointBranch(outputName)] = new BranchHistos(GetPointBranch(outputName), 1000, 1000);
	      fMCPointArrays[FairRootManager::Instance()->GetBranchId(GetPointBranch(outputName))] = (TClonesArray*) FairRootManager::Instance()->GetObject(GetPointBranch(outputName));
	  }
	  std::cout << "-I- PndEventBuilderAnaTask::Init Hit and Point Array: " << outputName << " " << GetPointBranch(outputName)<< std::endl;
  }



//  fMCPointArrays[FairRootManager::Instance()->GetBranchId("STTPoint")] = (TClonesArray*) FairRootManager::Instance()->GetObject("STTPoint");
//  fMCPointArrays[FairRootManager::Instance()->GetBranchId("MVDPoint")] = (TClonesArray*) FairRootManager::Instance()->GetObject("MVDPoint");
//  fMCPointArrays[FairRootManager::Instance()->GetBranchId("GEMPoint")] = (TClonesArray*) FairRootManager::Instance()->GetObject("GEMPoint");
//  fMCPointArrays[FairRootManager::Instance()->GetBranchId("SciTPoint")] = (TClonesArray*) FairRootManager::Instance()->GetObject("SciTPoint");
//  fMCPointArrays[FairRootManager::Instance()->GetBranchId("EmcHit")] = (TClonesArray*) FairRootManager::Instance()->GetObject("EmcHit");
//  fMCPointArrays[FairRootManager::Instance()->GetBranchId("FTSPoint")] = (TClonesArray*) FairRootManager::Instance()->GetObject("FTSPoint");
//  fMCPointArrays[FairRootManager::Instance()->GetBranchId("FtofPoint")] = (TClonesArray*) FairRootManager::Instance()->GetObject("FtofPoint");

  std::cout << "-I- PndEventBuilderAnaTask: Initialisation successfull" << std::endl;
  fInitDone = kTRUE;

  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void PndEventBuilderAnaTask::Exec(Option_t*)
{

    std::cout << "--- EntryNr " << FairRootManager::Instance()->GetEntryNr() << " ---" << std::endl;
    fEntryNr = FairRootManager::Instance()->GetEntryNr();
    fEventInfo.push_back(EventInfo());

//    std::set<int> MCEventID = GetMCEventIDs(fMainHitArray);
    std::set<int> MCEventID = GetMCEventIDs(fAddHitArray[fMainBranchName]);

    std::cout << "-I- PndEventBuilderAnaTask::Exec " << fMainBranchName << " has: " << fAddHitArray[fMainBranchName]->GetEntriesFast() << std::endl;

	if (fVerbose > 0){
        std::cout << "MCEvents in Entry: ";
        for (auto id : MCEventID){
            std::cout << id << " ";
        }
        std::cout << std::endl;
	}

	for (auto id : MCEventID){
	    if (fEventInfo[fEntryNr].fMCEvents.count(id) == 0){
	        fEventInfo[fEntryNr].fMCEvents[id] = GetMCInfo(id);
	        std::cout << "MCPointBranches for event: " << id << " " << fEventInfo[fEntryNr].fMCEvents[id].fMCPoints.size() << std::endl;
	    }
	}

//	for (auto & event : fEventInfo[fEntryNr].fMCEvents){
//	    std::cout << "-I- PndEventBuilderAnaTask::Exec Reset event: " << event.first << std::endl;
//	    event.second.ResetFoundPoints();
//	}

//	if (fVerbose > 0){
//        for (auto mcevent : fMCEvents){
//            std::cout << "Hits for Event " << mcevent.first << std::endl;
//            std::cout << mcevent.second;
//        }
//    }

//	AssignHitsToPoints(fMainBranchName, fMainHitArray);
	for (auto branch : fAddHitArray){
	    AssignHitsToPoints(branch.first, branch.second);
	}


	if (fVerbose > 0){
        for (auto mcevent : fEventInfo[fEntryNr].fMCEvents){
            std::cout << "Found hits for Event " << mcevent.first << std::endl;
            std::cout << mcevent.second;
        }
    }

	fEventInfo[fEntryNr].CalculateBranchInfo();

	if (fVerbose > 0){
	    std::cout << "-I- PndEventBuilderAnaTask::Exec EventInfo" << std::endl;
	    std::cout << fEventInfo[fEntryNr]<<std::endl;
	}

	FillHistos();


}

void PndEventBuilderAnaTask::AssignHitsToPoints(TString branchName, TClonesArray* hitArray)
{
    std::cout << "-I- PndEventBuilderAnaTask::AssignHitsToPoints " << branchName << " size: " << hitArray->GetEntries() << std::endl;
    for (int i = 0; i < hitArray->GetEntriesFast(); i++){
        FairMultiLinkedData_Interface* data = (FairMultiLinkedData_Interface*)hitArray->At(i);
        FairMultiLinkedData links = data->GetLinksWithType(FairRootManager::Instance()->GetBranchId(GetPointBranch(branchName)));
//        std::cout << links << std::endl;
        for (int j = 0; j <  links.GetNLinks(); j++){
            FairLink link = links.GetLink(j);
            if (fEventInfo[fEntryNr].fMCEvents.count(link.GetEntry())>0){
                if (fEventInfo[fEntryNr].fMCEvents[link.GetEntry()].fMCPoints.count(link.GetType()) > 0){
                    fEventInfo[fEntryNr].fMCEvents[link.GetEntry()].fMCPoints[link.GetType()][link.GetIndex()].isFound = true;
                } else {
//                    std::cout << "-W- PndEventBuilderAnaTask::AssignHitsToPoints no MCPoint array for " << link.GetType() << std::endl;
                }
            } else {
//                std::cout << "-W- PndEventBuilderAnaTask::AssignHitsToPoints no MCEvents for event " << link.GetEntry() << std::endl;
                fEventInfo[fEntryNr].fBranchInfo[link.GetType()].fNumberOfPointsOutsideMC++;
            }
        }
    }
}

std::set<int> PndEventBuilderAnaTask::GetMCEventIDs(TClonesArray* array)
{
   std::set<int> MCEventID;

   for (int i = 0; i < array->GetEntriesFast(); i++){
       FairMultiLinkedData_Interface* data = (FairMultiLinkedData_Interface*)fAddHitArray[fMainBranchName]->At(i);
//       std::cout << "-I- PndEventBuilderAnaTask::GetMCEventIDs full Links: " << *data << std::endl;
       std::vector<FairLink> MCLinks = data->GetSortedMCTracks();
//       std::cout << "-I- PndEventBuilderAnaTask::GetMCEventIDs MCLinks: ";
       for (auto mclink : MCLinks){
           MCEventID.insert(mclink.GetEntry());
       }
//       std::cout << std::endl;
   }
   return MCEventID;
}

MCEvent PndEventBuilderAnaTask::GetMCInfo(int entryNr){
    MCEvent event;

    int oldEntryNr = FairRootManager::Instance()->GetEntryNr();
    TTree* tree = FairRootManager::Instance()->GetInTree();
    TBranch* mcBranch = tree->GetBranch("MCTrack");
    mcBranch->GetEntry(entryNr);

    for (int i = 0; i < fMCArray->GetEntriesFast(); i++){
        PndMCTrack* mcTrack = (PndMCTrack*)fMCArray->At(i);
        if (mcTrack->GetMotherID() < 0)
            event.fMCTracks.push_back(FairLink(-1, entryNr, 0, i, -1));
    }

    mcBranch->GetEntry(oldEntryNr);

    for (auto branch : fMCPointArrays){

        TBranch* pointBranch = tree->GetBranch(FairRootManager::Instance()->GetBranchName(branch.first));
        pointBranch->GetEntry(entryNr);
        for (int j = 0; j < branch.second->GetEntriesFast(); j++){
            FairMultiLinkedData_Interface* data = (FairMultiLinkedData_Interface*)branch.second->At(j);
            std::vector<FairLink> mctracks = data->GetSortedMCTracks();
            int motherId = -2;
            if (mctracks.size() > 0){
//                std::cout << "-I-  PndEventBuilderAnaTask::GetMCInfo mctracks[0] " << mctracks[0] << std::endl;
                PndMCTrack* mcTrack = (PndMCTrack*)fMCArray->At(mctracks[0].GetIndex());
                if (mcTrack != nullptr){
                    motherId = mcTrack->GetMotherID();
//                    std::cout << motherId << std::endl;
                }
                else
                    std::cout << "-W- PndEventBuilderAnaTask::GetMCInfo no mcTrack " << mcTrack << std::endl;
            }
            MCPoint point;
            point.link = FairLink(-1, entryNr, branch.first,j, motherId);
            point.isPrimary = ((motherId == -1) ? true : false);
            event.fMCPoints[branch.first].push_back(point);
        }
        pointBranch->GetEntry(oldEntryNr);
    }

//    tree->GetEntry(oldEntryNr);
    return event;
}

void PndEventBuilderAnaTask::FillHistos()
{
    for (auto branchInfo : fEventInfo[fEntryNr].fBranchInfo) {
        std::cout <<"FillHistos for " << FairRootManager::Instance()->GetBranchName(branchInfo.first) << std::endl;
        if (fBranchHistos.count(FairRootManager::Instance()->GetBranchName(branchInfo.first)) > 0 && fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)] != nullptr){
            for (auto foundMCPoints : branchInfo.second.fNumberOfFoundMCPoints) {
                std::cout << fEntryNr << " MCEvent: " << foundMCPoints.first << " AbsoluteHits: " << foundMCPoints.second << std::endl;
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fAbsoluteHits->Fill(fEntryNr, foundMCPoints.first, foundMCPoints.second);
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fRelativeHits->Fill(fEntryNr, foundMCPoints.first, branchInfo.second.fPercentageMCPoints[foundMCPoints.first]);
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fAbsoluteHitsHisto->Fill(foundMCPoints.second);
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fRelativeHitsHisto->Fill(branchInfo.second.fPercentageMCPoints[foundMCPoints.first]);
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fAbsolutePrimaryHits->Fill(fEntryNr, foundMCPoints.first, branchInfo.second.fNumberOfFoundPrimaryMCPoints[foundMCPoints.first]);
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fRelativePrimaryHits->Fill(fEntryNr, foundMCPoints.first, branchInfo.second.fPercentagePrimaryMCPoints[foundMCPoints.first]);
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fAbsolutePrimaryHitsHisto->Fill(branchInfo.second.fNumberOfFoundPrimaryMCPoints[foundMCPoints.first]);
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fRelativePrimaryHitsHisto->Fill(branchInfo.second.fPercentagePrimaryMCPoints[foundMCPoints.first]);
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fOutsideMC->Fill(fEntryNr, branchInfo.second.fNumberOfPointsOutsideMC);
                fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fOutsideMCHisto->Fill(branchInfo.second.fNumberOfPointsOutsideMC);
            }
            fBranchHistos[FairRootManager::Instance()->GetBranchName(branchInfo.first)]->fEventOverlap->Fill(branchInfo.second.fNumberOfFoundMCPoints.size());
        }
    }
}

void PndEventBuilderAnaTask::FinishEvent()
{
}

void PndEventBuilderAnaTask::FinishTask()
{
    for (auto branchHisto : fBranchHistos){
        branchHisto.second->WriteHistos();
    }
}


ClassImp(PndEventBuilderAnaTask);

