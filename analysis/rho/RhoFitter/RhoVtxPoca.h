#ifndef RHOVTXPOCA_H
#define RHOVTXPOCA_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoVtxPoca                                                           //
//                                                                      //
// Author: R. Kliemt 2010                                               //
// extended for multiple tracks                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TVector3.h"

class RhoCandidate;
class RhoCandList;

class RhoVtxPoca
{
  public:
    RhoVtxPoca() ;
    virtual ~RhoVtxPoca();
    Double_t GetPocaVtx(TVector3& vertex, RhoCandidate* composite);
    Double_t GetPocaVtx(TVector3& vertex, RhoCandList  &cands);     // KG 11/2014
  private:
    Double_t GetPoca(TVector3& vertex,RhoCandidate* a, RhoCandidate* b);
    Double_t GetPocaTwoCharged(TVector3& vertex,RhoCandidate* a, RhoCandidate* b);
    Double_t GetPocaChargedToNeutral(TVector3& vertex,RhoCandidate* a, RhoCandidate* b);
    Double_t GetPocaTwoNeutral(TVector3& vertex,RhoCandidate* a, RhoCandidate* b);

    ClassDef(RhoVtxPoca,1) //A poca Vertex Finder
};

#endif
