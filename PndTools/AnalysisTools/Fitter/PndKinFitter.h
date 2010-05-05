#ifndef PndKinFitter_H
#define PndKinFitter_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndKinFitter			  					//
//                                                                      //
// V. Jha 2010	FZ Juelich                                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/VAbsFitter.h"
#include "PndKinFitter.h"
#include "RhoBase/TCandList.h"
#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TLorentzVector.h"
#include "TDecompLU.h"

class TCandidate;
 
class PndKinFitter : public VAbsFitter
{
public:
    PndKinFitter( const TCandidate& b) ;
    virtual ~PndKinFitter();

    void Add4MomConstraint(TLorentzVector lv);
    void AddMomConstraint(TVector3 v);
    void AddTotEConstraint(double energy);
    void AddTotMomConstraint(double momentum);
    void AddMassConstraint(double mass);
    void Fit();

    double GlobalChi2() {return fGlobChi2;}
    double  Chi2Diff() {return fChi2Diff;}
    double GetPull() {return fPull;}
    int Getdof() {return fdgf;} 
 // int GetNiter() {return nIter;}

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
  void Solve();
  void SetOutput();

  TCandList  fDaughters;

  TMatrixD al0; //original parameters
   TMatrixD al1;  //fitted parameters
  TMatrixD V_al0;//Covariance matrix for original parameters
   TMatrixD V_al1;//Covariance matrix for original parameters

  TMatrixD mD;      //Matrix of constraint derivitives
  TMatrixD mE;      // Matrix of vertex constraints
  TMatrixD md;      //Vector of evaluated constraints
  TMatrixD mPull;

  Int_t fNvar; //Number of variables (=4 for 4 vector)
  Int_t fNpar; //Number of parameters=Npart*fNvar
  Int_t fNcon; //Number of constraints
  Int_t fNc; // countNumber of constraints added
  Int_t fNiter; // Number of times Solve has been called
  Int_t fNunKnown; // Number of unknowns
  Int_t NumCon; // Number of constraints ...

  void FindAndAddGenericDaughters(TCandidate *head);

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


  double         fGlobChi2;
  double         fChi2Diff;
  double         fPull;
  int            fnDof;
  double         fchiSquare;
  int            fdgf;

  
  ClassDef(PndKinFitter,1) 
};

#endif 
