#ifndef PNDMVDACCESSRTDBTASK_H
#define PNDMVDACCESSRTDBTASK_H


#include "FairTask.h"
#include "PndMvdPixelDigiPar.h"
#include "PndMvdStripDigiPar.h"

#include <string>
#include <vector>

class TClonesArray;

class PndMvdAccessRTDBTask : public FairTask
{
 public:

  /** Default constructor **/
 PndMvdAccessRTDBTask(){};

  /** Destructor **/
  virtual ~PndMvdAccessRTDBTask(){};


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  /** Virtual method Finish **/
  virtual void Finish();

private:
  PndMvdPixelDigiPar* fPixelDigiPar;
  PndMvdStripDigiPar* fStripDigiParRect;
  PndMvdStripDigiPar* fStripDigiParTrap;

  ClassDef(PndMvdAccessRTDBTask,1);
};

#endif
