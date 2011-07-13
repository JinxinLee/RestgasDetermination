//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcdEdxTask
//      see PndTpcdEdxTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer, Christian Hoeppner     TUM         
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcdEdxTask.h"
#include <cmath>
// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h> 

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "PndTpcCluster.h"
#include "PndTpcDigiMapper.h"
#include "PndTpcFrontend.h"
#include "PndTpcSPHit.h"
#include "PndMCTrack.h"
#include "PndTpcPoint.h"


#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"

#include "GeaneTrackRep.h"
#include "GFAbsTrackRep.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"

#include "GFDetPlane.h"
#include "PndTpcdEdx.h"

#include "GFAbsRecoHit.h"
#include "TVector3.h"

#include <signal.h>
#include <stdlib.h>

#include "PndDetectorList.h"

// Class Member definitions -----------


PndTpcdEdxTask::PndTpcdEdxTask()
  : FairTask("dE/dx Task"), _persistence(kFALSE),
    _DX(1.), _idealdEdx(kFALSE)
{
  _trackBranchName = "TrackPostFit";
  fVerbose = 0;
}


PndTpcdEdxTask::~PndTpcdEdxTask()
{
}

InitStatus
PndTpcdEdxTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcdEdxTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  if(_trackArray==0)
    {
      Error("PndTpcdEdxTask::Init","track-array not found!");
      return kERROR;
    }

  _mcTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(_mcTrackArray==0)
    {
      Error("PndTpcdEdxTask::Init","MCTrack-array not found! Cannot calculate MC dEdx");
      _idealdEdx=false;
    }

  _pointArray=(TClonesArray*) ioman->GetObject("PndTpcPoint");
  if(_pointArray==0)
    {
      Error("PndTpcdEdxTask::Init","Point-array not found! Cannot calculate MC dEdx");
      _idealdEdx=false;
    }

  _clusterArray=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  if(_clusterArray==0)
    {
      Error("PndTpcdEdxTask::Init","Cluster-array not found!");
      return kERROR;
    }
 

  _dEdxOutArray = new TClonesArray("PndTpcdEdx");
  ioman->Register("dEdx","PndTpc",_dEdxOutArray,_persistence);
  
  _dEdxMCOutArray = new TClonesArray("PndTpcdEdx");
  ioman->Register("dEdx_MC","PndTpc",_dEdxMCOutArray,_persistence);

  //TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  // if(ar==0){
  //   Error("PndTpcdEdxTask::Init","PndTpcCluster array not found");
  // }

  //init the digi mapper
  PndTpcDigiMapper::getInstance(false)->init(_par->getPadPlane(),
					     NULL,
					     _par->getGas(),
					     _par->getPadShapes(),
					     _par->getZGem(),
					     _par->getFrontend()->t0(),
					     _par->getFrontend()->samplingFrequency());
					     
  // setup histograms
  _distHist=new TH1D("distHist","Stepping lentghs",500,-3.,3.);
  _dirHist=new TH1D("dirHist","direction abundances",10,-2.,2.);
 
  return kSUCCESS;
}


void
PndTpcdEdxTask::SetParContainers() {

  std::cout<<"PndTpcClusterFinderTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  _par= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! _par ) Fatal("SetParContainers", "PndTpcDigiPar not found");

}



