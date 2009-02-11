//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class DemoKalmanTask
//      see DemoKalmanTask.hh for details
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
#include "DemoKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
//#include "PndTpcPoint.h"
#include "DemoRecoHit.h"
#include "DemoSPHit.h"
#include "FairMCPoint.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"

// Class Member definitions -----------


DemoKalmanTask::DemoKalmanTask()
  : FairTask("Kalman Filter"), _persistence(kFALSE),_smooth(kFALSE), _evt(0)
{
  _trackBranchName = "Track";
}


DemoKalmanTask::~DemoKalmanTask()
{
  if(_pH!=NULL)delete _pH;
  if(_chi2H!=NULL)delete _chi2H;
}

InitStatus
DemoKalmanTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("DemoKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  
  if(_trackArray==0)
    {
      Error("DemoKalmanTask::Init","track-array not found!");
      return kERROR;
    }
  
  // create and register output array
  //_trackArray = new TClonesArray("Track"); 
  //ioman->Register("TrackPreFit","GenFit",_trackArray,_persistence);
 
  // Build hit factory -----------------------------
  _theRecoHitFactory = new RecoHitFactory();
  std::map<unsigned int,TString>::iterator iter=_hitBranchMap.begin();
  while(iter!=_hitBranchMap.end()){
   TClonesArray* ar=(TClonesArray*) ioman->GetObject(iter->second);
   if(ar==0){
     Error("DemoKalmanTask::Init","point-array %s not found!",iter->second.Data());
   }
   else{ 
     // the next lines is not general because it will work only for CmMCPoints!
     _theRecoHitFactory->addProducer(iter->first,new RecoHitProducer<FairMCPoint,DemoSPHit>(ar));
   }
  ++iter;
  }//end loops over hit types

  // setup histograms
  _pH=new TH1D("pH","p",50,0.3,0.7);
  _chi2H=new TH1D("chi2H","chi2",100,0,20);
  _xresH=new TH1D("xres","xres",100,-5,5);
  _yresH=new TH1D("yres","yres",100,-5,5);
  _xresFitH=new TH1D("xresfit","xres after fit",100,-5,5);
  _yresFitH=new TH1D("yresfit","yres after fit",100,-5,5);
  _pEnd=new TH2D("pEnd","Endpoint",100,-40,40,100,-50,150);

  return kSUCCESS;
}


void
DemoKalmanTask::Exec(Option_t* opt)
{
  std::cout<<"DemoKalmanTask::Exec Event "<<_evt++<<std::endl;
  // Reset output Array
  //if(_trackArray==0) Fatal("DemoKalman::Exec)","No TrackArray");
  // _trackArray->Delete();

  Int_t ntracks=_trackArray->GetEntriesFast();
  

  

  // Fitting ---------------- can go to another task!
  Kalman fitter;
  fitter.setLazy(1); // tell the fitter to skip hits if error occurs  
  fitter.setNumIterations(3);
  for(Int_t itr=0;itr<ntracks;++itr){
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
    
    /*
    // fill tpc residuals
    std::vector<double> res;
    trk->getResiduals(2,0,0,res);
    for(int i=0;i<res.size();++i){
      _xresH->Fill(res[i]);
    }
    res.clear();
    trk->getResiduals(2,1,0,res);
    for(int i=0;i<res.size();++i){
      _yresH->Fill(res[i]);
    }
    res.clear();
    */
    // Start Fitter
    try{
      fitter.processTrack(trk);
      if(_smooth)fitter.smoothing(trk);
    }
    catch (FitterException e){
      std::cout<<"*** FITTER EXCEPTION ***"<<std::endl;
      std::cout<<e.what()<<std::endl;
      
    }

    std::cout<<"SUCESSFULL FIT!"<<std::endl;

    // Fill some histos after fit
    if(trk->getTrackRep(0)->getStatusFlag()==0){
      // propagate backwards
      GeaneTrackRep* gtrk=dynamic_cast<GeaneTrackRep*>(trk->getTrackRep(0));
      if(gtrk!=NULL)gtrk->setPropDir(-1);
      trk->getCardinalRep()->Print();
      //DetPlane pl(TVector3(0,0,0),TVector3(1,0,0),TVector3(0,1,0));
      double p=trk->getTrackRep(0)->getMom().Mag();
      _pH->Fill(p);
      TVector3 pos=trk->getPos();
      _pEnd->Fill(pos.X(),pos.Z());

      double chi2=trk->getChiSqu();
      _chi2H->Fill(chi2);
      ++_trackcount;


      /*
      // fill tpc residuals
      trk->getResiduals(2,0,0,res);
      for(int i=0;i<res.size();++i){
	_xresFitH->Fill(res[i]);
      }
      res.clear();
      trk->getResiduals(2,1,0,res);
      for(int i=0;i<res.size();++i){
	_yresFitH->Fill(res[i]);
      }
      res.clear();
      */
    }
  }
  
  return;
}

void 
DemoKalmanTask::WriteHistograms(){
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("DemoKalman");
  file->cd("DemoKalman");

  _pH->Write();
  delete _pH;
  _pH=NULL;

  _chi2H->Write();
  delete _chi2H;
  _chi2H=NULL;

  _xresH->Write();
  delete _xresH;
  _xresH=NULL;

  _yresH->Write();
  delete _yresH;
  _yresH=NULL;

  _xresFitH->Write();
  delete _xresFitH;
  _xresFitH=NULL;

  _yresFitH->Write();
  delete _yresFitH;
  _yresFitH=NULL;

 _pEnd->Write();
  delete _pEnd;
  _pEnd=NULL;
}


ClassImp(DemoKalmanTask)
