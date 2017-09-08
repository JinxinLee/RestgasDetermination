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

#define MAXBR 1000000  // maximum number per type; 

TString typearr[8] = {"D","F","I","B","D[]","F[]","I[]","B[]"};


// -------------------------------------------------------------------------------

PndSimpleNtuple::PndSimpleNtuple(TString name, TString title, TString precut, int maxsize) :
	fTree(new TTree(name, title)), fGrowBuf(maxsize),
	fPrecut(precut), fIsConfigured(false)
{
	fTree->SetDirectory(0);
	
	fDValues  = new Double_t[fGrowBuf];
	fFValues  = new Float_t[fGrowBuf];
	fIValues  = new Int_t[fGrowBuf];
	fBValues  = new Char_t[fGrowBuf];
	
	//fDPointers  = new Double_t*[fGrowBuf];
	//fFPointers  = new Float_t*[fGrowBuf];
	//fIPointers  = new Int_t*[fGrowBuf];
	//fBPointers  = new Char_t*[fGrowBuf];
		
	// this is the portion fMax for the different arrays grows by, if hitting the current limit
	for (int i=0;i<8;++i) 
	{
		fMax[i]  = fGrowBuf;
		fSize[i] = 0;
	}
	
	fBranchMap.clear();
};

// ----------------------------------
//          DOUBLE VALUES
// ----------------------------------
void PndSimpleNtuple::Column(TString name, Double_t value)
{
	int curtype = 0; // Double_t 
	int cursize = fSize[curtype];
	int curmax  = fMax[curtype];
	
	// if existing, should be 0 <= bridx < 1e6 for double
	int bridx = BranchIndex(name);	
	int datatype = bridx>=0 ? bridx/MAXBR : curtype;
	bridx = bridx>=0 ? bridx%MAXBR : -1;
	
	// new branch to be created
	if (bridx<0)
	{
		// do we need to expand the array?
		if (cursize>=curmax)
		{
			fMax[curtype] += fGrowBuf;
			//std::cout <<" [INFO] PndSimpleNtuple : GROWING array Double_t by "<< fGrowBuf <<" to max="<<fMax[curtype]<<std::endl;
			
			// create temp array (we'll keep this and delete the old one)
			Double_t *tmpD = new Double_t[fMax[curtype]];
			for (int i=0;i<curmax;++i) tmpD[i] = fDValues[i];
			delete[] fDValues;
			fDValues = tmpD;
			
			// take over and update branch addresses
			for ( std::map<TString, Int_t>::iterator it = fBranchMap.begin(); it != fBranchMap.end(); it++ )
				if (it->second/MAXBR == curtype)
					fTree->SetBranchAddress(it->first.Data(), &(fDValues[it->second%MAXBR]));		
		}
		
		// create the new branch
		fTree->Branch(name.Data(),&(fDValues[cursize]),(name+"/D").Data());
		bridx = fSize[curtype]++;
		fBranchMap[name] = bridx + MAXBR*curtype;
	}
	
	//wrong type (no double)
	if (datatype != curtype)
	{
		std::cout<<" - WARNING - Type mismatch of branch "<<name<<" ("<<typearr[curtype]<<" -> "<<typearr[datatype]<<")!"<<std::endl;
		return;
	}
	
	fDValues[bridx] = value;
}

