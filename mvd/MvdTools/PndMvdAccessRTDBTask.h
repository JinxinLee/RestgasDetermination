#ifndef PNDMVDACCESSRTDBTASK_H
#define PNDMVDACCESSRTDBTASK_H


#include "FairTask.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsStripDigiPar.h"

#include <string>
#include <vector>

class TClonesArray;

class PndMvdAccessRTDBTask : public FairTask
{
 public:

  /** Default constructor **/
  PndMvdAccessRTDBTask():
    fPixelDigiPar(NULL),
    fStripDigiParRect(NULL),
    fStripDigiParTrap(NULL)
  {};

  /** Destructor **/
  virtual ~PndMvdAccessRTDBTask(){};
  
  PndMvdAccessRTDBTask(const PndMvdAccessRTDBTask& ) = delete;
  
  PndMvdAccessRTDBTask& operator=(const PndMvdAccessRTDBTask& ) = delete;

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  /** Virtual method Finish **/
  virtual void Finish();

private:
  PndSdsPixelDigiPar* fPixelDigiPar;
  PndSdsStripDigiPar* fStripDigiParRect;
  PndSdsStripDigiPar* fStripDigiParTrap;

  ClassDef(PndMvdAccessRTDBTask,1);
};

#endif
