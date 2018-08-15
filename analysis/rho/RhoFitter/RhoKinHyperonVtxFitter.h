#ifndef RhoKinHyperonVtxFitter_H
#define RhoKinHyperonVtxFitter_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoKinHyperonVtxFitter                  //
//                                                                      //
// Author: V. Jha 2010                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/RhoFitterBase.h"
#include "RhoBase/RhoCandList.h"

#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TLorentzVector.h"
#include "TDecompLU.h"

class RhoCandidate;

class RhoKinHyperonVtxFitter : public RhoFitterBase
{
  public:
    RhoKinHyperonVtxFitter( RhoCandidate* b) ;
    virtual ~RhoKinHyperonVtxFitter();
    void AddMassConstraint(double mass);
    //void AddPointingConstraint(TVector3 pVtx);
    double GetPull() {return fPull;}

    void SetNMaxIterations(int nit=20){fNMaxIterations=nit;fIterateExact=false;};
    void SetNIterationsExact(int nit=2){fNMaxIterations=nit;fIterateExact=true;};
    void SetMinDChisq(double m=0.001){fMinDChisq=fabs(m);};
    TMatrixD GetAl0(){return al0;}
    TMatrixD GetVal0(){return V_al0;}
    TMatrixD GetVal_composite(){return covC;}


  private:
    Bool_t FitNode(RhoCandidate* b);

    void SetMatrices();
    void ResetMatrices();
    void ReadMatrix();

    void ReadKinMatrix();
    void ReadMassKinMatrix();
    //void ReadPointingKinMatrix() {ReadPointingKinMatrix(fHeadOfTree);};
    //void ReadPointingKinMatrix(RhoCandidate* head);


    Bool_t Compute(RhoCandidate* c);
    void SetOutput(RhoCandidate* head);

    void TransportToPoca(TMatrixD&, TMatrixD&, TMatrixD&, TMatrixD&, TMatrixD&);
    void TransportToVertex(TMatrixD&, TMatrixD&, TMatrixD&, TMatrixD&, TMatrixD&);
    void GetCovariance(TMatrixD& a_cov0, TMatrixD& cov_al_x, TMatrixD& V_vtx, TMatrixD& covS);


    TMatrixD al0; //original parameters
    TMatrixD al1;  //fitted parameters
    TMatrixD V_al0;//Covariance matrix for original parameters
    TMatrixD V_al1;//Covariance matrix for original parameters
    TMatrixD covC; //Covariance matrix for composite

    TMatrixD mD;      //Matrix of constraint derivitives
    TMatrixD mE;      // Matrix of vertex constraints
    TMatrixD md;      //Vector of evaluated constraints
    TMatrixD mPull;


    Int_t fNvar; //Number of variables (=4 for 4 vector)
    Int_t fNpar; //Number of parameters=Npart*fNvar
    Int_t fNpart; //Number of particles
    Int_t fNcon; //Number of constraints


    Int_t fNc; // countNumber of constraints added
    Int_t fNiter; // Number of times Solve has been called
    Int_t fNumKnown; // Number of unknowns
    Int_t NumCon; // Number of constraints ...



    int            niter;
    double         fChi2Diff;
    double         fPull;


    // VtxVertex                   _vertex;
    TMatrixD vtx_ex;
    TMatrixD vtx_st;


    double fMass;
    //TVector3 fpVtx;

    int fMassConstraint;
    //int fPointConstraint;

    // control abortion of fit
    double fMinDChisq;
    int  fNMaxIterations;
    bool fIterateExact;


    int                         fnDof;
    double                      fchiSquare;

    ClassDef(RhoKinHyperonVtxFitter,1) //A Kinematic Vertex Fitter
};

#endif
