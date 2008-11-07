// -------------------------------------------------------------------------
// -----                PndDchKalmanQATask2 source file                -----
// -----            Created 30.10.2008  by A. Wronska                  -----
// -----        based on the recotasks/demo/DemoTools by S.Neubert     -----
// -------------------------------------------------------------------------

// Panda Headers ----------------------
#include "PndDchKalmanQATask.h"
#include "CbmRootManager.h"
#include "Track.h"
#include "CbmMCPoint.h"
#include "CbmMCTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "PndDchTrackMatch.h"

// ROOT Headers -----------------------
#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
//#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TStyle.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>


PndDchKalmanQATask::PndDchKalmanQATask()
  : CbmTask("QA Task for Kalman od DCH"), fPersistence(kFALSE), fEvt(0)
{
  fTrackBranchName = "Track";
  fhP = NULL;
  fhChi2 = NULL;
  fThetaH = NULL;
  fPhiH = NULL;
  fCanvas = NULL;
}


PndDchKalmanQATask::~PndDchKalmanQATask() {
  
  WriteHistograms();

  if(fhP!=NULL)    delete fhP;
  if(fhChi2!=NULL) delete fhChi2;
  if(0!=fThetaH)   delete fThetaH;
  if(0!=fPhiH)     delete fPhiH;
  if(0!=fCanvas)   delete fCanvas;
}

InitStatus PndDchKalmanQATask::Init(){
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();
  if(ioman==0){
      Error("PndDchKalmanQATask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);
  if(fTrackArray==0) {
    Error("PndDchKalmanQATask::Init","track-array not found!");
    return kERROR;
  }

  fMCTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(fMCTrackArray==0) {
    Error("PndDchKalmanTask2::Init","MCTrack array not found!");
    return kERROR;
  }
  
  fDchTrackMatchArray = (TClonesArray*) ioman->GetObject("PndDchTrackMatch"); 
  if(fDchTrackMatchArray==0){
    Error("PndDchPrepareKalmanTracks2::Init","PndDchTrackMatch array not found!");
    return kERROR;
  }
 
  
  
  
  // // setup histograms
  fhP    = new TH1D("pullP","(p_{Rec}-p_{MC})/p_{MC}",60,-0.05,0.05);
  fhP->SetFillColor(9);
  fhChi2 = new TH1D("chi2","chi2",500,0,100);
  fhChi2->SetFillColor(2);
  fThetaH =new TH2D("theta","#theta_{rec}-#theta_{MC} vs #theta_{MC}",25,-5,25,100,-1,1.);
  fThetaH->SetFillColor(9);
  fPhiH   =new TH2D("phi",  "#phi_{rec}-#phi_{MC} vs #phi_{MC}",45,-180,180,100,-10,10);
  fPhiH->SetFillColor(9);
  
  return kSUCCESS;
}


void PndDchKalmanQATask::Exec(Option_t* opt) {
  if(fVerbose>0)
    std::cout<<"PndDchKalmanQATask::Exec Event "<<fEvt++<<std::endl;
  Int_t ntracks=fTrackArray->GetEntries();
  
  for(Int_t itr=0;itr<ntracks;++itr){
    Track* trk=new Track(*(Track*) fTrackArray->At(itr));
    if(trk->getTrackRep(0)->getStatusFlag()==0){
      TVector3 mom0 = trk->getMom();

      // AbsTrackRep* rep = trk->getCardinalRep()->clone();
      //       DetPlane pl(TVector3(0,0,0),TVector3(1,0,0),TVector3(0,1,0));
      //       TVector3 mom0=rep->getMom(pl);
      
      //      rep->extrapolate(firstHit->getDetPlane(rep));
      //       rep->setReferencePlane(firstHit->getDetPlane(rep));
      //       std::cout<<"************ Track at the first hit: \n\t Momentum ="<<
      // 	rep->getMom().Mag()<<"\t theta="<<rep->getMom().Theta()*TMath::RadToDeg()<<
      // 	"\t phi="<<rep->getMom().Phi()*TMath::RadToDeg()<<"\n";
      //       rep->Print();
            
      Int_t mcTrid = -1;
      Int_t id = 0;
      while(id<fDchTrackMatchArray->GetEntriesFast()){
	PndDchTrackMatch* dchtrmatch = (PndDchTrackMatch*) fDchTrackMatchArray->At(id);
	if(dchtrmatch->GetRecTrackID()==itr){
	  mcTrid = dchtrmatch->GetMCTrackID();
	  break;
	}
	id++;
      }
      if(mcTrid<0){
	if(fVerbose>0) std::cout<<"No MCTrack to compare the reco-track with!\n";
	return;
      }
      
      CbmMCTrack* mc=(CbmMCTrack*)fMCTrackArray->At(mcTrid);
      if(mc==0){
	Error("PndDchPrepareKalmanTracks::Exec","MCTrack Id=0 not found!");
	continue;
      }
      TVector3 mcmom = mc->GetMomentum();
      if(fhP) fhP->Fill((mom0.Mag()-mcmom.Mag())/mcmom.Mag());
      if(fhChi2) fhChi2->Fill(trk->getChiSqu());
      if(fThetaH) fThetaH->Fill(mcmom.Theta()*TMath::RadToDeg(),(-mcmom.Theta()+mom0.Theta())*TMath::RadToDeg());
      if(fPhiH) fPhiH->Fill(mcmom.Phi()*TMath::RadToDeg(),(-mcmom.Phi()+mom0.Phi())*TMath::RadToDeg());
    }
    delete trk;
  }
  return;
}
  
Bool_t  PndDchKalmanQATask::WriteHistograms(){
  TFile* file = CbmRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("DchKalmanQA");
  file->cd("DchKalmanQA");


  if(0!=fhP) fhP->Write();
  if(0!=fhChi2) fhChi2->Write();
  if(0!=fThetaH) fThetaH->Write();
  if(0!=fPhiH) fPhiH->Write();
  if(0!=fCanvas)  fCanvas->Write();
  return kTRUE;
}

void PndDchKalmanQATask::PlotHistograms(){
  gStyle->SetOptStat(111111);
  gStyle->SetPalette(1,0);
  fCanvas = new TCanvas("DchQACanvas","Results from the DchKalmanQA",1000,700);
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

 
void PndDchKalmanQATask::Finish(){
  WriteHistograms();
}
  

ClassImp(PndDchKalmanQATask)
