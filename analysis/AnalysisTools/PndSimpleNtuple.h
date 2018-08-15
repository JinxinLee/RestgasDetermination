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

#include "TString.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include <iostream>
#include <map>
#include <vector>

class PndSimpleNtuple {
public:
	PndSimpleNtuple(TString name, TString title, TString precut="");
		
	~PndSimpleNtuple(){if (fFml!=0) delete fFml;}
	
	void Column(const TString &name, double value);     // sets double value of a variable 'name'
	void Column(const TString &name, float value);      // sets float  value of a variable 'name'
	void Column(const TString &name, int value);        // sets integer value of a variable 'name'
	void Column(const TString &name, bool value);       // sets bool value of a variable 'name'

	void ColumnD(const TString &name, double value) {Column(name, value);} // sets double value of a variable 'name'
	void ColumnF(const TString &name, float value)  {Column(name, value);} // sets float  value of a variable 'name'
	void ColumnI(const TString &name, int value)    {Column(name, value);} // sets integer value of a variable 'name'
	void ColumnB(const TString &name, bool value)   {Column(name, value);} // sets bool value of a variable 'name'

	//void Column(TString name, Double_t *vpointer, TString idxvar);  // sets double value array of a variable 'name', index variable 'idxvar'
	//void Column(TString name, Float_t *vpointer,  TString idxvar);  // sets float value array of a variable 'name', index variable 'idxvar'
	//void Column(TString name, Int_t *vpointer,    TString idxvar);  // sets integer value array of a variable 'name', index variable 'idxvar'
	//void Column(TString name, Bool_t *vpointer,   TString idxvar);  // sets bool value of array a variable 'name', index variable 'idxvar'

	bool DumpData() { fTree->Fill(); fTmpTree->Reset(); return true; }           // writes the current event
	bool Accept();                                                               // check whether entry fulfills cut
	bool AcceptData() {if (Accept()) {DumpData(); return true;} return false; }  // writes current event if accepted
	
	TTree* GetTree() {return fTree;}
	double GetCurrentValue(TString name);
	void SetPrecut(TString precut); 
	bool BranchExists(TString name) { return (fBrTypes.find(name)!=fBrTypes.end()); }
	int  ShowBranches();
	
private:
	TTree     *fTree;
	TTree     *fTmpTree;
	
	std::map<TString, double>  fDValues;
	std::map<TString, float>   fFValues;
	std::map<TString, int>     fIValues;
	std::map<TString, char>    fBValues;
	
	std::map<TString, int>     fBrTypes;
		
	TString      fPrecut;       // precut in TTreeFormula notation
	TTreeFormula *fFml;         // the actual formula
};

#endif
