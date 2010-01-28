// ------------------------------------------------------------------------------
// -----                PndDchPreFitterTRQATask2 source file                -----
// -----            Created 03.11.2008  by A. Wronska, P.Hawranek           -----
// ------------------------------------------------------------------------------

// Panda Headers ----------------------
#include "PndDchPreFitterTRQATask.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "GFException.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"

// ROOT Headers -----------------------
#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TGeoManager.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TStyle.h"

// C/C++ Headers ----------------------
#include <iostream>


PndDchPreFitterTRQATask::PndDchPreFitterTRQATask()
  : FairTask("QA Task for Kalman od DCH"){
  fhP = NULL;
  fhPx = NULL;
  fhPy = NULL;
  fhPz = NULL;
  fhChi2 = NULL;
  fThetaH = NULL;
  fPhiH = NULL;
  fCanvas = NULL;
}


PndDchPreFitterTRQATask::~PndDchPreFitterTRQATask() {
  
  WriteHistograms();

  if(fhP!=NULL)    delete fhP;
  if(fhPx!=NULL)    delete fhPx;
  if(fhPy!=NULL)    delete fhPy;
  if(fhPz!=NULL)    delete fhPz;
  if(fhChi2!=NULL) delete fhChi2;
  if(0!=fThetaH)   delete fThetaH;
  if(0!=fPhiH)     delete fPhiH;
  if(0!=fCanvas)   delete fCanvas;
}

