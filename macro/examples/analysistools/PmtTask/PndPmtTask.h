#ifndef PNDPMTTASK_H
#define PNDPMTTASK_H 1
#include <cmath>

#include "TH1F.h"
#include "TH2F.h"
#include "TVector3.h"
#include "TParticlePDG.h"
#include "TClonesArray.h"
#include "TList.h"
#include "TStopwatch.h"

//#include "FairTask.h"

#include "PndPmtPoormantracks.h"

class PndPmtTask : public PndPmtPoormantracks// : public FairTask
{

  public:

    /** Default constructor **/
    PndPmtTask();

    /** Destructor **/
    ~PndPmtTask();

    /** Virtual method Init **/
    //virtual InitStatus
    void   Init();

    /** Virtual method Exec **/
    //virtual
    void Exec(Option_t* opt);

    //virtual
    void Finish();


  protected:

    TList* fHistoList;

    TH1F *fHVtxDiffX ,*fHVtxDiffY ,*fHVtxDiffZ ,
    *fHVtxDiffPX,*fHVtxDiffPY,*fHVtxDiffPZ,*fHVtxDiffE,
    *fHVtxPullPX,*fHVtxPullPY,*fHVtxPullPZ,*fHVtxPullE,
    *fHVtxDiffThe,*fHVtxDiffPhi;

    // POCA
    TH1F *fHVtxPocaX ,*fHVtxPocaY ,*fHVtxPocaZ ,
    *fHVtxPullPocaX,*fHVtxPullPocaY,*fHVtxPullPocaZ,
    *fHVtxPocas ,*fHVtxPocaEmpty;

    TH2F *fHVtxPocaXY,*fHVtxPocaRZ;

    // FAST FIT
    TH1F *fHVtxFastX,*fHVtxFastY,*fHVtxFastZ,
    *fHVtxErrFastX,*fHVtxErrFastY,*fHVtxErrFastZ,
    *fHVtxPullFastX,*fHVtxPullFastY,*fHVtxPullFastZ,
    *fHVtxChi2Fast,*fHVtxChiProbFast;

    TH2F *fHVtxFastXY,*fHVtxFastRZ;

    // FULL FIT
    TH1F *fHVtxFitX,*fHVtxFitY,*fHVtxFitZ,
    *fHVtxErrFitX,*fHVtxErrFitY,*fHVtxErrFitZ,
    *fHVtxPullFitX,*fHVtxPullFitY,*fHVtxPullFitZ,
    *fHVtxDiffFitPX,*fHVtxDiffFitPY,*fHVtxDiffFitPZ,*fHVtxDiffFitE,
    *fHVtxDiffFitThe,*fHVtxDiffFitPhi,
    *fHVtxPullFitPX,*fHVtxPullFitPY,*fHVtxPullFitPZ,*fHVtxPullFitE,
    *fHVtxChi2Fit,*fHVtxChiProbFit;

    TH2F *fHVtxFitXY, *fHVtxFitRZ;


    // KIN Fit
    TH1F *fHVtxKinX,*fHVtxKinY,*fHVtxKinZ,
    *fHVtxErrKinX,*fHVtxErrKinY,*fHVtxErrKinZ,
    *fHVtxPullKinX,*fHVtxPullKinY,*fHVtxPullKinZ,
    *fHVtxDiffKinPX,*fHVtxDiffKinPY,*fHVtxDiffKinPZ,*fHVtxDiffKinE,
    *fHVtxDiffKinThe,*fHVtxDiffKinPhi,
    *fHVtxPullKinPX,*fHVtxPullKinPY,*fHVtxPullKinPZ,*fHVtxPullKinE,
    *fHVtxChi2Kin,*fHVtxChiProbKin;

    TH2F *fHVtxKinXY,*fHVtxKinRZ;

    //Helix Params
    TH1F *fHPrgPull0,*fHPrgPull1,*fHPrgPull2,*fHPrgPull3,*fHPrgPull4;

    TH1D *fHCpu;

    TStopwatch fSwAll, fSwPMT, fSwPoca, fSwPrgfast, fSwPrgfull, fSwKin;


  private:

    /** Get parameter containers **/
    virtual void SetParContainers();



    ClassDef(PndPmtTask,1);

};

#endif
