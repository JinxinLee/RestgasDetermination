/******************************************************
Class PndMicroWriter

Collects Micro infromation from Reconstruction and
writes out PndMicroCandidates

Author: K.Goetzen, GSI, 06/2008

*******************************************************/ 

#include "PndMicroWriter.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
//#include "TParticle.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndLhePidTrack.h"
#include "PndStack.h"
#include "PndMCTrack.h"
#include "FairMCPoint.h"

#include "Track.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"
#include "PndEmcCluster.h"
#include "PndDetectorList.h"

#include "TVector3.h"
#include "TVectorD.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include <string>
#include <iostream>

#include "RhoBase/TCandidate.h"
#include "RhoBase/PndMicroCandidate.h"
#include "RhoTools/TEventShape.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/PndEventInfo.h"
#include "RhoBase/TFactory.h"
#include "RhoBase/TRho.h"

using std::cout;
using std::endl;


		
// -----   Default constructor   -------------------------------------------
PndMicroWriter::PndMicroWriter(TString inArrName) :
  FairTask("FastSim Dump") {
  fInArrName=inArrName;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMicroWriter::~PndMicroWriter() {
  if (fChargedCandidates) {fChargedCandidates->Delete(); delete fChargedCandidates;}
  if (fNeutralCandidates) {fNeutralCandidates->Delete(); delete fNeutralCandidates;}
  if (fMcCandidates) {fMcCandidates->Delete(); delete fMcCandidates;}
  if (fMicroCandidates) {fMicroCandidates->Delete(); delete fMicroCandidates;}
  if (fEventInfo) {fEventInfo->Delete(); delete fEventInfo;}

 }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndMicroWriter::Init() 
{
 
  fStoreNeutral=false;
  fStoreTrack=false;
  fStoreLheTrack=false;
  fStoreMC=false;
 
  //TDatabasePDG *dbpdg=TDatabasePDG::Instance();
  
  //TRho::Instance()->SetPDG(dbpdg);
  
  cout << " Inside the Init function****" << endl;
  
  //FairDetector::Initialize();
  //FairRun* sim = FairRun::Instance();
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndMicroWriter::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fTrArray = (TClonesArray*) ioman->GetObject(fInArrName);
  if ( ! fTrArray) {
    cout << "-W- PndMicroWriter::Init: "
	 << "No Track array!" << endl;
    fTrArray=new TClonesArray("Track");
 
   // return kERROR;
  } else    fStoreTrack=true;
  
  // Get input array
  fLheTrArray = (TClonesArray*) ioman->GetObject("LhePidTrack");
  if ( ! fLheTrArray) {
    cout << "-W- PndMicroWriter::Init: "
	 << "No LhePidTrack array!" << endl;
    fLheTrArray=new TClonesArray("PndLhePidTrack");
//    return kERROR;
  } else 
     fStoreLheTrack=true;
 
  fEmcArray = (TClonesArray*) ioman->GetObject("EmcCluster");
  if ( ! fEmcArray) {
    cout << "-W- PndMicroWriter::Init: "
	 << "No EmcCluster array!" << endl;
	 fEmcArray = new TClonesArray("EmcCluster");
  //  return kERROR;
  } else 
      fStoreNeutral=true;

  
  fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrack) {
    cout << "-W- PndMicroWriter::Init: "
	 << "No MCTrack array!" << endl;
	 fMCTrack = new TClonesArray("MCTrack");
  } else
    fStoreMC=true;
  
  fChargedCandidates = new TClonesArray("TCandidate");
  FairRootManager::Instance()->Register("PndChargedCandidates","FullSim", fChargedCandidates, kTRUE);
  
  fNeutralCandidates = new TClonesArray("TCandidate");
  FairRootManager::Instance()->Register("PndNeutralCandidates","FullSim", fNeutralCandidates, kTRUE);
  
  fMcCandidates = new TClonesArray("TCandidate");
  FairRootManager::Instance()->Register("PndMcTracks","FullSim", fMcCandidates, kTRUE);

  fMicroCandidates = new TClonesArray("PndMicroCandidate");
  FairRootManager::Instance()->Register("PndMicroCandidates","FullSim", fMicroCandidates, kTRUE);

  fEventInfo = new TClonesArray("PndEventInfo");
  FairRootManager::Instance()->Register("PndEventSummary","FullSim", fEventInfo, kTRUE);

  // Create and register output array
  cout << "-I- PndMicroWriter: Intialization successfull" << endl;

  //fInvMass = new TH1D("invmass","",100,0.0,4.0);
  
  evtcnt=0;
  
  return kSUCCESS;

}

