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
#include "Track.h"
#include "PndTpcCluster.h"
#include "PndTpcPlanarRecoHit.h"
#include "PndTpcSPHit.h"

#include "LSLTrackRep.h"
#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "DetPlane.h"

#include "AbsRecoHit.h"
#include "TVector3.h"


// Class Member definitions -----------


KalmanTask::KalmanTask()
  : FairTask("Kalman Filter"), _persistence(kFALSE),_lazy(0),_numIt(1)
{
  _trackBranchName = "TrackPreFit";
}


KalmanTask::~KalmanTask()
{
  if(_pH!=NULL)delete _pH;
  if(_chi2H!=NULL)delete _chi2H;
}

InitStatus
KalmanTask::Init()
{
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
  _theRecoHitFactory = new RecoHitFactory();
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
   if(ar==0){
     Error("KalmanTask::Init","PndTpcCluster array not found");
   }
   else{ 
     _theRecoHitFactory->addProducer(2,new RecoHitProducer<PndTpcCluster,PndTpcSPHit>(ar));
   }
 

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
  // Reset output Array
  //if(_trackArray==0) Fatal("Kalman::Exec)","No TrackArray");
  // _trackArray->Delete();

  Int_t ntracks=_trackArray->GetEntriesFast();
  
  if(ntracks>2000){
    std::cout<<"ntracks="<<ntracks<<" Evil Event! skipping"<<std::endl;
    return;
  }

  // Fitting ---------------- can go to another task!
  Kalman fitter;
  fitter.setLazy(_lazy);
  fitter.setNumIterations(_numIt);

  //std::vector<TLorentzVector*> particles;
  //std::vector<int> signs;
  
  for(Int_t itr=0;itr<ntracks;++itr){
    std::cout<<"starting track"<<itr<<std::endl;
    Track* trk=(Track*)_trackArray->At(itr);
          
    // Load RecoHits 
    try {
      trk->addHitVector(_theRecoHitFactory->createMany(trk->getCand()));
      std::cout<<trk->getNumHits()<<" hits in track "
	       <<itr<<std::endl;
    }
    catch(FitterException& e) {
      std::cout << e.what() << std::endl;
      throw e;
    }
    
    std::vector<AbsRecoHit*> hits = trk->getHits();
    std::cout<<"\nstd::vector<AbsRecoHit*> hits has "<< hits.size()<<" entries"<<std::endl;
    
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
	
    // Start Fitter
    try{
      std::cout<<"starting fit"<<std::endl;
      fitter.processTrack(trk);
    }
    catch (FitterException e){
      std::cout<<e.what()<<std::endl;
    }

    // Print Track Parameters after fit
    if(trk->getTrackRep(0)->getStatusFlag()==0){
      trk->getTrackRep(0)->Print();
      //DetPlane plane(TVector3(0,0,0.1),TVector3(1,0,0),TVector3(0,1,0));
      //TVector3 p3=trk->getTrackRep(0)->getMom(plane);
      double p=trk->getMom().Mag();
      _pH->Fill(p);
      
      double chi2=trk->getChiSqu();
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
