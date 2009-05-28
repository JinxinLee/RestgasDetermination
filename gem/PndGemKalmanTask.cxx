//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndGemKalmanTask
//      see PndGemKalmanTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt, TU Dresden             (Copied for MVD use)
//      Radoslaw Karabowicz, GSI            (converted for GEMs)
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndGemKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
#include "TDatabasePDG.h"

// #include "PndGemHit.h"
#include "FairMCPoint.h"

#include "PndGemRecoHit.h"

#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "DetPlane.h"
#include "FairTrackParH.h"

#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"

#include "TRandom3.h"

// Class Member definitions -----------


PndGemKalmanTask::PndGemKalmanTask()
  : FairTask("Kalman Filter"), fPersistence(kFALSE)
{
  fTrackBranchName = "FSTracks";

  fMomentum = 1.;
  fTheta = 5;
  fPhi = 0;
}


PndGemKalmanTask::~PndGemKalmanTask()
{
  if(fPH!=NULL)delete fPH;
  if(fChi2H!=NULL)delete fChi2H;
}

InitStatus
PndGemKalmanTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndGemKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);

  if(fTrackArray==0)
    {
      Error("PndGemKalmanTask::Init","track-array not found!");
      return kERROR;
    }


  // Build hit factory -----------------------------
  fTheRecoHitFactory = new RecoHitFactory();

  TClonesArray* hitar=(TClonesArray*) ioman->GetObject("GEMHit");
  if(hitar==0){ //TODO Convention on detector number needed
    Error("PndGemKalmanTask::Init","GEMHit array not found");
  } else {
    fTheRecoHitFactory->addProducer
      (1,new RecoHitProducer<PndGemHit,PndGemRecoHit>(hitar));
  }

  fPro = new FairGeanePro();


  // setup histograms
  fPH=new TH1D("pH","p",100,0.4,0.6);
  fChi2H=new TH1D("chi2H","chi2",100,0,20);
  fMassV0=new TH1D("massV0","massV0",100,0,5);
  fMassETAC=new TH1D("massEta","massEta",100,2.5,3.5);
  fMasses=new TH1D("masses","masses",100,0,5);
 
  fhMomentumX2D = new TH2D("fhMomentumX2D","fhMomentumX2D",1000,-5.,5.,1000,-5.,5.);
  fhMomentumY2D = new TH2D("fhMomentumY2D","fhMomentumY2D",1000,-5.,5.,1000,-5.,5.);
  fhMomentumZ2D = new TH2D("fhMomentumZ2D","fhMomentumZ2D",2000,-20.,20.,2000,-20.,20.);
  fhMomentumM2D = new TH2D("fhMomentumM2D","fhMomentumM2D",2000,  0.,20.,2000,  0.,20.);
  fhMomentumT2D = new TH2D("fhMomentumT2D","fhMomentumT2D",2100, -3.5,3.5,2100, -3.5,3.5);
  fhMomentumP2D = new TH2D("fhMomentumP2D","fhMomentumP2D",2100, -3.5,3.5,2100, -3.5,3.5);
  fhMomentumX1D = new TH1D("fhMomentumX1D","fhMomentumX1D",2000,-10.,10.);
  fhMomentumY1D = new TH1D("fhMomentumY1D","fhMomentumY1D",2000,-10.,10.);
  fhMomentumZ1D = new TH1D("fhMomentumZ1D","fhMomentumZ1D",2000,-10.,10.);
  fhMomentumM1D = new TH1D("fhMomentumM1D","fhMomentumM1D",2000,-10.,10.);
  fhMomentumT1D = new TH1D("fhMomentumT1D","fhMomentumT1D",2100,  -7.,7.);
  fhMomentumP1D = new TH1D("fhMomentumP1D","fhMomentumP1D",2100,  -7.,7.);
  fhMomentumRes = new TH2D("fhMomentumRes","fhMomentumRes",40,0.,20.,2000,-20.,20.);

  fhPositionX2D = new TH2D("fhPositionX2D","fhPositionX2D",2000,-100.,100.,2000,-100.,100.);
  fhPositionY2D = new TH2D("fhPositionY2D","fhPositionY2D",2000,-100.,100.,2000,-100.,100.);
  fhPositionX1D = new TH1D("fhPositionX1D","fhPositionX1D",2000,-10.,10.);
  fhPositionY1D = new TH1D("fhPositionY1D","fhPositionY1D",2000,-10.,10.);


  return kSUCCESS;
}


