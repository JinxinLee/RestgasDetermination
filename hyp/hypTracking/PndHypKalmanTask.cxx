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
#include "PndHypKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
#include "TDatabasePDG.h"
// #include "PndHypHit.h"
#include "FairMCPoint.h"

#include "PndHypRecoHit.h"


#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "DetPlane.h"

#include "TDatabasePDG.h"
#include "FairTrackParH.h"
	
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"
// Class Member definitions -----------


PndHypKalmanTask::PndHypKalmanTask()
  : FairTask("Kalman Filter"), fPersistence(kFALSE)
{
  fTrackBranchName = "HypTrackCand";
}


PndHypKalmanTask::~PndHypKalmanTask()
{
  if(fPH!=NULL)delete fPH;
  if(fChi2H!=NULL)delete fChi2H;
}

InitStatus
PndHypKalmanTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndHypKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);
  
  if(fTrackArray==0)
    {
      Error("PndHypKalmanTask::Init","track-array not found!");
      return kERROR;
    }
  

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new RecoHitFactory();

  TClonesArray* ar=(TClonesArray*) ioman->GetObject("HypHit");
   if(ar==0){
     Error("PndHypKalmanTask::Init","MVDHit array not found");
   }
   else{ 

  fTheRecoHitFactory->addProducer(2,new RecoHitProducer<PndHypHit,PndHypRecoHit>(ar));
  
  
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
PndHypKalmanTask::Exec(Option_t* opt)
{
  std::cout<<"PndHypKalmanTask::Exec"<<std::endl;
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
      TrackCand* trcnd = (TrackCand*)fTrackArray->At(itr);
      std::cout<< "TrackCand no. "<<itr<<" has "<<trcnd->getNHits()<<" hits."<<std::endl;
      std::cout<<"[ ihit | detid | index";
      for(unsigned int ihit=0;ihit<trcnd->getNHits();ihit++){
	trcnd->getHit(ihit,  detid,index);
	std::cout<<" ]\n[ "<<ihit<<" | "<<detid<<" | "<<index;
      }
      std::cout<<" ]"<<std::endl;
    }
  }
  
  if(ntracks>20){
    std::cout<<"ntracks="<<ntracks<<" Evil Event! skipping"<<std::endl;
    return;
  }

  // Fitting ---------------- can go to another task!
  Kalman fitter;

  std::vector<TLorentzVector*> particles;
  std::vector<Int_t> signs;
  
  for(Int_t itr=0;itr<ntracks;++itr){
    std::cout<<"starting track"<<itr<<std::endl;
    //AbsTrackRep* rep = new LSLTrackRep();
   

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
    DetPlane start_pl(StartPos,U,V);
    AbsTrackRep* rep = new GeaneTrackRep(fPro,
					 start_pl,StartMom,
					 StartPosErr,StartMomErr,
					 fCharge,PDGCode);
   
    Track* trk= new Track(rep);
    trk->setCandidate(*(TrackCand*)fTrackArray->At(itr));
    //Track* trk=(Track*)fTrackArray->At(itr);
    // Load RecoHits 
	try {
	  trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
	  std::cout<<trk->getNumHits()<<" hits in track "
			   <<itr<<std::endl;
    }
	catch(FitterException& e) {
	  std::cout << e.what() << std::endl;
	  throw e;
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
      //trk->getTrackRep(0)->Print();
      DetPlane plane(TVector3(0,0,0.1),TVector3(1,0,0),TVector3(0,1,0));
      TVector3 p3=trk->getTrackRep(0)->getMom(plane);
      Double_t p=trk->getMom().Mag();
      std::cout<<" oye"<<p<<std::endl;
      
      fPH->Fill(p);
      
      TLorentzVector* p4=new TLorentzVector();
      p4->SetXYZM(p3.X(),p3.Y(),p3.Z(),0.493677);
      particles.push_back(p4);
      signs.push_back((Int_t)trk->getTrackRep(0)->getCharge());

      Double_t chi2=trk->getChiSqu();
      fChi2H->Fill(chi2);

    }

  }

  std::cout<<"Fitting done"<<std::endl;

  // --- ANALYSIS ---
  if(particles.size()>20)return;
  std::cout<<"Starting Analysis"<<std::endl;


//   std::vector<TLorentzVector> V0s;
//   // try to reconstruct V0s
//   for(Int_t i=0;i<particles.size();++i){
//     for(Int_t j=i+1;j<particles.size();++j){
//       TLorentzVector v0=*(particles[i])+*(particles[j]);
//       V0s.push_back(v0);
//       if(signs[i]!=signs[j]){
//         fMassV0->Fill(v0.M());
//       }
//     }
//     delete particles[i];
//   }
// 
//   Double_t mphi=1.020;
// 
//   // try to build a etac
//   for(Int_t i=0;i<V0s.size();++i){
//     if(fabs(V0s[i].M()-mphi)>0.03)continue;
//     for(Int_t j=i+1;j<V0s.size();++j){
//       if(fabs(V0s[j].M()-mphi)>0.03)continue;
//       TLorentzVector v0=(V0s[i])+(V0s[j]);
//       fMassETAC->Fill(v0.M());
//     }
//   }
// 
//   V0s.clear();


  // --- "standard" analysis plots
   for(Int_t i=0;i<particles.size();++i){
     fMasses->Fill((particles[i])->M());
   }
 

  signs.clear();
  particles.clear();

  return;
}

void 
PndHypKalmanTask::WriteHistograms(const TString& filename){
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


ClassImp(PndHypKalmanTask)
