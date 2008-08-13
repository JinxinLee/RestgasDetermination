//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndLheKalmanTask
//      see PndLheKalmanTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Stefano Spataro, JLU Gießen         (Copied for LHE use)
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndLheKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
#include "TDatabasePDG.h"

// #include "PndMvdHit.h"
#include "CbmMCPoint.h"

#include "PndMvdRecoHit.h"
#include "PndTpcPlanarRecoHit.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheHit.h"

#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "TH1D.h"
#include "TObjArray.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "DetPlane.h"
#include "CbmTrackParH.h"

#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "CbmGeanePro.h"

// Class Member definitions -----------


PndLheKalmanTask::PndLheKalmanTask()
  : CbmTask("Kalman Filter"), fPersistence(kFALSE)
{
  fTrackBranchName = "PndTpcLheTrack";
  fFitTrackArray = new TClonesArray("Track");
  fUseGeane = kFALSE;
  fSmoothing = kFALSE;
  fNumIt = 1;
}


PndLheKalmanTask::~PndLheKalmanTask()
{
  if(fPH!=NULL)delete fPH;
  if(fChi2H!=NULL)delete fChi2H;
}

InitStatus
PndLheKalmanTask::Init()
{
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndLheKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);
  
  if(fTrackArray==0)
    {
      Error("PndLheKalmanTask::Init","track-array not found!");
      return kERROR;
    }
  

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new RecoHitFactory();

  TClonesArray* stripar=(TClonesArray*) ioman->GetObject("MVDHitsStrip");
  if(stripar==0)
    { //TODO Convention on detector number needed
      Error("PndLheKalmanTask::Init","MVDHitsStrip array not found");
    } 
  else
    {
      fTheRecoHitFactory->addProducer(4,new RecoHitProducer<PndMvdHit,PndMvdRecoHit>(stripar));
    }
  
  TClonesArray* pixelar=(TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if(pixelar==0)
    {
      Error("PndLheKalmanTask::Init","MVDHitsPixel array not found");
    } 
  else 
    { //TODO Convention on detector number needed
      fTheRecoHitFactory->addProducer(3,new RecoHitProducer<PndMvdHit,PndMvdRecoHit>(pixelar));
    }
  
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  if(ar==0)
    {
      Error("PndLheKalmanTask::Init","PndTpcCluster array not found");
    }
  else
    {
      fTheRecoHitFactory->addProducer(2,new RecoHitProducer<PndTpcCluster,PndTpcPlanarRecoHit>(ar));
    }
  
  if (fUseGeane)  fPro = new CbmGeanePro();
  
  CbmRootManager::Instance()->
    Register("LheGenTrack","Lhe", fFitTrackArray, kTRUE);
  
  // setup histograms
  fPH=new TH1D("pH","p",100,0.4,0.6);
  fChi2H=new TH1D("chi2H","chi2",100,0,20);
  fMassV0=new TH1D("massV0","massV0",100,0,5);
  fMassETAC=new TH1D("massEta","massEta",100,2.5,3.5);
  fMasses=new TH1D("masses","masses",100,0,5);

  return kSUCCESS;
}


