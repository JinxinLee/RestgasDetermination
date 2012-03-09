//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndMvdKalmanTask
//      see PndMvdKalmanTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt, TU Dresden             (Copied for MVD use)
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndMvdKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "TDatabasePDG.h"

// #include "PndSdsHit.h"
#include "FairMCPoint.h"

#include "PndSdsRecoHit.h"
#include "PndGeoHandling.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "GFDetPlane.h"
#include "FairTrackParH.h"

#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"

#include "PndDetectorList.h"

// Class Member definitions -----------


PndMvdKalmanTask::PndMvdKalmanTask()
  : FairTask("Kalman Filter"), fPersistence(kFALSE)
{
  fTrackBranchName = "MVDIdealGFTrackCand";
  PndGeoHandling::Instance();
}


PndMvdKalmanTask::~PndMvdKalmanTask()
{
  if(fPH!=NULL)delete fPH;
  if(fChi2H!=NULL)delete fChi2H;
}

InitStatus
PndMvdKalmanTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndMvdKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);

  if(fTrackArray==0)
    {
      Error("PndMvdKalmanTask::Init","track-array not found!");
      return kERROR;
    }


  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();

  TClonesArray* stripar=(TClonesArray*) ioman->GetObject("MVDHitsStrip");
  if(stripar==0){ //TODO Convention on detector number needed
    Error("PndMvdKalmanTask::Init","MVDHitsStrip array not found");
  } else {
    fTheRecoHitFactory->addProducer
      (ioman->GetBranchId("MVDHitsStrip"),new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(stripar));
  }

  TClonesArray* pixelar=(TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if(pixelar==0){
    Error("PndMvdKalmanTask::Init","MVDHitsPixel array not found");
  } else { //TODO Convention on detector number needed
    fTheRecoHitFactory->addProducer
      (ioman->GetBranchId("MVDHitsPixel"),new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(pixelar));
  }



  fPro = new FairGeanePro();


  // setup histograms
  fPH=new TH1D("pH","p",100,0.4,0.6);
  fChi2H=new TH1D("chi2H","chi2",100,0,20);
  fMassV0=new TH1D("massV0","massV0",100,0,5);
  fMassETAC=new TH1D("massEta","massEta",100,2.5,3.5);
  fMasses=new TH1D("masses","masses",100,0,5);



  return kSUCCESS;
}


