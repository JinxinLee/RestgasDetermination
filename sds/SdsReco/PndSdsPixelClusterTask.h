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


#ifndef PNDSDSPIXELCLUSTERTASK_H
#define PNDSDSPIXELCLUSTERTASK_H 

#include "FairTask.h"
//#include "PndSdsGeoPar.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsPixel.h"
// #include "PndSdsCluster.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndSdsHybridHitProducer.h"
#include "PndSdsStripHitProducer.h"
#include "PndSdsPixelDigiPar.h"

#include <string>
#include <vector>
 
class TClonesArray;

class PndSdsPixelClusterTask : public FairTask
{
 public:

    /** Default constructor **/  
    PndSdsPixelClusterTask();
  
    /** Destructor **/
    virtual ~PndSdsPixelClusterTask();

   /** pure virtual method SetBranchNames
   **
   ** called by Init()
   ** function to set individual branch names
   **/
   virtual void SetBranchNames()=0;

    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
    Bool_t GetPersistance() {return fPersistance;};
  
  protected:
  
    Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
  
    TString fBranchName;
    /** Input array of PndSdsDigis **/
     TClonesArray* fDigiArray;
     PndSdsPixelDigiPar* fDigiPar;

    TString fClustBranchName;
    TString fHitBranchName;
    TString fFolderName;
  /** Output array of PndSdsHits **/
      TClonesArray* fClusterArray;
      TClonesArray* fHitArray;  
//   TClonesArray* fPixelArray;
//   TClonesArray* fFePixelArray;
  
  void Register();
  void Reset();  
  void ProduceHits();

  std::vector<Double_t> fParams;
  
//   TGeoHMatrix GetTransformation (std::string detName);
//   void GetLocalHitPoints(PndSdsMCPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut);
//   PndSdsHit CalcGlobalPoint(std::vector<PndSdsPixel> pixels);
//   TVector3 GetSensorDimensions(std::string detName);  

  ClassDef(PndSdsPixelClusterTask,2);

};

#endif /* SDSCLUSTERTASK_H */

