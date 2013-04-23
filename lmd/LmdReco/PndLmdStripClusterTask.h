// -------------------------------------------------------------------------
// -----                 PndLmdStripClusterTask header file            -----
// -------------------------------------------------------------------------



#ifndef PNDLMDSTRIPCLUSTERTASK_H
#define PNDLMDSTRIPCLUSTERTASK_H

#include "PndSdsStripClusterTask.h"
#include "FairGeanePro.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
class PndLmdStripClusterTask : public PndSdsStripClusterTask
{
 public:

    /** Default constructor **/

    PndLmdStripClusterTask();


    /** Destructor **/
    virtual ~PndLmdStripClusterTask();
    /** Virtual method Init **/
    //virtual InitStatus Init();
    virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
    virtual void SetBranchNames();
    virtual void SetParContainers();
    virtual void SetCalculators();
    void SetClusterType(){fClusterType = kMVDClusterStrip;}
    TVector3 AddMSErr(TVector3 hit,TVector3 hiterr);
    virtual Bool_t Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr,
		    TVector3 &hitPos, TMatrixD &hitCov, Int_t &sensorID);
    // inline void SetyRotation(const Double_t inrot=0){fyRotation=inrot;};
    
    void Exec(Option_t* opt);
    InitStatus Init();
    PndGeoHandling* fGeoH;
    FairRun* ana;
    FairRuntimeDb* rtdb;

    void SetMSflag(bool fflag){flagMS = fflag;};
    ClassDef(PndLmdStripClusterTask,2);
  
 private:

    bool flagMS;
    //   TList* fAlignParamList;
    Double_t fPbeam;
    void combitransToLumiFrame(TVector3& hitPos);
    void rotateToLumiFrame(TVector3& hitPos);
    TMatrixD rotateToLumiFrame(TMatrixD& hitCov);
    /* void alignmentCorr(TVector3& hitPos, int sensID); */
    /* double fShiftX[32],fShiftY[32],fShiftZ[32]; */
    /* double fRotateX[32],fRotateY[32],fRotateZ[32]; */
};

#endif /* LMDCLUSTERTASK_H */

