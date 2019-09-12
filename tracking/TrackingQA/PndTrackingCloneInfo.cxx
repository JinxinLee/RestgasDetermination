#include "PndTrackingCloneInfo.h"
#include "FairMultiLinkedData_Interface.h"
#include <iostream>
#include <map>

using namespace std;

PndTrackingCloneInfo::PndTrackingCloneInfo() : fNumClonesTimeBased(0) {}
PndTrackingCloneInfo::PndTrackingCloneInfo(std::map<FairLink, Int_t> currentMap, TClonesArray* fMcTrackArray) : fNumClonesTimeBased(0) {}


PndTrackingCloneInfo::~PndTrackingCloneInfo() {}

void PndTrackingCloneInfo::CalcNumClones(std::map<FairLink, Int_t> timeBasedMCTrackFound, TClonesArray* mcTrackArray){

  // GetEntryNumber does not work for MC tracks so one can not loop over these and obtain the FairLink that way. 

for (std::map<FairLink, Int_t>::iterator iter = timeBasedMCTrackFound.begin(); iter != timeBasedMCTrackFound.end(); iter++) {

    FairLink myFairLink = iter->first;

    // It appears as if the number of clones obtained from map currentMap are negative, this is a protection for this case
    if(timeBasedMCTrackFound[myFairLink]>0)
    fNumClonesTimeBased = fNumClonesTimeBased + timeBasedMCTrackFound[myFairLink];

    if(timeBasedMCTrackFound[myFairLink]<0)
    fNumClonesTimeBased = fNumClonesTimeBased - timeBasedMCTrackFound[myFairLink];
  }

  if (fNumClonesTimeBased>0){
    fNumClonesTimeBased=fNumClonesTimeBased-timeBasedMCTrackFound.size();
  }

}

ClassImp(PndTrackingCloneInfo);
