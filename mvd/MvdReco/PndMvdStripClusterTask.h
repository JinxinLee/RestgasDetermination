// -------------------------------------------------------------------------
// -----                 PndMvdStripClusterTask header file             -----
// -------------------------------------------------------------------------



#ifndef PNDMVDSTRIPCLUSTERTASK_H
#define PNDMVDSTRIPCLUSTERTASK_H

#include "FairTask.h"
//#include "PndMvdGeoPar.h"
#include "PndMvdStripDigiPar.h"
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
#include "TH2F.h"
#include "PndMvdHybridHitProducer.h"
#include "PndMvdStripHitProducer.h"
#include "PndMvdStripClusterBuilder.h"

#include <string>
#include <vector>

class TClonesArray;
class PndMvdChargeWeightingAlgorithms;

class PndMvdStripClusterTask : public FairTask
{
 public:

    /** Default constructor **/


	PndMvdStripClusterTask(Int_t ClusterMod=0, Int_t RadChannel=2, Int_t RadTime=0);

	PndMvdStripClusterTask(Double_t chargecut, TString geofile, Int_t meanalgo=0, Int_t clustermod=0, Int_t RadChannel=2, Int_t RadTime=0);

    /** Destructor **/
    virtual ~PndMvdStripClusterTask();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    /** Virtual method Finish **/
    virtual void Finish();


  private:
    void SetCalculators();
    TVector2 CalcLineCross(TVector2 point1, TVector2 dir1, TVector2 point2, TVector2 dir2) const;
    Bool_t SelectSensorParams(TString detname);
    void CalcMeanCharge(std::vector<Int_t> &onecluster, Double_t &meanstrip, Double_t &meanerr, Double_t &charge);
    Bool_t Backmap( TVector2 meantopPoint, Double_t toperr , TVector2 meanbotPoint, Double_t boterr,
    	         	TVector3 &hitpos, TVector3 &hiterr, TString &detname);
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
    Int_t fMeanAlgo;
	Int_t fRadChannel;
	Int_t fRadTime;

  //! Digitization Parameters
  TList* fDigiParameterList;
  PndMvdStripDigiPar* fCurrentDigiPar;
  
  //! Calculator objects
  std::map<const char*,PndMvdCalcStrip*> fStripCalcTop;
  std::map<const char*,PndMvdCalcStrip*> fStripCalcBot;
  PndMvdCalcStrip* fCurrentStripCalcTop;
  PndMvdCalcStrip* fCurrentStripCalcBot;
//  PndMvdStripDigiPar* fDigiParRect;     //! Digitization Parameters (Barrel)
//    PndMvdStripDigiPar* fDigiParTrap;     //! Digitization Parameters (Disks)
//    PndMvdStripDigiPar* fCurrentDigiPar;  //! Digitization Parameters
//    //! Calculator objects
//    PndMvdCalcStrip* fStripCalcTopRect;
//    PndMvdCalcStrip* fStripCalcBotRect;
//    PndMvdCalcStrip* fStripCalcTopTrap;
//    PndMvdCalcStrip* fStripCalcBotTrap;
//    PndMvdCalcStrip* fStripCalcTop;
//    PndMvdCalcStrip* fStripCalcBot;

    Int_t fClusterMod;					/// selected clusterfinder 1...simple 0...default

    PndMvdGeoHandling* fGeoH;      //! Geometry name handling
  //  PndMvdGeoPar* fGeoPar;
    PndMvdChargeWeightingAlgorithms* fChargeAlgos;
    PndMvdStripClusterBuilder* fClusterfinder;
//     TH1F* fHChgDiff;
//     TH1F* fHChgMC;
//     TH1F* fHChgFake;
//     TH1F* fHChgGhost;

  ClassDef(PndMvdStripClusterTask,1);

};

#endif /* MVDCLUSTERTASK_H */

