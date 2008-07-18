
// Panda Headers ----------------------
#include "CbmRootManager.h"
#include "Track.h"
#include "TrackCand.h"
#include "CbmMCTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "CbmGeanePro.h"
#include "CbmTrackParP.h"
#include "PndDchPrepareKalmanTracks.h"
#include "PndDchTrack.h"
#include "PndDchTrackMatch.h"
#include "PndDchCylinderHit.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// ROOT Class Headers --------
#include "TClonesArray.h"



PndDchPrepareKalmanTracks::PndDchPrepareKalmanTracks()
  : CbmTask("Translation of PndDchTracks to Tracks"), fPersistence(kFALSE), fUseGeane(kFALSE)
{
}


PndDchPrepareKalmanTracks::~PndDchPrepareKalmanTracks()
{
}

InitStatus
PndDchPrepareKalmanTracks::Init()
{
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndDchPrepareKalmanTracks::Init","RootManager not instantiated!");
      return kERROR;
    }
 
  AddHitBranch(1,"PndDchCylinderHit");

  // open hit arrays
  std::map<unsigned int,TString>::iterator iter=fHitBranchNameMap.begin();
  while(iter!=fHitBranchNameMap.end()){
   TClonesArray* ar=(TClonesArray*) ioman->GetObject(iter->second);
   if(ar==0){
     Error("DemoPRTask::Init","point-array %s not found!",iter->second.Data());
   }
   else{ 
	 fHitBranchMap[iter->first] = ar;
   }
  ++iter;
  }//end loops over hit types
  fCHitArray = fHitBranchMap[1];
  

  // open MCTruth array
  fMcArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(fMcArray==0){
    Error("PndDchPrepareKalmanTracks::Init","mctrack-array not found!");
    return kERROR;
  } 
  // open input array of PndDchTracks
  fDchTrackArray = (TClonesArray*) ioman->GetObject("PndDchTrack"); 
  if(fDchTrackArray==0){
    Error("PndDchPrepareKalmanTracks::Init","PndDchTrack array not found!");
    return kERROR;
  }
  std::cout<<"DchTrack array found "<< fDchTrackArray<<std::endl;

  // open input array of PndDchTrackMatches
  fDchTrackMatchArray = (TClonesArray*) ioman->GetObject("PndDchTrackMatch"); 
  if(fDchTrackMatchArray==0){
    Error("PndDchPrepareKalmanTracks::Init","PndDchTrackMatch array not found!");
    return kERROR;
  }
  std::cout<<"DchTrackMatch array found "<< fDchTrackMatchArray<<std::endl;
 
 
  // create and register output array
  fTrackArray = new TClonesArray("Track"); 
  ioman->Register("Track","GenFit",fTrackArray,fPersistence);
  std::cout<<"Track array created "<< fTrackArray<<std::endl;
 
  
  // GeanePro will get Geometry and BField from the Run
  fGeanePro=new CbmGeanePro();
  std::cout<<"Geane created "<< fGeanePro<<std::endl;
  
  return kSUCCESS;
}


