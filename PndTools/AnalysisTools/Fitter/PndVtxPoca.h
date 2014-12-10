#ifndef PNDVTXPOCA_H
#define PNDVTXPOCA_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndVtxPoca                                                           //
//                                                                      //
// Author: R. Kliemt 2010                                               //
// idea taken from PndKinVtxFitter::GetStartVtx()                            //
// extended for multiple tracks                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TVector3.h"

class RhoCandidate;
class RhoCandList;

class PndVtxPoca
{
  public:
    PndVtxPoca() ;
    virtual ~PndVtxPoca();
    Double_t GetPocaVtx(TVector3& vertex, RhoCandidate* composite);
    Double_t GetPocaVtx(TVector3& vertex, RhoCandList  &cands);     // KG 11/2014
  private:
    Double_t GetPoca(TVector3& vertex,RhoCandidate* a, RhoCandidate* b);
    Double_t GetPocaTwoCharged(TVector3& vertex,RhoCandidate* a, RhoCandidate* b);
    Double_t GetPocaChargedToNeutral(TVector3& vertex,RhoCandidate* a, RhoCandidate* b);
    Double_t GetPocaTwoNeutral(TVector3& vertex,RhoCandidate* a, RhoCandidate* b);

    ClassDef(PndVtxPoca,1) //A poca Vertex Finder
};

#endif
