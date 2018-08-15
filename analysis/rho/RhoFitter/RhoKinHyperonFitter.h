#ifndef RhoKinHyperonFitter_H
#define RhoKinHyperonFitter_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoKinHyperonFitter                 //
//                                                                      //
// V. Jha 2010  FZ Juelich                                              //
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

class RhoKinHyperonFitter : public RhoFitterBase
{
  public:
    RhoKinHyperonFitter( RhoCandidate* b) ;
    virtual ~RhoKinHyperonFitter();

    void Add4MomConstraint(TLorentzVector lv);
    void AddMomConstraint(TVector3 v);
    void AddTotEConstraint(double energy);
    void AddTotMomConstraint(double momentum);
    void AddMassConstraint(double mass);
    Bool_t Fit();
    Bool_t FitAll() {Fatal("RhoKinHyperonFitter::FitAll()","Not applicable. Please use Fit()."); return kFALSE;};
    void SetInputMatrix(TMatrixD M1,TMatrixD V1);

    double Chi2Diff() {return fChi2Diff;}
    double GetPull() {return fPull;}
    double Chi2Contribution ( const RhoCandidate& ) const {Error("RhoKinHyperonFitter::Chi2Contribution","No chi2 contribution available.");return -9999.;};

  private:
    void SetMatrices();
    void ZeroMatrices();
    void ReadMatrix();
    void ReadKinMatrix();
    void ReadMassKinMatrix();
    void Read4MomKinMatrix();
    void ReadMomKinMatrix();
    void ReadTotEKinMatrix();
    void ReadTotMomKinMatrix();
    Bool_t Solve();
    void SetOutput();

    TMatrixD fAl0; //original parameters
    TMatrixD fAl1;  //fitted parameters
    TMatrixD fV_al0;//Covariance matrix for original parameters
    TMatrixD fV_al1;//Covariance matrix for original parameters

    TMatrixD fmD;      //Matrix of constraint derivitives
    TMatrixD fmE;      // Matrix of vertex constraints
    TMatrixD fmd;      //Vector of evaluated constraints
    TMatrixD fmPull;

    Int_t fNvar; //Number of variables (=4 for 4 vector)
    Int_t fNpar; //Number of parameters=Npart*fNvar
    Int_t fNcon; //Number of constraints
    Int_t fNc; // countNumber of constraints added
    Int_t fNiter; // Number of times Solve has been called
    Int_t fNunKnown; // Number of unknowns
    Int_t fNumCon; // Number of constraints ...

    double fMass;
    TLorentzVector flmm;
    TVector3 fmm;
    double fEc;
    double fMom;

    int f4MomConstraint;
    int fMomConstraint;
    int fTotEConstraint;
    int fMassConstraint;
    int fTotMomConstraint;

    double         fChi2Diff;
    double         fPull;


    ClassDef(RhoKinHyperonFitter,1)
};

#endif
