#ifndef PndMvdReadInTBDataTask_H_
#define PndMvdReadInTBDataTask_H_

#include "FairTask.h"
#include "PndGeoHandling.h"
#include "PndMvdReadInTBData.h"

#include "TString.h"

#include <fstream>
#include <vector>

class PndMvdReadInTBDataTask : public FairTask
{
public:
	PndMvdReadInTBDataTask();
	virtual ~PndMvdReadInTBDataTask();

	 /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    virtual void FinishEvent();

    void SetFrequency(Double_t val) {fClockFrequency = val;}

    virtual void AddFile(TString fileName){
    	fFileNames.push_back(fileName);
    };

private:

    std::vector<TString> fFileNames;
    std::vector<std::ifstream*> fFileHandles;
    std::vector<PndMvdReadInTBData*> fReader;
    std::vector<Bool_t> fEndOfFile;

    UInt_t fEvent;

    TClonesArray* fDigiArray;

	Bool_t fInitDone;
	Double_t fClockFrequency; //in MHz

  void Register();
  void Reset();

  ClassDef(PndMvdReadInTBDataTask,1);


};

#endif /*PndMvdReadInTBDataTask_H_*/
