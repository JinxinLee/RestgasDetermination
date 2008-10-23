
#ifndef PNDMVDCLUSTERTASK_H
#define PNDMVDCLUSTERTASK_H

#include "CbmTask.h"

class TClonesArray;

class PndMvdClusterTask : public CbmTask
{
 public:

    /** Default constructor **/
//     PndMvdClusterTask();

    PndMvdClusterTask(Double_t pixelRadius=1.8, Int_t stripChargecut=5000 , TString geoFile="");

    /** Destructor **/
    virtual ~PndMvdClusterTask();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);


 private:

  ClassDef(PndMvdClusterTask,1);

};

#endif /* MVDCLUSTERTASK_H */

