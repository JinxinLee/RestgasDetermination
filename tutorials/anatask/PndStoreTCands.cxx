/******************************************************

JPsi->e+e-
Reads the TPC tracks and reconstruct the InvariantMass 
of J/Psi: Dipak
*******************************************************/ 

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
//#include "TParticle.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLhePoint.h"
#include "CbmStack.h"
#include "CbmMCTrack.h"

#include "Track.h"
#include "LSLTrackRep.h"
#include "PndEmcCluster.h"


#include "TVector3.h"
#include "TVectorD.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndStoreTCands.h"
#include <string>
#include <iostream>

#include "RhoBase/TCandidate.h"
#include "RhoBase/PndMicroCandidate.h"

using std::cout;
using std::endl;


		
// -----   Default constructor   -------------------------------------------
PndStoreTCands::PndStoreTCands() :
  FairTask("FastSim Dump") { 
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndStoreTCands::~PndStoreTCands() {
  if (fChargedCandidates) {fChargedCandidates->Delete(); delete fChargedCandidates;}
  if (fNeutralCandidates) {fNeutralCandidates->Delete(); delete fNeutralCandidates;}
  if (fMcCandidates) {fMcCandidates->Delete(); delete fMcCandidates;}
  if (fMicroCandidates) {fMicroCandidates->Delete(); delete fMicroCandidates;}

 }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndStoreTCands::Init() {
 
  fStoreNeutral=true;
  fStoreCharged=true;
  fStoreMC=true;
 
  cout << " Inside the Init function****" << endl;
  
  //FairDetector::Initialize();
  //FairRun* sim = FairRun::Instance();
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndStoreTCands::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  // Get input array
  fTrArray = (TClonesArray*) ioman->GetObject("Track");
  if ( ! fTrArray) {
    cout << "-W- PndStoreTCands::Init: "
	 << "No TpcTrack array!" << endl;
    fTrArray=new TClonesArray("Track");
    fStoreCharged=false;
    //return kERROR;
  }
  fEmcArray = (TClonesArray*) ioman->GetObject("EmcCluster");
  if ( ! fEmcArray) {
    cout << "-W- PndStoreTCands::Init: "
	 << "No EmcCluster array!" << endl;
	 fEmcArray = new TClonesArray("EmcCluster");
    fStoreNeutral=false;
    //return kERROR;
  }
  
  fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrack) {
    cout << "-W- PndStoreTCands::Init: "
	 << "No MCTrack array!" << endl;
	 fMCTrack = new TClonesArray("MCTrack");
    fStoreMC=false;
    //return kERROR;
  }
  
  fChargedCandidates = new TClonesArray("TCandidate");
  FairRootManager::Instance()->Register("PndChargedCandidates","FullSim", fChargedCandidates, kTRUE);
  
  fNeutralCandidates = new TClonesArray("TCandidate");
  FairRootManager::Instance()->Register("PndNeutralCandidates","FullSim", fNeutralCandidates, kTRUE);
  
  fMcCandidates = new TClonesArray("TCandidate");
  FairRootManager::Instance()->Register("PndMcTracks","FullSim", fMcCandidates, kTRUE);

  fMicroCandidates = new TClonesArray("PndMicroCandidate");
  FairRootManager::Instance()->Register("PndMicroCandidates","FullSim", fMicroCandidates, kTRUE);

  // Create and register output array
  cout << "-I- PndStoreTCands: Intialization successfull" << endl;

  //fInvMass = new TH1D("invmass","",100,0.0,4.0);
  
  evtcnt=0;
  
  return kSUCCESS;

}

