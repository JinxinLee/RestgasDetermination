// -------------------------------------------------------------------------
// -----          PndDchPrepareKalmanTracks2 source file                -----
// -----            Created 30.09.2008  by A. Wronska                  -----
// -----        based on the recotasks/demo code  by S.Neubert         -----
// -------------------------------------------------------------------------

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
#include "PndDchPrepareKalmanTracks2.h"
#include "PndDchPoint.h"
#include "PndDchTrack.h"
#include "PndDchTrackMatch.h"
#include "PndDchCylinderHit.h"

// ROOT Class Headers --------
#include "TClonesArray.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>



PndDchPrepareKalmanTracks2::PndDchPrepareKalmanTracks2()
  : CbmTask("Translation of PndDchTracks to Tracks"), fPersistence(kFALSE), fUseGeane(kFALSE)
{
}


PndDchPrepareKalmanTracks2::~PndDchPrepareKalmanTracks2()
{
  if( 0!= fTrackArray){
    fTrackArray->Delete();
    delete fTrackArray;
  }
  if( 0!= fGeanePro)
    delete fGeanePro;
}

InitStatus
PndDchPrepareKalmanTracks2::Init()
{
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndDchPrepareKalmanTracks2::Init","RootManager not instantiated!");
      return kERROR;
    }
 
  AddHitBranch(1,"PndDchCylinderHit");

  // open hit arrays
  std::map<unsigned int,TString>::iterator iter=fHitBranchNameMap.begin();
  while(iter!=fHitBranchNameMap.end()){
   TClonesArray* ar=(TClonesArray*) ioman->GetObject(iter->second);
   if(ar==0){
     Error("DemoPRTask::Init","hit-array %s not found!",iter->second.Data());
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
    Error("PndDchPrepareKalmanTracks2::Init","mctrack-array not found!");
    return kERROR;
  } 
  // open point array
  fDchPointArray=(TClonesArray*) ioman->GetObject("PndDchPoint");
  if(fDchPointArray==0){
    Error("PndDchPrepareKalmanTracks2::Init","dchpoint-array not found!");
    return kERROR;
  } 
  // open input array of PndDchTracks
  fDchTrackArray = (TClonesArray*) ioman->GetObject("PndDchTrack"); 
  if(fDchTrackArray==0){
    Error("PndDchPrepareKalmanTracks2::Init","PndDchTrack array not found!");
    return kERROR;
  }

  // open input array of PndDchTrackMatches
  fDchTrackMatchArray = (TClonesArray*) ioman->GetObject("PndDchTrackMatch"); 
  if(fDchTrackMatchArray==0){
    Error("PndDchPrepareKalmanTracks2::Init","PndDchTrackMatch array not found!");
    return kERROR;
  }
 
  // create and register output array
  fTrackArray = new TClonesArray("Track"); 
  ioman->Register("Track","GenFit",fTrackArray,fPersistence);
  
  // GeanePro will get Geometry and BField from the Run
  fGeanePro=new CbmGeanePro();
  
  return kSUCCESS;
}


