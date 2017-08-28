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

#include "PndSimpleNtuple.h"

#include "TRegexp.h"
#include <iostream>

using std::cout;
using std::endl;

// -------------------------------------------------------------------------------

PndSimpleNtuple::PndSimpleNtuple(TString name, TString title, TString precut, int maxsize) :
	fTree(new TTree(name, title)), fDSize(0), fFSize(0), fISize(0), fMax(maxsize),
	fPrecut(precut), fIsConfigured(false)
{
	fTree->SetDirectory(0);
	
	fDValues  = new Double_t[maxsize];
	fFValues  = new Float_t[maxsize];
	fIValues  = new Int_t[maxsize];
	
	fBranchMap.clear();
};

// ----------------------------------

void PndSimpleNtuple::Column(TString name, Double_t value)
{
	// if existing, should be 0 <= bridx < fMax for double
	int bridx = BranchIndex(name);
	
	if (bridx<0)
	{
		if (fDSize<fMax) 
		{
			fTree->Branch(name.Data(),&(fDValues[fDSize]),(name+"/D").Data());
			bridx = fDSize++;
			fBranchMap[name] = bridx;
		}
		else
		{
			std::cout <<" - WARNING - max size reached, branch '"<<name.Data()<<"/D' is not created!"<<std::endl;
			return;
		}
	}
	//wrong type
	if (bridx >= fMax)
	{
		std::cout<<" - WARNING - Type mismatch of branch "<<name<<"!"<<std::endl;
		return;
	}
	
	fDValues[bridx] = value;
}

// ----------------------------------

void PndSimpleNtuple::Column(TString name, Float_t value)
{
	// if existing, should be fMax <= bridx < 2*fMax for float
	int bridx = BranchIndex(name);
	
	if (bridx<0)
	{
		if (fFSize<fMax) 
		{
			fTree->Branch(name.Data(),&(fFValues[fFSize]),(name+"/F").Data());
			bridx = fMax + fFSize++;
			fBranchMap[name] = bridx;
		}
		else
		{
			std::cout <<" - WARNING - max size reached, branch '"<<name.Data()<<"/F' is not created!"<<std::endl;
			return;
		}
	}
	
	//wrong type
	if (bridx < fMax || bridx >= 2*fMax)
	{
		std::cout<<" - WARNING - Type mismatch of branch "<<name<<"!"<<std::endl;
		return;
	}
	
	fFValues[bridx-fMax] = value;
}

// ----------------------------------

void PndSimpleNtuple::Column(TString name, Int_t value)
{
	// if existing, should be 2*fMax <= bridx < 3*fMax for int
	int bridx = BranchIndex(name);
	
	if (bridx<0)
	{
		if (fISize<fMax) 
		{
			fTree->Branch(name.Data(),&(fIValues[fISize]),(name+"/I").Data());
			bridx = 2*fMax + fISize++;
			fBranchMap[name] = bridx;
		}
		else
		{
			std::cout <<" - WARNING - max size reached, branch '"<<name.Data()<<"/I' is not created!"<<std::endl;
			return;
		}
	}
	
	//wrong type
	if (bridx < 2*fMax)
	{
		std::cout<<" - WARNING - Type mismatch of branch "<<name<<"!"<<std::endl;
		return;
	}
	
	fIValues[bridx-2*fMax] = value;
}


// ----------------------------------

Double_t PndSimpleNtuple::GetCurrentValue(TString name)
{
	int bridx = BranchIndex(name);
	
	// branch doesn't exist
	if (bridx<0) return sqrt(-1);
	
	// double value
	if (bridx<fMax) return fDValues[bridx];

	// float value
	else if (bridx<2*fMax) return (Double_t) fFValues[bridx-fMax];

	// int value
	else return (Double_t) fIValues[bridx-2*fMax];
}

// ----------------------------------

bool PndSimpleNtuple::Accept()
{
	if (fIsConfigured && fFml.GetNpar()==0) return true;
	
	if (!fIsConfigured)
	{
		fIsConfigured = true;
		fPrecutVars.clear();
		fFml.Clear();
		
		if (fPrecut=="") return true;
		
		std::vector<TString> svec;
		TRegexp rvar("[a-zA-Z_][a-zA-Z0-9_]*");
		
		TString tmp=fPrecut, mtch=tmp(rvar);

		while (mtch!="") 
		{
			tmp.ReplaceAll(mtch,"");
			if (BranchExists(mtch))
			{
				fPrecutVars.push_back(mtch);		
			}
			mtch = tmp(rvar);
		}
		tmp = fPrecut;
		for (uint i=0;i<fPrecutVars.size();++i) tmp.ReplaceAll(fPrecutVars[i],TString::Format("[%d]",i));
		if (fPrecutVars.size()>0 && fFml.Compile(tmp)==0) fFml.SetTitle(""); else fFml.Clear();
		cout <<"PndSimpleNtuple - TFormula : "<<fFml.GetExpFormula()<<endl;
	}	
	
	for (uint i=0;i<fPrecutVars.size();++i) fFml.SetParameter(i,GetCurrentValue(fPrecutVars[i]));
	
	if (fFml.Eval(0)) return true;
	
	return false;
}