void
PndTpcdEdxTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcdEdxTask::Exec()"<<std::endl;

  _dEdxOutArray->Delete();
  _dEdxMCOutArray->Delete();
  

  Int_t ntracks=_trackArray->GetEntriesFast();
  
  if(ntracks>20000){
    std::cout<<"ntracks="<<ntracks<<" Evil Event! skipping"<<std::endl;
    return;
  }

  //cut: only use single event tracks!
  if(ntracks>1){
    //return;
  }

  for(Int_t itr=0;itr<ntracks;++itr){
    if (fVerbose) std::cout<<"PndTpcdEdxTask::Exec(): starting track "<<itr<<std::endl;
    GFTrack* trk=(GFTrack*)_trackArray->At(itr);
    if (fVerbose) std::cout<<"*** Number of clusters in track: "<<trk->getNumHits()<<" ***"<<std::endl;
    
    GFAbsTrackRep* theRep = trk->getCardinalRep();
    if (theRep->getStatusFlag() != 0) continue;

    //check for GEANE trackrep
    if(dynamic_cast<GeaneTrackRep*>(theRep) != NULL) {
      ((GeaneTrackRep*)theRep)->setPropDir(0); // not needed for RKTrackRep
    }

    GFTrackCand cand = trk->getCand();
    std::vector<unsigned int> hitIDs = cand.GetHitIDs(FairRootManager::Instance()->GetBranchId("PndTpcCluster")); // get tpc hit ids
    

    //GET MC INFORMATION FOR CROSS-CHECK
    //only works with IdealTracking and pure trackIDs
    std::vector<PndTpcPoint*> pointlist;
    if(_idealdEdx) {
      if (fVerbose) std::cerr<<"collect MC points"<<std::endl;
      //loop over MC points and collect
      for(int p=0; p<_pointArray->GetEntriesFast(); ++p) {
        int id = ((PndTpcPoint*)_pointArray->At(p))->GetTrackID();
        if(id==0) //only primary tracks
          pointlist.push_back((PndTpcPoint*)_pointArray->At(p));
      }
    }
    

    if (fVerbose) std::cerr<<"collect digis"<<std::endl;
    // get all digis in the track
    std::vector<const PndTpcDigi*> digis;
    for(int i=0;i<hitIDs.size();++i){
      PndTpcCluster* cl = (PndTpcCluster*)(_clusterArray->At(hitIDs[i]));
      int ndigi = cl->nDigi();
      for(unsigned int j=0;j<ndigi;++j){
        digis.push_back(cl->getDigi(j));
      }
    }

    TVector3 startDir( ((PndTpcCluster*)(_clusterArray->At(hitIDs[2])))->pos() - ((PndTpcCluster*)(_clusterArray->At(hitIDs[0])))->pos() );

    TVector3 pos,mom;
    if (fVerbose) std::cerr<<"get pos and mom"<<std::endl;
    try{
      pos = theRep->getPos();
      mom = theRep->getMom();
    }
    catch(GFException& e){
      e.what();
      continue;
      //TODO: exception handling
    }

    if (fVerbose) {
      TVector3 pos0(((PndTpcCluster*)(_clusterArray->At(hitIDs[0])))->pos());
      std::cout<<"pos - pos0 mag"<< (pos-pos0).Mag() << std::endl;
    }


    PndTpcdEdx dedx;
    PndTpcdEdx dedx_MC;

    GFDetPlane here,next;
    here.setO(pos);
    here.setNormal(mom);
    int counter(0);


    bool exc(false);
    while(true){
      TVector3 destination = here.getO()+here.getNormal()*_DX;
      if (fVerbose) std::cerr<<"at counter "<<counter<<std::endl;
      if(counter++>0) here=next;//not the first time
      else if(here.getNormal() * startDir < 0) {
        _DX*=-1;
        destination*=-1;
      }
      TVector3 poca,dirInPoca;
      double dist;
      try{
        theRep->extrapolateToPoint(destination,poca,dirInPoca);
        next.setO(poca);
        next.setNormal(dirInPoca);
        dist = theRep->extrapolate(next);
      }
      catch(GFException& e){
        e.what();
        exc=true;
        break;
        //TODO: exception handling
      }
      
      TVector3 normHere = here.getNormal();
      TVector3 normNext = next.getNormal();
      
      double dE = 0.;
      bool _abort(true);

      for(unsigned int i=0;i<digis.size();++i){
        //if (fVerbose) std::cerr<<"at digi "<< i <<std::endl;
        TVector3 digiPos;
        PndTpcDigiMapper::getInstance()->map(digis[i],digiPos);
        if((here.getO()-digiPos).Mag() > (_DX+3.)) continue;
        double behindHere = normHere * (here.dist(digiPos));
        double behindNext = normNext * (next.dist(digiPos));
        if(behindHere<0.){  //in front of here
          _abort=false;
          if(behindNext>=0.){   // behind next
            dE+=digis[i]->amp();
          }
        }
        else{                // behind here
          if(behindNext<0.){ //in front of next
            _abort=false;
            dE+=digis[i]->amp();
          }
        }
      }

      if(dE>0. && (!exc)){
        dedx.add(dE,dist);
      }
      
      //if(_abort) break;
      dE = 0.;

      if(_idealdEdx) {
        for(unsigned int i=0;i<pointlist.size();++i){
          TVector3 pointPos;
          pointlist.at(i)->Position(pointPos);
          if((here.getO()-pointPos).Mag() > (_DX+3.)) continue;
          double behindHere = normHere * (here.dist(pointPos));
          double behindNext = normNext * (next.dist(pointPos));
          if(behindHere<0.){//in front of
            _abort=false;
            if(behindNext>=0.){
              dE+=pointlist.at(i)->GetEnergyLoss();
            }
          }
          else{
            if(behindNext<0.){//in front of
              _abort=false;
              dE+=pointlist.at(i)->GetEnergyLoss();
            }
          }
        }

        if(dE>0. && (!exc)){
          dedx_MC.add(dE*1E9,dist);
        }
      } // end if _idealdEdx

      if(_abort) break;
    }
  
    new((*_dEdxOutArray)[itr]) PndTpcdEdx(dedx);

    if(_idealdEdx) {
      new((*_dEdxMCOutArray)[itr]) PndTpcdEdx(dedx_MC);
    }
    
    
  }
}


void 
PndTpcdEdxTask::WriteHistograms(const TString& filename){
  TFile* file = new TFile(filename,"RECREATE");
  file->mkdir("DEDX");
  file->cd("DEDX");

  _distHist->Write();
  delete _distHist;

  _dirHist->Write();
  delete _dirHist;
   
  file->Close();
  delete file;
}



ClassImp(PndTpcdEdxTask)
