#ifndef PNDVTXPRG_H
#define PNDVTXPRG_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndVtxPRG			  		                                          			//
//                                                                      //
// Author: R. Kliemt 2010 			                              	        //
// Vertex finder & fitter using the perigee approximation               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/VAbsFitter.h"
#include "RhoBase/TCandList.h"
#include "TVector3.h"

class TCandidate;

class PndVtxPRG : public VAbsFitter
  {
  public:
    PndVtxPRG( const TCandidate& b) ;
    virtual ~PndVtxPRG();
    void Fit();

    void SetExpansionPoint(TVector3 P) {fExpansionPoint=P;}
    
    //void SetTrackArray(TClonesArray* tcar)
    //{fTrackArray = tcar; if (! fTrackArray) std::cout << "-E- PndVtxPRG::SetTrackArray(): No track array found." << std::endl; }
    
    double FitVertexFast(TVector3 &vtx, TMatrixD &cov, bool skipcov=false);
    double FitVertexFull(TVector3 &vtx, TMatrixD &cov, int niterations=1);
    
    void SetDebug(bool db = true){if(db)fVerbose=true; fDebug=db;};
    void SetSilent(){fVerbose=false; fDebug=false;};
  private:
    Bool_t Calculate(int niterations=1);
    Bool_t CalcPrgParams(TCandidate* cand,TVector3 expansionpoint); // calculate helix and its covariance
    Bool_t CalcPrgParams(TCandidate* cand){CalcPrgParams(cand,fExpansionPoint);}; // calculate helix and its covariance
    //double CalculateVertexFast(Bool_t skpcov=false);
    bool fDebug;
    TVector3 fExpansionPoint; // Expansion point
    // TODO: make std vectors from that?
    Float_t fPrgParams[5]; // Helix Paramterization around expansion point of each track
    TMatrixD fPrgCov; // Covariance matrix 5x5 of each track
    TMatrixD fJacobian; // Jacobian 5x7
    TVector3 fVertex; // current estimate
    //TClonesArray* fTrackArray;
    ClassDef(PndVtxPRG,1) 
  };

#endif 
