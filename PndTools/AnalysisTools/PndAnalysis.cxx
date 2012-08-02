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
#include "PndAnaPidSelector.h"
#include "PndAnaPidCombiner.h"
#include "PndMCTrack.h"
#include "PndVtxFitterParticle.h" // using a cov matrix tool
#include "PndAnalysisCalcTools.h"

ClassImp(PndAnalysis);

Int_t PndAnalysis::fVerbose=0;

PndAnalysis::PndAnalysis(TString tname1, TString tname2) :
fRootManager(FairRootManager::Instance()),
fPidSelector(0),
fEvtCount(0),
fChainEntries(0),
fEventRead(false),
fBuildMcCands(false),
fChargedPidName("PidAlgoIdealCharged"),
fNeutralPidName("PidAlgoIdealNeutral"),
fTracksName(tname1),
fTracksName2(tname2)
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
	if(0!=fPidSelector) delete fPidSelector;
}

TClonesArray* PndAnalysis::ReadTCA(TString tcaname)
{
  TClonesArray* tca = (TClonesArray*) fRootManager->GetObject(tcaname.Data());
  if (! tca) std::cout << "-I- PndAnalysis::ReadTCA(): No "<<tcaname.Data()<<" array found." << std::endl;
  return tca;
}

void PndAnalysis::Init()
{
	Reset();
  
  //read arrays
  fChargedCands = ReadTCA("PidChargedCand");  
  fChargedProbability = ReadTCA(fChargedPidName.Data());
  fNeutralCands = ReadTCA("PidNeutralCand");
  fNeutralProbability = ReadTCA(fNeutralPidName.Data());
  // -- Barrel Part
  std::cout << "-I- PndAnalysis::Init(): Trying "<<fTracksName.Data()<<" now." << std::endl; 
  fTracks = ReadTCA(fTracksName);
  if(!fTracks) {
    std::cout << "-I- PndAnalysis::Init(): Trying SttMvdGenTrack now." << std::endl; 
    fTracks = ReadTCA("LheGenTrack");
  }
  if(!fTracks) {
    std::cout << "-I- PndAnalysis::Init(): Trying SttMvdGenTrack now." << std::endl; 
    fTracks = ReadTCA("SttMvdGenTrack");
  }
  if(!fTracks) {
    std::cout << "-I- PndAnalysis::Init(): Trying SttMvdGemGenTrack now." << std::endl;
    fTracks = ReadTCA("SttMvdGemGenTrack");
  }
  if(!fTracks) {
    std::cout << "-I- PndAnalysis::Init(): Trying BarrelGenTrack now." << std::endl;
    fTracks = ReadTCA("BarrelGenTrack");
  }
  if(!fTracks) {
    std::cout << "-E- PndAnalysis::Init(): No track inpt array. Make a Zero TclonesArray." << std::endl;
  } else {
    fTracks->Print();
  }
  
  // -- Forward part
  std::cout << "-I- PndAnalysis::Init(): Second: Trying "<<fTracksName2.Data()<<" now." << std::endl; 
  fTracks2 = ReadTCA(fTracksName2);
  if(!fTracks2) {
    std::cout << "-I- PndAnalysis::Init(): Second: Trying FTSGenTrack now." << std::endl;
    fTracks2 = ReadTCA("FTSGenTrack");
  }
  if(!fTracks2) {
    std::cout << "-I- PndAnalysis::Init(): Second: Trying FTSTrkIdeal now." << std::endl;
    fTracks2 = ReadTCA("FTSTrkIdeal");
  }
  // -- MC Tracks 
  fBuildMcCands = false;
  fMcCands = ReadTCA("PndMcTracks"); // try already built TCandidates
  if ( ! fMcCands )
  {
    std::cout << "-I- PndAnalysis::Init(): Trying mc stack now." << std::endl;
    fMcTracks = (TClonesArray*) fRootManager->GetObject("MCTrack");
    if ( ! fMcTracks && fVerbose ) std::cout << "-W- PndAnalysis::Init(): No \"MCTrack\" array found. No MC info available." << std::endl;
    fMcCands =new TClonesArray("TCandidate");
    // next line commented by KG, 07/2012
    fRootManager->Register("PndMcTracks","PndMcTracksFolder", fMcCands, kTRUE); 
    fBuildMcCands = true;
  }
  fChainEntries =(fRootManager->GetInChain())->GetEntries();
  
  //TODO default constructor here?
	fPidSelector = new PndAnaPidSelector();
  fPidCombiner = new PndAnaPidCombiner();
  
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

FairMCEventHeader* PndAnalysis::GetEventHeader()
{
  if (!fEventRead)
  {
    fRootManager->ReadEvent(fEvtCount-1);
    fEventRead=kTRUE;
  }
  
  FairMCEventHeader*  evthead =  (FairMCEventHeader*)FairRootManager::Instance()->GetObject("MCEventHeader.");
  return evthead;
}

Bool_t PndAnalysis::FillList(TCandList &l, TString listkey, TString pidTcaNames)
{
  // Reads the specified List for the current event
  UInt_t _uid=0;
	l.Cleanup();
  
  
	// when the first list is requested read in the event
	if (!fEventRead) 
	{
    fRootManager->ReadEvent(fEvtCount-1);
		fEventRead=kTRUE;
	}
  
  // Get or build Monte-Carlo truth list
	if (listkey=="McTruth")
	{
		if (fMcCands) {
      if(fBuildMcCands) BuildMcCands();
      for (Int_t i1=0; i1<fMcCands->GetEntriesFast(); i1++){
        TCandidate* tc = (TCandidate *)fMcCands->At(i1);
        l.Add(*tc);
      }
      return kTRUE;
    } else return kFALSE;
	}
	
  if (allCands.GetLength() == 0) // do only when we didn't read something yet.
	{ 	// removed now compatibility to TCandidate readin ... instead read PndPidCandidates
    
		if (fNeutralCands && neutralCands.GetLength()==0)
    {
      for (Int_t i1=0; i1<fNeutralCands->GetEntriesFast(); i1++)
      {
        VAbsMicroCandidate *mic = (VAbsMicroCandidate *)fNeutralCands->At(i1);		
        _uid++; // uid will start from 1
        TCandidate tc(*mic,_uid);
        tc.SetTrackNumber(-1);//(i1);
        // TODO: Do we want to set something here? It is neutrals anyway.
        if(0!=fNeutralProbability && i1<fNeutralProbability->GetEntriesFast())
        {
          PndPidProbability *neuProb = (PndPidProbability*)fNeutralProbability->At(i1);
          if(neuProb == 0) {
            Error("FillList", "Neutral PID Probability object not found, skip setting pid for candidate %i.",i1);
            continue;
          }
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
    }
		if (fChargedCands && chargedCands.GetLength()==0) 
    {
      for (Int_t i2=0; i2<fChargedCands->GetEntriesFast(); i2++)
      {
        _uid++; // uid will start from (n_neutrals + 1)
        VAbsMicroCandidate *mic = (VAbsMicroCandidate *)fChargedCands->At(i2);
        TCandidate tc(*mic,_uid);
        tc.SetTrackNumber(i2);
        // TODO: Check that no i+1 is requested anymore elsewhere!!!
        chargedCands.Add(tc);
        allCands.Add(tc);
      }
    }
	}
	
  // Set which PID information should be used.
  if(pidTcaNames!="") fPidCombiner->SetTcaNames(pidTcaNames);
  else fPidCombiner->SetDefaults();
  
  // acceleration: just give the large lists directly
	if (listkey=="All" )
	{
    fPidCombiner->Apply(allCands);
		l=allCands;
		return kTRUE;
	}
  
	if (listkey=="Neutral") 
	{
    //fPidCombiner->Apply(neutralCands);
		l=neutralCands;
		return kTRUE;
	}
	
	if (listkey=="Charged") 
	{
    fPidCombiner->Apply(chargedCands);
		l=chargedCands;
		return kTRUE;
	}
  
  // Real selection requested:
	// set the base list for the PID list maker
	fPidSelector->SetCriterion(listkey);
  
  if(listkey.Contains("Neutral")) 
  {
    fPidCombiner->Apply(neutralCands);
    fPidSelector->Select(neutralCands,l);
    return kTRUE;
  }
  
  if( listkey.Contains("Electron")||listkey.Contains("Muon")||listkey.Contains("Pion")
     || listkey.Contains("Kaon")||listkey.Contains("Proton") 
     || listkey.Contains("Plus")||listkey.Contains("Minus") ) 
  {
    fPidCombiner->Apply(chargedCands);
    fPidSelector->Select(chargedCands,l);
    return kTRUE;
  }
  
  Error("FillList", "Unknown list key: %s",listkey.Data());
  return kFALSE;
}

Int_t PndAnalysis::GetEntries() 
{
  if (fRootManager) return (fRootManager->GetInChain())->GetEntries();
	else return 0;
}

void PndAnalysis::BuildMcCands()
{
  if (fMcCands->GetEntriesFast() > 100) fMcCands->Delete(); // deep cleanup after really busy events
  else if (fMcCands->GetEntriesFast() != 0)  fMcCands->Clear();
  Int_t mcMotherID = -1;
  // Get the Candidates
  for(Int_t i=0; i<fMcTracks->GetEntriesFast(); i++)
  {
  	PndMCTrack *part = (PndMCTrack*)fMcTracks->At(i);
  	//if (part->GetMotherID()!=-1) continue;
  	if(fVerbose>2){
      std::cout<<"Build MC cand: ";
      part->Print(i);
    }
    mcMotherID = part->GetMotherID();
    if(mcMotherID<0) mcMotherID=part->GetSecondMotherID(); // shadowed particle IDs
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
    pmc->SetMcMotherIdx(mcMotherID);
    
    if(fabs(charge)>0){
      Bool_t rc = PndAnalysisCalcTools::FillHelixParams(pmc, kTRUE);
      if(!rc && fVerbose>0) {
        Warning("BuildMcCands()","Faild calculation helix parameters");
        std::cout<<*pmc<<std::endl;
        stvtx.Print();
        std::cout<<"Mother pointer: "<<pmc->TheMother()<<std::endl;
      }
    }
    
  }
  // iterate again to set mother relations
  for (int i=0;i<fMcCands->GetEntriesFast();i++)
  {
    TCandidate* aMcCand=(TCandidate*)fMcCands->At(i);
    mcMotherID=aMcCand->GetMcMotherIdx();
    if (mcMotherID<0) continue;
    TCandidate* aMother=(TCandidate*)fMcCands->At(mcMotherID);
    if(0 == aMother) continue;
    aMcCand->SetMotherLink(aMother); // This should add the mother-daughter and daughter-mother relation
  }
  
  if(fVerbose) std::cout <<"-I- PndMcListConverter: found ="<<fMcCands->GetEntriesFast()<<std::endl;
  
}

Bool_t PndAnalysis::PropagateToIp(TCandidate* cand)
{ //Propagate from the tracks first parameter set to the POCA from (0,0,0)
  return PropagateToPoint(cand, new TVector3(0.,0.,0.) );
}  

Bool_t PndAnalysis::PropagateToZAxis(TCandidate* cand)
{
  if(!cand) {
    Error("PropagateToZAxis","Candidate not found: %p",cand);
    return kFALSE;
  }
  PndPidCandidate* pidCand = static_cast<PndPidCandidate*>(&cand->GetMicroCandidate());
  PndTrack* track = (PndTrack*)fTracks->At(pidCand->GetTrackIndex());
  if (!track) {
    Warning("PropagateToZAxis","Could not find track object of index %d",pidCand->GetTrackIndex()); 
    return kFALSE;
  }
  FairTrackParP tStart = track->GetParamFirst();
  return Propagator(2,tStart,cand,NULL,kFALSE);
}

Bool_t PndAnalysis::PropagateToPoint(TCandidate* cand, TVector3* mypoint)
{ //Propagate from the tracks first parameter set to the POCA from mypoint
  //The candidate is updated but the track not touched 
  //Only the uncorrelated errors are propagated, 
  //TODO: implement a real cov matrix
  if(!cand) {
    Error("PropagateToPoint","Candidate not found: %p",cand);
    return kFALSE;
  }
  PndPidCandidate* pidCand = static_cast<PndPidCandidate*>(&cand->GetMicroCandidate());
  PndTrack* track = (PndTrack*)fTracks->At(pidCand->GetTrackIndex());
  if (!track) {
    Warning("PropagateToPoint","Could not find track object of index %d",pidCand->GetTrackIndex()); 
    return kFALSE;
  }
  FairTrackParP tStart = track->GetParamFirst();
  return Propagator(1,tStart,cand,mypoint,kFALSE);
}

FairTrackParP PndAnalysis::GetFirstPar(TCandidate* cand)
{
  if(!cand) {
    Error("GetFirstPar","Candidate not found: %p",cand);
    FairTrackParP dummy;
    return dummy;
  }
  PndPidCandidate* pidCand = static_cast<PndPidCandidate*>(&cand->GetMicroCandidate());
  PndTrack* track = (PndTrack*)fTracks->At(pidCand->GetTrackIndex());
  if (!track) {
    Warning("GetFirstPar","Could not find track object of index %d",pidCand->GetTrackIndex()); 
    FairTrackParP dummy;
    return dummy;
  }
  FairTrackParP tStart = track->GetParamFirst();
  return tStart;
}

Bool_t PndAnalysis::ResetDaughters(TCandidate* cand)
{
  Bool_t success=kTRUE;
  for(Int_t daug =0;daug<cand->NDaughters();daug++)
  {
    TCandidate* a=cand->Daughter(daug);
    success = success && ResetCandidate(a);
  }
  return success;
}

Bool_t PndAnalysis::ResetCandidate(TCandidate* cand)
{
  FairTrackParP firstpar = GetFirstPar(cand);
  Double_t globalCov[6][6];
  firstpar.GetMARSCov(globalCov);
  TMatrixD err(6,6);
  for (Int_t ii=0;ii<6;ii++) for(Int_t jj=0;jj<6;jj++) err[ii][jj]=globalCov[ii][jj];
  //if(fVerbose>2){ std::cout<<"MARS cov (px,py,pz,E,x,y,z): ";err.Print();} 
  TLorentzVector lv = cand->P4();
  static PndVtxFitterParticle covTool; // external tool to convert from a 6x6 (p3,v) cov matrix to the 7x7(p4,v) cov matrix
  TMatrixD covPosMom = covTool.GetConverted7(covTool.GetFitError(lv, err));
  //if(fVerbose>2){ std::cout<<"covPosMom (x,y,z,px,py,pz,E): ";covPosMom.Print();} 
  
  cand->SetPosition(firstpar.GetPosition());
  cand->SetP3(firstpar.GetMomentum()); // implicitly uses the candidates mass to set P4
  cand->SetCov7(covPosMom);
  return kTRUE;
}


Bool_t PndAnalysis::Propagator(int mode, FairTrackParP &tStart, TCandidate* cand, TVector3* mypoint, Bool_t skipcov)
{
  //Propagate from the tracks first parameter set to the POCA from mypoint
  //The candidate is updated but the track not touched 
  //Only the uncorrelated errors are propagated, 
  //TODO: implement a real cov matrix
  
  Bool_t rc = kFALSE;
  static PndVtxFitterParticle covTool; // external tool to convert from a 6x6 (p3,v) cov matrix to the 7x7(p4,v) cov matrix
  FairGeanePro* geaneProp = new FairGeanePro();
  FairTrackParH* myStart = new FairTrackParH(tStart);
  FairTrackParH* myResult = new FairTrackParH();
  Int_t pdgcode = cand->PdgCode();
  if(fVerbose>0)cout<<"Try mode "<<mode<<" with pdgCode "<<pdgcode<<endl;
  if(fVerbose>2){std::cout<<"Start Params are:"<<std::endl; tStart.Print();}
  Double_t startCov[6][6];
  tStart.GetMARSCov(startCov);
  TMatrixD errst(6,6);
  for (Int_t ii=0;ii<6;ii++) for(Int_t jj=0;jj<6;jj++) errst[ii][jj]=startCov[ii][jj];
  if(fVerbose>2){std::cout<<"Start MARS cov: ";errst.Print();}
  
  if(1==mode && NULL!=mypoint){
    geaneProp->BackTrackToVertex(); //set where to propagate
    geaneProp->SetPoint(*mypoint);
  } else if(2==mode){
    geaneProp->PropagateToPCA(2, -1);// track back to z axis
    TVector3 ex1(0.,0.,-50.); // virtual wire of arbitrarily chosen size
    TVector3 ex2(0.,0.,100.);
    geaneProp->SetWire(ex1,ex2);
  } else {
    Error("Propagator()","Use mode 1 with a valid TVector3 pointer or mode 2. (Mode=%i & TVector3*=%p)",mode,mypoint);
    return kFALSE;
  }
  
  // now we propagate
  rc = geaneProp->Propagate(myStart, myResult,pdgcode);
  
  if (!rc) {
    if(fVerbose>0) Warning("Propagator()","Geane propagation failed");
    return kFALSE;
  }
  TVector3 pos(myResult->GetX(),myResult->GetY(),myResult->GetZ()); // I want to be sure... 
                                                                    //printout for checks
  TVector3 vecdiff=myStart->GetPosition() - myResult->GetPosition();
  if(fVerbose>1){
    std::cout<<"position start     :";  myStart->GetPosition().Print();
    std::cout<<"position ip        :";  myResult->GetPosition().Print();
    std::cout<<"position difference:";  vecdiff.Print();
    vecdiff=myStart->GetMomentum()-myResult->GetMomentum();
    std::cout<<"momentum start     :";  myStart->GetMomentum().Print();
    std::cout<<"momentum ip        :";  myResult->GetMomentum().Print();
    std::cout<<"momentum difference:";  vecdiff.Print();
  }
  cand->SetPosition(pos);
  cand->SetP3(myResult->GetMomentum()); // implicitly uses the candidates mass to set P4
  int ierr=0;
  TVector3 di = myResult->GetMomentum();
  di.SetMag(1.);
  TVector3 dj = di.Orthogonal();
  TVector3 dk = di.Cross(dj);
  FairTrackParP *myParab = new FairTrackParP(myResult, dj, dk, ierr);
  Double_t globalCov[6][6];
  myParab->GetMARSCov(globalCov);
  TMatrixD err(6,6);
  for (Int_t ii=0;ii<6;ii++) for(Int_t jj=0;jj<6;jj++) err[ii][jj]=globalCov[ii][jj];
  if(fVerbose>2){ std::cout<<"MARS cov (px,py,pz,E,x,y,z): ";err.Print();} 
  TLorentzVector lv = cand->P4();
  TMatrixD covPosMom = covTool.GetConverted7(covTool.GetFitError(lv, err));
  if(fVerbose>2){ std::cout<<"covPosMom (x,y,z,px,py,pz,E): ";covPosMom.Print();} 
  
  cand->SetCov7(covPosMom);
  
  rc = PndAnalysisCalcTools::FillHelixParams(cand,skipcov);
  if (!rc) {Warning("Propagator()","P7toHelix failed"); return kFALSE;}
  
  if(fVerbose>2) {
    std::cout<<" :::::::::::  Printout in PndAnalysis::Propagator() :::::::::::  "<<std::endl;
    
    //std::cout<<"Start Params:"<<std::endl;
    //myStart->Print();
    
    std::cout<<"SC system params:"
    <<"\nq/p    = "<<myResult->GetQp()
    <<"\nLambda = "<<myResult->GetLambda()
    <<"\nPhi    = "<<myResult->GetPhi()
    <<"\nX_sc   = "<<myResult->GetX_sc()
    <<"\nY_sc   = "<<myResult->GetY_sc()
    <<"\nZ_sc   = "<<myResult->GetZ_sc()
    <<std::endl;  
    
    std::cout<<"some values:"
    <<"\n Z0 ?= z_sc / cos(lambda)  = "<< myResult->GetZ_sc() / cos(myResult->GetLambda())
    <<"\n Z0 ?= sqrt(x_sc^2+z_sc^2) = "<<sqrt(myResult->GetX_sc()*myResult->GetX_sc()+myResult->GetZ_sc()*myResult->GetZ_sc())
    <<std::endl;
  }
  // COMMENT: 
  // When taking the Trackparams in the POCA to the z-axis, the SC system from GEANE matches the common helix params easier, i.e:
  // D0 = y_sc and Z0 = sqrt(x_sc^2 + z_sc^2) = z_sc*tan(Lambda)
  
  if(fVerbose>1)Info("Propagator  ","Succsess=%i",rc);
  return kTRUE;
}

