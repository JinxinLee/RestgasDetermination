#include "PndEventReader.h"

#include <string>
#include <iostream>

#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TBranch.h"

#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/VAbsPidSelector.h"
#include "RhoBase/TFactory.h"
#include "RhoBase/PndMicroCandidate.h"
#include "RhoSelector/TPidSelector.h"
#include "RhoTools/PndPidListMaker.h"
#include "RhoBase/PndEventInfo.h"
#include "RhoTools/TEventShape.h"

PndEventReader::PndEventReader(TChain *t) :
	fEvtCount(0),
	fChainEntries(0),
	fTreeName(""),
	fOwnChain(t),
	fOwnTree(false),
	fPidListMaker(0),
	fCurrentEventInfo(0)
{
	if (t) 
	{
		fTreeName=t->GetName();
		fChainEntries=t->GetEntries();
	}
	
	Init();
}

PndEventReader::PndEventReader(std::string fname, std::string treename) :
	fEvtCount(0),
	fChainEntries(0),
	fTreeName(treename),
	fOwnChain(0),	
	fOwnTree(true),
	fPidListMaker(0),
	fCurrentEventInfo(0)
{
	Init();
	Add(fname);
}

PndEventReader::PndEventReader(TString fname, TString treename) :
	fEvtCount(0),
	fChainEntries(0),
	fTreeName(std::string(treename.Data())),
	fOwnChain(0),	
	fOwnTree(true),
	fPidListMaker(0),
	fCurrentEventInfo(0)
{
	Init();
	Add(fname);
}

PndEventReader::PndEventReader(const char* fname, const char* treename) :
	fEvtCount(0),
	fChainEntries(0),
	fTreeName(std::string(treename)),
	fOwnChain(0),	
	fOwnTree(true),
	fPidListMaker(0),
	fCurrentEventInfo(0)
{
	Init();
	Add(fname);
}

PndEventReader::~PndEventReader()
{
	delete fPidListMaker;
	if (fOwnTree) delete fOwnChain;
}

void PndEventReader::Init()
{
	Reset();
	if (0==fOwnChain) fOwnChain=new TChain(fTreeName.c_str());
	
    fOwnChain->SetBranchAddress("PndChargedCandidates",&fChargedCands);
    fOwnChain->SetBranchAddress("PndNeutralCandidates",&fNeutralCands);
    fOwnChain->SetBranchAddress("PndMcTracks",&fMcCands);
	fOwnChain->SetBranchAddress("PndMicroCandidates",&fMicroCands);
//	fOwnChain->SetBranchAddress("PndEventSummary",&fCurrentEventInfo);
	fOwnChain->SetBranchAddress("PndEventSummary",&fEventInfo);
	
	fPidListMaker=new PndPidListMaker();
	
	SetupBranchNames();
}

void PndEventReader::SetupBranchNames()
{
}

Int_t PndEventReader::Add(const char* fname)
{
	Int_t result=fOwnChain->Add(fname);
	fChainEntries=fOwnChain->GetEntries();
	
	return result;
}

Int_t PndEventReader::Add(TChain *chain)
{
	Int_t result=fOwnChain->Add(chain);
	fChainEntries=fOwnChain->GetEntries();
	
	return result;
}

void PndEventReader::Rewind()
{
	fEvtCount=0;
}

int PndEventReader::GetEvent(int n)
{
	TFactory::Instance()->Reset();
	
	allCands.Cleanup();
	chargedCands.Cleanup();
	neutralCands.Cleanup();
	mcCands.Cleanup();
	
	fCurrentEventInfo=0;
	fEventRead=false;
	
	if (n>=0) fEvtCount=n+1;
	else fEvtCount++;
	
	if (fEvtCount<=fChainEntries) return fEvtCount;
	else fEvtCount=fChainEntries;
	
	return 0;
}