void PndStoreTCands::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

 
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndStoreTCands::Exec(Option_t* opt) 
{
  if ((++evtcnt)%100==0)
    cout <<"evt: "<<evtcnt<<endl;
  
  // find # in input array
  Int_t nTracks = 0;
  if (fStoreCharged) nTracks=fTrArray->GetEntriesFast();
  
  Int_t nCluster = 0;
  if (fStoreNeutral) nCluster=fEmcArray->GetEntriesFast();
  
  int nMCTrack=0;
  if (fStoreMC) nMCTrack=fMCTrack->GetEntriesFast();
  //CbmStack *fStack=(CbmStack*)gMC->GetStack();  
  //int nMCTracks=fStack->GetNtrack();

   // Reset output array
  if (fChargedCandidates->GetEntriesFast() != 0)  fChargedCandidates->Clear("C");
  if (fNeutralCandidates->GetEntriesFast() != 0)  fNeutralCandidates->Clear("C");
  if (fMcCandidates->GetEntriesFast() != 0)  fMcCandidates->Clear("C");
  if (fMicroCandidates->GetEntriesFast() != 0) fMicroCandidates->Clear("C");
  
  
  TClonesArray &chrgCandidates = *fChargedCandidates;
  TClonesArray &neutCandidates = *fNeutralCandidates;
  TClonesArray &mctracks 	   = *fMcCandidates;
  TClonesArray &microCandidates = *fMicroCandidates;

  
  for (Int_t imc=0; imc<nMCTrack; imc++)
  {
    Int_t mcsize = mctracks.GetEntriesFast();
	
	CbmMCTrack *t = (CbmMCTrack*)fMCTrack->At(imc);
	
	if (t->GetMotherID()!=-1) continue;
	
    TLorentzVector 	p4     = t->Get4Momentum();
    TVector3		stvtx  = t->GetStartVertex();
	int 			pdg    = t->GetPdgCode();
	TParticlePDG   *part   = TDatabasePDG::Instance()->GetParticle(pdg);
    double 			charge = -999;
	if (part) charge=part->Charge();
	
   	TCandidate *pmc=new (mctracks[mcsize]) TCandidate(p4,charge);
    pmc->SetPos(stvtx);
    pmc->SetMcIdx(imc);
	pmc->SetType(pdg);
	//cout<<"pdg:" <<pdg<<endl;
  }
  
  
  //  cout <<"number of tracks **** "<< nTracks <<endl;
  //PndTpcLheTrack *tr1;
  Track *tr1;
  PndEmcCluster *clus;
  
  
 
  for (Int_t i=0; i<nTracks; i++)
  {
    
    Int_t chcandsize = chrgCandidates.GetEntriesFast();
	Int_t micsize = microCandidates.GetEntriesFast();
    
    // Get the Tpc Track information
    tr1 = (Track *)fTrArray->At(i);  
   
    LSLTrackRep* myrep=dynamic_cast<LSLTrackRep*>(tr1->getCardinalRep());
    TVectorD d(6);
    d = myrep->getGlobal();
    
    TVector3 vtx(d[0],d[1],d[2]);
    TLorentzVector lv;
    lv.SetXYZM(d[3],d[4],d[5],0.13957);
    
    propagate(lv,vtx,myrep->getCharge());
      
	// create the TCandidate (keep this for the time being)
    TCandidate *tcand=new (chrgCandidates[chcandsize]) TCandidate(lv,myrep->getCharge());
    tcand->SetPos(vtx);
 
    //set the convariance matrix of tcand
   
    TMatrixD globalCov = myrep->getGlobalCov();
    TMatrixD mat(7,7);
    int ii,jj;
    for (ii=0;ii<6;ii++) for(jj=0;jj<6;jj++) mat[ii][jj]=globalCov[ii][jj]; 
    
    
    //Extend matrix for energy (with default pion hypothesis)
    double invE = 1./lv.E();
    mat[0+3][3+3] = mat[3+3][0+3] = (lv.X()*mat[0+3][0+3]+lv.Y()*mat[0+3][1+3]+lv.Z()*mat[0+3][2+3])*invE;
    mat[1+3][3+3] = mat[3+3][1+3] = (lv.X()*mat[0+3][1+3]+lv.Y()*mat[1+3][1+3]+lv.Z()*mat[1+3][2+3])*invE;
    mat[2+3][3+3] = mat[3+3][2+3] = (lv.X()*mat[0+3][2+3]+lv.Y()*mat[1+3][2+3]+lv.Z()*mat[2+3][2+3])*invE;
    mat[3+3][3+3] = (lv.X()*lv.X()*mat[0+3][0+3]+lv.Y()*lv.Y()*mat[1+3][1+3]+lv.Z()*lv.Z()*mat[2+3][2+3]
			+2.0*lv.X()*lv.Y()*mat[0+3][1+3]
			+2.0*lv.X()*lv.Z()*mat[0+3][2+3]
			+2.0*lv.Y()*lv.Z()*mat[1+3][2+3])*invE*invE;
			
    mat[3+3][4-4] = mat[4-4][3+3] = (lv.X()*mat[0+3][4-4]+lv.Y()*mat[1+3][4-4]+lv.Z()*mat[2+3][4-4])*invE;
    mat[3+3][5-4] = mat[5-4][3+3] = (lv.X()*mat[0+3][5-4]+lv.Y()*mat[1+3][5-4]+lv.Z()*mat[2+3][5-4])*invE;
    mat[3+3][6-4] = mat[6-4][3+3] = (lv.X()*mat[0+3][6-4]+lv.Y()*mat[1+3][6-4]+lv.Z()*mat[2+3][6-4])*invE;

    tcand->SetCov7(mat);
    
	// create the PndMicroCandidate
	PndMicroCandidate *micro=new (microCandidates[micsize])	PndMicroCandidate((Int_t)myrep->getCharge(),vtx,lv,mat);
  	
	unsigned int detId, hitId;
	unsigned int numhits=0,mvdhits=0,stthits=0,tpchits=0;
	
	numhits=tr1->getCand().getNHits();
	
	for (ii=0;ii<numhits;++ii)
	{ 
		tr1->getCand().getHit(ii,detId,hitId);
		switch (detId)
		{
		case 0:  mvd_hitidx[mvdhits]=hitId; if(mvdhits<1000) mvdhits++; break;
		case 1:  stt_hitidx[stthits]=hitId; if(stthits<1000) stthits++;break;
		default: tpc_hitidx[tpchits]=hitId; if(tpchits<1000) tpchits++;break;
		}
	}
	micro->SetMvdHitIndexArray(mvdhits, mvd_hitidx);
	micro->SetSttHitIndexArray(stthits, stt_hitidx);
	micro->SetTpcHitIndexArray(tpchits, tpc_hitidx);
  }
  
  for (Int_t i=0; i<nCluster; i++)
  {
    Int_t ncandsize = neutCandidates.GetEntriesFast();
	Int_t micsize = microCandidates.GetEntriesFast();
    
    // Get the Tpc Track information
    clus = (PndEmcCluster *)fEmcArray->At(i);  
    
	TVector3 vtx(0,0,0);
    TVector3 v1=clus->where();
    
    TVector3 p3;
    TLorentzVector lv;
	TMatrixD covP4=clus->Get4MomentumErrorMatrix();
    
    p3.SetMagThetaPhi(clus->energy(), v1.Theta(), v1.Phi());
    
    lv.SetVectM(p3,0.);
    
    TCandidate *tcand=new (neutCandidates[ncandsize]) TCandidate(lv,0.);
	tcand->SetCovP4(covP4);
	
	// create the PndMicroCandidate
	PndMicroCandidate *micro=new (microCandidates[micsize])	PndMicroCandidate(0,vtx,lv);
	micro->SetP4Cov(covP4);
  }


}
// -------------------------------------------------------------------------

