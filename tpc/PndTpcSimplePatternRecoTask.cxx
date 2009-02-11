//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSimplePatternRecoTask
//      see PndTpcSimplePatternRecoTask.hh for details
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
#include "PndTpcSimplePatternRecoTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
#include "McId.h"
#include "LSLTrackRep.h"
#include "RecoHitFactory.h"
#include "PndTpcXYRecoHit.h"
#include "Kalman.h"
#include "FitterExceptions.h"

// Class Member definitions -----------


PndTpcSimplePatternRecoTask::PndTpcSimplePatternRecoTask()
  : FairTask("TPC Simple Pattern Reco"), _persistence(kFALSE)
{
  _clusterBranchName = "PndTpcCluster";
}


PndTpcSimplePatternRecoTask::~PndTpcSimplePatternRecoTask()
{
}

InitStatus
PndTpcSimplePatternRecoTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcSimplePatternRecoTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
  
  if(_clusterArray==0)
    {
      Error("PndTpcSimplePatternRecoTask::Init","Cluster-array not found!");
      return kERROR;
    }
  
  // create and register output array
  _trackArray = new TClonesArray("Track"); 
  ioman->Register("TrackPreFit","GenFit",_trackArray,_persistence);
 
     
  _theRecoHitFactory = new RecoHitFactory();
  _theRecoHitFactory->addProducer(2,new RecoHitProducer<PndTpcCluster,PndTpcXYRecoHit>(_clusterArray));


  return kSUCCESS;
}


void
PndTpcSimplePatternRecoTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcSimplePatternRecoTask::Exec"<<std::endl;
  // Reset output Array
  if(_trackArray==0) Fatal("PndTpcSimplePatternReco::Exec)","No TrackArray");
   _trackArray->Delete();

   // use McId to distinguish data from different tracks
   std::map<McId,Track*> trackmap;

   //loop over cluster
   Int_t ncl=_clusterArray->GetEntriesFast();
   for(Int_t icl=0; icl<ncl; ++icl){
     PndTpcCluster* cl=(PndTpcCluster*)_clusterArray->At(icl);
     McId id=cl->mcId();
     // cut on strange ids
     if(id.mctrackID()>10000)continue;
     // look for track in mc map
     if(trackmap[id]==NULL){
       // create new track
       LSLTrackRep* rep=new LSLTrackRep();
       Track* trk=new((*_trackArray)[_trackArray->GetEntriesFast()]) Track(rep);
       trackmap[id]=trk;
     }
     // add hit to track
     // set tpcid=2;
     trackmap[id]->addHitIndices(2,icl);
   }// end loop over cluster

   // ------------------------------------------------------------------------
   // try to find some starting values
   // loop over tracks
   std::map<McId,Track*>::iterator trkIter=trackmap.begin();
   while(trkIter!=trackmap.end()){
     Track* trk=trkIter->second;
     PndTpcCluster* cl1=(PndTpcCluster*)_clusterArray->At(trk->getHitIndices()[0]);
     PndTpcCluster* cl2=(PndTpcCluster*)_clusterArray->At(trk->getHitIndices()[1]);
     TVector3 pos1=cl1->pos();
     TVector3 pos2=cl2->pos();
     TVector3 delta=pos2-pos1;
     double mx=delta.X()/delta.Z();
     double my=delta.Y()/delta.Z();
     std::cout<<"mx="<<mx<<"  my="<<my<<std::endl;
     TMatrixT<double> state(5,1);
     state[0][0]=pos1.X();
     state[1][0]=pos1.Y();
     state[2][0]=mx;
     state[3][0]=my;
     state[4][0]=0; // p=infty;
     trk->getTrackRep(0)->setState(state);
     TMatrixT<double> cov(5,5);
     cov[0][0]=100;
     cov[1][1]=100;
     cov[2][2]=16;
     cov[3][3]=16;
     cov[4][4]=5;
     trk->getTrackRep(0)->setCov(cov);
     trk->getTrackRep(0)->setS(pos1.Z());
     ++trkIter;
   }// end loop over tracks

   std::cout<<_trackArray->GetEntriesFast()<<" tracks created"<<std::endl;

   // Fitting ---------------- can go to another task!
   Kalman fitter;
   // try to load the hits into tracks.
   trkIter=trackmap.begin();
   while(trkIter!=trackmap.end()){
     Track* trk=trkIter->second;
     
     trk->addHitVector(_theRecoHitFactory->createMany(trk->getDetIDs(),
						      trk->getHitIndices()));
     std::cout<<trk->getNumHits()<<" hits in track "
	      <<trkIter->first<<std::endl;

     try{
       fitter.processTrack(trk);
     }
     catch (FitterException e){
       std::cout<<e.what()<<std::endl;
     }
     
     
     ++trkIter;
   }

   
  return;
}

ClassImp(PndTpcSimplePatternRecoTask)