// ----------------------------------
//           FLOAT VALUES
// ----------------------------------
void PndSimpleNtuple::Column(TString name, Float_t value)
{
	int curtype = 1; // Float_t 
	int cursize = fSize[curtype];
	int curmax  = fMax[curtype];
	
	// if existing, should be 1e6 <= bridx < 2e6 for float
	int bridx = BranchIndex(name);
	int datatype = bridx>=0 ? bridx/MAXBR : curtype;
	bridx = bridx>=0 ? bridx%MAXBR : -1;
	
	if (bridx<0)
	{
		// do we need to expand the array?
		if (cursize>=curmax)
		{
			fMax[curtype] += fGrowBuf;
			//std::cout <<" [INFO] PndSimpleNtuple : GROWING array Double_t by "<< fGrowBuf <<" to max="<<fMax[curtype]<<std::endl;
			
			// create temp array (we'll keep this and delete the old one)
			Float_t *tmpD = new Float_t[fMax[curtype]];
			for (int i=0;i<curmax;++i) tmpD[i] = fFValues[i];
			delete[] fFValues;
			fFValues = tmpD;
			
			// take over and update branch addresses
			for ( std::map<TString, Int_t>::iterator it = fBranchMap.begin(); it != fBranchMap.end(); it++ )
				if (it->second/MAXBR == curtype)
					fTree->SetBranchAddress(it->first.Data(), &(fFValues[it->second%MAXBR]));		
		}
		
		// create the new branch
		fTree->Branch(name.Data(),&(fFValues[cursize]),(name+"/F").Data());
		bridx = fSize[curtype]++;
		fBranchMap[name] = bridx + MAXBR*curtype;
	}
	
	//wrong type (no float) 
	if (datatype != curtype)
	{
		std::cout<<" - WARNING - Type mismatch of branch "<<name<<" ("<<typearr[curtype]<<" -> "<<typearr[datatype]<<")!"<<std::endl;
		return;
	}
	
	fFValues[bridx] = value;
}

// ----------------------------------
//           INT VALUES
// ----------------------------------
void PndSimpleNtuple::Column(TString name, Int_t value)
{
	int curtype = 2; // Int_t 
	int cursize = fSize[curtype];
	int curmax  = fMax[curtype];
	
	// if existing, should be 2e6 <= bridx < 3e6 for int
	int bridx = BranchIndex(name);
	int datatype = bridx>=0? bridx/MAXBR : curtype;
	bridx = bridx>=0 ? bridx%MAXBR : -1;
	
	if (bridx<0)
	{
		// do we need to expand the array?
		if (cursize>=curmax)
		{
			fMax[curtype] += fGrowBuf;
			//std::cout <<" [INFO] PndSimpleNtuple : GROWING array Double_t by "<< fGrowBuf <<" to max="<<fMax[curtype]<<std::endl;
			
			// create temp array (we'll keep this and delete the old one)
			Int_t *tmpD = new Int_t[fMax[curtype]];
			for (int i=0;i<curmax;++i) tmpD[i] = fIValues[i];
			delete[] fIValues;
			fIValues = tmpD;
			
			// take over and update branch addresses
			for ( std::map<TString, Int_t>::iterator it = fBranchMap.begin(); it != fBranchMap.end(); it++ )
				if (it->second/MAXBR == curtype)
					fTree->SetBranchAddress(it->first.Data(), &(fIValues[it->second%MAXBR]));		
		}
		
		// create the new branch
		fTree->Branch(name.Data(),&(fIValues[cursize]),(name+"/I").Data());
		bridx = fSize[curtype]++;
		fBranchMap[name] = bridx + MAXBR*curtype;
	}
	
	//wrong type (no int) 
	if (datatype != curtype)
	{
		std::cout<<" - WARNING - Type mismatch of branch "<<name<<" ("<<typearr[curtype]<<" -> "<<typearr[datatype]<<")!"<<std::endl;
		return;
	}
	
	fIValues[bridx] = value;
}

