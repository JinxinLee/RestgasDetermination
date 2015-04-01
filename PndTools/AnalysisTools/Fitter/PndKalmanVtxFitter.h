#ifndef PNDKALMANVTXFITTER_H
#define PNDKALMANVTXFITTER_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndKalmanVtxFitter                                                   //
//                                                                      //
// Author: R. Kliemt 2010                                               //
// Vertex finder & fitter using track separation and the perigee helix  //
// approximation                                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "RhoBase/RhoFitterBase.h"

#include "RhoBase/RhoCandList.h"
#include "TVector3.h"

class RhoCandidate;

class PndKalmanVtxFitter : public RhoFitterBase
{
  public:
    PndKalmanVtxFitter( RhoCandidate* b) ;
    virtual ~PndKalmanVtxFitter();
    void SetExpansionPoint(TVector3 P) {fExpansionPoint=P;}
    void SetNIterations(int i) {fNIterations=i;};

    double FitVertexFast(TVector3& vtx, TMatrixD& cov, bool skipcov=false, int niter=2);
    //double FitVertexFull(TVector3& vtx, TMatrixD& cov);

    void SetDebug(bool db = true) {if(db) { fVerbose=true; } fDebug=db;};
    void SetSilent() {fVerbose=false; fDebug=false;};
  private:
    Bool_t FitNode(RhoCandidate* b);
    RhoCandidate* fCurrentHead;

    Bool_t Calculate();
    Bool_t CalcPrgParams(RhoCandidate* cand,TVector3 expansionpoint); // calculate helix and its covariance
    Bool_t CalcPrgParams(RhoCandidate* cand) {return CalcPrgParams(cand,fExpansionPoint);}; // calculate helix and its covariance
    //double CalculateVertexFast(Bool_t skpcov=false);
    bool fDebug;
    int fNIterations;
    TVector3 fExpansionPoint; // Expansion point
    Float_t fPrgParams[5]; // Helix Paramterization around expansion point of each track
    TMatrixD fPrgCov; // Covariance matrix 5x5 of each track
    TMatrixD fJacobian; // Jacobian 5x7
    //TClonesArray* fTrackArray;
    ClassDef(PndKalmanVtxFitter,1)
};

#endif