void
PndMvdKalmanTask::Exec(Option_t* opt)
{
  std::cout<<"PndMvdKalmanTask::Exec"<<std::endl;
  // Reset output Array
  //if(fTrackArray==0) Fatal("Kalman::Exec)","No TrackArray");
  // fTrackArray->Delete();

  Int_t ntracks=fTrackArray->GetEntriesFast();

  // Detailed output
  if(fVerbose>1)std::cout<<" -I- PndMvdKalmanTask: contains "<<ntracks<<" Tracks."<<std::endl;
  if(fVerbose>2){
    std::cout<< " Detailed Debug info on the tracks:"<<std::endl;
    unsigned int detid=12345, index=12345;
    for(Int_t itr=0;itr<ntracks;++itr){
      GFTrackCand* trcnd = (GFTrackCand*)fTrackArray->At(itr);
      std::cout<< "GFTrackCand no. "<<itr<<" has "<<trcnd->getNHits()<<" hits."<<std::endl;
      std::cout<<"[ ihit | detid | index";
      for(unsigned int ihit=0;ihit<trcnd->getNHits();ihit++){
        trcnd->getHit(ihit,  detid,index); //detid and index are written here
        std::cout<<" ]\n[ "<<ihit<<" | "<<detid<<" | "<<index;
      }
      std::cout<<" ]"<<std::endl;
    }
  }

  // Cut too busy events TODO
  if(ntracks>20){
    std::cout<<"ntracks="<<ntracks<<" Evil Event! skipping"<<std::endl;
    return;
  }

  // Fitting ---------------- can go to another task!
  GFKalman fitter;
//   fitter.setVerbose(fVerbose);

  std::vector<TLorentzVector*> particles;
  std::vector<Int_t> signs;

  for(Int_t itr=0;itr<ntracks;++itr){
    std::cout<<"starting track"<<itr<<std::endl;
//     GFAbsTrackRep* rep = new LSLTrackRep();

  // Starting values for guessing
  Int_t PDGCode= 2212;
  TVector3 StartPos    = TVector3 (1.0,0.0,0.0);//cmn
  TVector3 StartPosErr = TVector3(0,0,0);
  TVector3 StartMom    = TVector3 (1.,0.,1.);
  StartMom.SetMagThetaPhi(1.05 , 70.*TMath::Pi()/360. , 0.);
//   StartMom.SetMag(1.1);
  TVector3 StartMomErr = TVector3(0,0,0);
  TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
  TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
  Double_t  fCharge= fParticle->Charge();
  // what to guess here?
  TVector3 U(1.,0.,0.);
  TVector3 V(0.,1.,0.);
  GFDetPlane start_pl(StartPos,U,V);
  GFAbsTrackRep* rep = new GeaneTrackRep(fPro,
                     start_pl,StartMom,
                     StartPosErr,StartMomErr,
                     fCharge,PDGCode);

    GFTrack* trk= new GFTrack(rep);
    trk->setCandidate(*(GFTrackCand*)fTrackArray->At(itr));
    //Track* trk=(Track*)fTrackArray->At(itr);

    // Load RecoHits
    try {
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      std::cout<<trk->getNumHits()<<" hits in track "
           <<itr<<std::endl;
    }
    catch(GFException& e) {
      std::cout << e.what() << std::endl;
      throw e;
    }

    // Start Fitter
    try{
      std::cout<<"starting fit"<<std::endl;
      fitter.processTrack(trk);
    }
    catch (GFException e){
      std::cout<<e.what()<<std::endl;
    }

    // Print Track Parameters after fit
    if(trk->getTrackRep(0)->getStatusFlag()==0){
      //trk->getTrackRep(0)->Print();
      GFDetPlane plane(TVector3(0,0,0.1),TVector3(1,0,0),TVector3(0,1,0));
      TVector3 p3=trk->getTrackRep(0)->getMom(plane);
      Double_t p=trk->getMom().Mag();
      fPH->Fill(p);

      TLorentzVector* p4=new TLorentzVector();
      p4->SetXYZM(p3.X(),p3.Y(),p3.Z(),0.493677);
      particles.push_back(p4);
      signs.push_back((Int_t)trk->getTrackRep(0)->getCharge());

      Double_t chi2=trk->getChiSqu();
      std::cout<<"ChiSq="<<chi2<<std::endl;
      fChi2H->Fill(chi2);
    }
  }

  std::cout<<"Fitting done"<<std::endl;

  // --- ANALYSIS ---
  if(particles.size()>20)return;
  std::cout<<"Starting Analysis"<<std::endl;

  // --- "standard" analysis plots
   for(UInt_t i=0;i<particles.size();++i){
     fMasses->Fill((particles[i])->M());
   }


  signs.clear();
  particles.clear();

  return;
}

void
PndMvdKalmanTask::WriteHistograms(const TString& filename){
  TFile* file = new TFile(filename,"UPDATE");
//   if(file->cd("Kalman")==false) file->mkdir("Kalman");
//   file->cd("Kalman");

  if(file->cd("Kalman")) file->Delete("Kalman;*");
  file->mkdir("Kalman");
  file->cd("Kalman");


  fPH->Write();
  delete fPH;
  fPH=NULL;

  fChi2H->Write();
  delete fChi2H;
  fChi2H=NULL;

  fMassV0->Write();
  delete fMassV0;
  fMassV0=NULL;

  fMassETAC->Write();
  delete fMassETAC;
  fMassETAC=NULL;

  fMasses->Write();
  delete fMasses;
  fMasses=NULL;

  file->Close();
  delete file;
}


ClassImp(PndMvdKalmanTask);
