// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#ifndef PNDLMDPIXELCLUSTERTASK_H
#define PNDLMDPIXELCLUSTERTASK_H 

#include "PndSdsPixelClusterTask.h"
#include "PndLmdSimplePixelClusterFinder.h"
#include "PndLmdChargeWeightedPixelMapping.h"
#include "TString.h"
#include "FairRootManager.h"
#include "PndLmdAlignPar.h"
#include "PndSdsClusterPixel.h"
#include "PndLmdDim.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
class PndLmdPixelClusterTask : public PndSdsPixelClusterTask
{
 public:

    /** Default constructor **/  
    PndLmdPixelClusterTask();
  
    /** Destructor **/
    virtual ~PndLmdPixelClusterTask();

    virtual void SetParContainers();
    virtual void SetBranchNames();
    virtual void SetClusterFinder();
    virtual void SetBackMapping();
    virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
    /** Virtual method Exec **/
    void Exec(Option_t* opt);
    FairRun* ana;
    FairRuntimeDb* rtdb;
    void SetMSflag(bool fflag){flagMS = fflag;};
    InitStatus Init();

    void SetMtxPath(TString _path){mtxpath = _path;};
    ClassDef(PndLmdPixelClusterTask,3);

 private:
    bool flagMS;
    double fPbeam;
    TVector3 AddMSErr(TVector3 hpos, TVector3 hposerr);
    TList* fAlignParamList;
    // void combitransToLumiFrame(TVector3& hitPos);
    // void rotateToLumiFrame(TVector3& hitPos);
    // TMatrixD rotateToLumiFrame(TMatrixD& hitCov);
    // void alignmentCorr(TVector3& hitPos, int sensID);
    void SetAlignConst();
    /* vector<double> fShiftX,fShiftY,fShiftZ; */
    /* vector<double> fRotateX,fRotateY,fRotateZ; */
    bool readAlign;
    PndLmdDim* lmddim;
    TString mtxpath;//path to file(s) with geometry matrix
};

#endif /* LMDCLUSTERTASK_H */