void
PndDchPrepareKalmanTracks::Exec(Option_t* opt)
{
  std::cout<<"\n\n-------------------------------------------------------"<<std::endl;
  std::cout<<"      PndDchPrepareKalmanTracks::Exec                      "<<std::endl;
  std::cout<<"-------------------------------------------------------"<<std::endl;
  // Reset output Array
  if(fTrackArray==0) Fatal("PndDchPrepareKalmanTracks::Exec)","No TrackArray");
  fTrackArray->Delete();

   std::map<unsigned int,TrackCand*> candmap;
   
   Int_t nuOfTracks = fDchTrackArray->GetEntriesFast();

   for (Int_t id=0; id<nuOfTracks; id++){
     PndDchTrackMatch* dchtrmatch = (PndDchTrackMatch*) fDchTrackMatchArray->At(id);
     Int_t tridx = dchtrmatch->GetRecTrackID();
     PndDchTrack* dchtrack = (PndDchTrack*) fDchTrackArray->At(tridx);
     Int_t nuOfChits = dchtrack->GetNofDchCylinderHits();

     if(candmap[id]==NULL){ 
       candmap[id]=new TrackCand;
     } else { std::cout<<"PndDchPrepareKalmanTracks::Exec()...:"<<
	 "this track ID was used already!"<<std::endl;
     }
     for(Int_t nuhit=0; nuhit<nuOfChits; nuhit++){
       Int_t globalCHitNu = dchtrack->GetDchCylinderHitIndex(nuhit);
       candmap[id]->addHit(1,globalCHitNu);
     }
   }
     

   // ------------------------------------------------------------------------
   // try to find some starting values
   // loop over tracks
   
   std::map<unsigned int,TrackCand*>::iterator candIter=candmap.begin();
   while(candIter!=candmap.end()){
     TrackCand* cand=candIter->second;
     if(cand->getNHits()<10){
       ++candIter;
       continue;
     }
     
     // Get MCTrack

     PndDchTrackMatch* dchTrackMatch = (PndDchTrackMatch*) fDchTrackMatchArray->At(candIter->first);
     Int_t mcTrId = dchTrackMatch->GetMCTrackID();
     CbmMCTrack* mc=(CbmMCTrack*)fMcArray->At(mcTrId);
     if(mc==0){
       Error("PndDchPrepareKalmanTracks::Exec","MCTrack Id=&i not found!",mcTrId);
       ++candIter;
       continue;
     }

     int pdg=mc->GetPdgCode();
     double q=TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;
     
     Double_t startPosAccuracy=2.*0.2;
     TVector3 pos=mc->GetStartVertex()+TVector3(gRandom->Uniform(-startPosAccuracy, startPosAccuracy),
						gRandom->Uniform(-startPosAccuracy, startPosAccuracy),
						gRandom->Uniform(-startPosAccuracy, startPosAccuracy));
     TVector3 poserr(startPosAccuracy,startPosAccuracy,startPosAccuracy);

     TVector3 startMomAccuracy(0.1,0.1,0.1);
     TVector3 mcMom = mc->GetMomentum();
     TVector3 mom=TVector3(gRandom->Gaus(mcMom.X(),mcMom.X()*startMomAccuracy.X()),
			   gRandom->Gaus(mcMom.Y(),mcMom.Y()*startMomAccuracy.Y()),
			   gRandom->Gaus(mcMom.Z(),mcMom.Z()*startMomAccuracy.Z()));
     TVector3 momerr(mcMom.X()*startMomAccuracy.X(),
		     mcMom.Y()*startMomAccuracy.Y(),
		     mcMom.Z()*startMomAccuracy.Z());
     
     TVector3 u(1.,0.,0.);
     TVector3 v(0.,1.,0.);

     // create track-representation object and initialize with start values
     AbsTrackRep* rep=0;
     if(fUseGeane){
       DetPlane pl(pos,u,v);
       rep=new GeaneTrackRep(fGeanePro,pl,mom,poserr,momerr,q,pdg);
     }
     else { // use LSLTrackRep
       TVector3 dir=mom.Unit();
       double dxdz=dir.X()/dir.Z();
       double dydz=dir.Y()/dir.Z();
       double qp=q/mom.Mag();
       rep=new LSLTrackRep(pos.Z(),pos.X(),pos.Y(),dxdz,dydz,qp,
     			   poserr.X(),poserr.Y(),0.1,0.1,0.1,NULL);
     }

     std::cout<<" Momentum\n\t";
     mom.Print();
     rep->Print();
     

     // create track object
     Track* trk=new((*fTrackArray)[fTrackArray->GetEntriesFast()]) Track(rep);
     trk->setCandidate(*cand); // here the candidate is copied! 
              

     ++candIter;
   }// end loop over tracks
   
   std::cout<<fTrackArray->GetEntriesFast()<<" tracks created"<<std::endl;

   return;
}

ClassImp(PndDchPrepareKalmanTracks)
