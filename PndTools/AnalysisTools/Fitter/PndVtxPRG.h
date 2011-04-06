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

    void SetPerigee(TVector3 P) {fPerigee=P;}
    
    void SetTrackArray(TClonesArray* tcar)
    {fTrackArray = tcar; if (! fTrackArray) std::cout << "-E- PndVtxPRG::SetTrackArray(): No track array found." << std::endl; }
    
    double CalculateVertexFast(TVector3 &vtx, TMatrixD &cov);
    double CalculateVertexFull(TVector3 &vtx, TMatrixD &cov);
    
    
  private:
    TVector3 fPerigee;
   // Bool_t Propagator(int mode, TCandidate* cand, TVector3* mypoint);
    TClonesArray* fTrackArray;
    ClassDef(PndVtxPRG,1) 
  };

#endif 
