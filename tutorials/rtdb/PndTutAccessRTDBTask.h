#ifndef PNDTUTACCESSRTDBTASK_H
#define PNDTUTACCESSRTDBTASK_H


#include "FairTask.h"
#include "PndTutPar.h"

#include <string>
#include <vector>
 
class TClonesArray;

class PndTutAccessRTDBTask : public FairTask
{
 public:

  /** Default constructor **/  
 PndTutAccessRTDBTask(){};
  
  /** Destructor **/
  virtual ~PndTutAccessRTDBTask(){};


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  /** Virtual method Finish **/
  virtual void Finish();

  PndTutPar* fTutParDef;
  PndTutPar* fTutParAlt;
             
  ClassDef(PndTutAccessRTDBTask,0);
};

#endif
