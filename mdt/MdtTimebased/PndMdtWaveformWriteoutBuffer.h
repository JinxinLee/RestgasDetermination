/*
 * PndMdtWaveformWriteoutBuffer.h
 */

#ifndef PNDMDTDIGIWRITEOUTBUFFER_H_
#define PNDMDTDIGIWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndMdtWaveform.h"
#include "TFile.h"
#include "TTree.h"
#include "THashTable.h"

class FairTimeStamp;

struct PndMdtWaveformLess {
  const bool operator()(const PndMdtWaveform* lw, const PndMdtWaveform* rw) const{
    return (*lw) < (*rw);
  }
};

class PndMdtWaveformWriteoutBuffer : public FairWriteoutBuffer
{

  public:
    PndMdtWaveformWriteoutBuffer();
    PndMdtWaveformWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
    virtual ~PndMdtWaveformWriteoutBuffer();

  private:
    void AddNewDataToTClonesArray(FairTimeStamp*);


    virtual double FindTimeForData(FairTimeStamp* data) ;
    virtual void FillDataMap(FairTimeStamp* data, double activeTime) ;
    virtual void EraseDataFromDataMap(FairTimeStamp* data);

    virtual std::vector<std::pair<double, FairTimeStamp*> > 
      Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp*> newData) ;

    std::map<PndMdtWaveform*, double, PndMdtWaveformLess > fData_map;

    ClassDef(PndMdtWaveformWriteoutBuffer, 1);
};

#endif /* PNDMDTDIGIWRITEOUTBUFFER_H_ */