bool PndEventReader::FillList(TCandList &l, std::string listkey)
{
	TCandidate *tc;
	l.Cleanup();
	
	int i1;
	
	// when the first list is requested read in the event
	//if (fOwnChain->GetReadEntry()!=fEvtCount-1) fOwnChain->GetEntry(fEvtCount-1,1);
	if (!fEventRead) 
	{
		fOwnChain->SetBranchStatus("*",1);
		fOwnChain->GetEntry(fEvtCount-1);
		fEventRead=true;
	}
	
	if (listkey=="McTruth")
	{
		if (fMcCands)
		for (int i1=0; i1<fMcCands->GetEntriesFast(); i1++){
			tc = (TCandidate *)fMcCands->At(i1);
			l.Add(*tc);
		}
		return true;
	}
	
	// fill all, neutral and charged from the PndMicroCandidate Array
	if (fMicroCands && allCands.GetLength()==0)
	{
		for (i1=0; i1<fMicroCands->GetEntriesFast(); i1++)
		{
			PndMicroCandidate *mic = (PndMicroCandidate *)fMicroCands->At(i1);
			TCandidate tc(*mic,i1+1);
			
			allCands.Add(tc);
			
			if (fabs(tc.Charge())>0.01) 
				chargedCands.Add(tc);
			else 
				neutralCands.Add(tc);
		}
	}
	else  // for compatibility reasons keep temporary the TCandidate readin
	{
		if (fNeutralCands && neutralCands.GetLength()==0)
		for (i1=0; i1<fNeutralCands->GetEntriesFast(); i1++)
		{
			tc = (TCandidate *)fNeutralCands->At(i1);
			neutralCands.Add(*tc);
			allCands.Add(*tc);
		}
		
		if (fChargedCands && chargedCands.GetLength()==0)
		for (int i1=0; i1<fChargedCands->GetEntriesFast(); i1++)
		{
			tc = (TCandidate *)fChargedCands->At(i1);
			chargedCands.Add(*tc);
			allCands.Add(*tc);
		}
	}
	
	// set the base list for the PID list maker
	fPidListMaker->SetBaseList(chargedCands);
	
	if (listkey=="All" )
	{
		l=allCands;
		return true;
	}
		
	if (listkey=="Neutral") 
	{
		l=neutralCands;
		return true;
	}
	
	if (listkey=="Charged") 
	{
		l=chargedCands;
		return true;
	}
		
	return fPidListMaker->FillList(l,listkey);
}


int PndEventReader::GetEntries() 
{
	if (fOwnChain) return fOwnChain->GetEntries();
	else return 0;
}


const PndEventInfo* PndEventReader::GetEventInfo()
{
    if (fCurrentEventInfo) return fCurrentEventInfo;
	
    Long64_t localEntry = fOwnChain->LoadTree(fEvtCount-1);
	TBranch *b=fOwnChain->GetBranch("PndEventSummary");
	
	if (b)
	{
    	b->GetEntry(localEntry);
		fCurrentEventInfo=(PndEventInfo*)fEventInfo->At(0);
	}
	
	return fCurrentEventInfo;
}

Float_t PndEventReader::GetTag(const char* bname)
{
    Long64_t localEntry = fOwnChain->LoadTree(fEvtCount-1);
	
	TBranch *b=fOwnChain->GetBranch(bname);
	
	Float_t val=0;
	if (b)
	{
		Float_t *tmpad=(Float_t*)b->GetAddress();
		b->GetEntry(localEntry);
		val=*tmpad;
	}
	return val;
}
	
Int_t PndEventReader::GetTagI(const char* bname)
{
    Long64_t localEntry = fOwnChain->LoadTree(fEvtCount-1);
	
	TBranch *b=fOwnChain->GetBranch(bname);
	
	Int_t val=0;
	if (b)
	{
		Int_t *tmpad=(Int_t*)b->GetAddress();
		b->GetEntry(localEntry);
		val=*tmpad;
	}
	return val;
}

