// PndAnalysis
// Needs a FairRunAna set up in the macro for file & parameter I/O

#include "PndAnalysis.h"

#include <string>
#include <iostream>

using std::cout;
using std::endl;

//Root stuff
#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "VAbsPidSelector.h"
#include "VAbsMicroCandidate.h"

//RHO stuff
#include "TRho.h"
#include "TFactory.h"
#include "TCandidate.h"
#include "TCandList.h"
#include "TPidSelector.h"

#include "FairTrackParP.h"
#include "FairTrackParH.h"
#include "FairGeanePro.h"
#include "FairRunAna.h"
#include "FairField.h"

#include "PndTrack.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "PndPidListMaker.h"
#include "PndMCTrack.h"

ClassImp(PndAnalysis);

PndAnalysis::PndAnalysis() :
fRootManager(FairRootManager::Instance()),
fPidListMaker(0),
fEvtCount(0),
fChainEntries(0),
fEventRead(false),
fBuildMcCands(false),
fVerbose(0),
fChargedPidName("PidAlgoIdealCharged"),
fNeutralPidName("PidAlgoIdealNeutral")
{
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

TClonesArray* PndAnalysis::ReadTCA(TString tcaname)
{
  TClonesArray* tca = (TClonesArray*) fRootManager->GetObject(tcaname.Data());
  if (! tca) std::cout << "-W- PndAnalysis::Init(): No "<<tcaname.Data()<<" array found." << std::endl;
  return tca;
}

void PndAnalysis::Init()
{
	Reset();
  
  //read arrays
  fChargedCands = ReadTCA("PidChargedCand");  
  fChargedProbability = ReadTCA(fChargedPidName.Data());
  fNeutralCands = ReadTCA("PidNeutralCand");
  fNeutralProbability = ReadTCA(fChargedPidName.Data());
  fTracks = ReadTCA("LheGenTrack");
  if(!fTracks) fTracks = ReadTCA("SttMvdGenTrack");
  if(!fTracks) fTracks = ReadTCA("FTSGenTrack");
  if(!fTracks) fTracks = ReadTCA("FTSTrkIdeal");
  fBuildMcCands = false;
  fMcCands = ReadTCA("PndMcTracks");
  if ( ! fMcCands )
  {
    if( fVerbose ) std::cout << "-I- PndAnalysis::Init(): Trying mc stack now." << std::endl;
    fMcTracks = (TClonesArray*) fRootManager->GetObject("MCTrack");
    if ( ! fMcTracks && fVerbose ) std::cout << "-W- PndAnalysis::Init(): No \"MCTrack\" array found. No MC info available." << std::endl;
    fMcCands =new TClonesArray("TCandidate");
    fRootManager->Register("PndMcTracks","PndMcTracks", fMcCands, kTRUE);
    fBuildMcCands = true;
  }
  fChainEntries =(fRootManager->GetInChain())->GetEntries();
  
	fPidListMaker = new PndPidListMaker();
  
  fPdg = TRho::Instance()->GetPDG();
  
}


void PndAnalysis::Rewind()
{
	fEvtCount=0;
}

Int_t PndAnalysis::GetEvent(Int_t n)
{	
	allCands.Cleanup();
	chargedCands.Cleanup();
	neutralCands.Cleanup();
	mcCands.Cleanup();
    TFactory::Instance()->Reset();

	fEventRead=false;
	
	if (n>=0) fEvtCount=n+1;
	else fEvtCount++;
	
	if (fEvtCount<=fChainEntries) return fEvtCount;
	else fEvtCount=fChainEntries;
	
	return 0;
}

Bool_t PndAnalysis::FillList(TCandList &l, std::string listkey)
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
      if(fBuildMcCands) BuildMcCands();
      for (Int_t i1=0; i1<fMcCands->GetEntriesFast(); i1++){
        TCandidate* tc = (TCandidate *)fMcCands->At(i1);
        l.Add(*tc);
      }
      return true;
    } else return false;
	}
	
  if (allCands.GetLength() == 0) // do only when we didn't read something yet.
	{ 	// removed now compatibility to TCandidate readin ... instead read PndPidCandidates
		if (fNeutralCands && neutralCands.GetLength()==0)
      for (Int_t i1=0; i1<fNeutralCands->GetEntriesFast(); i1++)
      {
        VAbsMicroCandidate *mic = (VAbsMicroCandidate *)fNeutralCands->At(i1);		
        TCandidate buffcand(*mic,i1+1);
        TCandidate *tc = TFactory::Instance()->NewCandidate(buffcand);

        // TODO: Do we want to set something here? It is neutrals anyway.
        if(i1<fNeutralProbability->GetEntriesFast())
        {
          PndPidProbability *neuProb = (PndPidProbability*)fNeutralProbability->At(i1);
          if(neuProb == 0) {
            Error("FillList", "Neutral PID Probability object not found, skip setting pid for candidate %i.",i1);
            continue;
          }
          // numbering see PndPidListMaker
          tc->SetPidInfo(0,neuProb->GetElectronPidProb());
          tc->SetPidInfo(1,neuProb->GetMuonPidProb());
          tc->SetPidInfo(2,neuProb->GetPionPidProb());
          tc->SetPidInfo(3,neuProb->GetKaonPidProb());
          tc->SetPidInfo(4,neuProb->GetProtonPidProb());
        }        
        neutralCands.Add(*tc);
        allCands.Add(*tc);
      }
    
		if (fChargedCands && chargedCands.GetLength()==0) 
    {
      for (Int_t i2=0; i2<fChargedCands->GetEntriesFast(); i2++)
      {
        VAbsMicroCandidate *mic = (VAbsMicroCandidate *)fChargedCands->At(i2);
        TCandidate buffcand(*mic,i2+1);
        TCandidate *tc = TFactory::Instance()->NewCandidate(buffcand);
        if(i2<fChargedProbability->GetEntriesFast())
        {
          PndPidProbability *chProb = (PndPidProbability*)fChargedProbability->At(i2);
          if(chProb == 0) {
            Error("FillList", "Charged PID Probability object not found, skip setting pid for candidate %i.",i2);
            continue;
          }
          // numbering see PndPidListMaker
          tc->SetPidInfo(0,chProb->GetElectronPidProb());
          tc->SetPidInfo(1,chProb->GetMuonPidProb());
          tc->SetPidInfo(2,chProb->GetPionPidProb());
          tc->SetPidInfo(3,chProb->GetKaonPidProb());
          tc->SetPidInfo(4,chProb->GetProtonPidProb());
        }        
        chargedCands.Add(*tc);
        allCands.Add(*tc);
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

Int_t PndAnalysis::GetEntries() 
{
  if (fRootManager) return (fRootManager->GetInChain())->GetEntries();
	else return 0;
}

void PndAnalysis::BuildMcCands()
{
  if (fMcCands->GetEntriesFast() != 0) fMcCands->Delete();
  //if (fMcCands->GetEntriesFast() != 0)  fMcCands->Clear("C");
  
  // Get the Candidates
  for(Int_t i=0; i<fMcTracks->GetEntriesFast(); i++)
  {
  	PndMCTrack *part = (PndMCTrack*)fMcTracks->At(i);
  	if (part->GetMotherID()!=-1) continue;
  	
    TLorentzVector p4 = part->Get4Momentum();
    TVector3    stvtx = part->GetStartVertex();
    
    TParticlePDG *ppdg = fPdg->GetParticle(part->GetPdgCode());
    
    double charge=0.0;
    
    if (ppdg) charge=ppdg->Charge();
    else if (fVerbose) cout <<"-W- PndMcListConverter: strange PDG code:"<<part->GetPdgCode()<<endl;
    if (fabs(charge)>2) charge/=3.;
    
    //TClonesArray& ref = *fMcCands;
    Int_t size = fMcCands->GetEntriesFast();
    
    //either
   	//TCandidate buffcand(p4,charge);
    //TCandidate *pmc = TFactory::Instance()->NewCandidate(buffcand);
    //fMcCands->Add(pmc);
    //or
    TCandidate *pmc=new ((*fMcCands)[size]) TCandidate(p4,charge);
    
   	
    //pmc->SetMcIdx(size);
    pmc->SetMcIdx(i);
    pmc->SetPos(stvtx);
    pmc->SetType(part->GetPdgCode());
    
    // additional helix parameters for checking... 
    Double_t pnt[3], Bf[3];
    pnt[0]=stvtx.X();
    pnt[1]=stvtx.Y();
    pnt[2]=stvtx.Z(); 
    FairRunAna::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
    Double_t B = Bf[0]*Bf[0]+Bf[1]*Bf[1]+Bf[2]*Bf[2];
    Float_t helixparams[5];
    helixparams[0]=stvtx.Perp(); //D0
    helixparams[1]=p4.Phi(); //phi0
    helixparams[2]=-0.2998*B*charge/p4.Perp(); //omega=rho=1/R[cm]=-2.998*B[kGs]*Q[e]/p_perp[GeV/c] 
    helixparams[3]=stvtx.Z(); //z0
    helixparams[4]=1/tan(p4.Theta()); 
    pmc->SetHelixParms(helixparams);
    
  }
  
  if(fVerbose) cout <<"-I- PndMcListConverter: found primaries="<<fMcCands->GetEntriesFast()<<endl;
  
}

Bool_t PndAnalysis::PropagateToIp(TCandidate* cand)
{ //Propagate from the tracks first parameter set to the POCA from (0,0,0)
  return PropagateToPoint(cand, new TVector3(0.,0.,0.) );
}  

Bool_t PndAnalysis::PropagateToPoint(TCandidate* cand, TVector3* mypoint)
{ //Propagate from the tracks first parameter set to the POCA from mypoint
  //The candidate is updated but the track not touched 
  //Only the uncorrelated errors are propagated, 
  //TODO: implement a real cov matrix
  
  Bool_t rc = kFALSE;
  if(!cand) {
    Error("PropagateToPoint","Candidate not found: %p",cand);
    return kFALSE;
  }
  PndPidCandidate* pidCand = static_cast<PndPidCandidate*>(&cand->GetMicroCandidate());
  PndTrack* track = (PndTrack*)fTracks->At(pidCand->GetTrackIndex());
  if (!track) {Warning("PropagateToPoint","Could not find track object of index %d",pidCand->GetTrackIndex()); return kFALSE;}
  FairGeanePro* geaneProp = new FairGeanePro();
  geaneProp->BackTrackToVertex(); //set where to propagate
  geaneProp->SetPoint(*mypoint);
  FairTrackParP tStart = track->GetParamFirst();
  FairTrackParH* myStart = new FairTrackParH(tStart);
  FairTrackParH* myResult = new FairTrackParH();
  Int_t pdgcode = cand->PdgCode();
  cout<<"Try pdgCode "<<pdgcode<<endl;
  
  // now we propagate
  rc = geaneProp->Propagate(myStart, myResult,pdgcode);
  
  if (!rc) return kFALSE;
  TVector3 pos(myResult->GetX(),myResult->GetY(),myResult->GetZ()); // I want to be sure... 
  //printout for checks
  TVector3 vecdiff=myStart->GetPosition() - myResult->GetPosition();
  std::cout<<"position start     :";  myStart->GetPosition().Print();
  std::cout<<"position ip        :";  myResult->GetPosition().Print();
  std::cout<<"position difference:";  vecdiff.Print();
  vecdiff=myStart->GetMomentum()-myResult->GetMomentum();
  std::cout<<"momentum start     :";  myStart->GetMomentum().Print();
  std::cout<<"momentum ip        :";  myResult->GetMomentum().Print();
  std::cout<<"momentum difference:";  vecdiff.Print();
  
  cand->SetPosition(pos);
  cand->SetP3(myResult->GetMomentum()); // implicitly uses the candidates mass to set P4
  
  TMatrixD covPosMom(7,7);
  for(Int_t ii=0;ii<7;ii++) for(Int_t jj=0;jj<7;jj++) covPosMom[ii][jj]=0.;
  Double_t A=0;
  A=myResult->GetDX();
  covPosMom[0][0]=A*A; // x x
  A=myResult->GetDY();
  covPosMom[1][1]=A*A; // y y
  A=myResult->GetDZ();
  covPosMom[2][2]=A*A; // z z
  A=myResult->GetDPx();
  covPosMom[3][3]=A*A; // px px
  A=myResult->GetDPy();
  covPosMom[4][4]=A*A; // py py
  A=myResult->GetDPz();
  covPosMom[5][5]=A*A; // pz pz
  
  Double_t M=cand->M();
  Double_t Q=myResult->GetQ();
  if(0==Q)return kFALSE;
  A=myResult->GetQp()/Q;
  A=A*A;
  A=A*A*(1+M*M*A);
  if(0==A) return kFALSE;
  Double_t dA=myResult->GetDQp()/Q;
  covPosMom[6][6]=dA*dA/A; // e e
  
  cand->SetCov7(covPosMom);
  
  Info("PropagateToPoint","Succsess=%b",rc);
  return kTRUE;
}

