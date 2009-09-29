#include "PndAnalysis.h"

#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
//#include "TFile.h"
//#include "TBranch.h"

#include "TCandidate.h"
#include "TCandList.h"
//#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "VAbsPidSelector.h"
//#include "PndMicroCandidate.h"
//#include "TFactory.h"
#include "VAbsMicroCandidate.h"
#include "TPidSelector.h"
//#include "TEventShape.h"
#include "PndPidListMaker.h"
//#include "PndEventInfo.h"

ClassImp(PndAnalysis);
 
PndAnalysis::PndAnalysis() :
fRootManager(FairRootManager::Instance()),
fPidListMaker(0),
fEvtCount(0),
fChainEntries(0),
fEventRead(false)
//fCurrentEventInfo(0)
{
//  fgRinstance=this;
 // fRootManager = FairRootManager::Instance();
  if ( 0 == fRootManager )
  {
    std::cout << "-E- PndAnalysis: RootManager not instantiated!" << std::endl;
    return;
  }
  Init();
}

PndAnalysis::~PndAnalysis()
{
	if(0!=fPidListMaker) delete fPidListMaker;
}

void PndAnalysis::Init()
{
	Reset();
    
  //read
  fChargedCands = (TClonesArray*) fRootManager->GetObject("PidChargedCand");
  fNeutralCands = (TClonesArray*) fRootManager->GetObject("PidNeutralCand");
  fChargedProbability = (TClonesArray*) fRootManager->GetObject("PidChargedProbability");
  fNeutralProbability = (TClonesArray*) fRootManager->GetObject("PidNeutralProbability");
  fMcCands = (TClonesArray*) fRootManager->GetObject("PndMcTracks");
  //fMicroCands = (TClonesArray*) fRootManager->GetObject("PndMicroCandidates");
  //fEventInfo = (TClonesArray*) fRootManager->GetObject("PndEventSummary");
  fChainEntries =(fRootManager->GetInChain())->GetEntries();
  
	fPidListMaker = new PndPidListMaker();
	
	//SetupBranchNames();
}

//void PndAnalysis::SetupBranchNames()
//{
//}

void PndAnalysis::Rewind()
{
	fEvtCount=0;
}

int PndAnalysis::GetEvent(int n)
{
	//TFactory::Instance()->Reset();
	
	allCands.Cleanup();
	chargedCands.Cleanup();
	neutralCands.Cleanup();
	mcCands.Cleanup();
	
	//fCurrentEventInfo=0;
	fEventRead=false;
	
	if (n>=0) fEvtCount=n+1;
	else fEvtCount++;
	
	if (fEvtCount<=fChainEntries) return fEvtCount;
	else fEvtCount=fChainEntries;
	
	return 0;
}

