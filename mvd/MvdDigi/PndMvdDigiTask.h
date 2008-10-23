
/** PndMvdDigiTask.h
 **
 **/


#ifndef PNDMVDDIGITASK_H
#define PNDMVDDIGITASK_H

#include "CbmTask.h"

class TClonesArray;

class PndMvdDigiTask : public CbmTask
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


 private:

  void Register();
  void Reset();
  void ProduceHits();
  ClassDef(PndMvdDigiTask,1);

};

#endif /* MVDDIGITASK_H */
