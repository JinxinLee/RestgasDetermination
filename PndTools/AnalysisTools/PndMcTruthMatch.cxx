//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndMcTruthMatch	                                                  //
//                                                                      //
// Provide Truth matching of complete trees.	                        //
//                                                                      //
// Author: K.Goetzen, GSI,     23.02.12                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// PndMcTruthMatch.cxx

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "PndMcTruthMatch.h"

ClassImp(PndMcTruthMatch)

void PndMcTruthMatch::SetType(TCandList &l, const TParticlePDG* pdt )
{
	for (Int_t i=0;i<l.GetLength();++i) l[i].SetType(pdt);
}

void PndMcTruthMatch::SetType(TCandList &l, const char* name )
{
	for (Int_t i=0;i<l.GetLength();++i) l[i].SetType(name);
}

void PndMcTruthMatch::SetType(TCandList &l, Int_t pdgcode )
{
	for (Int_t i=0;i<l.GetLength();++i) l[i].SetType(pdgcode);
}

Bool_t PndMcTruthMatch::MctMatch(TCandidate &c, TCandList &mct, Int_t level)
{
	Int_t nd  = c.NDaughters();
	Int_t pdg = c.PdgCode();
	
	Int_t nmct = mct.GetLength();
	
	if (0==nd) // final state particle
	{
		Int_t mcidx = c.GetMcIdx();
		if ( mcidx<0 || mcidx>=nmct) return false;
		if ( mct[mcidx].PdgCode() == pdg ) return true;
		else return false;
	}
	else  // need to match a decay tree
	{
		// check recursively whether all daughter trees match
		for (Int_t i=0; i<nd; ++i) if (!MctMatch(*(c.Daughter(i)), mct, level)) return false;   
		
		// ***
		// *** MATCH LEVEL 0: only PID of leaves are matched
		// ***
		if (0==level) return true;
		
		// fetch the mother index from the first daughter's mct match object
		Int_t dau0idx = c.Daughter(0)->GetMcIdx();
		Int_t mothidx = mct[dau0idx].GetMcMotherIdx();
		
		// check whether mother index is in range 
		if (mothidx<0 || mothidx>=nmct) return false; 
		
		// fetch #daughters from MC truth mother
		Int_t nd_of_m = mct[mothidx].NDaughters();
		
		// check whether all daughters have been reconstructed correctly
		// ******** since MCT objects don't have daughter info skipped for now !!! ********
		//if (nd!=nd_of_m) return false;

		// check whether the mothers of all daughters are the same
		for (Int_t i=1; i<nd; ++i)
		{
			Int_t dauidx = c.Daughter(i)->GetMcIdx();
			if (mct[dauidx].GetMcMotherIdx()!= mothidx) return false; 
		}
		
		// set c's mct index to that one of all daughters mother idx
		c.SetMcIdx(mothidx); 
		
		// ***
		// *** MATCH LEVEL 1: PID of the leaves and tree topology are matched
		// ***
		if (1==level) return true;
		
		// check whether all daughter's mother has correct PDG code
		if ( pdg != mct[mothidx].PdgCode()) return false; 
		
		// ***
		// *** MATCH LEVEL 2: PID of leaves, tree topology and intermediate particle types are matched
		// ***
		return true;  // c's tree matches!
	}
} 