InitStatus PndDchPreFitterTRQATask::Init(){
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0){
      Error("PndDchPreFitterTRQATask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  fTrackArray=(TClonesArray*) ioman->GetObject("DCHTrackCand");
  if(fTrackArray==0) {
    Error("PndDchPreFitterTRQATask::Init","DCHTrackCand not found!");
    return kERROR;
  }

  fMCTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(fMCTrackArray==0) {
    Error("PndDchPreFitterTRTask2::Init","MCTrack array not found!");
    return kERROR;
  }
  
  // // setup histograms
  fhP    = new TH1D("pullP","(p_{Rec}-p_{MC})/p_{MC}",60,-0.15,0.15);
  fhP->SetFillColor(9);
 fhPx   = new TH1D("pullPx","(p_{x,Rec}-p_{x,MC})/p_{x,MC}",50,-2.9,2.9);
  fhPy   = new TH1D("pullPy","(p_{y,Rec}-p_{y,MC})/p_{y,MC}",50,-2.9,2.9);
  fhPz   = new TH1D("pullPz","(p_{z,Rec}-p_{z,MC})/p_{z,MC}",50,-2.9,2.9);
  fhPx->SetFillColor(9);
  fhPy->SetFillColor(9);
  fhPz->SetFillColor(9);
  fhChi2 = new TH1D("chi2","chi2",500,0,100);
  fhChi2->SetFillColor(2);
  fThetaH =new TH2D("theta","#theta_{rec}-#theta_{MC} vs #theta_{MC}",25,-5,25,100,-1,1.);
  fThetaH->SetFillColor(9);
  fPhiH   =new TH2D("phi",  "#phi_{rec}-#phi_{MC} vs #phi_{MC}",45,-180,180,100,-10,10);
  fPhiH->SetFillColor(9);
  
  std::cout << "-I- " << GetName() << ": Intialization successfull" << std::endl;

  return kSUCCESS;
}


void PndDchPreFitterTRQATask::Exec(Option_t* opt) {
  if(fVerbose>0)
    std::cout<<"Entering PndDchPreFitterTRQATask::Exec"<<std::endl;
  Int_t ntracks=fTrackArray->GetEntries();
  
  for(Int_t itr=0;itr<ntracks;++itr){
    PndTrackCand* trk= (PndTrackCand*)fTrackArray->At(itr);
    if(trk!=0){
      TVector3 mom0 = trk->getDirSeed();
      mom0 *= TMath::Abs(trk->getQoverPseed());
      
      Int_t mcTrid = trk->getMcTrackId();
      if(mcTrid<0){
	if(fVerbose>0) std::cout<<"No MCTrack to compare the PndTrackCand with!\n";
	return;
      }
      
      PndMCTrack* mc=(PndMCTrack*)fMCTrackArray->At(mcTrid);
      if(mc==0){
	Error("PndDchPrepareKalmanTracks::Exec","MCTrack Id=0 not found!");
	continue;
      }
      TVector3 mcmom = mc->GetMomentum();
      if(0!=mcmom.Mag())
	if(fhP) fhP->Fill((mom0.Mag()-mcmom.Mag())/mcmom.Mag());

      if(0!=mcmom.X())
	if(fhPx) fhPx->Fill((mom0.X()-mcmom.X())/mcmom.X());
      if(0!=mcmom.Y())
	if(fhPy) fhPy->Fill((mom0.Y()-mcmom.Y())/mcmom.Y());
      if(0!=mcmom.Z())
	if(fhPz) fhPz->Fill((mom0.Z()-mcmom.Z())/mcmom.Z());
      if(fThetaH) fThetaH->Fill(mcmom.Theta()*TMath::RadToDeg(),
				(-mcmom.Theta()+mom0.Theta())*TMath::RadToDeg());
      if(fPhiH) fPhiH->Fill(mcmom.Phi()*TMath::RadToDeg(),
			    (-mcmom.Phi()+mom0.Phi())*TMath::RadToDeg());
    }
  }
  return;
}

Bool_t  PndDchPreFitterTRQATask::WriteHistograms(){
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("DchPreFitterTRQA");
  file->cd("DchPreFitterTRQA");


  if(0!=fhP) fhP->Write();
  if(0!=fhPx) fhPx->Write();
  if(0!=fhPy) fhPy->Write();
  if(0!=fhPz) fhPz->Write();
  if(0!=fhChi2) fhChi2->Write();
  if(0!=fThetaH) fThetaH->Write();
  if(0!=fPhiH) fPhiH->Write();
  if(0!=fCanvas)  fCanvas->Write();
  return kTRUE;
}

void PndDchPreFitterTRQATask::PlotHistograms(){
  gStyle->SetOptStat(111111);
  gStyle->SetOptFit(0101);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.15);
  gStyle->SetPalette(1,0);
  fCanvas = new TCanvas("DchPreFitterQACanvas","Results from the DchPreFitterTRQA",1000,700);
  fCanvas->Divide(4,2);
  
  Int_t i = 1;

  if(fhP!=0){
    fCanvas->cd(i++);
    gPad->SetGrid(1,0);
    fhP->Draw();
  }

 if(fhPx!=0){
  fCanvas->cd(i++);     
  gPad->SetGrid(1,0); 
  fhPx->Draw();	      
  fhPx->Fit("gaus");   
 }
 
 if(fhPy!=0){
   fCanvas->cd(i++);     
   gPad->SetGrid(1,0); 
   fhPy->Draw();	      
   fhPy->Fit("gaus");   
 }
  
  if(fhPz!=0){  
    fCanvas->cd(i++);     
    gPad->SetGrid(1,0); 
    fhPz->Draw();	      
    fhPz->Fit("gaus");   
 }

  if(0!=fPhiH){
    fCanvas->cd(i++);
    gPad->SetGrid(1,1);
    fPhiH->Draw("colz");
  }
  if(0!=fThetaH){
    fCanvas->cd(i++);
    gPad->SetGrid(1,1);
    fThetaH->Draw("colz");
  }
  if(0!=fhChi2){
    fCanvas->cd(i++);
    gPad->SetGrid(1,0);
    fhChi2->Draw();
  }
}

 
void PndDchPreFitterTRQATask::Finish(){
  WriteHistograms();
}
  

ClassImp(PndDchPreFitterTRQATask)
