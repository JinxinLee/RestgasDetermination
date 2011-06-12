//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class KalmanTask
//      see KalmanTask.hh for details
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
#include "KalmanTask.h"
#include <cmath>
// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "PndTpcCluster.h"
#include "PndTpcPlanarRecoHit.h"
#include "PndTpcSPHit.h"

#include "LSLTrackRep.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "GFDetPlane.h"

#include "PndGeoHandling.h"
#include "PndSdsRecoHit.h"
#include "PndGemRecoHit.h"
#include "PndDchRecoHit2.h"

#include "PndDchCylinderHit.h"
#include "PndSdsHit.h"
#include "PndGemHit.h"

#include "PndDetectorList.h"

#include "GFAbsRecoHit.h"
#include "TVector3.h"

#include <signal.h>
#include <stdlib.h>


void sighandler(int sig){
  std::cerr << "sighandler for sig " << sig << std::endl;
  abort();
}


// Class Member definitions -----------


KalmanTask::KalmanTask()
  : FairTask("Kalman Filter"), _persistence(kFALSE),_lazy(0),_numIt(1), _trackBranchName("TrackPreFit"), _outBranchName("TrackPostFit")
{

}


KalmanTask::~KalmanTask()
{
  if(_pH!=NULL)delete _pH;
  if(_chi2H!=NULL)delete _chi2H;
}

