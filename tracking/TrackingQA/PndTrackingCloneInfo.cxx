#include "PndTrackingCloneInfo.h"
#include "FairMultiLinkedData_Interface.h"
#include <iostream>
#include <map>

using namespace std;

PndTrackingCloneInfo::PndTrackingCloneInfo() : fNumClonesTimeBased(0) {}
PndTrackingCloneInfo::PndTrackingCloneInfo(std::map<FairLink, Int_t> currentMap, TClonesArray* fMcTrackArray) : fNumClonesTimeBased(0) {}


PndTrackingCloneInfo::~PndTrackingCloneInfo() {}

void PndTrackingCloneInfo::CalcNumClones(std::map<FairLink, Int_t> timeBasedMCTrackFound, TClonesArray* mcTrackArray){
	//std::cout << "PndTrackingCloneInfo() start." << std::endl;

	fNumClonesTimeBased=0;

	for (std::map<FairLink, Int_t>::iterator iter = timeBasedMCTrackFound.begin(); iter != timeBasedMCTrackFound.end(); iter++) {

		FairLink myFairLink = iter->first;

		if(timeBasedMCTrackFound[myFairLink]>0){
			fNumClonesTimeBased = fNumClonesTimeBased + timeBasedMCTrackFound[myFairLink]-1;}

	}

}

ClassImp(PndTrackingCloneInfo);