// ----------------------------------
//           BOOL VALUES
// ----------------------------------
void PndSimpleNtuple::Column(TString name, Bool_t value)
{
	int curtype = 3; // Bool_t 
	int cursize = fSize[curtype];
	int curmax  = fMax[curtype];
	
	// if existing, should be 3e6fMax <= bridx < 4e6 for bool
	int bridx = BranchIndex(name);
	int datatype = bridx>=0 ? bridx/MAXBR : curtype;
	bridx = bridx>=0 ? bridx%MAXBR : -1;
	
	if (bridx<0)
	{
		// do we need to expand the array?
		if (cursize>=curmax)
		{
			fMax[curtype] += fGrowBuf;
			//std::cout <<" [INFO] PndSimpleNtuple : GROWING array Double_t by "<< fGrowBuf <<" to max="<<fMax[curtype]<<std::endl;
			
			// create temp array (we'll keep this and delete the old one)
			Char_t *tmpD = new Char_t[fMax[curtype]];
			for (int i=0;i<curmax;++i) tmpD[i] = fBValues[i];
			delete[] fBValues;
			fBValues = tmpD;
			
			// take over and update branch addresses
			for ( std::map<TString, Int_t>::iterator it = fBranchMap.begin(); it != fBranchMap.end(); it++ )
				if (it->second/MAXBR == curtype)
					fTree->SetBranchAddress(it->first.Data(), &(fBValues[it->second%MAXBR]));		
		}
		
		// create the new branch
		fTree->Branch(name.Data(),&(fBValues[cursize]),(name+"/B").Data());
		bridx = fSize[curtype]++;
		fBranchMap[name] = bridx + MAXBR*curtype;
	}
	
	//wrong type (no bool) 
	if (datatype != curtype)
	{
		std::cout<<" - WARNING - Type mismatch of branch "<<name<<" ("<<typearr[curtype]<<" -> "<<typearr[datatype]<<")!"<<std::endl;
		return;
	}
	
	fBValues[bridx] = value;
}


// ----------------------------------
// sets double value array of a variable 'name', index variable 'idxvar'
void PndSimpleNtuple::Column(TString name, Double_t *vpointer, TString idxvar) 
{
	int curtype = 4; // Double_t* (array var) 
	
	// if existing, should be 4e6fMax <= bridx < 5e6 for double*
	int bridx = BranchIndex(name);
	int datatype = bridx>=0 ? bridx/MAXBR : curtype;
	bridx = bridx>=0 ? bridx%MAXBR : -1;
	
	if (bridx<0)
	{
		// create the new branch
		fTree->Branch(name.Data(),vpointer,(name+"["+idxvar+"]/D").Data());
		bridx = fSize[curtype]++;
		fBranchMap[name] = bridx + MAXBR*curtype;
	}
	else 
		fTree->SetBranchAddress(name.Data(), vpointer);
	
	//wrong type (no double*) 
	if (datatype != curtype)
	{
		std::cout<<" - WARNING - Type mismatch of branch "<<name<<" ("<<typearr[curtype]<<" -> "<<typearr[datatype]<<")!"<<std::endl;
		return;
	}
		
	//fDPointers[bridx] = vpointer;
}

//// ----------------------------------
//// sets float value array of a variable 'name', index variable 'idxvar'
//void PndSimpleNtuple::Column(TString name, Float_t *vpointer,  TString idxvar) 
//{
//}

//// ----------------------------------
//// sets integer value array of a variable 'name', index variable 'idxvar'
//void PndSimpleNtuple::Column(TString name, Int_t *vpointer,    TString idxvar)
//{
//}

//// ----------------------------------
//// sets bool value of array a variable 'name', index variable 'idxvar'
//void PndSimpleNtuple::Column(TString name, Bool_t *vpointer,   TString idxvar)
//{
//}

// ----------------------------------

Double_t PndSimpleNtuple::GetCurrentValue(TString name)
{
	int bridx = BranchIndex(name);
	int datatype = bridx/MAXBR;
	bridx = bridx%MAXBR;
	
	// branch doesn't exist
	if (bridx<0) return sqrt(-1);
	
	switch (datatype)
	{
	case 0 : return fDValues[bridx]; break; // double 
	case 1 : return fFValues[bridx]; break; // float 
	case 2 : return fIValues[bridx]; break; // int 
	case 3 : return fBValues[bridx]; break; // bool 
	
	default : return sqrt(-1);
	}
	
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

// ----------------------------------

int PndSimpleNtuple::ShowBranches()
{
	std::map<TString, Int_t>::iterator it;

	for ( it = fBranchMap.begin(); it != fBranchMap.end(); it++ )
	{
		int type = it->second / MAXBR;
		std::cout << it->first<<"/"<<typearr[type]  // string (key)
				  << " : "
				  << it->second   // string's value 
				  << std::endl ;
	}	
	
	return (int)fBranchMap.size();
}
