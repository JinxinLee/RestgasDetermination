// -------------------------------------------------------------------------
// -----          PndGlobalPrepareKalmanTracks  source file            -----
// -----            Created 13.01.2010  by R. Karabowicz               -----
// -----        based on the recotasks/demo code  by S.Neubert         -----
// -------------------------------------------------------------------------

// Panda Headers ----------------------
#include "FairRootManager.h"
#include "PndDetectorList.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "PndMCTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFKalman.h"
#include "GFException.h"
#include "FairGeanePro.h"
#include "FairTrackParP.h"
#include "PndGlobalPrepareKalmanTracks.h"
#include "PndGemMCPoint.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"

// ROOT Class Headers --------
#include "TClonesArray.h"
#include "TDatabasePDG.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

//using namespace::std;

PndGlobalPrepareKalmanTracks::PndGlobalPrepareKalmanTracks()
  : FairTask("Translation of PndGlobalTracks to Tracks"), fPersistence(kFALSE), fUseGeane(kFALSE), fPDG(11), fUseMC(kFALSE), fMinNofHits(5)
{
}


PndGlobalPrepareKalmanTracks::~PndGlobalPrepareKalmanTracks()
{
  if( 0!= fOutputTrackArray){
    fOutputTrackArray->Delete();
    delete fOutputTrackArray;
  }
  if( 0!= fGeanePro)
    delete fGeanePro;
}

InitStatus PndGlobalPrepareKalmanTracks::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0) {
    Error("PndGlobalPrepareKalmanTracks::Init","RootManager not instantiated!");
    return kERROR;
  }
  
  if(fUseMC){
    // open MCTruth array
    fMcArray=(TClonesArray*) ioman->GetObject("MCTrack");
    if(fMcArray==0){
      Error("PndGlobalPrepareKalmanTracks::Init","mctrack-array not found!");
      return kERROR;
    } 
  }

  // open input array of PndGlobalTracks
  fGlobalTrackArray = (TClonesArray*) ioman->GetObject("GlobalTrackCand"); 
  if(fGlobalTrackArray==0){
    Error("PndGlobalPrepareKalmanTracks::Init","PndGlobalTrack array not found!");
    return kERROR;
  }
  
  // create and register output array
  fOutputTrackArray = new TClonesArray("GFTrack"); 
  ioman->Register("GlobalTrack","GenFit",fOutputTrackArray,fPersistence);
  
  // GeanePro will get Geometry and BField from the Run
  fGeanePro=new FairGeanePro();
  
  return kSUCCESS;
}

