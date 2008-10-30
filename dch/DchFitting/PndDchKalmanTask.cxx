// -------------------------------------------------------------------------
// -----                 PndDchKalmanTask source file                  -----
// -----            Created 15.05.2008  by A. Wronska                  -----
// -----        based on the recotasks/demo code  by S.Neubert         -----
// -------------------------------------------------------------------------

// Panda Headers ----------------------
#include "CbmRootManager.h"
#include "PndDchKalmanTask.h"
#include "Track.h"
#include "CbmMCTrack.h"
#include "PndDchRecoHit.h"
#include "PndDchCylinderHit.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// ROOT Headers --------
#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TStyle.h"


PndDchKalmanTask::PndDchKalmanTask()
  : CbmTask("Kalman Filter"), fPersistence(kFALSE),fSmooth(kFALSE), fEvtCount(0), fNumIt(1)
{
  fTrackBranchName = "Track";
}


PndDchKalmanTask::~PndDchKalmanTask()
{
  if(fhP!=NULL) delete fhP;
  if(fhChi2!=NULL) delete fhChi2;
  if(0!=fThetaH) delete fThetaH;
  if(0!=fPhiH) delete fPhiH;
  if(0!=fChi2HitversusZ) delete fChi2HitversusZ;
  if(0!=fCanvas) delete fCanvas;
}

InitStatus PndDchKalmanTask::Init()
{
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndDchKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);
  
  if(fTrackArray==0) {
    Error("PndDchKalmanTask::Init","Track array not found!");
    return kERROR;
  }
  
  fMCTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(fMCTrackArray==0) {
    Error("PndDchKalmanTask::Init","MCTrack array not found!");
    return kERROR;
  }
  
  // Build hit factory -----------------------------
  fTheRecoHitFactory = new RecoHitFactory();
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndDchCylinderHit");
  if(ar==0){
    Error("PndDchKalmanTask::Init","PndDchCylinderHit array not found!");
  }
  else{ 
    fTheRecoHitFactory->addProducer(1,new RecoHitProducer<PndDchCylinderHit,PndDchRecoHit>(ar));
  } // "1" stands here for kind of detector
  
  fCanvas = new TCanvas("Results of Kalman","Results of Kalman",1000,700);
  // setup histograms
  fhP    = new TH1D("pullP","(p_{RecTarget}-p_{MC})/p_{MC}",60,-0.3,0.3);
  fhP->SetFillColor(9);
  //fhP->SetOptStat(111111);
  fhChi2 = new TH1D("chi2","chi2",50,0,100);
  fhChi2->SetFillColor(2);
  //fhChi2->SetOptStat(111111);
  fChi2HitversusZ = new TH2D("chi2HitversusZ","chi2hit versus Z",50,100,200,20,0,200);
  fThetaH =new TH2D("theta","#theta_{rec}-#theta_{MC} vs #theta_{MC}",25,0,25,40,-10,10.);
  fThetaH->SetFillColor(9);
  //fThetaH->SetOptStat(111111);
  fPhiH   =new TH2D("phi",  "#phi_{rec}-#phi_{MC} vs #phi_{MC}",45,-180,180,40,-10,10);
  fPhiH->SetFillColor(9);
  //fPhiH->SetOptStat(111111);
  return kSUCCESS;
}


