// -------------------------------------------------------------------------
// -----                PndMCTrackInfoTask source file             -----
// -------------------------------------------------------------------------

#include "PndMCTrackInfoTask.h"

#include "FairRun.h"
#include "FairRootManager.h"
#include "FairMultiLinkedData_Interface.h"

// -----   Default constructor   -------------------------------------------
PndMCTrackInfoTask::PndMCTrackInfoTask() :
  PndPersistencyTask("MCTrackInfo Task"), fMCTracks(nullptr), fMaxStage(3)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMCTrackInfoTask::~PndMCTrackInfoTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMCTrackInfoTask::SetParContainers()
{
  return;
}

InitStatus PndMCTrackInfoTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTrackInfoTask::Init()
{
  
  FairRun* ana = FairRun::Instance();
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
    std::cout << "-E- PndMCTrackInfoTask::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  if (fBranches.size() == 0){
      // Use hits of all tracking subsystems if nothing is given
      AddBranchName("STTPoint");
      AddBranchName("MVDPoint");
      AddBranchName("GEMPoint");
      AddBranchName("FTSPoint");
  }
  
  for (auto branch : fBranches)
  {
    if (ioman->GetObject(branch.first) != 0)
    {
        fBranches[branch.first] = (TClonesArray*) ioman->GetObject(branch.first);
    }
  }

  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( !fMCTracks )
  {
    std::cout << "-W- PndMCTrackInfoTask::Init: "
    << "No MCTrack array!" << std::endl;
    return kERROR;
  }
  
  fMCTrackInfo = new TClonesArray("PndMCTrackInfo");
  ioman->Register("MCTrackInfo", "MC", fMCTrackInfo, GetPersistency());

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMCTrackInfoTask::Exec(Option_t*)
{
  fTrackInfo.clear();
  fMCTrackInfo->Delete();

  for (int i = 0; i < fMCTracks->GetEntriesFast(); i++){
      PndMCTrack* myTrack = (PndMCTrack*)fMCTracks->At(i);
      std::vector<int> pidMothers = GetPIDMothers(myTrack->GetMotherID());
      if (fMaxStage > -1 && pidMothers.size() > fMaxStage){
          fTrackInfo[i];                                            //create empty MCTrackInfo object to keep the index in-line with the MCTracks
          continue;
      }
      fTrackInfo[i].SetPIDMothers(pidMothers);

      int motherID = myTrack->GetMotherID();
      if (motherID > -1){
          fTrackInfo[motherID].AddDaughter(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId("MCTrack"), i));
          fTrackInfo[motherID].AddPIDDaughter(myTrack->GetPdgCode());
          if (fTrackInfo[motherID].GetDecayTime() == 0){
              fTrackInfo[motherID].SetDecayTime(myTrack->GetStartTime());
              fTrackInfo[motherID].SetDecayVertex(myTrack->GetStartVertex());
          } else {
              if (fTrackInfo[motherID].GetDecayTime() != myTrack->GetStartTime()){
                  std::cout << "-W- PndMCTrackInfoTask::Exec DecayTimes not equal: " << fTrackInfo[motherID].GetDecayTime() << " != " << myTrack->GetStartTime() << std::endl;
              }
              if (fTrackInfo[motherID].GetDecayVertex() != myTrack->GetStartVertex()){
                  std::cout << "-W- PndMCTrackInfoTask::Exec DecayVertices not equal: "
                          << fTrackInfo[motherID].GetDecayVertex().x() << "/" << fTrackInfo[motherID].GetDecayVertex().y() << "/" << fTrackInfo[motherID].GetDecayVertex().z()
                          << " != " << myTrack->GetStartVertex().x() << "/" << myTrack->GetStartVertex().y() << "/" << myTrack->GetStartVertex().z() << std::endl;
              }
          }
      }
  }

  AssignPoints();
  FillPointInfo();

  FillOutputArray();

  if (fVerbose > 0){
      std::cout << "-- Event " << FairRootManager::Instance()->GetEntryNr() << "---" << std::endl;
      for (int i = 0; i < fMCTracks->GetEntriesFast(); i++){
          PndMCTrack* myTrack = (PndMCTrack*)fMCTracks->At(i);
          std::cout << i << " : " << *myTrack;
          std::cout << fTrackInfo[i] << std::endl;
      }
  }
}
// -------------------------------------------------------------------------

std::vector<int> PndMCTrackInfoTask::GetPIDMothers(int motherID)
{
    std::vector<int> result;
    if (motherID == -1)
        return result;

    result = fTrackInfo[motherID].GetPIDMothers();
    PndMCTrack* myTrack = (PndMCTrack*)fMCTracks->At(motherID);
    result.push_back(myTrack->GetPdgCode());

    return result;
}

void PndMCTrackInfoTask::AssignPoints()
{
    FairRootManager* ioman = FairRootManager::Instance();
    for (auto branch : fBranches){
        for (int i = 0; i < branch.second->GetEntriesFast(); i++){
            FairMultiLinkedData_Interface* links = (FairMultiLinkedData_Interface*)branch.second->At(i);
            std::vector<FairLink> mcLinks = links->GetSortedMCTracks();
            for (auto mclink : mcLinks){
                fTrackInfo[mclink.GetIndex()].AddLink(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(branch.first), i));
            }
        }
    }
}

void PndMCTrackInfoTask::FillPointInfo(){
    for (auto & trackInfo : fTrackInfo){
        trackInfo.second.FillPoints();
    }
}

void PndMCTrackInfoTask::FillOutputArray(){
    for (auto track : fTrackInfo){
        new((*fMCTrackInfo)[fMCTrackInfo->GetEntriesFast()]) PndMCTrackInfo(track.second);
    }
}

void PndMCTrackInfoTask::FinishEvent()
{

  FinishEvents();
}
// -------------------------------------------------------------------------

void PndMCTrackInfoTask::FinishTask()
{
}

ClassImp(PndMCTrackInfoTask);
