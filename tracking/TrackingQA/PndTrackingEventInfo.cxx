#include "PndTrackingEventInfo.h"
#include "FairMultiLinkedData_Interface.h"
#include "FairLink.h"
#include <iostream>
#include <vector>
#include "PndTrack.h"
#include "PndMCTrack.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

using namespace std;

PndTrackingEventInfo::PndTrackingEventInfo(PndTrack* currentTrack, FairLink currentLink) : fEventPurity(-1) {}

PndTrackingEventInfo::~PndTrackingEventInfo() {}

void PndTrackingEventInfo::CalcEventPurity(PndTrack* currentTrack, FairLink currentLink){

 int nHitsEventIndex=0;
 int nHitsAllIndex=0;
 int eventIndex=-1;

 std::cout << "Current track fair link: " << currentTrack->GetEntryNr()  << std::endl;

  std::cout << "PndTrackingEventInfo::CalcEventPurity" << std::endl;
  std::cout << "Before loop" << std::endl;
  std::cout << "Event index: " << nHitsEventIndex << ", All index: " << nHitsAllIndex << std::endl;

  //std::vector<FairLink> sortedMcTracks = currentTrack->GetSortedMCTracks();

  //std::cout << "Number of MC tracks " << sortedMcTracks.size() << std::endl;
  
  //eventIndex=sortedMcTracks[0].GetEntry();

  //eventIndex=currentLink.GetEntry();

  eventIndex=currentTrack->GetEntryNr().GetEntry();
  
  std::cout << "Entry of link: " << eventIndex << std::endl;
 
  FairMultiLinkedData sttHitsFairLinks = currentTrack->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTSortedHits"));

  for(int j = 0; j < sttHitsFairLinks.GetNLinks(); j++){
  std::cout << "In loop" << std::endl;
  std::cout << "Event index: " << nHitsEventIndex << ", All index: " << nHitsAllIndex << std::endl;

  std::cout << "Entry of hit: " << sttHitsFairLinks.GetLink(j).GetEntry() << std::endl;
  
  nHitsAllIndex++;
    
    if(sttHitsFairLinks.GetLink(j).GetEntry()==eventIndex){nHitsEventIndex++;}
 
  }
  
  std::cout << "After loop" << std::endl;
  std::cout << "Event index: " << nHitsEventIndex << ", All index: " << nHitsAllIndex << std::endl;

  if(nHitsAllIndex!=0){fEventPurity = (double) nHitsEventIndex/ nHitsAllIndex;}
   
}

ClassImp(PndTrackingEventInfo);
