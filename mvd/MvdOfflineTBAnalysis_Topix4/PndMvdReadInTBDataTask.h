#ifndef PndMvdReadInTBDataTask_H_
#define PndMvdReadInTBDataTask_H_

#include "FairTask.h"
#include "PndGeoHandling.h"
#include "PndMvdReadInToPix4TBData.h"

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
    void SetNumberOfFrontEnds(Int_t numberfe);

    virtual void AddFile(Int_t asic,TString fileName){
      std::cout << "File Added to asic " << asic-1 << " fileName " << fileName << std::endl;
      if (fFileNames.size() < asic){
    	  fFileNames.resize(asic);
      }
      fFileNames[asic-1].push_back(fileName);
    };

private:
    std::vector<std::vector<TString> > fFileNames;

    //std::vector<TString> fFileNames;
    std::vector<std::ifstream*> fFileHandles;
    std::vector<PndMvdReadInToPix4TBData*> fReader;
    std::vector<Bool_t> fEndOfFile;

    UInt_t fEvent;

    TClonesArray* fDigiArray;
    TClonesArray* fFrameHeaderArray;
    TClonesArray* fAllFrameHeaderArray;

	Bool_t fInitDone;
	Double_t fClockFrequency; //in MHz

  void Register();
  void Reset();

  ClassDef(PndMvdReadInTBDataTask,1);


};

#endif /*PndMvdReadInTBDataTask_H_*/
