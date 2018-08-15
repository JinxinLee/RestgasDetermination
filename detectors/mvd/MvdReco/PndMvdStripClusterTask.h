// -------------------------------------------------------------------------
// -----                 PndMvdStripClusterTask header file             -----
// -------------------------------------------------------------------------



#ifndef PNDMVDSTRIPCLUSTERTASK_H
#define PNDMVDSTRIPCLUSTERTASK_H 1

#include "PndSdsStripClusterTask.h"

class PndMvdStripClusterTask : public PndSdsStripClusterTask
  {
  public:
    
    /** Default constructor **/
  	PndMvdStripClusterTask();
    
    /** Destructor **/
    virtual ~PndMvdStripClusterTask();
    
    virtual void GetParList(TList* tempList);
    virtual void SetParContainersMQ(TList* tempList);

    void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
    virtual void SetBranchNames();
    
    virtual void SetParContainers();
    virtual void SetCalculators();

    ClassDef(PndMvdStripClusterTask,3);
  };

#endif /* MVDCLUSTERTASK_H */

