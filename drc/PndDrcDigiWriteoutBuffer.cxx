/*
 * PndDrcDigiWriteoutBuffer.cxx
 *
 */
// -------------------------------------------------------------------------
// -----                   PndDrcDigiWriteoutBuffer source file        -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include <PndDrcDigiWriteoutBuffer.h>

#include "PndDrcDigi.h"


ClassImp(PndDrcDigiWriteoutBuffer);


PndDrcDigiWriteoutBuffer::PndDrcDigiWriteoutBuffer():FairWriteoutBuffer() {

	// TODO Auto-generated constructor stub

}


PndDrcDigiWriteoutBuffer::PndDrcDigiWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance): FairWriteoutBuffer(branchName, "PndDrcDigi", folderName, persistance)
{
}

PndDrcDigiWriteoutBuffer::~PndDrcDigiWriteoutBuffer() {
	// TODO Auto-generated destructor stub
}

/////////////////////////////////////////////////////////////////////////////
void PndDrcDigiWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp* data)
{
  FairRootManager* ioman = FairRootManager::Instance();
  TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1){
	  std::cout << "Data Inserted: "  <<  *(PndDrcDigi*)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndDrcDigi(*(PndDrcDigi*)(data));
}
/////////////////////////////////////////////////////////////////////////////
double PndDrcDigiWriteoutBuffer::FindTimeForData(FairTimeStamp* data)
{
  std::map<PndDrcDigi, double>::iterator it;
  PndDrcDigi myData = *(PndDrcDigi*)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
/////////////////////////////////////////////////////////////////////////////
void PndDrcDigiWriteoutBuffer::FillDataMap(FairTimeStamp* data, double activeTime)
{
  PndDrcDigi myData = *(PndDrcDigi*)data;
  fData_map[myData] = activeTime;
}
/////////////////////////////////////////////////////////////////////////////
void PndDrcDigiWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp* data)
{
  PndDrcDigi myData = *(PndDrcDigi*)data;
  if (fData_map.find(myData) != fData_map.end()){
    fData_map.erase(fData_map.find(myData));
  }
}
/////////////////////////////////////////////////////////////////////////////
std::vector<std::pair<double, FairTimeStamp*> > PndDrcDigiWriteoutBuffer::Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp*> newData) {
  
  std::vector<std::pair<double, FairTimeStamp*> > result;
  std::pair<double, FairTimeStamp*> Result1;
  if (newData.first > 0)
  
  Result1.first = oldData.first + (newData.first - oldData.first);
  Result1.second = oldData.second;
  if (fVerbose > 0){
    std::cout << "Modify hit" << std::endl;
    std::cout << "OldData: " << oldData.first << " : " << oldData.second << " NewData: " << newData.first << " : " << newData.second << std::endl;
    std::cout << "Resulting Data: " << Result1.first << " : " << Result1.second << std::endl;
  }
  
  result.push_back(Result1);
  return result;
}
void PndDrcDigiWriteoutBuffer::FillNewData(FairTimeStamp* data, double startTime, double activeTime)
{
  FairTimeStamp* dataClone = static_cast<FairTimeStamp*>(data->Clone());

  if (fActivateBuffering) {
    if (fVerbose > 0) {
      std::cout << "StartTime: " << startTime << std::endl;
    }
    std::pair<double, FairTimeStamp*> timeData(activeTime, dataClone);
    fStartTime_map.insert(std::pair<double, std::pair<double, FairTimeStamp*> >(startTime, timeData));
  } else {
    AddNewDataToTClonesArray(dataClone);
    delete dataClone;
  }

}
//_____________________________________________________________________________

void PndDrcDigiWriteoutBuffer::FillDataToDeadTimeMap(FairTimeStamp* data, double activeTime)
{
  if (fActivateBuffering) {
    typedef  std::multimap<double, FairTimeStamp*>::iterator DTMapIter;
    typedef  std::map<FairTimeStamp, double>::iterator DataMapIter;

    double timeOfOldData = FindTimeForData(data);
    // PrintDeadTimeMap();
    if(timeOfOldData > -1) {        //if an older active data object is already present
      if (fVerbose > 1) {
        std::cout << " OldData found! " << timeOfOldData << std::endl;
      }
      if (fVerbose > 1) {
        std::cout << "New Data: " << activeTime << " : " << data << std::endl;
      }
      double currentdeadtime = timeOfOldData;
      FairTimeStamp* oldData;
      bool dataFound = false;

      // PrintDeadTimeMap();
      for (DTMapIter it = fDeadTime_map.lower_bound(currentdeadtime); it != fDeadTime_map.upper_bound(currentdeadtime); it++) {
        oldData = it->second;
        if (fVerbose > 1) {
          std::cout << "Check Data: " << it->first << " : " << oldData << std::endl;
        }
            
        if (oldData->equal(data)) {
          if (fVerbose > 1) {
            std::cout << " oldData == data " << std::endl;
          }
          if (fVerbose > 1) {
            std::cout << it->first << " : " << it->second << std::endl;
          }
          fDeadTime_map.erase(it);
          EraseDataFromDataMap(oldData);
          dataFound = true;
          break;
        }
      fDeadTime_map.insert(std::pair<double, FairTimeStamp*>(activeTime, data));
      FillDataMap(data, activeTime);
      break;
      }

      if (dataFound == true) {
        std::vector<std::pair<double, FairTimeStamp*> > modifiedData = Modify(std::pair<double, FairTimeStamp*>(currentdeadtime, oldData), std::pair<double, FairTimeStamp*>(activeTime, data));
        for (int i = 0; i < modifiedData.size(); i++) {
        FillDataToDeadTimeMap(modifiedData[i].second, modifiedData[i].first);
          if (fVerbose > 1) {
            std::cout << i << " :Modified Data: " << modifiedData[i].first << " : " << modifiedData[i].second << std::endl;
          }
        }
      } else {
        std::cout << "-E- PndDrcDigiWriteoutBuffer::FillDataToDeadTimeMap: old data present in dataMap but not in deadTimeMap!" << std::endl;
      }

    } else {
      if (fVerbose > 1) {
        std::cout << "-I- FairWriteoutBuffer::FillDataToDeadTimeMap Data Inserted: " << activeTime << " : ";
        data->Print();
        std::cout << std::endl;
      }
      fDeadTime_map.insert(std::pair<double, FairTimeStamp*>(activeTime, data));
      FillDataMap(data, activeTime);
    }
  } else {
    AddNewDataToTClonesArray(data);
    delete data;
  }
}
//_____________________________________________________________________________
