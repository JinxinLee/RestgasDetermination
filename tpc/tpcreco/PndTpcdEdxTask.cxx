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



// Class Member definitions -----------


PndTpcdEdxTask::PndTpcdEdxTask()
  : FairTask("dE/dx Task"), _persistence(kFALSE), _DX(1.)
{
  _trackBranchName = "TrackPostFit";
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
      Error("PndTpcdEdxTask::Init","MCTrack-array not found!");
      return kERROR;
    }

  _pointArray=(TClonesArray*) ioman->GetObject("PndTpcPoint");
  
  if(_pointArray==0)
    {
      Error("PndTpcdEdxTask::Init","Point-array not found!");
      return kERROR;
    }
 

  _dEdxOutArray = new TClonesArray("PndTpcdEdx");
  ioman->Register("dEdx","PndTpc",_dEdxOutArray,kTRUE);
  
  _dEdxMCOutArray = new TClonesArray("PndTpcdEdx");
  ioman->Register("dEdx_MC","PndTpc",_dEdxMCOutArray,kTRUE);

  //TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  // if(ar==0){
  //   Error("PndTpcdEdxTask::Init","PndTpcCluster array not found");
  // }

  //init the digi mapper
  PndTpcDigiMapper::getInstance(false)->init(_par->getPadPlane(),
					     NULL,
					     _par->getGas(),
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
  _dEdxOutArray->Delete();
  _dEdxMCOutArray->Delete();
  

  Int_t ntracks=_trackArray->GetEntriesFast();
  
  if(ntracks>2000){
    std::cout<<"ntracks="<<ntracks<<" Evil Event! skipping"<<std::endl;
    return;
  }

  //cut: only use single event tracks!
  if(ntracks>1)
    return;

  for(Int_t itr=0;itr<ntracks;++itr){
    std::cout<<"PndTpcdEdxTask::Exec(): starting track "<<itr<<std::endl;
    GFTrack* trk=(GFTrack*)_trackArray->At(itr);
    std::cout<<"*** Number of clusters in track: "<<trk->getNumHits()<<" ***"<<std::endl;
    
    GFAbsTrackRep* absrep = trk->getCardinalRep();
     
    //check for GEANE trackrep
    if(dynamic_cast<GeaneTrackRep*>(absrep) == NULL) {
      std::cerr<<"WRONG trackrep! Need GEANE to process ... skipping track"<<std::endl;
      continue;
    }

    //temporary fix
    GFAbsTrackRep* theRep = absrep->clone();
    ((GeaneTrackRep*)theRep)->setPropDir(0);

    
    std::vector<GFAbsRecoHit*> hits = trk->getHits();
    std::cout<<"\nstd::vector<GFAbsRecoHit*> hits has "<< hits.size()<<" entries"<<std::endl;
    
    //skip evil events
    if(hits.size() > 500)
      continue;
    
    std::vector<GFAbsRecoHit*>::iterator it;
    
    for(it = hits.begin(); it!=hits.end(); it++) {
      PndTpcSPHit* the_sphit = dynamic_cast<PndTpcSPHit*>(*it);
      //erase non-TPC hits
      if(the_sphit==NULL)
	hits.erase(it);
    }

    //GET MC INFORMATION FOR CROSS-CHECK
    //only works with IdealTracking and pure trackIDs
    std::vector<PndTpcPoint*> pointlist;
    
    // int trackID = 0;
//     //testing loop
//     for(int t=0; t<hits.size(); ++t) {
//       if(t==0)
// 	int trackID = ((PndTpcSPHit*)hits.at(t))->getCluster()->mcId().DominantID().mctrackID();
//       else
// 	if(((PndTpcSPHit*)hits.at(t))->getCluster()->mcId().DominantID().mctrackID() != trackID) {
// 	  break;
// 	}
//     }

    
    //loop over MC points and collect 
    for(int p=0; p<_pointArray->GetEntriesFast(); ++p) {
      int id = ((PndTpcPoint*)_pointArray->At(p))->GetTrackID();
      if(id==0) //only primary tracks
	pointlist.push_back((PndTpcPoint*)_pointArray->At(p));
    }
      

    PndTpcdEdx dedx;
    PndTpcdEdx dedx_MC;
    
    bool unsorted =false;
    
    std::vector<const PndTpcDigi*> digis;
    for(int i=0;i<hits.size();++i){
      int ndigi = ((PndTpcSPHit*)(hits.at(i)))->getCluster()->nDigi();
      for(unsigned int j=0;j<ndigi;++j){
	digis.push_back(((PndTpcSPHit*)(hits.at(i)))->getCluster()->getDigi(j));
	//((PndTpcSPHit*)(hits.at(i)))->getCluster()->getDigi(j).Print();
	//TVector3 vec;
	//PndTpcDigiMapper::getInstance()->map(&(((PndTpcSPHit*)(hits.at(i)))->getCluster()->getDigi(j)),vec);
	//vec.Print();
      }
      //hits.at(i)->getRawHitCoord().Print();
      //std::cout << "#################" << std::endl;
    }


    TVector3 pos,mom;
    
    try{
      pos = theRep->getPos();
      mom = theRep->getMom();
    }
    catch(GFException& e){
      e.what();
      continue;
      //TODO: exception handling
    }

    GFDetPlane here,next;
    here.setO(pos);
    here.setNormal(mom);
    int counter(0);

    bool exc(false);
    while(true){
      if(counter++>0) here=next;//just the first time
      TVector3 destination = here.getO()+here.getNormal()*_DX;
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
	TVector3 digiPos;
	PndTpcDigiMapper::getInstance()->map(digis[i],digiPos);
	double behindHere = normHere * (here.dist(digiPos));
	double behindNext = normNext * (next.dist(digiPos));
	if(behindHere<0.){//in front of
	  _abort=false;
	  if(behindNext>=0.){
	    dE+=digis[i]->amp();
	  }
	}
	else{
	  if(behindNext<0.){//in front of
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

      for(unsigned int i=0;i<pointlist.size();++i){
	TVector3 pointPos;
	pointlist.at(i)->Position(pointPos);
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
      if(_abort) break;
    }
  
    int size = _dEdxOutArray->GetEntriesFast();
    new((*_dEdxOutArray)[size]) PndTpcdEdx(dedx);
    size = _dEdxMCOutArray->GetEntriesFast();
    new((*_dEdxMCOutArray)[size]) PndTpcdEdx(dedx_MC);
    
    
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
