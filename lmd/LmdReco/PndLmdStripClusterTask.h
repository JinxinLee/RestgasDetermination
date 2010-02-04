// -------------------------------------------------------------------------
// -----                 PndLmdStripClusterTask header file             -----
// -------------------------------------------------------------------------



#ifndef PNDLMDSTRIPCLUSTERTASK_H
#define PNDLMDSTRIPCLUSTERTASK_H

#include "PndSdsStripClusterTask.h"

class PndLmdStripClusterTask : public PndSdsStripClusterTask
{
 public:

    /** Default constructor **/

    PndLmdStripClusterTask();


    /** Destructor **/
    virtual ~PndLmdStripClusterTask();

  virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
  virtual void SetBranchNames();
  virtual void SetParContainers();

  ClassDef(PndLmdStripClusterTask,2);

};

#endif /* LMDCLUSTERTASK_H */

