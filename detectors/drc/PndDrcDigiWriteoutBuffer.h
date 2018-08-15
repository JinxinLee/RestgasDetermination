/*
 * PndDrcDigiWriteoutBuffer.h
 *
 */
// -------------------------------------------------------------------------
// -----                   PndDrcDigiWriteoutBuffer header file        -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDDRCDIGIWRITEOUTBUFFER_H_
#define PNDDRCDIGIWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndDrcDigi.h"

class FairTimeStamp;

class PndDrcDigiWriteoutBuffer : public FairWriteoutBuffer{

public:
  PndDrcDigiWriteoutBuffer();
  PndDrcDigiWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);

  virtual ~PndDrcDigiWriteoutBuffer();

  void AddNewDataToTClonesArray(FairTimeStamp*);

  virtual double FindTimeForData(FairTimeStamp* data) ;
  virtual void FillDataMap(FairTimeStamp* data, double activeTime) ;
  virtual void EraseDataFromDataMap(FairTimeStamp* data);
  virtual std::vector<std::pair<double, FairTimeStamp*> > Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp*> newData);

  virtual Int_t GetNData() {return fStartTime_map.size();}

protected:	

  std::map<PndDrcDigi, double> fData_map;
	
  ClassDef(PndDrcDigiWriteoutBuffer, 1);
};

#endif /* PNDDrcDigiWRITEOUTBUFFER_H_ */
