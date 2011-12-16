#ifndef PndKinVtxFitter_H
#define PndKinVtxFitter_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndKinVtxFitter			  					//
//                                                                      //
// Author: V. Jha 2010 				        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/VAbsFitter.h"
//#include "PndKinVtxFitter.h"
#include "RhoBase/TCandList.h"

#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TLorentzVector.h"
#include "TDecompLU.h"

class TCandidate;

class PndKinVtxFitter : public VAbsFitter
  {
  public:
    PndKinVtxFitter( const TCandidate& b) ;
    virtual ~PndKinVtxFitter();// {if(fHeadOfTree) delete fHeadOfTree;}
    void AddMassConstraint(double mass);
    void AddPointingConstraint(TVector3 pVtx);
    void Fit();
    double GlobalChi2() {return fGlobChi2;}
    double GetPull() {return fPull;}
    int Getdof() {return fdgf;} //VJ
    
  private:
    TCandList  fDaughters;
    void FindAndAddGenericDaughters(TCandidate *head);
    void FitLeaf(TCandidate *head);
    
    
    void SetMatrices();
    void ResetMatrices();
    void ReadMatrix(); 
    
    void ReadKinMatrix();
    void ReadMassKinMatrix(); 
    void ReadPointingKinMatrix(){ReadPointingKinMatrix(fHeadOfTree);};
    void ReadPointingKinMatrix(TCandidate *head);
    
    
    void Compute();
    void SetOutput(){SetOutput(fHeadOfTree);};
    void SetOutput(TCandidate *head);
    
    void TransportToVertex(TMatrixD &, TMatrixD &, TMatrixD &, TMatrixD &, TMatrixD &);
    void GetStartVtx(TVector3 *); 
    
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
    double         fGlobChi2;
    double         fChi2Diff;
    double         fPull;
    
    // VtxVertex                   _vertex;
    TMatrixD vtx_ex;
    TMatrixD vtx_st;
    
    
    double fMass;
    TVector3 fpVtx;
    
    int fMassConstraint;
    int fPointConstraint;
    
    
    int                         fnDof;
    double                      fchiSquare;
    int                         fdgf;
    
    ClassDef(PndKinVtxFitter,1) //A Kinematic Vertex Fitter
  };

#endif 
