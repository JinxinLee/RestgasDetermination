// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#ifndef PNDMVDPIXELCLUSTERTASK_H
#define PNDMVDPIXELCLUSTERTASK_H 

#include "PndSdsPixelClusterTask.h"
#include "PndMvdSimplePixelClusterFinder.h"
#include "PndMvdChargeWeightedPixelMapping.h"
#include "TString.h"
#include "FairRootManager.h"

class PndMvdPixelClusterTask : public PndSdsPixelClusterTask
{
 public:

    /** Default constructor **/  
    PndMvdPixelClusterTask(TString parName = "MVDPixelDigiPar", TString totParName = "MVDPixelTotDigiPar");
  
    /** Destructor **/
    virtual ~PndMvdPixelClusterTask();
    PndMvdPixelClusterTask(const PndMvdPixelClusterTask& ) = delete;
    PndMvdPixelClusterTask& operator=(const PndMvdPixelClusterTask& ) = delete;

    virtual void GetParList(TList* tempList);
    virtual void SetClusterFinderMQ(TList* tempList);
    virtual void SetBackMappingMQ(TList* tempList);

    virtual void SetParContainers();
    virtual void SetBranchNames();
    virtual void SetClusterFinder();
    virtual void SetBackMapping();
    virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);

 private:
    TString fParName;
    TString fTotParName;
	PndSdsTotDigiPar* fTotDigiPar;
	PndSensorNamePar* fSensorNamePar;
  ClassDef(PndMvdPixelClusterTask,3);
};

#endif /* MVDCLUSTERTASK_H */

