// -------------------------------------------------------------------------
// -----                      PndMCTrackInfo source file               -----
// -----                  Created 16/09/19  by T.Stockmanns            -----
// -------------------------------------------------------------------------

#include <FairRootManager.h>

#include <iostream>

#include "PndMCTrackInfo.h"
using namespace std;

// -----   Default constructor   -------------------------------------------
PndMCTrackInfo::PndMCTrackInfo(): FairMultiLinkedData_Interface(),
  fDecayVertex(), fDecayTime(), fStage(-1), fMVDPoints(0), fSTTPoints(0), fGEMPoints(0), fFTSPoints(0), fTotalPoints(0)
{
}
// -------------------------------------------------------------------------

  
// -----   Destructor   ----------------------------------------------------
PndMCTrackInfo::~PndMCTrackInfo() { }
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndMCTrackInfo::Print()
{
  cout << "DecayTime: " << fDecayTime << " DecayVertex: " << fDecayVertex.x() << "/" << fDecayVertex.y() << "/" << fDecayVertex.z() << endl;
  cout << *((FairMultiLinkedData_Interface*)this) << std::endl;
  std::cout << "Stage: " << GetStage() << " Mothers: ";
  std::vector<int> motherPID = GetPIDMothers();
  if (motherPID.size() > 0){
      for (auto mpid : motherPID){
          std::cout << mpid << " : ";
      }
  }
  std::cout << std::endl;

  std::cout << "Daughters: ";
  std::vector<int> daughterPID = GetPIDDaughters();
  if (daughterPID.size() > 0){
      for (auto dpid : daughterPID)
          std::cout << dpid << " ";
      std::cout << std::endl;

//      std::vector<int> daughters = GetDaughters();
//      for (auto daughter : daughters)
//          std::cout << daughter << " ";
  }
      std::cout << std::endl;
}
// -------------------------------------------------------------------------

void PndMCTrackInfo::AddDaughter   (FairLink daughter){
    AddLink(daughter);
    fDaughters.push_back(daughter.GetIndex());
}

std::vector<int> PndMCTrackInfo::GetDaughters()
{
    return fDaughters;
}

Int_t PndMCTrackInfo::GetNHits(TString branchName){
    return GetLinksWithType(FairRootManager::Instance()->GetBranchId(branchName)).GetNLinks();
}

void PndMCTrackInfo::FillPoints(){
    fMVDPoints = GetNHits("MVDPoint");
    fSTTPoints = GetNHits("STTPoint");
    fGEMPoints = GetNHits("GEMPoint");
    fFTSPoints  = GetNHits("FTSPoint");
    fTotalPoints = fMVDPoints + fSTTPoints + fGEMPoints + fFTSPoints;
}

ClassImp(PndMCTrackInfo);
