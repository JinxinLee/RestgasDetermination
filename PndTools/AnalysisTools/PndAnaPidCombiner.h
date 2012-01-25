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

class TCandidate;
class TCandList;
class VAbsMicroCandidate;
class TClonesArray;
class PndPidProbability;
class TString;

class PndAnaPidCombiner : public TNamed {
  
public:
  //Constructor
  PndAnaPidCombiner(const char *name="PndAnaPidCombiner", TString tcanames="");
  //Destructor
  ~PndAnaPidCombiner() {/*empty*/};
  void AddTcaName(const TString &tcaname) {fPidArrays[tcaname]=0;};
  void SetTcaNames(TString &names);
  void SetDefaults();
  void ClearNames(){fPidArrays.clear(); };
  void Init();
  Bool_t Apply(TCandidate &tc);
  Bool_t Apply(TCandList &tcl);

private:
  TClonesArray* ReadTCA(const TString &tcaname);
  FairRootManager*   fRootManager;
  std::map<TString,TClonesArray*> fPidArrays; // pid data
  PndPidProbability *fPidResult;
  Bool_t fInitialized;

public:
  ClassDef(PndAnaPidCombiner,1)
};    


#endif




