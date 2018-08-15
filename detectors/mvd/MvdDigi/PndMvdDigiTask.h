
/** PndMvdDigiTask.h
 **
 **/


#ifndef PNDMVDDIGITASK_H
#define PNDMVDDIGITASK_H

#include "PndBlackBoxTask.h"

class TClonesArray;

class PndMvdDigiTask : public PndBlackBoxTask
{
 public:

    /** Default constructor **/
    PndMvdDigiTask();

    /** Destructor **/
    virtual ~PndMvdDigiTask();
    
  void SetPersistance(Bool_t p = kTRUE);
  Bool_t GetPersistance() {return fPersistance;};
  void RunTimeBased();

 private:
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
  void Register();
  void Reset();
  void ProduceHits();
  ClassDef(PndMvdDigiTask,1);

};

#endif /* MVDDIGITASK_H */
