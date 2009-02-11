
/** PndMvdIdealClusterTask.h
 *@author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Ideal cluster finding task 
 */


#ifndef PndMvdIdealClusterTASK_H
#define PndMvdIdealClusterTASK_H 

#include "FairTask.h"
#include "PndMvdGeoPar.h"
#include "PndMvdHit.h"
#include "PndMvdMCPoint.h"
#include "PndMvdPixel.h"
#include "PndMvdIdealPixelClusterFinder.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndMvdHybridHitProducer.h"
#include "PndMvdStripHitProducer.h"

#include <string>
#include <vector>
 
class TClonesArray;

class PndMvdIdealClusterTask : public FairTask
{
 public:

    /** Default constructor **/  
    PndMvdIdealClusterTask();
    PndMvdIdealClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile);
    /** Destructor **/
    virtual ~PndMvdIdealClusterTask();


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

  /** Output array of PndMvdHits **/
      TClonesArray* fClusterArray;
      TClonesArray* fHitArray;
//   TClonesArray* fPixelArray;
//   TClonesArray* fFePixelArray;
  
  void Register();
  void Reset();  
  void ProduceHits();

	
  std::vector<Double_t> fParams;
  Double_t fRadius;
  Int_t fFEcolumns;
  Int_t fFErows;
  TString fGeoFile;
  
//   TGeoHMatrix GetTransformation (std::string detName);
//   void GetLocalHitPoints(PndMvdMCPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut);
//   PndMvdHit CalcGlobalPoint(std::vector<PndMvdPixel> pixels);
//   TVector3 GetSensorDimensions(std::string detName);  

  ClassDef(PndMvdIdealClusterTask,1);

};

#endif /* PndMvdIdealClusterTASK_H */

