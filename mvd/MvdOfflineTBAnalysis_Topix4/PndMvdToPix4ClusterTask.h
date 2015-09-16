// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#ifndef PNDMVDTOPIX4CLUSTERTASK_H
#define PNDMVDTOPIX4CLUSTERTASK_H

#include "PndMvdPixelClusterTask.h"
#include "PndMvdSimplePixelClusterFinder.h"
#include "PndMvdChargeWeightedPixelMapping.h"
#include "TString.h"
#include "FairRootManager.h"

class PndMvdToPix4ClusterTask : public PndMvdPixelClusterTask
{
 public:
    /** Default constructor **/  
    PndMvdToPix4ClusterTask(TString parName = "MVDPixelDigiPar", TString totParName = "MVDPixelTotDigiPar");
  
    /** Destructor **/
    virtual ~PndMvdToPix4ClusterTask();

 protected:
    virtual std::vector<PndSdsDigiPixel> ConvertAndFilter(TClonesArray* digidata);

 private:

  ClassDef(PndMvdToPix4ClusterTask,1);
};

#endif /* MVDTOPIX4CLUSTERTASK_H */