void PndLheKalmanTask::Exec(Option_t* opt)
{
  if (fVerbose>0) std::cout<<"PndLheKalmanTask::Exec"<<std::endl;
  // Reset output Array
  //if(fTrackArray==0) Fatal("Kalman::Exec)","No TrackArray");
  
  fFitTrackArray->Delete();
  
  Int_t ntracks=fTrackArray->GetEntriesFast();

  // Detailed output
  if (fVerbose>1) std::cout << " -I- PndLheKalmanTask: contains " << ntracks << " Tracks."<< std::endl;
    
  // Cut too busy events TODO
  if(ntracks>20){
    std::cout<<" -I- PndLheKalmanTask::Exec: ntracks=" << ntracks << " Evil Event! skipping" << std::endl;
    return;
  }
  
  std::vector<TLorentzVector*> particles;
  std::vector<Int_t> signs;

  // Fitting ---------------- can go to another task!
  Kalman fitter;
  fitter.setLazy(1); // tell the fitter to skip hits if error occurs
  fitter.setNumIterations(fNumIt);

  for(Int_t itr=0;itr<ntracks;++itr){
    if (fVerbose>1) std::cout<<"starting track"<<itr<<std::endl;
    PndTpcLheTrack *lheTrack = (PndTpcLheTrack*)fTrackArray->At(itr);
    
    TVector3 StartPos(0., 0., 0.); 
    //TVector3 StartPos    = lheTrack->GetFirstHit().GetCoord();
    //TVector3 StartPosErr = lheTrack->GetFirstHit().GetError();
    TVector3 StartPosErr(0.5, 0.5, 0.5);
    TVector3 StartMom(0., 0., 0.);
    //TVector3 StartMom = lheTrack->GetMomentum();
    TVector3 StartMomErr = TVector3(0.1*StartMom);
    Float_t phi = lheTrack->ExtrapolateToZ(&StartMom, &StartPos, 0.);
    
    // Starting values for guessing
    Int_t PDGCode= 211;
    TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
    TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
    Double_t  fCharge= lheTrack->GetCharge();
    // what to guess here?
    TVector3 U(1.,0.,0.);
    TVector3 V(0.,1.,0.);
    DetPlane start_pl(StartPos,U,V);

    AbsTrackRep* rep;
    
    if (fUseGeane)
      {
	rep = new GeaneTrackRep(fPro,
				start_pl,StartMom,
				StartPosErr,StartMomErr,
				fCharge,PDGCode);
      }
    else
      {
	
	TVector3 dir=StartMom.Unit();
	Double_t dxdz=dir.X()/dir.Z();
	Double_t dydz=dir.Y()/dir.Z();
	Double_t qp=fCharge/StartMom.Mag();
	rep = new LSLTrackRep(StartPos.Z(),StartPos.X(),StartPos.Y(),dxdz,dydz,qp,
			      StartPosErr.X(),StartPosErr.Y(),0.1,0.1,0.1,NULL);
      }
    
    Track* trk= new Track(rep);
    trk->setCandidate(*(TrackCand*)lheTrack->GetTrackCand());
         
    // Load RecoHits
    try {
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      if (fVerbose>0) std::cout<<trk->getNumHits()<<" hits in track "
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
      if(fSmoothing)fitter.smoothing(trk);
    }
    catch (FitterException e){
      std::cout<<"*** FITTER EXCEPTION ***"<<std::endl;
      std::cout<<e.what()<<std::endl;

    }

    if (fVerbose>0) std::cout<<"SUCESSFULL FIT!"<<std::endl;


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
      if (fVerbose>0) std::cout<<"ChiSq="<<chi2<<std::endl;
      fChi2H->Fill(chi2);
      }

      
    AddTrack(trk);
    lheTrack->SetFitTrackIndex(fFitTrackArray->GetEntriesFast()-1);
    
  }

  if (fVerbose>0) std::cout<<"Fitting done"<<std::endl;

  // --- ANALYSIS ---
  if(particles.size()>20)return;
  if (fVerbose>0) std::cout<<"Starting Analysis"<<std::endl;

  // --- "standard" analysis plots
   for(Int_t i=0;i<particles.size();++i){
     fMasses->Fill((particles[i])->M());
   }
 

  signs.clear();
  particles.clear();

  return;
}


void 
PndLheKalmanTask::WriteHistograms(const TString& filename){
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

//_________________________________________________________________
Track* PndLheKalmanTask::AddTrack(Track* track) {
  // Creates a new hit in the TClonesArray.
  
  TClonesArray& trkRef = *fFitTrackArray;
  Int_t size = trkRef.GetEntriesFast();
  return new(trkRef[size]) Track(*track);
}

ClassImp(PndLheKalmanTask);