void PndGlobalPrepareKalmanTracks::Exec(Option_t* opt)
{
  if(fVerbose>0){
    std::cout<<"\n\n-------------------------------------------------------"<<std::endl;
    std::cout<<"      PndGlobalPrepareKalmanTracks::Exec               "<<std::endl;
    std::cout<<"-------------------------------------------------------"<<std::endl;
  }
  if(fOutputTrackArray==0) Fatal("PndGlobalPrepareKalmanTracks::Exec)","No TrackArray");
  fOutputTrackArray->Delete();
  
  Int_t nofGlobalTracks = fGlobalTrackArray->GetEntriesFast();

  // loop over tracks
  std::cout << "Looping over " << nofGlobalTracks << " global tracks" << endl;
  for (Int_t itr=0; itr<nofGlobalTracks; itr++){
    if(fVerbose>0)
      std::cout<<"PndGlobalPrepareKalmanTracks::Exec(): Processing global track id= "<<itr<<std::endl;
    PndTrackCand* globalTrackCand = (PndTrackCand*) fGlobalTrackArray->At(itr);
    Int_t nofGlobalHits = globalTrackCand->GetNHits();
    if(fVerbose>0)
      std::cout<<"PndGlobalPrepareKalmanTracks::Exec(): I found here "<<nofGlobalHits<<" hits \n";
    
    GFTrackCand* cand = new GFTrackCand();

    for(Int_t ihit=0; ihit<nofGlobalHits; ihit++){
      PndTrackCandHit tch = globalTrackCand->GetSortedHit(ihit);
      cand->addHit(tch.GetDetId(),tch.GetHitId());
    }
    
    if(cand->getNHits()<fMinNofHits)
      continue;
    
    Int_t pdg;
    Double_t q;
    TVector3 pos, mom;

    pos  = globalTrackCand->getPosSeed();
    mom  = globalTrackCand->getDirSeed();
    mom *= 1./TMath::Abs(globalTrackCand->getQoverPseed());
    q    = ( globalTrackCand->getQoverPseed()>0 ? +1 : -1 );
    pdg = fPDG;

    if ( fUseMC ) {
      Int_t globalMCTrackID = globalTrackCand->getMcTrackId();
      std::cout << "!!!!!!!! THIS TRACK HAD MC ID " << globalMCTrackID << " !!!!!!" << std::endl;
    
      PndMCTrack* mctrack = (PndMCTrack*)fMcArray->At(globalMCTrackID);
      pdg = mctrack->GetPdgCode();
      q   = TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.; 
    }
      
    std::cout<<"positions and momenta initialised as follows: ... "<<std::endl;
    std::cout<<"position: " << std::flush;
    pos.Print();
    std::cout<<"momentum: " << std::flush;
    mom.Print();
    if(mom.Mag()>1e3){
      Error("PndGlobalPrepareKalmanTracks::Exec",
	    "Track was incorrectly prefitted - abandoned in Kalman!");
      continue;
    }

    Double_t startPosAccuracy = 0.01;
    TVector3 poserr(startPosAccuracy,startPosAccuracy,startPosAccuracy);
    TVector3 startMomAccuracy(0.001,0.001,0.001);
    TVector3 momerr(mom.X()*startMomAccuracy.X(),
		    mom.Y()*startMomAccuracy.Y(),
		    mom.Z()*startMomAccuracy.Z());    
    TVector3 u(1.,0.,0.);
    TVector3 v(0.,1.,0.);
    
    // create track-representation object and initialize with start values
    GFAbsTrackRep* rep=0;
    if(fUseGeane){
      GFDetPlane pl(pos,u,v);
      GeaneTrackRep *grep=new GeaneTrackRep(fGeanePro,pl,mom,poserr,momerr,q,pdg);
      grep->setPropDir(1); // propagate in flight direction
      //  if(fVerbose>0){
      std::cout<<" ^^^^^^^^^^^^^I prepare the following GeaneTrackRep:"<<std::endl;
      grep->Print();
      std::cout<<" ^^^^^^^^^^^^^End of GeaneTrackRep printout"<<std::endl;
      //}
      rep = grep;
    }
    else {
      TVector3 dir=mom.Unit();
      double dxdz=dir.X()/dir.Z();
      double dydz=dir.Y()/dir.Z();
      double qp=q/mom.Mag();
      rep=new LSLTrackRep(pos.Z(),pos.X(),pos.Y(),dxdz,dydz,qp,
			  poserr.X(),poserr.Y(),0.1,0.1,0.1,NULL);
      if(fVerbose>0){
	std::cout<<" ^^^^^^^^^^^^^I prepare the following LSLTrackRep:"<<std::endl;
	rep->Print();
	std::cout<<" ^^^^^^^^^^^^^End of LSLTrackRep printout"<<std::endl;
      }
    }
    // create track object
    GFTrack* trk=new((*fOutputTrackArray)[fOutputTrackArray->GetEntriesFast()]) GFTrack(rep);
    trk->setCandidate(*cand);
    cout << "!!!!candidate " <<fOutputTrackArray->GetEntriesFast() <<  " has " << cand->getNHits() << " hits" << endl;
  }
  
  if(fVerbose>0)
    std::cout<<fOutputTrackArray->GetEntriesFast()<<" tracks created"<<std::endl;
  
  return;
}

ClassImp(PndGlobalPrepareKalmanTracks)



