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
	PndSimpleNtuple(TString name, TString title, TString precut="", int maxsize = 1000);
		
	~PndSimpleNtuple(){delete fTree; delete[] fDValues; delete[] fFValues; delete[] fIValues; }
	
	void Column(TString name, Double_t value);     // sets double value of a variable 'name'
	void Column(TString name, Float_t value);      // sets float  value of a variable 'name'
	void Column(TString name, Int_t value);        // sets integer value of a variable 'name'
	void Column(TString name, Bool_t value);       // sets bool value of a variable 'name'

	void ColumnD(TString name, Double_t value) {Column(name, value);} // sets double value of a variable 'name'
	void ColumnF(TString name, Float_t value)  {Column(name, value);} // sets float  value of a variable 'name'
	void ColumnI(TString name, Int_t value)    {Column(name, value);} // sets integer value of a variable 'name'
	void ColumnB(TString name, Bool_t value)   {Column(name, value);} // sets bool value of a variable 'name'

	void Column(TString name, Double_t *vpointer, TString idxvar);  // sets double value array of a variable 'name', index variable 'idxvar'
	//void Column(TString name, Float_t *vpointer,  TString idxvar);  // sets float value array of a variable 'name', index variable 'idxvar'
	//void Column(TString name, Int_t *vpointer,    TString idxvar);  // sets integer value array of a variable 'name', index variable 'idxvar'
	//void Column(TString name, Bool_t *vpointer,   TString idxvar);  // sets bool value of array a variable 'name', index variable 'idxvar'

	void DumpData() { fTree->Fill(); }             // writes the current event
	bool Accept();                                 // check whether entry fulfills cut
	void AcceptData() {if (Accept()) DumpData();}  // writes current event if accepted
	
	TTree* GetTree() {return fTree;}
	Double_t GetCurrentValue(TString name);
	void SetPrecut(TString precut) { fPrecut = precut; fIsConfigured=false; }
	int  BranchIndex(TString name) { if (fBranchMap.find(name) != fBranchMap.end()) return fBranchMap[name]; return -1; }
	bool BranchExists(TString name) { return (BranchIndex(name)>=0);}
	int  ShowBranches();
	
private:
	TTree     *fTree;
	
	Double_t  *fDValues;     // array with double values
	Float_t   *fFValues;     // array with float values
	Int_t     *fIValues;     // array with int values
	Char_t    *fBValues;     // array with bool values
	
	//Double_t  **fDPointers;  // array with double pointers (for array branches)
	//Float_t   **fFPointers;  // array with float pointers (for array branches)
	//Int_t     **fIPointers;  // array with int pointers (for array branches)
	//Char_t    **fBPointers;  // array with bool pointers (for array branches)
	
	Int_t     fSize[8];      // current sizes of arrays
	
	int       fGrowBuf;      // initial size and amount by which each array grows if hitting the current limit
	int       fMax[8];       // current limits of array sizes
	
	TString   fPrecut;       // precut in TTreeFormula notation
	TFormula  fFml;          // the actual formula
	bool      fIsConfigured; // flag, whether the SimpleNutple is already configured; reset if precut is changed
	
	std::map<TString, Int_t> fBranchMap;  // mapping of branch names to index position
		
	std::vector<TString>     fPrecutVars; // internal vector of names of variables in precut
};

#endif