void PndMicroWriter::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

 
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMicroWriter::Exec(Option_t* opt) 
{
  if ((++evtcnt)%100==0)
    cout <<"evt: "<<evtcnt<<endl;
  
  // find # in input array
  Int_t nTracks = 0;
  if (fStoreTrack) nTracks=fTrArray->GetEntriesFast();
  
  Int_t nLheTracks = 0;
  if (fStoreLheTrack) nLheTracks=fLheTrArray->GetEntriesFast();
  
  Int_t nCluster = 0;
  if (fStoreNeutral) nCluster=fEmcArray->GetEntriesFast();
  
  int nMCTrack=0;
  if (fStoreMC) nMCTrack=fMCTrack->GetEntriesFast();
  //PndStack *fStack=(PndStack*)gMC->GetStack();  
  //int nMCTracks=fStack->GetNtrack();

   // Reset output array
  if (fChargedCandidates->GetEntriesFast() != 0)  fChargedCandidates->Clear("C");
  if (fNeutralCandidates->GetEntriesFast() != 0)  fNeutralCandidates->Clear("C");
  if (fMcCandidates->GetEntriesFast() != 0)  fMcCandidates->Clear("C");
  if (fMicroCandidates->GetEntriesFast() != 0) fMicroCandidates->Clear("C");
  if (fEventInfo->GetEntriesFast() != 0) fEventInfo->Clear("C");
 
  
  TClonesArray &chrgCandidates  = *fChargedCandidates;
  TClonesArray &neutCandidates  = *fNeutralCandidates;
  TClonesArray &mctracks 	    = *fMcCandidates;
  TClonesArray &microCandidates = *fMicroCandidates;
  TClonesArray &evtInfo         = *fEventInfo;

  TCandList l;
  
  TLorentzVector McSumP4(0,0,0,0);
  TVector3 McAvgVtx(0,0,0);
  
  int nPrimary=0;
  
  for (Int_t imc=0; imc<nMCTrack; imc++)
  {
    Int_t mcsize = mctracks.GetEntriesFast();
    
    PndMCTrack *t = (PndMCTrack*)fMCTrack->At(imc);
    
    if (t->GetMotherID()!=-1) continue;
    
    nPrimary++;
	 
    
    TLorentzVector 	p4     = t->Get4Momentum();
    TVector3		stvtx  = t->GetStartVertex();
    int 			pdg    = t->GetPdgCode();
    TParticlePDG   *part   = TRho::Instance()->GetPDG()->GetParticle(pdg);
    //TParticlePDG   *part   = TDatabasePDG::Instance()->GetParticle(pdg);
    double 			charge = -999;
    if (part) charge=part->Charge();
    if (charge!=0) charge/=fabs(charge);
    
   	TCandidate *pmc=new (mctracks[mcsize]) TCandidate(p4,charge);
    pmc->SetPos(stvtx);
    pmc->SetMcIdx(imc);
    pmc->SetType(pdg);

    McSumP4+=p4;
    McAvgVtx+=stvtx;
    
    //cout<<"pdg:" <<pdg<<endl;
  }
  McAvgVtx*=1./(double)nPrimary;
  
  
    cout <<"number of tracks **** "<< nTracks <<endl;
  
  Track *tr1;
  PndEmcCluster *clus;
  PndLhePidTrack *lhetr;
    
   // *************************
  // Loop over the charged LHE tracks
  // ************************
  
  for (Int_t i=0; i<nLheTracks; i++)
  {
    //Int_t chcandsize = chrgCandidates.GetEntriesFast();
  	Int_t micsize = microCandidates.GetEntriesFast();
    
    cout<<"-I- PndMicroWriter::Exec(): Array fLheTrArray "<<fLheTrArray<<" with "
    << fLheTrArray->GetEntriesFast() <<" entries, tried to acces entry "<<i<<"."<<endl;
    fLheTrArray->Print();

    lhetr = (PndLhePidTrack *)fLheTrArray->At(i);  
    
	  TVector3 mom(0,0,0);
	  TVector3 pos(0,0,0);
    
	  lhetr->ExtrapolateToZ(&mom,&pos);
    
  	TLorentzVector lv(0,0,0,0);
	  lv.SetVectM(mom,0.13957);
    
	  // create the PndMicroCandidate
	  PndMicroCandidate *micro=new (microCandidates[micsize])    PndMicroCandidate((Int_t)lhetr->GetCharge(),pos,lv);
    
  	if ( lhetr->GetMvdHitCounts()>0 ) 
	  	micro->SetMvdMeanDEdx(lhetr->GetMvdELoss()/lhetr->GetMvdHitCounts());
	  else 
	  	micro->SetMvdMeanDEdx(-1.0);
		
  	micro->SetEmcRawEnergy(lhetr->GetEmcELoss());
    
    cout<<"-I- PndMicroWriter::Exec(): Array fEmcArray "<<fEmcArray<<" with "
    << fEmcArray->GetEntriesFast() <<" entries, tried to acces entry "<<lhetr->GetEmcIndex()<<"."<<endl;
    fEmcArray->Print();
    
    if (lhetr->GetEmcIndex()!=-1 &&  fEmcArray->GetEntriesFast()>lhetr->GetEmcIndex()) {
      if(0!=fEmcArray->At(lhetr->GetEmcIndex()))
        micro->SetEmcNumberOfCrystals( ((PndEmcCluster*)fEmcArray->At(lhetr->GetEmcIndex()))->NumberOfDigis() );
    }
	  micro->SetTofStopTime(lhetr->GetTof());
  	micro->SetBarrelDrcThetaC(lhetr->GetDrcThetaC());
  	micro->SetBarrelDrcThetaCErr(lhetr->GetDrcThetaCErr());
  	micro->SetBarrelDrcNumberOfPhotons(lhetr->GetDrcNPhotons());
  	micro->SetMvdMeanDEdx(lhetr->GetMvdDEDX());
  }
  
  // *************************
  // Loop over the charged genfit tracks
  // ************************
  
//  std::vector<TVector3>       posCache;
//  std::vector<TLorentzVector> p4Cache;
  
  for (Int_t i=0; i<nTracks; i++)
  {
    
    Int_t chcandsize = chrgCandidates.GetEntriesFast();
	  Int_t micsize = microCandidates.GetEntriesFast();
    
    // Get the Tpc Track information
    cout<<"-I- PndMicroWriter::Exec(): Array fTrArray "<<fTrArray<<" with "
    << fTrArray->GetEntriesFast() <<" entries, tried to acces entry "<<i<<"."<<endl;
    fTrArray->Print();
	  tr1 = (Track *)fTrArray->At(i);  
    if(tr1 == 0){
      cout<<"-E- PndMicroWriter::Exec(): Track object is not there. Array fTrArray with "
        << fTrArray->GetEntriesFast() <<" entries, tried to acces entry "<<i<<"."<<endl;
    }
    cout<<"-I- PndMicroWriter::Exec(): Info on Track* tr1:";
    cout<<"\n\tpos ("<<(tr1->getPos()).X()<<","<<(tr1->getPos()).Y()<<","<<(tr1->getPos()).Z()<<") ";
    cout<<"\n\tmom ("<<(tr1->getMom()).X()<<","<<(tr1->getMom()).Y()<<","<<(tr1->getMom()).Z()<<") ";
    cout<<"\n\tcharge "<<tr1->getCharge()<<" \tchisquare "<<tr1->getChiSqu()<<" \tcardinal rep. pointer"<<tr1->getCardinalRep();
    cout<<endl;

    AbsTrackRep* myrep2=tr1->getCardinalRep();
    cout<<"-I- PndMicroWriter::Exec(): Pointer myrep2 "<<myrep2<<endl;
    myrep2->Print();

    if(false)
    {
      LSLTrackRep* myrep=dynamic_cast<LSLTrackRep*>(tr1->getCardinalRep());
      //LSLTrackRep* myrep=(LSLTrackRep*)tr1->getCardinalRep();
      //AbsTrackRep* myrep= tr1->getCardinalRep();
      cout<<"-I- PndMicroWriter::Exec(): Pointer myrep "<<myrep<<endl;
      TVectorD d(6);
      d = myrep->getGlobal();
    
      TVector3 vtx(d[0],d[1],d[2]);
      TLorentzVector lv;
      lv.SetXYZM(d[3],d[4],d[5],0.13957);
    
	 //   posCache.push_back(vtx);
  	//  p4Cache.push_back(lv);
    
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
    
	    l.Add(*tcand);
    
	    // create the PndMicroCandidate
	    PndMicroCandidate *micro=new (microCandidates[micsize])    PndMicroCandidate((Int_t)myrep->getCharge(),vtx,lv,mat);
	    unsigned int detId, hitId;
	    unsigned int numhits=0,mvdhits=0,stthits=0,tpchits=0;
    
	    numhits=tr1->getCand().getNHits();
    
	    for (unsigned int kk=0;kk<numhits;++kk)
	    { 
		    tr1->getCand().getHit(kk,detId,hitId);
		    switch (detId)
		    {
		      case kMVD:  mvd_hitidx[mvdhits]=hitId; if(mvdhits<1000) mvdhits++; break;
		      case kSTT:  stt_hitidx[stthits]=hitId; if(stthits<1000) stthits++;break;
		      default: tpc_hitidx[tpchits]=hitId; if(tpchits<1000) tpchits++;break;
 		    }
	    }
	    micro->SetMvdHitIndexArray(mvdhits, mvd_hitidx);
	    micro->SetSttHitIndexArray(stthits, stt_hitidx);
	    micro->SetTpcHitIndexArray(tpchits, tpc_hitidx);
     
    } else
    {
      /*
      GeaneTrackRep* myrep=dynamic_cast<GeaneTrackRep*>(tr1->getCardinalRep()); // assuming we know it is GeaneTrackRep
      cout<<"-I- PndMicroWriter::Exec(): Pointer myrep "<<myrep<<endl;
      
      TVector3 o(0.,0.,0.), u(1.,0.,0.), v(0.,1.,0.);
      DetPlane detplane(o,u,v);
      
      TMatrixD state, stateCov;      
      
      myrep->extrapolate(detplane,state,stateCov); // go to a defined point. //TODO: Here should be the particle vertex at some point, right?
      
      ...
 */    
      // genfit/Track::getPosMomCov(const DetPlane& pl,TVector3& pos,TVector3& mom,TMatrixT<double>& cov)
      // is existing now. cov is 6x6 large
//TODO: develop further, mind the covariances (how to ghet them from the Track* ?
/*
      TVector3 vtx,mom;
      mom = myrep->
      TLorentzVector lv;
      lv.SetXYZM(mom.x(),mom.y(),mom.z(),0.13957); // initial pion hypothesis
      
      propagate(lv,vtx,myrep->getCharge());
      
	    // create the TCandidate (keep this for the time being)
      TCandidate *tcand=new (chrgCandidates[chcandsize]) TCandidate(lv,myrep->getCharge());
      tcand->SetPos(vtx);
      
      //set the convariance matrix of tcand
      
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
      l.Add(*tcand);
      
	    // create the PndMicroCandidate
	    PndMicroCandidate *micro=new (microCandidates[micsize])    PndMicroCandidate((Int_t)myrep->getCharge(),vtx,lv,mat);
	    unsigned int detId, hitId;
	    unsigned int numhits=0,mvdhits=0,stthits=0,tpchits=0;
      
	    numhits=tr1->getCand().getNHits();
      
	    for (unsigned int kk=0;kk<numhits;++kk)
	    { 
		    tr1->getCand().getHit(kk,detId,hitId);
		    switch (detId)
		    {
            case kMVD:  mvd_hitidx[mvdhits]=hitId; if(mvdhits<1000) mvdhits++; break;
            case kSTT:  stt_hitidx[stthits]=hitId; if(stthits<1000) stthits++;break;
            default: tpc_hitidx[tpchits]=hitId; if(tpchits<1000) tpchits++;break;
 		    }
	    }
	    micro->SetMvdHitIndexArray(mvdhits, mvd_hitidx);
	    micro->SetSttHitIndexArray(stthits, stt_hitidx);
	    micro->SetTpcHitIndexArray(tpchits, tpc_hitidx);
      
      
  */    
      

    }

  }
  
  // *************************
  // Loop over the neutral clusters
  // ************************
  
  double calFactor=1.035;
  
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
    
    p3.SetMagThetaPhi(clus->energy()*calFactor, v1.Theta(), v1.Phi());
    
    lv.SetVectM(p3,0.);
    
    TCandidate *tcand=new (neutCandidates[ncandsize]) TCandidate(lv,0.);
    tcand->SetCovP4(covP4);
    
    l.Add(*tcand);
    
    // create the PndMicroCandidate
    PndMicroCandidate *micro=new (microCandidates[micsize])    PndMicroCandidate(0,vtx,lv);
    micro->SetP4Cov(covP4);
    micro->SetEmcRawEnergy(clus->energy());
    micro->SetEmcCalEnergy(clus->energy()*calFactor);
    //micro->SetEmcNumberOfBumps(clus->nBumps());
    micro->SetEmcNumberOfCrystals(clus->NumberOfDigis());
    
  }
  
  PndEventInfo *eventInfo=new (evtInfo[evtInfo.GetEntriesFast()]) PndEventInfo();
  eventInfo->SetIPTruth(McAvgVtx);
  eventInfo->SetCmFrame(McSumP4);
  eventInfo->SetCharged(nTracks);
  eventInfo->SetNeutrals(nCluster);
  
  TEventShape shape(l);
  eventInfo->SetEventShape(shape);
  
  
  // some cleanup 
  
  TFactory::Instance()->Reset();
  

  if (fTrArray) fTrArray->Delete();
  if (fLheTrArray) fLheTrArray->Delete();
  if (fEmcArray) fEmcArray->Delete();
  if (fMCTrack) fMCTrack->Delete();
 
}
// -------------------------------------------------------------------------

void PndMicroWriter::Finish()
{
   fChargedCandidates->Delete();
   fNeutralCandidates->Delete(); 
   fMcCandidates->Delete(); 
   fMicroCandidates->Delete();
   fEventInfo->Delete(); 
 //fInvMass->Write();
}

// -------------------------------------------------------------------------


void PndMicroWriter::propagate(TLorentzVector &l, TVector3 &p, float charge)
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
  
  double x=x0 - px0/a*sinrs + py0/a*(1-cosrs); 
  double y=y0 - py0/a*sinrs - px0/a*(1-cosrs); 
  double z=z0 - lambda*s_t;
  
  l.SetX(px);
  l.SetY(py);
  l.SetZ(pz);
  
  p.SetX(x*100);
  p.SetY(y*100);
  p.SetZ(z*100);
}

ClassImp(PndMicroWriter)
