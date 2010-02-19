
#ifndef PNDMVDCLUSTERTASK_H
#define PNDMVDCLUSTERTASK_H

#include "FairTask.h"

class TClonesArray;

class PndMvdClusterTask : public FairTask
{
 public:

    /** Default constructor **/
//     PndMvdClusterTask();

    PndMvdClusterTask();

    /** Destructor **/
    virtual ~PndMvdClusterTask();


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
  
  ClassDef(PndMvdClusterTask,2);

};

#endif /* MVDCLUSTERTASK_H */

