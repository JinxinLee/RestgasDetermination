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


#ifndef PNDMVDPIXELCLUSTERTASK_H
#define PNDMVDPIXELCLUSTERTASK_H 

#include "FairTask.h"
//#include "PndMvdGeoPar.h"
#include "PndMvdHit.h"
#include "PndMvdMCPoint.h"
#include "PndMvdPixel.h"
// #include "PndMvdCluster.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndMvdHybridHitProducer.h"
#include "PndMvdStripHitProducer.h"
#include "PndMvdPixelDigiPar.h"

#include <string>
#include <vector>
 
class TClonesArray;

class PndMvdPixelClusterTask : public FairTask
{
 public:

    /** Default constructor **/  
    PndMvdPixelClusterTask();
  
    /** Destructor **/
    virtual ~PndMvdPixelClusterTask();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);


 private:
  
    
    TString fBranchName;
    /** Input array of PndMvdDigis **/
     TClonesArray* fDigiArray;
     PndMvdPixelDigiPar* fDigiPar;

  /** Output array of PndMvdHits **/
      TClonesArray* fClusterArray;
      TClonesArray* fHitArray;  
//   TClonesArray* fPixelArray;
//   TClonesArray* fFePixelArray;
  
  void Register();
  void Reset();  
  void ProduceHits();
	
  std::vector<Double_t> fParams;
  
//   TGeoHMatrix GetTransformation (std::string detName);
//   void GetLocalHitPoints(PndMvdMCPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut);
//   PndMvdHit CalcGlobalPoint(std::vector<PndMvdPixel> pixels);
//   TVector3 GetSensorDimensions(std::string detName);  

  ClassDef(PndMvdPixelClusterTask,2);

};

#endif /* MVDCLUSTERTASK_H */

