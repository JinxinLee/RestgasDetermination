// -------------------------------------------------------------------------------
// ---------                    PndSimpleNtuple                           --------
// -------------------------------------------------------------------------------
//
// Represents a simplified N-Tuple based on a ROOT TTree.
//
// Branches of type double, float or int are created on the fly with the command 
//
//   'Column(TString name, double/float/int value);'
//
// At the end of each event (i.e. when the N-Tuple should be filled) either
//
//   'DumpData()'    or 
//   'AcceptData()' 
//
// has to be called. The method 'AcceptData()' is a conditional fill in case a
// certain precut is fulfilled, which can be set either in the constructor with
//
//   PndSimpleNtuple(TString name, TString title, TString <precut>)
//
// or with the method
//
//   SetPrecut(TString precut)
//
// which can even be changed inbetween. The precut is a TFormula directly based
// on the branches of the TTree.
//
// -------------------------------------------------------------------------------

#ifndef PndSimpleNtuple_H
#define PndSimpleNtuple_H 1

#include "TFormula.h"
#include "TString.h"
#include "TTree.h"
#include <iostream>
#include <map>
#include <vector>

class PndSimpleNtuple {
public:
	PndSimpleNtuple(TString name, TString title, TString precut="", int maxsize = 2000);
		
	~PndSimpleNtuple(){delete fTree; delete[] fDValues; delete[] fFValues; delete[] fIValues; }
	
	void Column(TString name, Double_t value);     // sets double value of a variable 'name'
	void Column(TString name, Float_t value);      // sets float  value of a variable 'name'
	void Column(TString name, Int_t value);        // sets integer value of a variable 'name'

	void ColumnD(TString name, Double_t value) {Column(name, value);} // sets double value of a variable 'name'
	void ColumnF(TString name, Float_t value)  {Column(name, value);} // sets float  value of a variable 'name'
	void ColumnI(TString name, Int_t value)    {Column(name, value);} // sets integer value of a variable 'name'

	void DumpData() { fTree->Fill(); }             // writes the current event
	bool Accept();                                 // check whether entry fulfills cut
	void AcceptData() {if (Accept()) DumpData();}  // writes current event if accepted
	
	TTree* GetTree() {return fTree;}
	Double_t GetCurrentValue(TString name);
	void SetPrecut(TString precut) { fPrecut = precut; fIsConfigured=false; }
	int  BranchIndex(TString name) { if (fBranchMap.find(name) != fBranchMap.end()) return fBranchMap[name]; return -1; }
	bool BranchExists(TString name) { return (BranchIndex(name)>=0);}
	
private:
	TTree     *fTree;
	
	Double_t  *fDValues;
	Float_t   *fFValues;
	Int_t     *fIValues;
	
	Int_t     fDSize;
	Int_t     fFSize;
	Int_t     fISize;
	
	int       fMax;
	TString   fPrecut;
	TFormula  fFml;
	bool      fIsConfigured;
	
	std::map<TString, Int_t> fBranchMap;
		
	std::vector<TString>     fPrecutVars;
};

#endif
