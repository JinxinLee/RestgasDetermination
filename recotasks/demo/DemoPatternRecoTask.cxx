//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class DemoPatternRecoTask
//      see DemoPatternRecoTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "DemoPatternRecoTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "FairMCPoint.h"
#include "PndMCTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFKalman.h"
#include "GFException.h"
#include "FairGeanePro.h"
#include "FairTrackParP.h"
#include "TRandom.h"

// Class Member definitions -----------

DemoPatternRecoTask::DemoPatternRecoTask()
  : FairTask("Ideal Pattern Reco"), _persistence(kFALSE), _useGeane(kFALSE)
{
}


DemoPatternRecoTask::~DemoPatternRecoTask()
{
}

InitStatus
DemoPatternRecoTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("DemoPatternRecoTask::Init","RootManager not instantiated!");
      return kERROR;
    }
 
  // open hit arrays
  std::map<unsigned int,TString>::iterator iter=_hitBranchNameMap.begin();
  while(iter!=_hitBranchNameMap.end()){
   TClonesArray* ar=(TClonesArray*) ioman->GetObject(iter->second);
   if(ar==0){
     Error("DemoPRTask::Init","point-array %s not found!",iter->second.Data());
   }
   else{ 
	 _hitBranchMap[iter->first] = ar;
   }
  ++iter;
  }//end loops over hit types
  

  // open MCTruth array
  _mcArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(_mcArray==0){
    Error("DemoPRTask::Init","mctrack-array not found!");
    return kERROR;
  }
   
  // create and register output array
  _trackArray = new TClonesArray("GFTrack"); 
  ioman->Register("Track","GenFit",_trackArray,_persistence);
 
  
  // GeanePro will get Geometry and BField from the Run
  _geanePro=new FairGeanePro();
  
  return kSUCCESS;
}


void
DemoPatternRecoTask::Exec(Option_t* opt)
{
  std::cout<<"DemoPatternRecoTask::Exec"<<std::endl;
  // Reset output Array
  if(_trackArray==0) Fatal("DemoPatternReco::Exec)","No TrackArray");
   _trackArray->Delete();

   // use McId to distinguish data from different tracks
   std::map<unsigned int,GFTrackCand*> candmap;


   std::map<unsigned int,TClonesArray*>::iterator iter=_hitBranchMap.begin();
   while(iter!=_hitBranchMap.end()){
	 TClonesArray* pointArray = iter->second;//_hitBranchMap[iter->first];
	 //loop over points
	 Int_t np=pointArray->GetEntriesFast();
	 for(Int_t ip=0; ip<np; ++ip){
	   FairMCPoint* point=(FairMCPoint*)pointArray->At(ip);
	   unsigned int id=point->GetTrackID();
	   // cut on insane ids
	   if(id>100000)continue;
	   // look for track in mc map
	   if(candmap[id]==NULL){
		 // create new track
		 std::cout<<"Creating new track candidate with id="<<id<<std::endl;
		 candmap[id]=new GFTrackCand;
	   }
	   // add hit to track
	   // set detectorid=2;
	   candmap[id]->addHit(iter->first,ip);
	 }// end loop over cluster
	 ++iter;
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
     PndMCTrack* mc=(PndMCTrack*)_mcArray->At(candIter->first);
     if(mc==0){
       Error("DemoPRTask::Exec","MCTrack Id=&i not found!",candIter->first);
       ++candIter;
       continue;
     }

     int pdg=mc->GetPdgCode();
     double q=TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;
     
     TVector3 pos=mc->GetStartVertex()+TVector3(0.001,0.001,0.001);
     double pre=mc->GetMomentum().Mag()*0.02;
     TVector3 mom=mc->GetMomentum()+TVector3(gRandom->Gaus(0,pre),
					      gRandom->Gaus(0,pre),
					      gRandom->Gaus(0,pre));
     TVector3 poserr(1.,1.,1.);
     TVector3 momerr=0.3*mom; //  10% error
     momerr+=TVector3(0.1,0.1,0.1);
     
     TVector3 u(1.,0.,0.);//=mom.Orthogonal();
     //u.SetMag(1.);
     TVector3 v(0.,1.,0.);//=mom.Cross(u);
     //v.SetMag(1.);

     // create track-representation object and initialize with start values
     GFAbsTrackRep* rep=0;
     if(_useGeane){
       GFDetPlane pl(pos,u,v);
       GeaneTrackRep* grep=new GeaneTrackRep(_geanePro,pl,mom,poserr,momerr,q,pdg);
       grep->setPropDir(1); // propagate in flight direction!
       rep=grep;
     }
     else { // use LSLTrackRep
       // calc momentum projections
       TVector3 dir=mom.Unit();
       double dxdz=dir.X()/dir.Z();
       double dydz=dir.Y()/dir.Z();
       double qp=q/mom.Mag();
       rep=new LSLTrackRep(pos.Z(),pos.X(),pos.Y(),dxdz,dydz,qp,
     			   poserr.X(),poserr.Y(),0.1,0.1,0.1,NULL);
       
     }

       //LSLTrackRep* rep=new LSLTrackRep(pos.Z(),pos.X(),pos.Y(),dxdz,dydz,qp,
     //		   poserr.X(),poserr.Y(),0.1,0.1,0.1,NULL);
     mom.Print();
     rep->Print();
     

     // create track object
     GFTrack* trk=new((*_trackArray)[_trackArray->GetEntriesFast()]) GFTrack(rep);
     trk->setCandidate(*cand); // here the candidate is copied! 
              

     ++candIter;
   }// end loop over tracks
   
   std::cout<<_trackArray->GetEntriesFast()<<" tracks created"<<std::endl;

   return;
}

ClassImp(DemoPatternRecoTask)
