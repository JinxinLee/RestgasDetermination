
#ifndef PNDBUFFERTESTTASK_H
#define PNDBUFFERTESTTASK_H

#include "PndBlackBoxTask.h"

class TClonesArray;

class PndBufferTestTask : public PndBlackBoxTask
  {
  public:
    
    /** Default constructor **/
    //     PndBufferTestTask();
    
    PndBufferTestTask();
    
    /** Destructor **/
    virtual ~PndBufferTestTask();

virtual InitStatus Init(){};
virtual void Exec(Option_t* opt);
    
  private:
    
    ClassDef(PndBufferTestTask,1);
    
  };

#endif /* PNDBUFFERTESTTASK_H */