void PndStoreTCands::Finish()
{
   fChargedCandidates->Delete();
   fNeutralCandidates->Delete(); 
 //fInvMass->Write();
}

// -------------------------------------------------------------------------


void PndStoreTCands::propagate(TLorentzVector &l, TVector3 &p, float charge)
{
  double x0=p.X()/100;
  double y0=p.Y()/100;
  double z0=p.Z()/100;
  
  double px0=l.Px();
  double py0=l.Py();
  double pz0=l.Pz();
  
  double B=2;
  
  double pt=sqrt(px0*px0+py0*py0);
  double lambda=pz0/pt;
  double s_t=z0/lambda;
  double a=-0.2998*B*charge;
  double rho=a/pt;
    
  double cosrs=cos(rho*s_t);
  double sinrs=sin(rho*s_t);
  
  double px = px0*cosrs + py0*sinrs;
  double py = py0*cosrs - px0*sinrs;
  double pz = pz0;
  
  double x=x0 - px/a*sinrs + py/a*(1-cosrs); 
  double y=y0 - py/a*sinrs - px/a*(1-cosrs); 
  double z=z0 - lambda*s_t;
  
  l.SetX(px);
  l.SetY(py);
  l.SetZ(pz);
  
  p.SetX(x*100);
  p.SetY(y*100);
  p.SetZ(z*100);
}

ClassImp(PndStoreTCands)
