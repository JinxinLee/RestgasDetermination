#ifndef PNDANATASK_H
#define PNDANATASK_H 1

#include <string>
#include <vector>

#include "TString.h"

#include "RhoCandList.h"
#include "FairRootManager.h"
#include "FairTask.h"

class TClonesArray;
class TBranch;

class PndPidListMaker;
class PndEventInfo;


class PndAnaTask: public FairTask
{
  public:
    PndAnaTask();
    ~PndAnaTask();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();

  protected:

    void InitArrays();
    int  GetEvent();
    bool FillList(RhoCandList& l, std::string listkey="All");


    //TClonesArray *fEventInfo;
    //PndEventInfo *fCurrentEventInfo;


  private:

    TClonesArray* fChargedCands;
    TClonesArray* fNeutralCands;
    TClonesArray* fChargedProbability;
    TClonesArray* fNeutralProbability;
    TClonesArray* fMcCands;

    RhoCandList fLAllCands;
    RhoCandList fLChargedCands;
    RhoCandList fLNeutralCands;
    RhoCandList fLMcCands;

    PndPidListMaker* fPidListMaker;
    Bool_t fEventRead;
    //void SetupBranchNames();

    // Private Member Variables


    ClassDef(PndAnaTask,0);
};


#endif

