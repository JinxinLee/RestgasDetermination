// -------------------------------------------------------------------------
// -----                 PndMvdStripClusterTask header file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------



#ifndef PNDMVDSTRIPCLUSTERTASK_H
#define PNDMVDSTRIPCLUSTERTASK_H 

#include "CbmTask.h"
#include "PndMvdGeoPar.h"
#include "PndMvdStripDigiPar.h"
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

class PndMvdStripClusterTask : public CbmTask
{
 public:

    /** Default constructor **/  
    PndMvdStripClusterTask();

    PndMvdStripClusterTask(Double_t chargecut, TString geofile);
  
    /** Destructor **/
    virtual ~PndMvdStripClusterTask();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);


  private:

    TVector2 CalcLineCross(TVector2 point1, TVector2 dir1, TVector2 point2, TVector2 dir2);
    void SelectSensorParams(TString detname);

    TClonesArray* fDigiArray;  // Input array of PndMvdDigis 
    TClonesArray* fClusterArray; // Output array of PndMvdClusters
    TClonesArray* fHitArray;  // Output array of PndMvdHits
  
    void Register();
    void Reset();  
    void ProduceHits();

    Double_t fRadius;
    Int_t fFEcolumns;
    Int_t fFErows;
    TString fGeoFile;
	  Double_t fChargeCut;

    PndMvdStripDigiPar* fDigiParRect;     //! Digitization Parameters (Barrel)
    PndMvdStripDigiPar* fDigiParTrap;     //! Digitization Parameters (Disks)
    PndMvdStripDigiPar* fCurrentDigiPar;  //! Digitization Parameters
    //! Calculator objects
    PndMvdCalcStrip* fStripCalcTopRect;
    PndMvdCalcStrip* fStripCalcBotRect;
    PndMvdCalcStrip* fStripCalcTopTrap;
    PndMvdCalcStrip* fStripCalcBotTrap;
    PndMvdCalcStrip* fStripCalcTop;
    PndMvdCalcStrip* fStripCalcBot;

    PndMvdGeoHandling* fGeoH;      //! Geometry name handling
    PndMvdGeoPar* fGeoPar;

  ClassDef(PndMvdStripClusterTask,1);

};

#endif /* MVDCLUSTERTASK_H */

