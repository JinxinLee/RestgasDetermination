//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndMcTruthMatch	                                                  //
//                                                                      //
// Provide Truth matching of complete trees.	                        //
//                                                                      //
// Author: K.Goetzen, GSI,     23.02.12                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// PndMcTruthMatch.h

#ifndef PNDMCTRUTHMATCH_H
#define PNDMCTRUTHMATCH_H

#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"

class TParticlePDG;

class PndMcTruthMatch
{
public:
	// contstructor
 	PndMcTruthMatch(){};
	// destructor
	~PndMcTruthMatch(){};
    
    // Set the type for a TCandList
    void   SetType(TCandList &l, const TParticlePDG* pdt );
    void   SetType(TCandList &l, const char* name );
    void   SetType(TCandList &l, int pdgcode );

	Bool_t MctMatch(TCandidate &c, TCandList &mct, Int_t level = 2); 
  	
  	ClassDef(PndMcTruthMatch,1);	
};

#endif