void
PndDchKalmanTask::Exec(Option_t* opt)
{
  std::cout<<"\n\n-------------------------------------------------------\n";
  std::cout<<"      PndDchKalmanTask::Exec                       "<<std::endl;
  std::cout<<"-------------------------------------------------------"<<std::endl;
  
  Int_t ntracks=fTrackArray->GetEntriesFast();
  
  Kalman fitter;
  fitter.setNumIterations(fNumIt);
  
  for(Int_t itr=0; itr<ntracks; ++itr){
    Track* trk = (Track*)fTrackArray->At(itr);
    // Load RecoHits 
    try {
      std::cout<<"Creating reco hits..."<<std::endl;
      std::cout<<"the candidate had "<<trk->getCand().getNHits()<<" hits"<<std::endl;
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      std::cout<<"\t"<<trk->getNumHits()<<" reco hits were created for track "
	       <<itr<<std::endl;
    }
    catch(FitterException& e) {
      std::cout<<"Creation of reco hits failed :-( "<<std::endl;
      std::cout << e.what() << std::endl;
      throw e;
    }
    
    // Start Fitter
    try{
      fitter.processTrack(trk);
      if(fSmooth)fitter.smoothing(trk);
    }
    catch (FitterException e){
      std::cout<<"*** FITTER EXCEPTION ***"<<std::endl;
      std::cout<<e.what()<<std::endl;
      
    }

    // Fill some histos after fit

    if(trk->getTrackRep(0)->getStatusFlag()==0){
      AbsRecoHit* firstHit = trk->getHit(0);
      AbsTrackRep* rep = trk->getCardinalRep()->clone();
      std::cout<<"************ Track at the last hit: \n\t Momentum ="<<
	rep->getMom().Mag()<<"\t theta="<<rep->getMom().Theta()*TMath::RadToDeg()<<
	"\t phi="<<rep->getMom().Phi()*TMath::RadToDeg()<<"\n";
      rep->Print();
      // rep->extrapolate(firstHit->getDetPlane(rep));
//       rep->setReferencePlane(firstHit->getDetPlane(rep));
//       std::cout<<"************ Track at the first hit: \n\t Momentum ="<<
// 	rep->getMom().Mag()<<"\t theta="<<rep->getMom().Theta()*TMath::RadToDeg()<<
// 	"\t phi="<<rep->getMom().Phi()*TMath::RadToDeg()<<"\n";
//       rep->Print();

      CbmMCTrack* mc=(CbmMCTrack*)fMCTrackArray->At(0);
      if(mc==0){
	Error("PndDchPrepareKalmanTracks::Exec","MCTrack Id=0 not found!");
	continue;
      }
      TVector3 mcmom = mc->GetMomentum();
      DetPlane pl(TVector3(0,0,0),TVector3(1,0,0),TVector3(0,1,0));
      TVector3 mom0=rep->getMom(pl);

      std::cout<<"************ Track at the target: \n\t Momentum ="<<
	mom0.Mag()<<"\t theta="<<mom0.Theta()*TMath::RadToDeg()<<
	"\t phi="<<mom0.Phi()*TMath::RadToDeg()<<"\n";

      //      fhP->Fill((mom0.Mag()-mcmom.Mag())/mcmom.Mag());
      //fhChi2->Fill(trk->getChiSqu());
      //fThetaH->Fill(mcmom.Theta()*180./3.1416,(mom0.Theta()-mcmom.Theta())/mcmom.Theta()*180./3.1416);
      //fPhiH->Fill(mcmom.Phi()*180./3.1416,(mom0.Phi()-mcmom.Phi())/mcmom.Phi()*180./3.1416);


      for(Int_t ihit = 0; ihit<trk->getNumHits(); ihit++){
	AbsRecoHit* hit = trk->getHit(ihit);
	Double_t z = hit->getDetPlane(rep).getO().Z();
	Double_t chi2hit = fitter.getChi2Hit(hit,rep);
	//	fChi2HitversusZ->Fill(z,chi2hit);
      }
      ++fTrackCount;
    }
  }
  
  return;
}

void PndDchKalmanTask::WriteHistograms(){
  TFile* file = CbmRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("DchKalman");
  file->cd("DchKalman");

  fhP->Write();
  fhChi2->Write();
  fThetaH->Write();
  fPhiH->Write();
  fChi2HitversusZ->Write();
  fCanvas->Write();

}

void PndDchKalmanTask::PlotHistograms(){
  gStyle->SetOptStat(111111);
  gStyle->SetPalette(1,0);
  fCanvas->Divide(2,2);
  
  fCanvas->cd(1);
  gPad->SetGrid(1,0);
  fhP->Draw();
  fCanvas->cd(2);
  gPad->SetGrid(1,1);
  fPhiH->Draw("colz");
  fCanvas->cd(3);
  gPad->SetGrid(1,1);
  fThetaH->Draw("colz");
  fCanvas->cd(4);
  gPad->SetGrid(1,0);
  fhChi2->Draw();
}

void PndDchKalmanTask::Finish(){
  WriteHistograms();
}


ClassImp(PndDchKalmanTask)
