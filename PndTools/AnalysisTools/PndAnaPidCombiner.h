//
//  PndAnaPidCombiner.h
//  PANDAROOT
//
//  Created by Ralf Kliemt on 12/16/11.
//
#ifndef PNDANAPIDCOMBINER_H
#define PNDANAPIDCOMBINER_H

#include <iostream>
#include "TNamed.h"
#include "FairRootManager.h"
#include <map>
#include <vector>

class RhoCandidate;
class RhoCandList;
class FairRecoCandidate;
class TClonesArray;
class PndPidProbability;
class TString;

class PndAnaPidCombiner : public TNamed
{

  public:
    //Constructor
    PndAnaPidCombiner(const char* name="PndAnaPidCombiner", TString tcanames="");
    //Destructor
    ~PndAnaPidCombiner() {/*empty*/};
    //void AddTcaName(const TString& tcaname) {fPidArrays[tcaname]=0;};
    void SetTcaNames(TString& names);
    void SetDefaults();
    void ClearNames() {fPidArrays.clear(); };
    void Init();

    void ApplyFlat(RhoCandidate* tc);
    Bool_t Apply(RhoCandidate* tc);
    Bool_t Apply(RhoCandList& tcl);

  private:
    TClonesArray* ReadTCA(const TString& tcaname);
    FairRootManager*   fRootManager; //!
    std::map<TString,TClonesArray*> fPidArrays; //! pid data map, full
    std::vector<TString> fCurrentPidArrays; //! pid data current request
    PndPidProbability* fPidResult; //!
    Bool_t fInitialized;

  public:
    ClassDef(PndAnaPidCombiner,1)
};


#endif
