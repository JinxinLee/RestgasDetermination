// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#ifndef PNDMVDPIXELCLUSTERTASK_H
#define PNDMVDPIXELCLUSTERTASK_H 

#include "PndSdsPixelClusterTask.h"
#include "PndMvdSimplePixelClusterFinder.h"
#include "PndMvdChargeWeightedPixelMapping.h"
#include "TString.h"

class PndMvdPixelClusterTask : public PndSdsPixelClusterTask
{
 public:

    /** Default constructor **/  
    PndMvdPixelClusterTask();
  
    /** Destructor **/
    virtual ~PndMvdPixelClusterTask();

    virtual void SetParContainers();
    virtual void SetBranchNames();
    virtual void SetClusterFinder();
    virtual void SetBackMapping();
    void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
    void SetClusterType(){fClusterType = kMVDClusterPixel;}

  ClassDef(PndMvdPixelClusterTask,3);
};

#endif /* MVDCLUSTERTASK_H */

