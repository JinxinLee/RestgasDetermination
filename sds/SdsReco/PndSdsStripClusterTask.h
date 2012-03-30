// -------------------------------------------------------------------------
// -----                 PndSdsStripClusterTask header file             -----
// -------------------------------------------------------------------------



#ifndef PNDSDSSTRIPCLUSTERTASK_H
#define PNDSDSSTRIPCLUSTERTASK_H

#include "PndSdsTask.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
// #include "PndSdsCluster.h"
//#include "PndSdsRecoCharge.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TH2F.h"
#include "PndSdsStripHitProducer.h"
#include "PndSdsStripClusterer.h"
#include "PndDetectorList.h" 
#include "PndSdsChargeConversion.h"


#include "FairTSBufferFunctional.h"

#include <string>
#include <vector>

class TClonesArray;
class PndSdsChargeWeightingAlgorithms;

class PndSdsStripClusterTask : public PndSdsTask
  {
  public:
    
    /** Default constructor **/
    
    PndSdsStripClusterTask(); 
    PndSdsStripClusterTask(const char* name); 
    
    /** Destructor **/
    virtual ~PndSdsStripClusterTask();

    
    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();
    
    virtual void SetInBranchId(){
 		FairRootManager *ioman = FairRootManager::Instance();
 		fInBranchId = ioman->GetBranchId(fInBranchName);
 		std::cout << "InBranchId: " << fInBranchId << " for Branch: " << fInBranchName.Data() << std::endl;
 		fClusterType = ioman->GetBranchId(fClustBranchName);
 		std::cout << "fClusterType: " << fClusterType << " for Branch: " << fClustBranchName.Data() << std::endl;

 	}

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    /** Virtual method Finish **/
    virtual void Finish();
    
    void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
    Bool_t GetPersistance() {return fPersistance;};
    
  protected:
    
	TString fPath;

    Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
    
    virtual void SetCalculators(); 
    TVector2 CalcLineCross(TVector2 point1, TVector2 dir1, TVector2 point2, TVector2 dir2) const;
    void SetCurrentCalculators(PndSdsStripDigiPar* digipar); 
    Bool_t SelectSensorParams(Int_t sensorID);
    void CalcMeanCharge(PndSdsClusterStrip* onecluster, Double_t &meanstrip, Double_t &meanerr, Double_t &charge, Double_t &timestamp, Double_t &timestampError);
    Bool_t Backmap( TVector2 meantopPoint, Double_t toperr , TVector2 meanbotPoint, Double_t boterr,
                   TVector3 &hitpos, TMatrixD &hitCov, Int_t &sensorID);
    TClonesArray* fDigiArray;  // Input array of PndSdsDigis
    TClonesArray* fClusterArray; // Output array of PndSdsClusters
    TClonesArray* fHitArray;  // Output array of PndSdsHits
    
    TString fClustBranchName;
    Int_t fClusterType;
    Int_t fEventNr;
    
    void Register();
    void Reset();
    void ProduceHits();
    void ResetClusterFinders(); 
    void FillClusterFinders();
    void ClearCalculators();
    
    Int_t fFEcolumns;
    Int_t fFErows;
    Double_t fChargeCut;
    Int_t fRadChannel;
    Int_t fRadTime;
    Double_t fSingleStripChargeThreshold;
    
    //! Digitization Parameters 
    TList* 	fDigiParameterList;
    PndSdsStripDigiPar* fCurrentDigiPar; 
    TList* fChargeDigiParameterList;
    //! Calculator objects 
    std::map<const char*,PndSdsCalcStrip*> fStripCalcTop; 
    std::map<const char*,PndSdsCalcStrip*> fStripCalcBot; 
    std::map<const char*,PndSdsChargeConversion*> fChargeConverter;
    
    PndSdsCalcStrip* fCurrentStripCalcTop; 
    PndSdsCalcStrip* fCurrentStripCalcBot; 
    PndSdsChargeWeightingAlgorithms* fChargeAlgos;
    PndSdsChargeConversion* fCurrentChargeConverter;

	  
    PndGeoHandling* fGeoH;      //! Geometry name handling
    PndSdsStripClusterer* fCurrentClusterfinder;
    std::map<const char*,PndSdsStripClusterer*> fClusterFinderList;
    
    BinaryFunctor* fFunctor;

    ClassDef(PndSdsStripClusterTask,2);
    
  private:

    TH1F* eta_rect;
    TH1F* eta_trap;
    TFile* etahistofile;

  };

#endif /* SDSCLUSTERTASK_H */

