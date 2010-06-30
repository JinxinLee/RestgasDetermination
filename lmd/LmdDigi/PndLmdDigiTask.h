
/** PndLmdDigiTask.h
 **
 **/


#ifndef PNDLMDDIGITASK_H
#define PNDLMDDIGITASK_H

#include "FairTask.h"

class TClonesArray;

class PndLmdDigiTask : public FairTask
{
 public:

    /** Default constructor **/
    PndLmdDigiTask();


    /** Destructor **/
    virtual ~PndLmdDigiTask();


    /** Virtual method Init **/
 //   virtual void SetParContainers();
 //   virtual InitStatus Init();
 //   virtual InitStatus ReInit();

    /** Virtual method Exec **/
 //   virtual void Exec(Option_t* opt);


 private:

//  void Register();
//  void Reset();
//  void ProduceHits();
  ClassDef(PndLmdDigiTask,2);

};

#endif /* LMDDIGITASK_H */