void
PndGemKalmanTask::Exec(Option_t* opt)
{
  std::cout<<"PndGemKalmanTask::Exec"<<std::endl;
  // Reset output Array
  //if(fTrackArray==0) Fatal("Kalman::Exec)","No TrackArray");
  // fTrackArray->Delete();

  Int_t ntracks=fTrackArray->GetEntriesFast();

  // Detailed output
  if(fVerbose>1)std::cout<<" -I- PndGemKalmanTask: contains "<<ntracks<<" Tracks."<<std::endl;
  if(fVerbose>2){
    std::cout<< " Detailed Debug info on the tracks:"<<std::endl;
    unsigned int detid=12345, index=12345;
    for(Int_t itr=0;itr<ntracks;++itr){
      Track* trac = (Track*)fTrackArray->At(itr);
      TrackCand trcnd = (TrackCand)trac->getCand();
      //      TrackCand* trcnd = (TrackCand*)fTrackArray->At(itr);
      std::cout<< "TrackCand no. "<<itr<<" has "<<trcnd.getNHits()<<" hits."<<std::endl;
      std::cout<<"[ ihit | detid | index";
      for(unsigned int ihit=0;ihit<trcnd.getNHits();ihit++){
        trcnd.getHit(ihit,  detid,index); //detid and index are written here
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
  Kalman fitter;
  fitter.setNumIterations(fNumIt);
  //   fitter.setVerbose(fVerbose);

  std::vector<TLorentzVector*> particles;
  std::vector<Int_t> signs;

  for(Int_t itr=0;itr<ntracks;++itr){
    std::cout<<"starting track"<<itr<<std::endl;
    //     AbsTrackRep* rep = new LSLTrackRep();

      Track* trac = (Track*)fTrackArray->At(itr);
      TrackCand trcnd = (TrackCand)trac->getCand();
      TVector3 beforePos = trac->getPos();
      TVector3 beforeMom = trac->getMom();

      beforePos.SetXYZ(0.,0.,0.);
      beforeMom.SetMagThetaPhi(fMomentum,TMath::DegToRad()*(Double_t)fTheta,TMath::DegToRad()*(Double_t)fPhi);

      Double_t rad = TMath::Sqrt(beforePos.X()*beforePos.X()+beforePos.Y()*beforePos.Y());
      if ( rad > 20 ) 
	std::cout << "RADIUS BIGGER THAN 20" << std::endl;
    
    // Starting values for guessing
      Int_t PDGCode= 211;
//     TVector3 StartPos    = TVector3 (beforePos.X()+gRandom->Gaus(0,0.1),
// 				     beforePos.Y()+gRandom->Gaus(0,0.1),
// 				     beforePos.Z()+gRandom->Gaus(0,0.1));
    TVector3 StartPos    = TVector3 (0.,0.,0.);

				     //1.0,0.0,0.0);//cmn
    TVector3 StartPosErr = TVector3(0.01,0.01,0.01);
    TVector3 StartMom    = TVector3 (1.,0.,1.); 
    std::cout << "setting mag to " << beforeMom.Mag() << std::endl;
//     StartMom.SetMagThetaPhi(beforeMom.Mag()-0.5  ,//gRandom->Gaus(0,0.1),
//  			    beforeMom.Theta(),//+gRandom->Gaus(0,0.1),
//  			    beforeMom.Phi()  );//+gRandom->Gaus(0,0.1));
    StartMom.SetMagThetaPhi(fMomentum+0.05,TMath::DegToRad()*(Double_t)fTheta,TMath::DegToRad()*(Double_t)fPhi);
    std::cout << "momenta set to " << StartMom.Mag() << std::endl;
//    StartMom.SetMagThetaPhi(2.,10.*TMath::Pi()/180.,0.);
    //5.05 , 30.*TMath::Pi()/360. , 0.);
//     StartMom.SetXYZ(0.01*TMath::Ceil(100.*beforeMom.X()),
// 		    0.01*TMath::Ceil(100.*beforeMom.Y()),
// 		    0.01*TMath::Ceil(100.*beforeMom.Z()));
    //   StartMom.SetMag(1.1);
    TVector3 StartMomErr = TVector3(0.1*StartMom);//1.,1.,1.);
    TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
    TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
    Double_t  fCharge= fParticle->Charge()/3.;
    // what to guess here?
    TVector3 U(1.,0.,0.);
    TVector3 V(0.,1.,0.);
    DetPlane start_pl(StartPos,U,V);

    AbsTrackRep* rep = new GeaneTrackRep(fPro,
 					 start_pl,StartMom,
 					 StartPosErr,StartMomErr,
 					 fCharge,PDGCode);
    
    /*    TVector3 dir=StartMom.Unit();
    Double_t dxdz=dir.X()/dir.Z();
    Double_t dydz=dir.Y()/dir.Z();
    Double_t qp=fCharge/StartMom.Mag();
    std::cout << "2momenta set to " << StartMom.Mag() << " but fcharge = " << fCharge << std::endl;
    std::cout << "moment moment " << dxdz << " " << dydz << " " << qp << std::endl;
    AbsTrackRep* rep = new LSLTrackRep(StartPos.Z(),StartPos.X(),StartPos.Y(),dxdz,dydz,qp,
				       StartPosErr.X(),StartPosErr.Y(),0.1,0.1,0.1,NULL);
    */
  
    Track* trk= new Track(rep);


    trk->setCandidate(trcnd);
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
      fPH->Fill(p);
      
      TLorentzVector* p4=new TLorentzVector();
      p4->SetXYZM(p3.X(),p3.Y(),p3.Z(),0.493677);
      particles.push_back(p4);
      signs.push_back((Int_t)trk->getTrackRep(0)->getCharge());
      
      Double_t chi2=trk->getChiSqu();
      std::cout<<"ChiSq="<<chi2<<std::endl;
      fChi2H->Fill(chi2);
    }

    DetPlane plane(TVector3(0,0,0.1),TVector3(1,0,0),TVector3(0,1,0));
      TVector3 resultMom=trk->getTrackRep(0)->getMom(plane);
 
    TVector3 resultPos = trk->getPos();
    //TVector3 resultMom = trk->getMom();
    std::cout << "PHIMC = " << beforeMom.Phi() << " PHIRECO = " << resultMom.Phi() << std::endl;

    std::cout << "********************************************************" << std::endl;
    std::cout << "result pos = (" 
	      << resultPos.X() << ","
	      << resultPos.Y() << ","
	      << resultPos.Z() << ")" << std::endl;
    std::cout << "result mom = (" 
	      << resultMom.X() << ","
	      << resultMom.Y() << ","
	      << resultMom.Z() << ") ----> " << resultMom.Mag() << std::endl;

    GeaneTrackRep* tempRep = (GeaneTrackRep*)trac->getTrackRep(0);
    std::cout << "BUT IT WAS " << tempRep->getPDG() << std::endl;

    fhMomentumX2D->Fill(beforeMom.X(),resultMom.X());
    fhMomentumY2D->Fill(beforeMom.Y(),resultMom.Y());
    fhMomentumZ2D->Fill(beforeMom.Z(),resultMom.Z());
    fhMomentumM2D->Fill(beforeMom.Mag(),resultMom.Mag());
    fhMomentumT2D->Fill(beforeMom.Theta(),resultMom.Theta());
    fhMomentumP2D->Fill(beforeMom.Phi(),resultMom.Phi());
    fhMomentumX1D->Fill(beforeMom.X()+resultMom.X());
    fhMomentumY1D->Fill(beforeMom.Y()+resultMom.Y());
    fhMomentumZ1D->Fill(beforeMom.Z()+resultMom.Z());
    fhMomentumM1D->Fill(beforeMom.Mag()-resultMom.Mag());
    fhMomentumT1D->Fill(beforeMom.Theta()-resultMom.Theta());
    fhMomentumP1D->Fill(beforeMom.Phi()-resultMom.Phi());
    fhMomentumRes->Fill(beforeMom.Mag(),100.*(beforeMom.Mag()-resultMom.Mag())/beforeMom.Mag());
    
    fhPositionX2D->Fill(beforePos.X(),resultPos.X());
    fhPositionY2D->Fill(beforePos.Y(),resultPos.Y());
    fhPositionX1D->Fill(beforePos.X()-resultPos.X());
    fhPositionY1D->Fill(beforePos.Y()-resultPos.Y());

  }

  std::cout<<"Fitting done"<<std::endl;
  
  // --- ANALYSIS ---
  if(particles.size()>20)return;
  std::cout<<"Starting Analysis"<<std::endl;
  
  // --- "standard" analysis plots
  for(Int_t i=0;i<particles.size();++i){
    fMasses->Fill((particles[i])->M());
  }
  
  
  signs.clear();
  particles.clear();
  
  return;
}

void
PndGemKalmanTask::WriteHistograms(){
  std::cout<<"  PndDchKalmanQATask::WriteHistograms() "<<std::endl;
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("GemKalmanQA");
  file->cd("GemKalmanQA");

  
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

  fhMomentumX2D->Write();
  fhMomentumY2D->Write();
  fhMomentumZ2D->Write(); 
  fhMomentumM2D->Write();
  fhMomentumT2D->Write();
  fhMomentumP2D->Write();
  fhMomentumX1D->Write();
  fhMomentumY1D->Write();
  fhMomentumZ1D->Write();
  fhMomentumM1D->Write();
  fhMomentumT1D->Write();
  fhMomentumP1D->Write();
  fhMomentumRes->Write();

  fhPositionX2D->Write();
  fhPositionY2D->Write();
  fhPositionX1D->Write();
  fhPositionY1D->Write();

  delete fhMomentumX2D;
  delete fhMomentumY2D;
  delete fhMomentumZ2D;
  delete fhMomentumM2D;  
  delete fhMomentumT2D;
  delete fhMomentumP2D;
  delete fhMomentumX1D;
  delete fhMomentumY1D;
  delete fhMomentumZ1D; 
  delete fhMomentumM1D;
  delete fhMomentumT1D;
  delete fhMomentumP1D;
  delete fhMomentumRes;

  delete fhPositionX2D;
  delete fhPositionY2D;
  delete fhPositionX1D;
  delete fhPositionY1D;

  fhMomentumX2D = NULL;
  fhMomentumY2D = NULL;
  fhMomentumZ2D = NULL;
  fhMomentumM2D = NULL;
  fhMomentumT2D = NULL;
  fhMomentumP2D = NULL;
  fhMomentumX1D = NULL;
  fhMomentumY1D = NULL;
  fhMomentumZ1D = NULL;
  fhMomentumM1D = NULL;
  fhMomentumT1D = NULL;
  fhMomentumP1D = NULL;
  fhMomentumRes = NULL;

  fhPositionX2D = NULL;
  fhPositionY2D = NULL;
  fhPositionX1D = NULL;
  fhPositionY1D = NULL;


  //  file->Close();
  //  delete file;
}

void PndGemKalmanTask::Finish(){
  WriteHistograms();
}


ClassImp(PndGemKalmanTask);
