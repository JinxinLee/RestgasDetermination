//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndMcTruthMatch                                                    //
//                                                                      //
// Provide Truth matching of complete trees.                          //
//                                                                      //
// Author: K.Goetzen, GSI,     23.02.12                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// PndMcTruthMatch.cxx

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "PndMcTruthMatch.h"

ClassImp ( PndMcTruthMatch )


Bool_t PndMcTruthMatch::MctMatch ( RhoCandidate& c, RhoCandList& mct, Int_t level, bool verbose )
{
  Int_t nd  = c.NDaughters();
  Int_t pdg = c.PdgCode();

  Int_t nmct = mct.GetLength();

  if ( 0==nd ) { // final state particle
    Int_t mcidx = c.GetMcIdx();

    if ( mcidx<0 || mcidx>=nmct ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected final state by out-of-bound mcID (%i conflicts with [0;%i])",mcidx,nmct);
      return false;
    }

    if ( mct[mcidx].PdgCode() == pdg ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","accepted final state by PDG code (pdg=%i)",pdg);
      return true;
    } else {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected final state by PDG Code (pdg=%i|%i)",pdg, mct[mcidx].PdgCode());
      return false;
    }
  } else { // need to match a decay tree
    // check recursively whether all daughter trees match
    for ( Int_t i=0; i<nd; ++i ) if ( !MctMatch ( * ( c.Daughter ( i ) ), mct, level, verbose ) ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected composite (pdg=%i) by non-matching daughter: idau=%i",pdg,i);
      return false;
      }

    // ***
    // *** MATCH LEVEL 0: only PID of leaves are matched
    // ***
    if ( 0==level ) {
      return true;
    }

    // fetch the mother index from the first daughter's mct match object
    Int_t dau0MCidx = c.Daughter ( 0 )->GetMcIdx();

    Int_t mothidx = 0;//mct[dau0MCidx].GetMcMotherIdx();

    // check whether mother index is in range
    if ( mothidx<0 || mothidx>=nmct ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by out-of-bound mother mcID");
      return false;
    }

    // fetch #daughters from MC truth mother
    //Int_t nd_of_m = mct[mothidx].NDaughters();  //unused?

    // check whether all daughters have been reconstructed correctly
    // ******** since MCT objects don't have daughter info skipped for now !!! ********
    //if (nd!=nd_of_m) return false;

    // check whether the mothers of all daughters are the same
    for ( Int_t i=1; i<nd; ++i ) {
      Int_t dauidx = c.Daughter ( i )->GetMcIdx();

     // if ( mct[dauidx].GetMcMotherIdx() != mothidx ) {
 //       if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not matching mother ids in daughters");
      //  return false;
     // }
    }

    // set c's mct index to that one of all daughters mother idx
    c.SetMcIdx ( mothidx );

    // ***
    // *** MATCH LEVEL 1: PID of the leaves and tree topology are matched
    // ***
    if ( 1==level ) {
      return true;
    }

    // check whether all daughter's mother has correct PDG code
    if ( pdg != mct[mothidx].PdgCode() ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by nonmatching mother %i by (pdgcode|mcpdgcode) (%i|%i)",mothidx,pdg,mct[mothidx].PdgCode());
      return false;
    }

    // ***
    // *** MATCH LEVEL 2: PID of leaves, tree topology and intermediate particle types are matched
    // ***
    return true;  // c's tree matches!
  }
}



