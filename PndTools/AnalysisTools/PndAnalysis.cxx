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
#include "PndVtxFitterParticle.h"

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
  fMcCands = ReadTCA("PndMcTracks");
  if ( ! fMcCands )
  {
    std::cout << "-I- PndAnalysis::Init(): Trying mc stack now." << std::endl;
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
        if(0!=fNeutralProbability && i1<fNeutralProbability->GetEntriesFast())
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
        if(0!=fChargedProbability && i2<fChargedProbability->GetEntriesFast())
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
  	//if (part->GetMotherID()!=-1) continue;
  	
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
    
    //if(false){
    //// additional helix parameters for checking... 
    //// Calculate DOCA from circle projection
    //Double_t pnt[3], Bf[3];
    //pnt[0]=stvtx.X();
    //pnt[1]=stvtx.Y();
    //pnt[2]=stvtx.Z(); 
    //FairRunAna::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
    //const double B = Bf[0]*Bf[0]+Bf[1]*Bf[1]+Bf[2]*Bf[2];
    //const double qBc = -0.299792458*B*charge;
    //const double xp = stvtx.X();
    //const double yp = stvtx.Y();
    //const double zp = stvtx.Z();
    //const double phip = p4.Phi();
    //const double pti=1/p4.Perp();
    
    //      //// get rho
    //const double rho = qBc*pti;
    
    //      //// get tan(dip)
    //const double tanDip=p4.Pz()*pti;
    //const double rhoinverse = 1./rho;
    
    //      ////circle center
    //const double xc = xp - rhoinverse*TMath::Sin(phip);
    //const double yc = yp + rhoinverse*TMath::Cos(phip);
    
    //      ////get phi0 at doca
    //const double phi0 = TMath::ATan2(xc,yc);
    
    //      ////get D0
    //const double D0 = TMath::Sqrt(xc*xc + yc*yc) - fabs(rhoinverse);
    ////or
    ////Double_t dfi=(stvtx.Phi()-p4.Phi())*TMath::RadToDeg();
    ////if(dfi>180)dfi-=360;
    ////if(dfi<-180)dfi+=360;
    ////Double_t sign=-charge*((dfi>0)?1:-1); // TODO get a decent D0 sign!!!
    ////const double x0 = xc + rhoinverse*TMath::Sin(phi0);
    ////const double y0 = yc - rhoinverse*TMath::Cos(phi0);
    ////const double D0 = sign*TMath::Sqrt(x0*x0 + y0*y0);
    
    //      ////get z0
    //const double z0 = zp - tanDip*rhoinverse*(phip-phi0);
    
    //      //Float_t helixparams[5];
    //helixparams[0]=D0;
    //helixparams[1]=phi0;
    //helixparams[2]=rho;
    //helixparams[3]=z0;
    //helixparams[4]=tanDip;
    
    //      //pmc->SetHelixParms(helixparams);
    
    //    //} // if false
    Double_t covMARS[6][6];
    for(int d=0;d<6;d++)for(int c=0;c<6;c++)covMARS[d][c]=0.;
    TVector3 mom=p4.Vect();
    TVector3 di = mom;
    di.SetMag(1.);
    TVector3 dj = di.Orthogonal();
    TVector3 dk = di.Cross(dj);
    FairTrackParP tStart(stvtx, mom, covMARS, (Int_t)charge, stvtx,  dj,  dk);
    Bool_t rc = Propagator(2,tStart,pmc,NULL,kTRUE);
    if(!rc && fVerbose>0) {
      Warning("BuildMcCands()","Faild propagation of mc particle no.%i to z axis",i);
      std::cout<<*pmc<<std::endl;
      stvtx.Print();
      std::cout<<"Mother pointer: "<<pmc->TheMother()<<std::endl;
    }
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
  if(fVerbose>2){ std::cout<<"MARS cov: ";err.Print();} 
  TLorentzVector lv = cand->P4();
  TMatrixD covPosMom = covTool.GetConverted7(covTool.GetFitError(lv, err));
  
  cand->SetCov7(covPosMom);
  Double_t Q=myResult->GetQ();
  
  // write helix parameters & helix cov to TCandidate/TFitParams
  Float_t helixparams[5];
  TMatrixD helixcov(5,5);
  rc = P7toHelix(pos, cand->P4(), Q, covPosMom, helixparams, helixcov, skipcov);
  if (!rc) {Warning("Propagator()","P7toHelix failed");return kFALSE;}
  
  cand->SetHelixParms(helixparams);
  
  Float_t rhohelixcov[15];
  Int_t klz=0;
  if(!skipcov){
    //for(int kli=0;kli<5;kli++) 
    //{
    //for(int klj=kli;klj<5;klj++)
    //{
    //rhohelixcov[klz]=helixcov[kli][klj];        
    //klz++;
    //}
    //}
    rhohelixcov[0]  = helixcov[0][0];
    rhohelixcov[1]  = helixcov[1][0];
    rhohelixcov[2]  = helixcov[2][0];
    rhohelixcov[3]  = helixcov[3][0];
    rhohelixcov[4]  = helixcov[4][0];
    rhohelixcov[5]  = helixcov[1][1];
    rhohelixcov[6]  = helixcov[2][1];
    rhohelixcov[7]  = helixcov[3][1];
    rhohelixcov[8]  = helixcov[4][1];
    rhohelixcov[9]  = helixcov[2][2];
    rhohelixcov[10] = helixcov[3][2];
    rhohelixcov[11] = helixcov[4][2];
    rhohelixcov[12] = helixcov[3][3];
    rhohelixcov[13] = helixcov[4][3];
    rhohelixcov[14] = helixcov[4][4];
    cand->SetHelixCov(rhohelixcov); 
  }
  
  if(fVerbose>2) {
    std::cout<<" :::::::::::  Printout in PndAnalysis::Propagator() :::::::::::  "<<std::endl;
    
    //std::cout<<"Start Params:"<<std::endl;
    //myStart->Print();
    std::cout<<"calculated helix Params:"
    <<"\nD0    ="<<helixparams[0]
    <<"\nPhi0  ="<<helixparams[1]
    <<"\nRho   ="<<helixparams[2]
    <<"\nZ0    ="<<helixparams[3]
    <<"\ncotTh ="<<helixparams[4]
    <<std::endl;
    
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


Bool_t PndAnalysis::P7toHelix(const TVector3 &pos, const TLorentzVector &p4, const Double_t Q, 
                              const TMatrixD &cov77, Float_t *helixparams, TMatrixD &helixCov, Bool_t skipcov)
{ 
  // Convert from fourmomentum (vx,vy,vz,px,py,pz,e)
  // to RHO helix parameters (D0,Phi0,rho(omega),Z0,tan(dip))
  // Assuming vx,vy,vz give the POCA to the z axis.
  if(p4.Perp()< 1e-9) {Warning("P7toHelix","Too small transverse momentum: %g",p4.Perp());return kFALSE;}
  Double_t pnt[3], Bf[3];
  pnt[0]=pos.X();
  pnt[1]=pos.Y();
  pnt[2]=pos.Z(); 
  FairRunAna::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
  //Double_t B = sqrt(Bf[0]*Bf[0]+Bf[1]*Bf[1]+Bf[2]*Bf[2]);
  Double_t B = Bf[2];
  //Double_t B = 20.;
  if(fVerbose>1)printf("P7ToHelix: BField is %g kGs\n",B);
  Double_t qBc = -0.000299792458*B*Q;//Mind factor from momenta being in GeV
  //Double_t pti=1/p4.Perp();
  //Double_t dfi=(pos.Phi()-p4.Phi())*TMath::RadToDeg();
  //if(dfi>180)dfi-=360;
  //if(dfi<-180)dfi+=360;
  //Double_t sign=-Q*((dfi>0)?1:-1); // TODO get a decent D0 sign!!!
  //helixparams[0]=sign*pos.Perp(); //D0
  //helixparams[1]=p4.Phi(); //phi0
  //helixparams[2]=qBc*pti; //omega=rho=1/R[cm]=-2.998*B[kGs]*Q[e]/p_perp[GeV/c] 
  //helixparams[3]=pos.Z(); //z0
  //helixparams[4]=p4.Pz()/p4.Perp(); //lambda(averey)=cot(theta)=tan(lambda(geane))
  if(fVerbose>1)printf("P7ToHelix: Charge is %g e-\n",Q);
  if(fVerbose>1)printf("P7ToHelix: QBc is %g \n",qBc);
  
  const double xp = pos.X();
  const double yp = pos.Y();
  const double zp = pos.Z();
  const double px = p4.Px();
  const double py = p4.Py();
  const double pz = p4.Pz();
  const double phip = TMath::ATan2(py,px);
  const double pti = 1/TMath::Sqrt(px*px+py*py);
  //const double phip = p4.Phi();
  //const double pti = 1/p4.Perp();
  if(fVerbose>1)printf("P7ToHelix: P_t is %g GeV/c\n",p4.Perp());
  if(fVerbose>1)printf("P7ToHelix: P_t^-1 is %g c/GeV\n",pti);
  
  // get rho
  const double rho = qBc*pti;
  if(fVerbose>1)printf("P7ToHelix: rho is %g cm^-1\n",rho);
  
  // get tan(dip)
  const double tanDip=pz*pti;
  const double R0 = 1./rho;
  if(fVerbose>1)printf("P7ToHelix: tanDip is %g \n",tanDip);
  if(fVerbose>1)printf("P7ToHelix: R0 = rho^-1 is %g cm\n",R0);
  
  //circle center
  const double xc = xp - py/qBc;
  const double yc = yp + px/qBc;
  //const double xc = xp - R0*p4.Py()*pti;
  //const double yc = yp + R0*p4.Px()*pti;
  //const double xc = xp - R0*TMath::Sin(phip);
  //const double yc = yp + R0*TMath::Cos(phip);
  const double DC = TMath::Sqrt(xc*xc+yc*yc);
  
  //get phi0 at doca
  const double phi0 = -TMath::ATan2(xc,yc);
  if(fVerbose>1)printf("P7ToHelix: phi0 is %g, phiP is %g, DeltaPhi = %g \n",phi0,phip,phip-phi0);
  
  //get D0
  const double D0 = DC - TMath::Abs(R0);
  //const double x0 = D0*TMath::Cos(phi0);
  //const double y0 = D0*TMath::Sin(phi0);
  if(fVerbose>1)printf("P7ToHelix: D0 is %g cm\n",D0);
  
  //get z0
  const double z0 = zp - pz*(phip-phi0)/qBc;
  //const double z0 = zp - tanDip*R0*(phip-phi0);
  if(fVerbose>1)printf("P7ToHelix: z0 is %g cm\n",z0);
  
  if(fVerbose>1)std::cout<<std::endl;
  helixparams[0]=D0;
  helixparams[1]=phi0;
  helixparams[2]=rho;
  helixparams[3]=z0;
  helixparams[4]=tanDip; // == lambda
  if(fVerbose>1) {
    for(int ai=0;ai<5;ai++){
      std::cout<<"helixparams["<<ai<<"]="<<helixparams[ai]<<std::endl;
    }
  }
  
  if(!skipcov)
  {
    TMatrixD jacobian(5,7); 
    
    jacobian[0][0] = xc/DC; // dD0  / dvx
    jacobian[0][1] = yc/DC; // dD0   /dvy
    jacobian[0][2] = 0.; // dD0   /dvz
    jacobian[0][3] = yc/(qBc*DC)-px*pti*pti/fabs(rho); // dD0   /dpx
    jacobian[0][4] = xc/(qBc*DC)-py*pti*pti/fabs(rho); // dD0   /dpy
    jacobian[0][5] = 0.; // dD0   /dpz
    jacobian[0][6] = 0.; // dD0   /de
    
    jacobian[1][0] = yc/(DC*DC); // dPhi0 /dvx
    jacobian[1][1] = -1.*xc/(DC*DC); // dPhi0 /dvy
    jacobian[1][2] = 0.; // dPhi0 /dvz
    jacobian[1][3] = -1.*xc/(DC*DC*qBc); // dPhi0 /dpx 
    jacobian[1][4] = -1.*yc/(DC*DC*qBc); // dPhi0 /dpy 
    jacobian[1][5] = 0.; // dPhi0 /dpz 
    jacobian[1][6] = 0.; // dPhi0 /de
    
    jacobian[2][0] = 0.; // drho  /dvx
    jacobian[2][1] = 0.; // drho  /dvy
    jacobian[2][2] = 0.; // drho  /dvz
    jacobian[2][3] = -1.*rho*px*pti*pti; // drho  /dpx
    jacobian[2][4] = -1.*rho*py*pti*pti; // drho  /dpy
    jacobian[2][5] = 0.; // drho  /dpz
    jacobian[2][6] = 0.; // drho  /de
    
    jacobian[3][0] = 0.; // dZ0   /dvx
    jacobian[3][1] = 0.; // dZ0   /dvy
    jacobian[3][2] = 1.; // dZ0   /dvz
    jacobian[3][3] = pz*yc/(DC*DC*qBc); // dZ0   /dpx
    jacobian[3][4] = -1.*pz*xc/(DC*DC*qBc); // dZ0   /dpy
    jacobian[3][5] = (phi0-phip)/qBc; // dZ0   /dpz
    jacobian[3][6] = 0.; // dZ0   /de
    
    jacobian[4][0] = 0.; // dtLam /dvx
    jacobian[4][1] = 0.; // dtLam /dvy
    jacobian[4][2] = 0.; // dtLam /dvz
    jacobian[4][3] = -1.*tanDip*px*pti*pti; // dtLam /dpx
    jacobian[4][4] = -1.*tanDip*py*pti*pti; // dtLam /dpy
    jacobian[4][5] = pti; // dtLam /dpz
    jacobian[4][6] = 0.; // dtLam /de
    
    //TMatrixD sigmas(cov77);
    //for(int cci=0;cci<7;cci++){
      //for(int ccj=cci+1;ccj<7;ccj++)
      //{
        //sigmas[cci][ccj]=0.;
        //sigmas[ccj][cci]=0.;
      //}
    //}
    //TMatrixD tempmat(jacobian,TMatrixD::kMult,sigmas);
    TMatrixD tempmat(jacobian,TMatrixD::kMult,cov77);
    TMatrixD covrho(tempmat,TMatrixD::kMultTranspose,jacobian);
    helixCov=covrho;
    
    if (fVerbose>2) {
      std::cout<<"cov77: "; cov77.Print();
      //std::cout<<"sigmas: "; sigmas.Print();
      std::cout<<"jacobian: "; jacobian.Print();
      std::cout<<"covrho: "; covrho.Print();
      if(fVerbose>1) {
        std::cout<<"helixparams[0] = D0 \t= ("<<helixparams[0]<<" \t+- "<<sqrt(helixCov[0][0])<<") cm"<<std::endl;
        std::cout<<"helixparams[1] = Phi0 \t= ("<<helixparams[1]<<" \t+- "<<sqrt(helixCov[1][1])<<") rad"<<std::endl;
        std::cout<<"helixparams[2] = rho \t= ("<<helixparams[2]<<" \t+- "<<sqrt(helixCov[2][2])<<") 1/cm"<<std::endl;
        std::cout<<"helixparams[3] = Z0 \t= ("<<helixparams[3]<<" \t+- "<<sqrt(helixCov[3][3])<<") cm"<<std::endl;
        std::cout<<"helixparams[4] = tanDip\t= ("<<helixparams[4]<<" \t+- "<<sqrt(helixCov[4][4])<<")"<<std::endl;
      }
      
    }
  } // skip cov or not
  return kTRUE;
}



