// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#ifndef PNDLMDPIXELCLUSTERTASK_H
#define PNDLMDPIXELCLUSTERTASK_H 

#include "PndSdsPixelClusterTask.h"
#include "PndLmdSimplePixelClusterFinder.h"
#include "PndLmdChargeWeightedPixelMapping.h"
#include "TString.h"
#include "FairRootManager.h"

class PndLmdPixelClusterTask : public PndSdsPixelClusterTask
{
 public:

    /** Default constructor **/  
    PndLmdPixelClusterTask();
  
    /** Destructor **/
    virtual ~PndLmdPixelClusterTask();

    virtual void SetParContainers();
    virtual void SetBranchNames();
    virtual void SetClusterFinder();
    virtual void SetBackMapping();
    virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);

  ClassDef(PndLmdPixelClusterTask,3);
};

#endif /* LMDCLUSTERTASK_H */

