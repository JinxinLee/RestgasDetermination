//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndHypDPatternRecoTask
//      see PndHypDPatternRecoTask.hh for details
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
#include "PndHypDPatternRecoTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
#include "TrackCand.h"
#include "FairMCPoint.h"
//#include "../../tof/PndTofPoint.h"
#include "PndHypPoint.h"
#include "PndHypHit.h"
#include "PndMCTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "FairGeanePro.h"
#include "FairTrackParP.h"
#include "TRandom.h"

// Class Member definitions -----------

PndHypDPatternRecoTask::PndHypDPatternRecoTask()
  : FairTask("Ideal Pattern Reco"), _persistence(kFALSE), _useGeane(kFALSE), fEventNr(0)
{
}


PndHypDPatternRecoTask::~PndHypDPatternRecoTask()
{
}

InitStatus
PndHypDPatternRecoTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndHypDPatternRecoTask::Init","RootManager not instantiated!");
      return kERROR;
    }
 
  // open hit arrays
  std::map<unsigned int,TString>::iterator iter=_hitBranchNameMap.begin();
  while(iter!=_hitBranchNameMap.end()){
   TClonesArray* ar=(TClonesArray*) ioman->GetObject(iter->second);
   if(ar==0){
     Error("PndHypDPRTask::Init","point-array %s not found!",iter->second.Data());
   }
   else{ 
	 _hitBranchMap[iter->first] = ar;
   }
  ++iter;
  }//end loops over hit types
  

  // open MCTruth array
  _mcArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(_mcArray==0){
    Error("PndHypDPRTask::Init","mctrack-array not found!");
    return kERROR;
  }
   
  pointArray=(TClonesArray*) ioman->GetObject("HypPoint");
  if(pointArray==0){
    Error("PndHypDPRTask::Init","hit-array not found!");
    return kERROR;
    }


  // create and register output array
  _trackArray = new TClonesArray("Track"); 
  ioman->Register("Track","GenFit",_trackArray,_persistence);
 
  
  // GeanePro will get Geometry and BField from the Run
  _geanePro=new FairGeanePro();
  
  return kSUCCESS;
}


void
PndHypDPatternRecoTask::Exec(Option_t* opt)
{
  std::cout<<"PndHypDPatternRecoTask::Exec"<<std::endl;
  // Reset output Array
  if(_trackArray==0) Fatal("PndHypDPatternReco::Exec)","No TrackArray");
   _trackArray->Delete();

   // use McId to distinguish data from different tracks
   std::map<unsigned int,TrackCand*> candmap;
   std::cout<<"<<<<< Event "<<fEventNr++<<" <<<"<<std::endl;
   

   std::map<unsigned int,TClonesArray*>::iterator iter=_hitBranchMap.begin();
   while(iter!=_hitBranchMap.end()){
     //TClonesArray* pointArray = iter->second;//_hitBranchMap[iter->first];
	 TClonesArray* hitArray = iter->second;
	 //loop over points
	 Int_t np=hitArray->GetEntriesFast();

	 for(Int_t ip=0; ip<np; ++ip){
	   //PndHypPoint* point=(PndHypPoint*)hitArray->At(ip);
	   PndHypHit* hit =(PndHypHit*)hitArray->At(ip);
	   PndHypPoint* point=(PndHypPoint*)pointArray->At(hit->GetRefIndex());
	   if(point==0)continue;

	   unsigned int id=point->GetTrackID();
	   // cut on insane ids
	   if(id>10000)continue;
	   // look for track in mc map
	   if(candmap[id]==NULL){
		 // create new track
		 //std::cout<<"Creating new track candidate with id="<<id<<std::endl;
		 candmap[id]=new TrackCand;
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
   
   std::map<unsigned int,TrackCand*>::iterator candIter=candmap.begin();
   while(candIter!=candmap.end()){
     TrackCand* cand=candIter->second;
     if(cand->getNHits()<3){
       ++candIter;
       continue;
     }
     
     // Get MCTrack
     PndMCTrack* mc=(PndMCTrack*)_mcArray->At(candIter->first);
     if(mc==0){
       Error("PndHypDPRTask::Exec","MCTrack Id=&i not found!",candIter->first);
       ++candIter;
       continue;
     }

     int pdg=mc->GetPdgCode();
     double q;int PDG;
     
      std::cout<<" particle "<<pdg<<std::endl;
 
      
     if(pdg>5000){
       
       q = GetChargeIon(pdg);
       //std::cout<<" particle "<<q<<std::endl;
     }
     else{ q=TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;
       //std::cout<<" particle "<<q<<std::endl;
     }
     
     
     TVector3 pos=mc->GetStartVertex()+TVector3(0.001,0.001,0.001);
     double pre=mc->GetMomentum().Mag()*0.02;
     TVector3 mom=mc->GetMomentum()+TVector3(gRandom->Gaus(0,pre),
					      gRandom->Gaus(0,pre),
					      gRandom->Gaus(0,pre));

     if(mom.Mag()==0) {
       Error("PndHypDPRTask::Exec","Momentum is zero!",candIter->first);
       ++candIter;
       continue;
     }

     TVector3 poserr(0.01,0.01,0.005);
     TVector3 momerr=0.3*mom; //  10% error
     momerr+=TVector3(0.1,0.1,0.1);
     
     TVector3 u(1.,0.,0.);//=mom.Orthogonal();
     //u.SetMag(1.);
     TVector3 v(0.,1.,0.);//=mom.Cross(u);
     //v.SetMag(1.);

     // create track-representation object and initialize with start values
     AbsTrackRep* rep=0;

     if(_useGeane){
       DetPlane pl(pos,u,v);
       if(q>0)PDG=211;
       if(q<0)PDG=-211;
       
	 GeaneTrackRep* grep=new GeaneTrackRep(_geanePro,pl,mom,poserr,momerr,q,PDG);
       
       //grep->setPropDir(1); // propagate in flight direction!
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
    
     //mom.Print();
     //rep->Print();
     

     // create track object
     Track* trk=new((*_trackArray)[_trackArray->GetEntriesFast()]) Track(rep);
     //std::cout<<" particle "<<cand->getNHits()<<std::endl;
     if(cand==NULL) {
       Error("PndHypDPRTask::Exec","Momentum is zero!",candIter->first);
       //std::cout<<_trackArray->GetEntriesFast()<<" tracks created"<<std::endl;
       ++candIter;
       continue;
     }
     if(trk!=NULL)trk->setCandidate(*cand); // here the candidate is copied! 
     //std::cout<<" particle "<<(trk->getNumHits())<<std::endl;

     ++candIter;
   }// end loop over tracks
   
   std::cout<<_trackArray->GetEntriesFast()<<" tracks created"<<std::endl;
   
   
   //return;
}

Int_t PndHypDPatternRecoTask::GetChargeIon(Int_t ion)
{
  Int_t A,Z,L;
  
  if(ion>1000000000&&(ion<1010000000))
    { ion -= 1000000000;
      Z = ion/10000;
      ion -= 10000*Z;
      A = ion/10;
      cout<<" ion charge "<<Z<<endl;
      
      return Z;
      
    }
if((ion>1010000000||ion>1020000000))
  { 
    ion -= 1000000000;
    L = ion/10000000;
    ion -= 10000000*L;
    Z = ion/10000;
    ion -= 10000*Z;
    A = ion/10;
    cout<<L<<" hypernuclei charge "<<Z<<endl;
    
    return Z;
  
  }
}


ClassImp(PndHypDPatternRecoTask)