void
PndDchPrepareKalmanTracks2::Exec(Option_t* opt)
{
  if(fVerbose>0){
    std::cout<<"\n\n-------------------------------------------------------"<<std::endl;
    std::cout<<"      PndDchPrepareKalmanTracks2::Exec                      "<<std::endl;
    std::cout<<"-------------------------------------------------------"<<std::endl;
  }
  if(fTrackArray==0) Fatal("PndDchPrepareKalmanTracks2::Exec)","No TrackArray");
  fTrackArray->Delete();

   std::map<unsigned int,TrackCand*> candmap;
   
   Int_t nuOfTracks = fDchTrackArray->GetEntriesFast();

   for (Int_t id=0; id<nuOfTracks; id++){
     PndDchTrackMatch* dchtrmatch = (PndDchTrackMatch*) fDchTrackMatchArray->At(id);
     Int_t tridx = dchtrmatch->GetRecTrackID();
     PndDchTrack* dchtrack = (PndDchTrack*) fDchTrackArray->At(tridx);
     Int_t nuOfChits = dchtrack->GetNofDchCylinderHits();
     if(fVerbose>0)
       std::cout<<"PndDchPrepareKalmanTracks2::Exec(): I found here "<<nuOfChits<<" cyl hits \n";

     if(candmap[id]==NULL){ 
       candmap[id]=new TrackCand;
     } else { 
       std::cout<<"PndDchPrepareKalmanTracks2::Exec()...:"<<
	 "this track ID was used already!"<<std::endl;
     }
     for(Int_t nuhit=0; nuhit<nuOfChits; nuhit++){
       Int_t globalCHitNu = dchtrack->GetDchCylinderHitIndex(nuhit);
       candmap[id]->addHit(1,globalCHitNu);
     }
   }
     

   // ------------------------------------------------------------------------
   // then loop over tracks
   
   std::map<unsigned int,TrackCand*>::iterator candIter=candmap.begin();
   while(candIter!=candmap.end()){
     TrackCand* cand=candIter->second;
     if(cand->getNHits()<10){
       ++candIter;
       continue;
     }
     
     // Get MCTrack and try to find some starting values for fitting (mom and pos)

     PndDchTrackMatch* dchTrackMatch = 
       (PndDchTrackMatch*) fDchTrackMatchArray->At(candIter->first);
     Int_t mcTrId = dchTrackMatch->GetMCTrackID();
     CbmMCTrack* mc=(CbmMCTrack*)fMcArray->At(mcTrId);
     if(mc==0){
       Error("PndDchPrepareKalmanTracks2::Exec","MCTrack Id=&i not found!",mcTrId);
       ++candIter;
       continue;
     }

     Int_t pdg  = mc->GetPdgCode();
     Double_t q = TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;

     Int_t pointidx = 0;
     TVector3 pos;
     TVector3 mom;
     while(pointidx<fDchPointArray->GetEntries()){
       PndDchPoint* pnt=(PndDchPoint*)fDchPointArray->At(pointidx);
       if(pnt->GetTrackID()==mcTrId){
	 pnt->Position(pos);
	 pnt->Momentum(mom);
	 break;
       }
     }

     // pos=mc->GetStartVertex();
     Double_t startPosAccuracy = 0.5;
     TVector3 poserr(startPosAccuracy,startPosAccuracy,3.*startPosAccuracy);
     TVector3 startMomAccuracy(0.1,0.1,0.1);
     TVector3 mcMom = mc->GetMomentum();
     TVector3 momerr(mom.X()*startMomAccuracy.X(),
 		     mom.Y()*startMomAccuracy.Y(),
 		     mom.Z()*startMomAccuracy.Z());
     
     TVector3 u(1.,0.,0.);
     TVector3 v(0.,1.,0.);

     // create track-representation object and initialize with start values
     AbsTrackRep* rep=0;
     if(fUseGeane){
       DetPlane pl(pos,u,v);
       GeaneTrackRep *grep=new GeaneTrackRep(fGeanePro,pl,mom,poserr,momerr,q,pdg);
       grep->setPropDir(1); // propagate in flight direction
       if(fVerbose>0){
	 std::cout<<" ^^^^^^^^^^^^^I prepare the following GeaneTrackRep:"<<std::endl;
	 grep->Print();
	 std::cout<<" ^^^^^^^^^^^^^End of GeaneTrackRep printout"<<std::endl;
       }
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
     Track* trk=new((*fTrackArray)[fTrackArray->GetEntriesFast()]) Track(rep);
     trk->setCandidate(*cand);
     ++candIter;
   }// end loop over tracks
   
   if(fVerbose>0)
     std::cout<<fTrackArray->GetEntriesFast()<<" tracks created"<<std::endl;

   return;
}

ClassImp(PndDchPrepareKalmanTracks2)
