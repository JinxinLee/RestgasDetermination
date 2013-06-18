//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndMcTruthMatch                                                    //
//                                                                      //
// Provide Truth matching of complete trees.                          //
//                                                                      //
// Author: K.Goetzen, GSI,     23.02.12                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// PndMcTruthMatch.h

#ifndef PNDMCTRUTHMATCH_H
#define PNDMCTRUTHMATCH_H

#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoCandList.h"
#include "PndPidCandidate.h"

class TParticlePDG;

class PndMcTruthMatch
{
  public:
    // contstructor
    PndMcTruthMatch() {};
    // destructor
    ~PndMcTruthMatch() {};
    
    Bool_t MctMatch(RhoCandidate& c, RhoCandList& mct, Int_t level = 2, bool verbose=false){ MctMatch(&c,mct,level,verbose); };
    Bool_t MctMatch(RhoCandidate* c, RhoCandList& mct, Int_t level = 2, bool verbose=false);

    ClassDef(PndMcTruthMatch,1);
};

#endif