InitStatus
KalmanTask::Init()
{
  
  PndGeoHandling::Instance();
  
  //book fpe handler
  //signal(8,sighandler);
  //signal(10,sighandler);
  //signal(11,sighandler);
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("KalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  if(_trackArray==0)
    {
      Error("KalmanTask::Init","track-array not found!");
      return kERROR;
    }
  

  // Build hit factory -----------------------------
  _theRecoHitFactory = new GFRecoHitFactory();
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
   if(ar==0){
     Error("KalmanTask::Init","PndTpcCluster array not found");
   }
   else{ 
     _theRecoHitFactory->addProducer(2,new GFRecoHitProducer<PndTpcCluster,PndTpcSPHit>(ar));
   }
 
   TClonesArray* mvdHitArray=(TClonesArray*) ioman->GetObject("MVDHit");
   if(mvdHitArray==0){ //TODO Convention on detector number needed
     Error("PndFwdKalmanTask::Init","MVDHit array not found");
   } else {
     _theRecoHitFactory->addProducer(kMVD,new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(mvdHitArray));
   }
   
   
   TClonesArray* gemHitArray=(TClonesArray*) ioman->GetObject("GEMHit");
   if(gemHitArray==0){ //TODO Convention on detector number needed
     Error("PndFwdKalmanTask::Init","GEMHit array not found");
   } else {
     _theRecoHitFactory->addProducer(kGEM,new GFRecoHitProducer<PndGemHit,PndGemRecoHit>(gemHitArray));
     _theRecoHitFactory->addProducer(5,new GFRecoHitProducer<PndGemHit,PndGemRecoHit>(gemHitArray));
   }
   
   
   
   TClonesArray* dchCylHitArray=(TClonesArray*) ioman->GetObject("PndDchCylinderHit");
   if(dchCylHitArray==0){ //TODO Convention on detector number needed
     Error("PndFwdKalmanTask::Init","PndDchCylinderHit array not found");
   } else {
     _theRecoHitFactory->addProducer(kDCH,new GFRecoHitProducer<PndDchCylinderHit,PndDchRecoHit2>(dchCylHitArray));
   }
   
   _trackOutArray = new TClonesArray("GFTrack");
   ioman->Register(_outBranchName,"",_trackOutArray,kTRUE);


  // setup histograms
  _pH=new TH1D("pH","p",100,0.4,0.6);
  _chi2H=new TH1D("chi2H","chi2",100,0,20);
  _massV0=new TH1D("massV0","massV0",100,0,5);
  _massETAC=new TH1D("massEta","massEta",100,2.5,3.5);

  return kSUCCESS;
}


void
KalmanTask::Exec(Option_t* opt)
{
  std::cout<<"KalmanTask::Exec"<<std::endl;
  if(_trackOutArray->GetEntriesFast()>0)_trackOutArray->Delete();
  std::cout<<"KalmanTask::Exec2"<<std::endl;
  // Reset output Array
  if(_trackArray==0) Fatal("Kalman::Exec)","No TrackArray");
    std::cout<<"KalmanTask::Exec3"<<std::endl;


  Int_t ntracks=_trackArray->GetEntriesFast();
  
  if(ntracks>2000){
    std::cout<<"ntracks="<<ntracks<<" Evil Event! skipping"<<std::endl;
    return;
  }

  // Fitting ---------------- can go to another task!
  GFKalman fitter;
  //  fitter.setLazy(_lazy);
  fitter.setNumIterations(3);

  //std::vector<TLorentzVector*> particles;
  //std::vector<int> signs;
  
  for(Int_t itr=0;itr<ntracks;++itr){
    std::cout<<"starting track"<<itr<<std::endl;
    GFTrack* trk=(GFTrack*)_trackArray->At(itr);
    
    // Load RecoHits 
    try {
      trk->addHitVector(_theRecoHitFactory->createMany(trk->getCand()));
      std::cout<<trk->getNumHits()<<" hits in track "
	       <<itr<<std::endl;
    }
    catch(GFException& e) {
      std::cout << e.what() << std::endl;
      e.info();
      throw e;
    }
    std::cout<<"*** Number of clusters in track: "<<trk->getNumHits()<<" ***"<<std::endl;
    /*
    trk->getCand().print();
    trk->getCardinalRep()->Print();

    GFAbsTrackRep *rep = trk->getCardinalRep();
    for(int i=0; i<trk->getNumHits();++i){
      GFAbsRecoHit *hit = trk->getHit(i);
      std::cout << "###################hit " << i << std::endl;
      GFDetPlane pl = hit->getDetPlane(rep);
      TMatrixT<double> statePred(5,1);
      TMatrixT<double> covPred(5,5);
      rep->extrapolate(pl,statePred,covPred);
      pl.Print();
      statePred.Print();
    }
    return;
    */
    /*
    std::vector<GFAbsRecoHit*> hits = trk->getHits();
    std::cout<<"\nstd::vector<GFAbsRecoHit*> hits has "<< hits.size()<<" entries"<<std::endl;
    
    // HACK: print out hit positions:

    std::cout<<"\n *** PndTpcSPHit positions ***"<<std::endl;
    for(int hit=0; hit<hits.size(); hit++) {
      PndTpcSPHit* the_sphit = dynamic_cast<PndTpcSPHit*>(hits[hit]);
      TMatrixT<double> rawcoord =the_sphit->getRawHitCoord();
      TVector3 the_pos(rawcoord[0][0],rawcoord[1][0],rawcoord[2][0]);
      std::cout<<"("<<the_pos.X()<<", "<<the_pos.Y()<<", "<<the_pos.Z()<<");    ";
      if(hit%4 == 0 && hit>0)
	std::cout<<std::endl;
    }
    */	

    Int_t size = _trackOutArray->GetEntriesFast();
    GFTrack* trkCopy = new((*_trackOutArray)[size]) GFTrack(*trk);

    
    // Start Fitter
    try{
      std::cout<<"starting fit"<<std::endl;
      std::cerr << "Calling processTrack" << std::endl;
      fitter.processTrack(trkCopy);
    }
    catch (GFException e){
      std::cout<<e.what()<<std::endl;
    }

    

    // Print Track Parameters after fit
    if(trkCopy->getTrackRep(0)->getStatusFlag()==0){
      trkCopy->getTrackRep(0)->Print();
      //GFDetPlane plane(TVector3(0,0,0.1),TVector3(1,0,0),TVector3(0,1,0));
      //TVector3 p3=trk->getTrackRep(0)->getMom(plane);
      double p=trkCopy->getMom().Mag();
      _pH->Fill(p);
      
      
      double chi2=trkCopy->getChiSqu();
      _chi2H->Fill(chi2);

    }

  }

  std::cout<<"Fitting done"<<std::endl;
   

  return;
}

void 
KalmanTask::WriteHistograms(const TString& filename){
  TFile* file = new TFile(filename,"UPDATE");
  file->mkdir("Kalman");
  file->cd("Kalman");

  _pH->Write();
  delete _pH;
  _pH=NULL;

  _chi2H->Write();
  delete _chi2H;
  _chi2H=NULL;

  _massV0->Write();
  delete _massV0;
  _massV0=NULL;

  _massETAC->Write();
  delete _massETAC;
  _massETAC=NULL;

  file->Close();
  delete file;
}


ClassImp(KalmanTask)
