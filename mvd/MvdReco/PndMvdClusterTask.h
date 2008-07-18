// -------------------------------------------------------------------------
// -----                 CbmStsHitProducerIdel header file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsHitProducerIdeal.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** The ideal hit producer produces hits of type CbmStsMapsHit by copying
 ** the MCPoint position. The position error is set to 1 mum, much 
 ** smaller than can be obtained by any detector. Using the hits from 
 ** this HitProducer is thus equivalent to using MC information
 ** directly, but with the correct data interface.
 **/


#ifndef PNDMVDCLUSTERTASK_H
#define PNDMVDCLUSTERTASK_H 

#include "CbmTask.h"
#include "PndMvdGeoPar.h"
#include "PndMvdHit.h"
#include "PndMvdMCPoint.h"
#include "PndMvdPixel.h"
// #include "PndMvdCluster.h"
#include "CbmGeoVector.h"
#include "CbmGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndMvdHybridHitProducer.h"
#include "PndMvdStripHitProducer.h"

#include <string>
#include <vector>
 
class TClonesArray;

class PndMvdClusterTask : public CbmTask
{
 public:

    /** Default constructor **/  
    PndMvdClusterTask();
  
    PndMvdClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile = "");

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

