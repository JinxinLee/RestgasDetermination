// -------------------------------------------------------------------------------
// ---------                    PndSimpleNtuple                           --------
// -------------------------------------------------------------------------------
//
// Represents a simplified N-Tuple based on a ROOT TTree.
//
// Branches of type double, float, int or bool are created on the fly with the 
// command 
//
//   'Column(TString name, double/float/int/bool value);'
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

#include "PndSimpleNtuple.h"

#include "TRegexp.h"
#include <iostream>

using std::cout;
using std::endl;

TString typearr[8] = {"D","F","I","B","D[]","F[]","I[]","B[]"};


// -------------------------------------------------------------------------------

PndSimpleNtuple::PndSimpleNtuple(TString name, TString title, TString precut) :
	fTree(new TTree(name, title)),fTmpTree(new TTree("tmp", "")), fPrecut(precut), fFml(0)
{
	fTree->SetDirectory(0);
	fTmpTree->SetDirectory(0);
};

// ----------------------------------
//          DOUBLE VALUES
// ----------------------------------
void PndSimpleNtuple::Column(const TString &name, double value)
{
	// new branch?
	if (fBrTypes.find(name) == fBrTypes.end())
	{
		fBrTypes[name] = 0;  // type double
		fDValues[name] = value;
		fTree->Branch(name.Data(), &(fDValues[name]), (name+"/D").Data());
		fTmpTree->Branch(name.Data(), &(fDValues[name]), (name+"/D").Data());
	}
	else if ( fBrTypes[name] != 0)
	{
		std::cout<<" - WARNING - Type mismatch of branch '"<<name<<"/"<<typearr[fBrTypes[name]]<<"' with type '"<<typearr[0]<<"'"<<std::endl;
		return;		
	}
	else
		fDValues[name] = value;
}

// ----------------------------------
//          FLOAT VALUES
// ----------------------------------
void PndSimpleNtuple::Column(const TString &name, float value)
{
	// new branch?
	if (fBrTypes.find(name) == fBrTypes.end())
	{
		fBrTypes[name] = 1;  // type double
		fFValues[name] = value;
		fTree->Branch(name.Data(), &(fFValues[name]), (name+"/F").Data());
		fTmpTree->Branch(name.Data(), &(fFValues[name]), (name+"/F").Data());
	}
	else if ( fBrTypes[name] != 1)
	{
		std::cout<<" - WARNING - Type mismatch of branch '"<<name<<"/"<<typearr[fBrTypes[name]]<<"' with type '"<<typearr[1]<<"'"<<std::endl;
		return;		
	}
	else
		fFValues[name] = value;
}

// ----------------------------------
//          INT VALUES
// ----------------------------------
void PndSimpleNtuple::Column(const TString &name, int value)
{
	// new branch?
	if (fBrTypes.find(name) == fBrTypes.end())
	{
		fBrTypes[name] = 2;  // type double
		fIValues[name] = value;
		fTree->Branch(name.Data(), &(fIValues[name]), (name+"/I").Data());
		fTmpTree->Branch(name.Data(), &(fIValues[name]), (name+"/I").Data());
	}
	else if ( fBrTypes[name] != 2)
	{
		std::cout<<" - WARNING - Type mismatch of branch '"<<name<<"/"<<typearr[fBrTypes[name]]<<"' with type '"<<typearr[2]<<"'"<<std::endl;
		return;		
	}
	else
		fIValues[name] = value;
}

// ----------------------------------
//          BOOL VALUES
// ----------------------------------
void PndSimpleNtuple::Column(const TString &name, bool value)
{
	// new branch?
	if (fBrTypes.find(name) == fBrTypes.end())
	{
		fBrTypes[name] = 3;  // type double
		fBValues[name] = value;
		fTree->Branch(name.Data(), &(fBValues[name]), (name+"/B").Data());
		fTmpTree->Branch(name.Data(), &(fBValues[name]), (name+"/B").Data());
	}
	else if ( fBrTypes[name] != 3)
	{
		std::cout<<" - WARNING - Type mismatch of branch '"<<name<<"/"<<typearr[fBrTypes[name]]<<"' with type '"<<typearr[3]<<"'"<<std::endl;
		return;		
	}
	else
		fBValues[name] = value;
}

// ----------------------------------

double PndSimpleNtuple::GetCurrentValue(TString name)
{
	// branch does not exist
	if (fBrTypes.find(name)==fBrTypes.end()) return sqrt(-1.);
	
	int brtype = fBrTypes[name];
	
	switch (brtype) 
	{
		case 0: return fDValues[name]; 
		case 1: return (double) fFValues[name]; 
		case 2: return (double) fIValues[name]; 
		case 3: return (double) fBValues[name]; 
	}
		
	return sqrt(-1);
}

// ----------------------------------

bool PndSimpleNtuple::Accept()
{
	if (fPrecut=="") return true;
	
	if (fFml==0) fFml = new TTreeFormula("fFml", fPrecut, fTmpTree);
	
	fTmpTree->Fill();
	fTmpTree->GetEntry(fTmpTree->GetEntriesFast()-1);
	return fFml->EvalInstance();
}

// ----------------------------------

int PndSimpleNtuple::ShowBranches()
{
	int cnt=0;
	for (auto x: fBrTypes)
	{
		cout <<cnt++<<" : "<<x.first<<"/"<<typearr[x.second]<<endl;
	}
	
	return fBrTypes.size();
}

// ----------------------------------

void PndSimpleNtuple::SetPrecut(TString precut) //{ if (fFml) delete fFml; fFml = 0; if (precut!="") fFml = new TTreeFormula("fFml",precut,fTmpTree);}
{
	if (precut!=fPrecut)
	{
		if (fFml!=0) {delete fFml; fFml=0;}
		fPrecut=precut;
	}
}


