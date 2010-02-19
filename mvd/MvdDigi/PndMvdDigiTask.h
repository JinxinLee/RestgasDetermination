
/** PndMvdDigiTask.h
 **
 **/


#ifndef PNDMVDDIGITASK_H
#define PNDMVDDIGITASK_H

#include "FairTask.h"

class TClonesArray;

class PndMvdDigiTask : public FairTask
{
 public:

    /** Default constructor **/
    PndMvdDigiTask();


    /** Destructor **/
    virtual ~PndMvdDigiTask();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    
  void SetPersistance(Bool_t p = kTRUE);
  Bool_t GetPersistance() {return fPersistance;};

 private:
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
  void Register();
  void Reset();
  void ProduceHits();
  ClassDef(PndMvdDigiTask,1);

};

#endif /* MVDDIGITASK_H */
