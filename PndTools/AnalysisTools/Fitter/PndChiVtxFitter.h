#ifndef PndChiVtxFitter_H
#define PndChiVtxFitter_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndChiVtxFitter                //
//                                                                      //
// Author: V. Jha, FZ Juelich 2010                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/RhoFitterBase.h"
#include "PndKinVtxFitter.h"
#include "RhoBase/RhoCandList.h"
#include "RhoCalculationTools.h"

#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TLorentzVector.h"
#include "TDecompLU.h"

class RhoCandidate;

class PndChiVtxFitter : public RhoFitterBase
{
  public:
    PndChiVtxFitter( RhoCandidate* b) ;
    virtual ~PndChiVtxFitter();// {if(fHeadOfTree) delete fHeadOfTree;}
//   void AddMassConstraint(double mass);
//   void AddPointingConstraint(TVector3 pVtx);
    void SetIterations(Int_t niter){fMaxIter=niter;};

  private:
    RhoCandList fDaughters;
    RhoCandidate* fCurrentNode;
    
    void FitNode(RhoCandidate* b);

    void SetMatrices();
    void ResetMatrices();
    void ReadMatrix();

    void Compute();
    void SetOutput();
    void GetStartVtx(TVector3*);  //VJ
    bool FindVertex();


    TMatrixD al0; //original parameters
    TMatrixD V_al0;//Covariance matrix for original parameters

    Int_t fNvar; //Number of variables (=4 for 4 vector)
    Int_t fNpar; //Number of parameters=Npart*fNvar
    Int_t fNpart; //Number of particles


    Int_t fMaxIter; // Maximum number of times Solve has to be called
    Int_t fNIter; // Number of times Solve has been called


    TMatrixD xxCov;
    TMatrixD ppCov;
    TMatrixD xpCov;

    TMatrixD vtxPos;
    TMatrixD vtxMom;
    double   vtxEnergy;


    // matrices obtained from calculation


    ClassDef(PndChiVtxFitter,1) //A Chi^2 Vertex Fitter (Based on Babar VtxFitter)
};

#endif
