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


Bool_t PndMcTruthMatch::MctMatch ( RhoCandidate* c, RhoCandList& mct, Int_t level, bool verbose )
{
  Int_t nd  = c->NDaughters();
  Int_t pdg = c->PdgCode();
  Int_t nmct = mct.GetLength();

  if ( 0==nd ) { // final state particle
    RhoCandidate* mccnd = c->GetMcTruth();
    if ( !mccnd ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected final state by nonexistent mc truth pointer");
      return false;
    }
    if ( mccnd->PdgCode() == pdg ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","accepted final state by PDG code (pdg=%i)",pdg);
      //if(verbose) std::cout<<*c<<std::endl;
      return true;
    } else {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected final state by PDG Code (pdg=%i|mcpdg=%i)",pdg, mccnd->PdgCode());
      return false;
    }
  } 
    
    // check recursively whether all daughter trees match
  for ( Int_t i=0; i<nd; i++ ) {
    if ( !MctMatch ( * ( c->Daughter ( i ) ), mct, level, verbose ) ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected composite (pdg=%i) by non-matching daughter: idau=%i",pdg,i);
      return false;
    }
  }

  // ***
  // *** MATCH LEVEL 0 reached: only PID of final state particles are matched
  // ***
  if ( 0==level ) {
    return true;
  }
  //if(verbose)std::cout<<"going Level 1"<<std::endl;
  // reset mc truth pointer
  RhoCandidate* mccnd = c->GetMcTruth();
  if(mccnd) {
    if(verbose) Warning("PndMcTruthMatch::MctMatch","Existing MC truth found. Will reset it now.");
    c->SetMcTruth(0);
    //return false;
  }
    
  // find this particle's truth in the mc decay tree
  RhoCandidate* dauzero = c->Daughter(0);
  if (!dauzero) {
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing daughter zero");
    return false;
  }
  RhoCandidate* mcdauzero = dauzero->GetMcTruth();
  if (!mcdauzero) {
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing MC truth of daughter zero");
    return false;
  }
  RhoCandidate* mcdauzeromother=mcdauzero->TheMother();
  if (!mcdauzeromother) {
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing mother of MC truth of daughter zero");
    return false;
  }
  
  //now check the tree structure:
  //  first daughter number
  if( nd != mcdauzeromother->NDaughters() ){
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by differing daughter count: cand:%i mc:%i",c->NDaughters(),mcdauzeromother->NDaughters());
    return false;
  }
  //  now if all daughters MC-Mother is the same
  for(int idau=1;idau<nd;idau++){
    // look if all daughters mc mothers are the same
    RhoCandidate* dau = c->Daughter(idau);
    if (!dau) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing daughter %i",idau);
      return false;
    }      
    RhoCandidate* mcdau = dau->GetMcTruth();
    if (!mcdau) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing MC truth of daughter %i",idau);
      //if(verbose) std::cout<<*dau<<std::endl;
      return false;
    }
    RhoCandidate* mcdaumother=mcdau->TheMother();
    if (!mcdaumother) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing mother of MC truth of daughter %i",idau);
      return false;
    }
    if(mcdaumother!=mcdauzeromother){
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by mc mother of daughter %i is different to mc mother of daughter zero -> Tree does not match",idau);
      return false;
    }
  }

  // ***
  // *** MATCH LEVEL 1: PID of the leaves and tree topology are matched
  // ***
  if ( 1==level ) {
    return true;
  }
  //if(verbose)std::cout<<"going Level 2"<<std::endl;
  // check whether all daughter's mother has correct PDG code
  if ( pdg != mcdauzeromother->PdgCode() ) {
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by nonmatching pdg code in tree (pdgcode|mcpdgcode) (%i|%i)",pdg,mcdauzeromother->PdgCode());
    return false;
  }
  // ***
  // *** MATCH LEVEL 2: PID of leaves, tree topology and intermediate particle types are matched
  // ***
  return true;  // c's tree matches!
}



