// -------------------------------------------------------------------------
// -----          PndDchPrepareKalmanTracks source file                -----
// -----            Created 15.05.2008  by A. Wronska                  -----
// -----        based on the recotasks/demo code  by S.Neubert         -----
// -------------------------------------------------------------------------

// Panda Headers ----------------------
#include "FairRootManager.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "PndMCTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFKalman.h"
#include "GFException.h"
#include "FairGeanePro.h"
#include "FairTrackParP.h"
#include "PndDchPrepareKalmanTracks.h"
#include "PndDchPoint.h"
#include "PndDchTrack.h"
#include "PndDchTrackMatch.h"
#include "PndDchCylinderHit.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// ROOT Class Headers --------
#include "TClonesArray.h"



PndDchPrepareKalmanTracks::PndDchPrepareKalmanTracks()
  : FairTask("Translation of PndDchTracks to Tracks"), fPersistence(kFALSE), fUseGeane(kFALSE)
{
}


PndDchPrepareKalmanTracks::~PndDchPrepareKalmanTracks()
{
}

InitStatus
PndDchPrepareKalmanTracks::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

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
    Error("PndDchPrepareKalmanTracks::Init","mctrack-array not found!");
    return kERROR;
  } 
  // open point array
  fDchPointArray=(TClonesArray*) ioman->GetObject("PndDchPoint");
  if(fDchPointArray==0){
    Error("PndDchPrepareKalmanTracks::Init","dchpoint-array not found!");
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
  fTrackArray = new TClonesArray("GFTrack"); 
  ioman->Register("Track","GenFit",fTrackArray,fPersistence);
  std::cout<<"Track array created "<< fTrackArray<<std::endl;
 
  
  // GeanePro will get Geometry and BField from the Run
  fGeanePro=new FairGeanePro();
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

   std::map<unsigned int,GFTrackCand*> candmap;
   
   Int_t nuOfTracks = fDchTrackArray->GetEntriesFast();

   for (Int_t id=0; id<nuOfTracks; id++){
     PndDchTrackMatch* dchtrmatch = (PndDchTrackMatch*) fDchTrackMatchArray->At(id);
     Int_t tridx = dchtrmatch->GetRecTrackID();
     PndDchTrack* dchtrack = (PndDchTrack*) fDchTrackArray->At(tridx);
     Int_t nuOfChits = dchtrack->GetNofDchCylinderHits();
     std::cout<<"PndDchPrepareKalmanTracks::Exec(): I found here "<<nuOfChits<<" cyl hits \n";

     if(candmap[id]==NULL){ 
       candmap[id]=new GFTrackCand;
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
   
   std::map<unsigned int,GFTrackCand*>::iterator candIter=candmap.begin();
   while(candIter!=candmap.end()){
     GFTrackCand* cand=candIter->second;
     if(cand->getNHits()<10){
       ++candIter;
       continue;
     }
     
     // Get MCTrack

     PndDchTrackMatch* dchTrackMatch = (PndDchTrackMatch*) fDchTrackMatchArray->At(candIter->first);
     Int_t mcTrId = dchTrackMatch->GetMCTrackID();
     PndMCTrack* mc=(PndMCTrack*)fMcArray->At(mcTrId);
     if(mc==0){
       Error("PndDchPrepareKalmanTracks::Exec","MCTrack Id=&i not found!",mcTrId);
       ++candIter;
       continue;
     }

     int pdg=mc->GetPdgCode();
     double q=TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;

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
     Double_t startPosAccuracy = 0.2;
     TVector3 poserr(startPosAccuracy,startPosAccuracy,startPosAccuracy);
     TVector3 startMomAccuracy(0.1,0.1,0.1);
     TVector3 mcMom = mc->GetMomentum();
     // mom.SetXYZ(gRandom->Gaus(mom.X(),mom.X()*startMomAccuracy.X()),
// 		gRandom->Gaus(mom.Y(),mom.Y()*startMomAccuracy.Y()),
// 		gRandom->Gaus(mom.Z(),mom.Z()*startMomAccuracy.Z()));
     TVector3 momerr(mom.X()*startMomAccuracy.X(),
 		     mom.Y()*startMomAccuracy.Y(),
 		     mom.Z()*startMomAccuracy.Z());
     
     TVector3 u(1.,0.,0.);
     TVector3 v(0.,1.,0.);

     // create track-representation object and initialize with start values
     GFAbsTrackRep* rep=0;
     if(fUseGeane){
       GFDetPlane pl(pos,u,v);
       rep=new GeaneTrackRep(fGeanePro,pl,mom,poserr,momerr,q,pdg);
       std::cout<<" ^^^^^^^^^^^^^I prepare the following GeaneTrackRep:"<<std::endl;
       rep->Print();
       std::cout<<" ^^^^^^^^^^^^^End of GeaneTrackRep printout"<<std::endl;
       
     }
     else { // use LSLTrackRep
       TVector3 dir=mom.Unit();
       double dxdz=dir.X()/dir.Z();
       double dydz=dir.Y()/dir.Z();
       double qp=q/mom.Mag();
       rep=new LSLTrackRep(pos.Z(),pos.X(),pos.Y(),dxdz,dydz,qp,
     			   poserr.X(),poserr.Y(),0.1,0.1,0.1,NULL);
       std::cout<<" ^^^^^^^^^^^^^I prepare the following LSLTrackRep:"<<std::endl;
       rep->Print();
       std::cout<<" ^^^^^^^^^^^^^End of LSLTrackRep printout"<<std::endl;
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
