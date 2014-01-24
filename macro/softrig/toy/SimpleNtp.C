#include "TTree.h"
#include "TString.h"
#include <map>
#include <iostream>

using std::cout;
using std::endl;

class SimpleNtp {
public:
		
	SimpleNtp(TString name, TString title, int maxsize = 100) {
		fTree    = new TTree(name, title);
		fValues  = new Float_t[maxsize];
		fBranchMap.clear();
		fSize=0;
		fMax=maxsize;
	};
		
	~SimpleNtp(){delete fTree;}
	
	void Column(TString name, Float_t value);
	void DumpData() { fTree->Fill(); }
	
private:
	TTree *fTree;
	std::map<TString, Int_t> fBranchMap;
	Float_t *fValues;
	int fSize;
	int fMax;
};

void SimpleNtp::Column(TString name, Float_t value)
{
	if (fBranchMap.find(name)==fBranchMap.end())
	{
		if (fSize<fMax) 
		{
			fTree->Branch(name.Data(),&(fValues[fSize]));
			fBranchMap[name]=fSize++;
		}
		else
		{
			cout <<" - WARNING - max size reached, branch '"<<name.Data()<<"' is not created!"<<endl;
			return;
		}
	}
	fValues[fBranchMap[name]]=value;
}
