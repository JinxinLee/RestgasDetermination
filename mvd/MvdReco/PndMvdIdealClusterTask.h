
/** PndMvdIdealClusterTask.h
 *@author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Ideal cluster finding task 
 */


#ifndef PndMvdIdealClusterTASK_H
#define PndMvdIdealClusterTASK_H 1

#include "PndSdsIdealClusterTask.h"
#include "TString.h"

class PndMvdIdealClusterTask : public PndSdsIdealClusterTask
{
 public:

    /** Default constructor **/  
    PndMvdIdealClusterTask();
    PndMvdIdealClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile);
    /** Destructor **/
    virtual ~PndMvdIdealClusterTask();


  virtual void SetParContainers();
  virtual void SetBranchNames();
  void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
  
  ClassDef(PndMvdIdealClusterTask,2);

};

#endif /* PndMvdIdealClusterTASK_H */