bool PndAnalysis::FillList(TCandList &l, std::string listkey)
{
  // Reads the specified List for the current event
  
	l.Cleanup();
	
	
	// when the first list is requested read in the event
	if (!fEventRead) 
	{
    fRootManager->ReadEvent(fEvtCount-1);
		fEventRead=true;
	}
	
	if (listkey=="McTruth")
	{
		if (fMcCands) {
      for (int i1=0; i1<fMcCands->GetEntriesFast(); i1++){
        TCandidate* tc = (TCandidate *)fMcCands->At(i1);
        l.Add(*tc);
      }
      return true;
    } else return false;
	}
	
	// fill all, neutral and charged from the PndMicroCandidate Array
//	if (fMicroCands && allCands.GetLength()==0)
//	{
//		for (int i1=0; i1<fMicroCands->GetEntriesFast(); i1++)
//		{
//			VAbsMicroCandidate *mic = (VAbsMicroCandidate *)fMicroCands->At(i1);
//			TCandidate tc(*mic,i1+1);
//			
//			allCands.Add(tc);
//			
//			if (fabs(tc.Charge())>0.01) 
//				chargedCands.Add(tc);
//			else 
//				neutralCands.Add(tc);
//		}
//	}
	//else 
  if (allCands.GetLength() == 0) // do only when we didn't read something yet.
	{ 	// removed now compatibility to TCandidate readin ... instead read PndPidCandidates
		if (fNeutralCands && neutralCands.GetLength()==0)
		for (int i1=0; i1<fNeutralCands->GetEntriesFast(); i1++)
		{
			VAbsMicroCandidate *mic = (VAbsMicroCandidate *)fNeutralCands->At(i1);		
			TCandidate tc(*mic,i1+1);
      // TODO: Do we want to set something here? It is neutrals anyway.
      if(i1<fNeutralProbability->GetEntriesFast())
      {
        PndPidProbability *neuProb = (PndPidProbability*)fNeutralProbability->At(i1);
        // numbering see PndPidListMaker
        tc.SetPidInfo(0,neuProb->GetElectronPidProb());
        tc.SetPidInfo(1,neuProb->GetMuonPidProb());
        tc.SetPidInfo(2,neuProb->GetPionPidProb());
        tc.SetPidInfo(3,neuProb->GetKaonPidProb());
        tc.SetPidInfo(4,neuProb->GetProtonPidProb());
      }        
			neutralCands.Add(tc);
			allCands.Add(tc);
		}
    
		if (fChargedCands && chargedCands.GetLength()==0) 
    {
      for (int i1=0; i1<fChargedCands->GetEntriesFast(); i1++)
      {
        VAbsMicroCandidate *mic = (VAbsMicroCandidate *)fChargedCands->At(i1);
        TCandidate tc(*mic,i1+1);
        if(i1<fChargedProbability->GetEntriesFast())
        {
          PndPidProbability *chProb = (PndPidProbability*)fChargedProbability->At(i1);
          // numbering see PndPidListMaker
          tc.SetPidInfo(0,chProb->GetElectronPidProb());
          tc.SetPidInfo(1,chProb->GetMuonPidProb());
          tc.SetPidInfo(2,chProb->GetPionPidProb());
          tc.SetPidInfo(3,chProb->GetKaonPidProb());
          tc.SetPidInfo(4,chProb->GetProtonPidProb());
        }        
        chargedCands.Add(tc);
        allCands.Add(tc);
      }
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


int PndAnalysis::GetEntries() 
{
  if (fRootManager) return (fRootManager->GetInChain())->GetEntries();
	else return 0;
}


//const PndEventInfo* PndAnalysis::GetEventInfo()
//{
//  if (fCurrentEventInfo) return fCurrentEventInfo;
//
//  TClonesArray* aArray = (TClonesArray*) fRootManager->GetObject("PndEventSummary");
//  fRootManager->ReadEvent(fEvtCount-1);
//
//  if (aArray){
//    fCurrentEventInfo = (PndEventInfo*) aArray->At(0);
//  }
//  return fCurrentEventInfo;
//}
//
//Float_t PndAnalysis::GetTag(const char* bname)
//{
//	TChain* theChain = fRootManager->GetInChain();
//  Long64_t localEntry = theChain->LoadTree(fEvtCount-1);
//	theChain = fRootManager->GetInChain();
//	TBranch *b=theChain->GetBranch(bname);
//	
//	Float_t val=0;
//	if (b)
//	{
//		Float_t *tmpad=(Float_t*)b->GetAddress();
//		b->GetEntry(localEntry);
//		val=*tmpad;
//	}
//	return val;
//}
//	
//Int_t PndAnalysis::GetTagI(const char* bname)
//{
//	TChain* theChain = fRootManager->GetInChain();
//  Long64_t localEntry = theChain->LoadTree(fEvtCount-1);
//	TBranch *b=theChain->GetBranch(bname);
//	
//	Int_t val=0;
//	if (b)
//	{
//		Int_t *tmpad=(Int_t*)b->GetAddress();
//		b->GetEntry(localEntry);
//		val=*tmpad;
//	}
//	return val;
//}
//

