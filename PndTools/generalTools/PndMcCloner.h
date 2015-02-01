#ifndef PNDMCCLONER_H
#define PNDMCCLONER_H 1


#include "FairTask.h"
#include "PndMCTrack.h"
#include <map>

using namespace std;

class TClonesArray;

class PndMcCloner : public FairTask
{

 public:

  /** Default constructor **/  
  PndMcCloner();


  /** Destructor **/
  ~PndMcCloner();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetCleanMc(Bool_t opt = kTRUE) { fCleanMC = opt; };

 protected:

  void FindUsedMCIndices();  
  void CloneMCTrack();
  void CloneAndCleanMCTrack();
  void CorrectPidIndices();

  /** Input array of PndMCTrack **/
  TClonesArray* fInputArray;

  /** Input array of PidChargedCand **/
  TClonesArray* fPidChargedArray;

  /** Input array of PidNeutralCand **/
  TClonesArray* fPidNeutralArray;

  /** Output array of PndMCTrack **/
  TClonesArray* fOutputArray;  

  map<Int_t, Int_t> mapMCIndex; // Map <old mc index, new mc index>
   
  Bool_t fCleanMC; // Flag to clean the MCTrack from unused indices

  ClassDef(PndMcCloner,2);

};

#endif
