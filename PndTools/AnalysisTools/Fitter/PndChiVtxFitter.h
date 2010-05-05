#ifndef PndChiVtxFitter_H
#define PndChiVtxFitter_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndChiVtxFitter		  					//
//                                                                      //
// Author: V. Jha, FZ Juelich 2010				        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/VAbsFitter.h"
#include "PndKinVtxFitter.h"
#include "RhoBase/TCandList.h"

#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TLorentzVector.h"
#include "TDecompLU.h"

class TCandidate;

class PndChiVtxFitter : public VAbsFitter
{
public:
    PndChiVtxFitter( const TCandidate& b) ;
    virtual ~PndChiVtxFitter();// {if(fHeadOfTree) delete fHeadOfTree;}
 //   void AddMassConstraint(double mass);
 //   void AddPointingConstraint(TVector3 pVtx);
    void Fit();
    double GlobalChi2() {return fGlobChi2;}
    int Getdof() {return fdgf;} //VJ

private:
  TCandList  fDaughters;
  void FindAndAddGenericDaughters(TCandidate *head);

  void SetMatrices();
  void ResetMatrices();
  void ReadMatrix(); 

  void Compute();
  void SetOutput();
  void GetStartVtx(TVector3 *); //VJ
  bool FindVertex();


  TMatrixD al0; //original parameters
  TMatrixD V_al0;//Covariance matrix for original parameters

  Int_t fNvar; //Number of variables (=4 for 4 vector)
  Int_t fNpar; //Number of parameters=Npart*fNvar
  Int_t fNpart; //Number of particles
  Int_t fNcon; //Number of constraints


  Int_t fNc; // countNumber of constraints added
  Int_t fNiter; // Number of times Solve has been called
  Int_t fNumKnown; // Number of unknowns
  Int_t NumCon; // Number of constraints ...


  TMatrixD xxCov;
  TMatrixD ppCov;
  TMatrixD xpCov;

  TMatrixD vtxPos;
  TMatrixD vtxMom;
  double   vtxEnergy;
  Int_t niter;

  // matrices obtained from calculation
  int    fdgf;
  double fGlobChi2;

  
  ClassDef(PndChiVtxFitter,1) //A Chi^2 Vertex Fitter (Based on Babar VtxFitter)
};

#endif 
