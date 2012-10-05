// -------------------------------------------------------------------------
// -----                  PndLmdHitMergeTask source file               -----
// -------------------------------------------------------------------------

#include <cmath>
#include <vector>
#include <map>

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndLmdHitMergeTask.h"
#include "PndSdsHit.h"

// -----   Default constructor   -------------------------------------------
PndLmdHitMergeTask::PndLmdHitMergeTask() :
FairTask("LMD Hit Merging Task")
{
  fHitBranchName = "LMDHitsPixel";
}

// -----   Named constructor   -------------------------------------------
PndLmdHitMergeTask::PndLmdHitMergeTask(const char* name) :
FairTask(name)
{
  fHitBranchName = "LMDHitsPixel";
}

// -----   Destructor   ----------------------------------------------------
PndLmdHitMergeTask::~PndLmdHitMergeTask()
{

}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdHitMergeTask::Init()
{
  
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
    std::cout << "-E- PndLmdHitMergeTask::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject(fHitBranchName);
//
  if ( ! fHitArray )
  {
    std::cout << "-W- PndLmdHitMergeTask::Init: "
    << "No LmdHitsPixel array!" << std::endl;
    return kERROR;
  }

  // set output arrays
  fMergedHitArray = new TClonesArray("PndSdsHit");
  ioman->Register("LmdHits", "PndLmd", fMergedHitArray, true);
  
  Info("Init","Initialisation successfull");
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndLmdHitMergeTask::Exec(Option_t* opt)
{
  if (fVerbose > 2)
    std::cout<<" **Starting PndLmdHitMergeTask::Exec()**"<<std::endl;

  // Reset output array
  //fMergedHitArray = FairRootManager::Instance()->GetTClonesArray("LmdHits");
  //if ( ! fMergedHitArray ) Fatal("Exec", "No OutputArray");
  fMergedHitArray->Delete();

  // Get input array
  fHitArray = (TClonesArray*)FairRootManager::Instance()->GetObject(fHitBranchName);
  if ( ! fHitArray )
  {
    std::cout << "-W- PndLmdHitMergeTask::Init: "
    << "No Hit array!" << std::endl;
    return;
  }

  // when we have no hits, we can end the event here.
  if (fHitArray->GetEntriesFast() == 0) return;

  // -------   SEARCH Close Hits  ------
  std::vector<unsigned int> mergewithIDs, mergedHits;
  //std::map<unsigned int, std::vector<unsigned int>> backmap;
  PndSdsHit* tmphit;
  unsigned int newHits=0;
  for(Int_t iHit = 0; iHit < fHitArray->GetEntriesFast(); iHit++){
    bool skip=false;
    for(unsigned int ijk=0; ijk<mergedHits.size(); ijk++) //check if already merged
      if(iHit==mergedHits.at(ijk))
        skip=true;
    if(skip) continue;

    mergewithIDs.clear();
    PndSdsHit* myHit1 = (PndSdsHit*)(fHitArray->At(iHit));
    for(Int_t jHit = iHit+1; jHit < fHitArray->GetEntriesFast(); jHit++){ //check other hits
      PndSdsHit* myHit2 = (PndSdsHit*)(fHitArray->At(jHit));
    
      //    if( fabs(myHit1->GetZ()-myHit2->GetZ())<0.1 ){  //actually same Hit: merge
    if( fabs(myHit1->GetZ()-myHit2->GetZ())<0.05 && 
	fabs(myHit1->GetZ()-myHit2->GetZ())>0.01 &&
	fabs(myHit1->GetX()-myHit2->GetX())<0.016&& 
	fabs(myHit1->GetY()-myHit2->GetY())<0.016){  //actually same Hit: merge (too pixel size fo x-y)
      if (fVerbose > 2){
	std::cout<<"----- We are going to merge HITS: -----"<<std::endl;
	myHit1->Print();
	std::cout<<".... AND ...."<<std::endl;
	myHit2->Print();
	std::cout<<"---------------------------------------"<<std::endl;
      }
        mergewithIDs.push_back(jHit);
        mergedHits.push_back(jHit);
      }
    }

    if(mergewithIDs.size()>0){ //merge hits
      tmphit = new((*fMergedHitArray)[newHits]) PndSdsHit(*myHit1);
      double x=tmphit->GetX(), y=tmphit->GetY(), z=tmphit->GetZ();
      for(unsigned int iMerge=0; iMerge<mergewithIDs.size(); iMerge++){ //loop over hits to merge in
        PndSdsHit* myHit2 = (PndSdsHit*)(fHitArray->At(mergewithIDs.at(iMerge)));
        x+=myHit2->GetX(); y+=myHit2->GetY(); z+=myHit2->GetZ();
      }
      x/=mergewithIDs.size(); y/=mergewithIDs.size(); z/=mergewithIDs.size();
      tmphit->SetX(x); tmphit->SetY(y); tmphit->SetZ(z);
    }else{ //dont merge, just use original hit
      tmphit = new((*fMergedHitArray)[newHits]) PndSdsHit(*myHit1);
    }

    newHits++;
  }
  
  if (fVerbose > 1)
    std::cout << "-I- PndLmdHitMergeTask: out of " << fHitArray->GetEntriesFast()
    << " Hits " << fMergedHitArray->GetEntriesFast()<<" Hits merged."
    << std::endl;
  return;
}

ClassImp(PndLmdHitMergeTask);

