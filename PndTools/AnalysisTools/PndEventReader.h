#ifndef PNDEVENTREADER_H
#define PNDEVENTREADER_H 1

#include <string>
#include <vector>

#include "TString.h"

#include "RhoBase/RhoCandList.h"

class TChain;
class TClonesArray;
class TBranch;

class PndPidListMaker;
class PndEventInfo;


class PndEventReader
{
  public:
    //PndEventReader(std::string treename="cbmsim");
    PndEventReader(TChain* t);
    PndEventReader(std::string fname, std::string treename="cbmsim");
    PndEventReader(TString fname, TString treename="cbmsim");
    PndEventReader(const char* fname, const char* treename="cbmsim");

    virtual ~PndEventReader();

    Int_t Add(TString fname) { return Add(fname.Data());}
    Int_t Add(std::string fname) { return Add(fname.c_str());}
    Int_t Add(const char* fname);
    Int_t Add(TChain* chain);

    void Rewind();
    void Reset() { Rewind();}
    int  GetEvent(int n=-1);
    int  GetEntries();
    const PndEventInfo* GetEventInfo();
    bool FillList(RhoCandList& l, std::string listkey="All");
    Float_t GetTag(const char* bname);
    Int_t   GetTagI(const char* bname);

  private:
    void Init();
    void SetupBranchNames();

    // Private Member Variables

    int       fEvtCount;
    int       fChainEntries;
    std::string   fTreeName;
    TChain*      fOwnChain;
    bool      fOwnTree;
    bool      fEventRead;
    PndPidListMaker* fPidListMaker;

    TClonesArray* fChargedCands;
    TClonesArray* fNeutralCands;
    TClonesArray* fChargedProbability;
    TClonesArray* fNeutralProbability;
    TClonesArray* fMcCands;
    TClonesArray* fMicroCands;

    TClonesArray* fEventInfo;
    PndEventInfo* fCurrentEventInfo;

    RhoCandList allCands;
    RhoCandList chargedCands;
    RhoCandList neutralCands;
    RhoCandList mcCands;

    ClassDef(PndEventReader,1);
};


#endif

