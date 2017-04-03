/*
 * PndRichHitWriteoutBuffer.cxx
 *
 *  Created on: Mar 12, 2017
 *      Author: K. Beloborodov
 */

#include <PndRichHitWriteoutBuffer.h>

ClassImp(PndRichHitWriteoutBuffer);

#include "PndRichPDHit.h"


PndRichHitWriteoutBuffer::PndRichHitWriteoutBuffer():FairWriteoutBuffer() {
}


PndRichHitWriteoutBuffer::PndRichHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance): FairWriteoutBuffer(branchName, "PndRichPDHit", folderName, persistance)
{
}

PndRichHitWriteoutBuffer::~PndRichHitWriteoutBuffer() {
	// TODO Auto-generated destructor stub
}


void PndRichHitWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp* data)
{
  FairRootManager* ioman = FairRootManager::Instance();
  TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1) std::cout << "Data Inserted: "  <<  *(PndRichPDHit*)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndRichPDHit(*(PndRichPDHit*)(data));
}

double PndRichHitWriteoutBuffer::FindTimeForData(FairTimeStamp* data)
{
  std::map<PndRichPDHit, double>::iterator it;
  PndRichPDHit myData = *(PndRichPDHit*)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
void PndRichHitWriteoutBuffer::FillDataMap(FairTimeStamp* data, double activeTime)
{
  PndRichPDHit myData = *(PndRichPDHit*)data;
  fData_map[myData] = activeTime;
}
void PndRichHitWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp* data)
{
  PndRichPDHit myData = *(PndRichPDHit*)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}
