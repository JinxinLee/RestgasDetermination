#ifndef PNDLUMICLUSTERTASK_H_
#define PNDLUMICLUSTERTASK_H_

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"

#include "PndLumiGeoPar.h"
#include "PndLumiDigiPara.h"
#include "PndLumiDigi.h"
#include "PndLumiStrip.h"
#include "PndLumiHit.h"
#include "PndLumiTransposition.h"
#include "PndLumiCluster.h"
#include "PndLumiStripClusterBuilder.h"
#include "PndLumiSimpleStripClusterFinder.h"
#include "PndLumiStripClusterFinder.h"

#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include <map>
#include <string>
#include <vector>

class PndLumiClusterTask : public FairTask
{
 public:

    /** Default constructor **/


	PndLumiClusterTask(Int_t ClusterMod=0, Int_t RadChannel=2, Int_t RadTime=0, Int_t verb=0);

	PndLumiClusterTask(Double_t chargecut, Int_t clustermod=0, Int_t RadChannel=2,
			Int_t RadTime=0, Int_t verb=0);

    /** Destructor **/
    virtual ~PndLumiClusterTask();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    /** Virtual method Finish **/
    //virtual void Finish();

    /**Identify, for cluster multiplicity >2, left and right **/
    std::map<Int_t,Double_t>  GetLeftAndRight(std::map<Int_t,PndLumiStrip> clust);
    Double_t GetStripZeroId(SensorSide side);


  private:

    /**Calculate the reconstructed the stripID (Double_t) and the charge deposited*/
       void CalcClusterPositionAndCharge(std::vector<Int_t> &onecluster, SensorSide side, Double_t &meanstrip,
          		Double_t &meanerr, Double_t &charge);

    /**For Double-Sided Sensors this computes the intersection point of the 2
     * reconstructed lines */
    TVector2 CalcLineCross( Double_t yf, Double_t yb,  Double_t of, Double_t ob) const;


    Bool_t BackmapPoint( Double_t &meantopPoint, Double_t &meantoperr, Double_t &meanbotPoint,
    		Double_t &meanboterr, TVector3 &hitPos, TVector3 &hitErr, TString &detname);


    TClonesArray* fDigiArray;  // Input array of PndLumiDigi
    TClonesArray* fClusterArray; // Output array of PndLumiCluster
    TClonesArray* fHitArray;  // Output array of PndLumiHit

    PndLumiDigiPara* fDigiPar;     //! Digitization Parameters
    PndLumiDigiPara* fCurrentDigiPar;  //! Digitization Parameters


    PndLumiGeoPar* fGeoPar;

    PndLumiStripClusterBuilder* fClusterfinder;

    Double_t fChargeCut;//charge difference threshold

    // parameters for cluster finder:
	Int_t fRadChannel;
	Int_t fRadTime;

	Int_t fClusterMod;					/// selected clusterfinder 1...simple 0...default

    //digitization pamameters
	Double_t fPitch, fOrient_front,	fOrient_back,fSigma;
    TVector2 fStripZeroFrontPos, fStripZeroBackPos;
    Int_t fVerbose;

  ClassDef(PndLumiClusterTask,2);
 };

#endif /* PNDLUMICLUSTERTASK_H_ */

