// -------------------------------------------------------------------------
// -----                PndDchKalmanQATask2 source file                -----
// -----            Created 30.10.2008  by A. Wronska                  -----
// -----        based on the recotasks/demo/DemoTools by S.Neubert     -----
// -------------------------------------------------------------------------

// Panda Headers ----------------------
#include "PndDchKalmanQATask.h"
#include "FairRootManager.h"
#include "GFTrack.h"
#include "FairMCPoint.h"
#include "PndMCTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "PndTrackCand.h"

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
  : FairTask("QA Task for Kalman od DCH"), fPersistence(kFALSE), fEvt(0), fApproach(0), fSignPatch(0)
{
  fTrackBranchName = "FSTracks";
  fhP = NULL;
  fhPx = NULL;
  fhPy = NULL;
  fhPz = NULL;
  fhChi2 = NULL;
  fThetaH = NULL;
  fPhiH = NULL;
  fCanvas = NULL;
}


PndDchKalmanQATask::~PndDchKalmanQATask() {
  
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

InitStatus PndDchKalmanQATask::Init(){
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
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
    Error("PndDchKalmanQATask::Init","MCTrack array not found!");
    return kERROR;
  }
  
  fPointArray=(TClonesArray*) ioman->GetObject("PndDchPoint");
  if(fPointArray==0) {
    Error("PndDchKalmanQATask::Init","PndDchPoint array not found!");
    return kERROR;
  }
  
  fDchTrackArray = (TClonesArray*) ioman->GetObject("DCHTrackCand");
  if(fDchTrackArray==0){
    Error("PndDchKalmaQATask::Init","PndDchTrack array not found!");
    return kERROR;
  }
   
  // // setup histograms
  fhP    = new TH1D("pullP","(p_{Rec}-p_{MC})/p_{MC}",50,-0.05,0.05);
  fhPx   = new TH1D("pullPx","(p_{x,Rec}-p_{x,MC})/p_{x,MC}",50,-2.9,2.9);
  fhPy   = new TH1D("pullPy","(p_{y,Rec}-p_{y,MC})/p_{y,MC}",50,-2.9,2.9);
  fhPz   = new TH1D("pullPz","(p_{z,Rec}-p_{z,MC})/p_{z,MC}",50,-2.9,2.9);
  fhP->SetFillColor(9);
  fhPx->SetFillColor(9);
  fhPy->SetFillColor(9);
  fhPz->SetFillColor(9);
  fhChi2 = new TH1D("chi2","chi2",500,0,1000);
  fhChi2->SetFillColor(2);
  fThetaH =new TH2D("theta","#theta_{rec}-#theta_{MC} vs #theta_{MC}",
		    24,0,8,60,-3.,3.);
  fPhiH   =new TH2D("phi",  "#phi_{rec}-#phi_{MC} vs #phi_{MC}",
		    45,-180,180,100,-20,20);

  std::cout << "-I- " << GetName() << ": Intialization successfull" << std::endl;

  return kSUCCESS;
}


void PndDchKalmanQATask::Exec(Option_t* opt) {
  if(fVerbose>0)
    std::cout<<"PndDchKalmanQATask::Exec Event "<<fEvt++<<std::endl;
  Int_t ntracks=fTrackArray->GetEntries();
  
  for(Int_t itr=0;itr<ntracks;++itr){
    GFTrack* trk=new GFTrack(*(GFTrack*) fTrackArray->At(itr));
    if(trk->getTrackRep(0)->getStatusFlag()==0){
      TVector3 mom0;
      GFAbsTrackRep* rep = 0;
      rep = trk->getCardinalRep()->clone();
      std::cout<<" Particle charge = "<<rep->getCharge()<<std::endl;
      GeaneTrackRep* grep = 0;
      if(fApproach==0){
	// approach zero - working when only dipole field on
	// all three components of momentum then reconstructed properly
	mom0 = trk->getMom();
      }
      else if(fApproach==1){
	// approach one - failing, since for ca. 50% of all events
	// one of the momentum components has a mismatch in sign
	GFDetPlane pl(TVector3(0,0,0),TVector3(1,0,0),TVector3(0,1,0));
	grep=dynamic_cast<GeaneTrackRep*>(rep);
	grep->setPropDir(-1);
	mom0=grep->getMom(pl);
      }
      else if(fApproach==2){
	continue;
 	//approach two
 	/*TMatrixT<double> statePred(5,1);
 	TMatrixT<double> covPred(5,5);
 	GFDetPlane planePred;
 	grep=dynamic_cast<GeaneTrackRep*>(rep);
 	grep->setPropDir(-1);
 	grep->extrapolateToPoca( TVector3(0,0,0), statePred, covPred, planePred );
 	grep->setState(statePred);
 	grep->setCov(covPred);
 	grep->setReferencePlane(planePred);
	grep->setPropDir(1);
 	mom0=grep->getMom(planePred);*/
      }
      else{
	continue;
      }
      if(fSignPatch)
	if(mom0.Z()<0)
	  mom0 = -mom0;
      std::cout<<"mom0 reconstructed = "<<std::endl;
      mom0.Print();
      
      // ---------------------------------------
      // THIS PART OF CODE MAY CAUSE PROBLEMS...
      // USED TO BE:
      //      Int_t mcTrid = -1;
      //      Int_t id = 0;
      //      while(id<fDchTrackMatchArray->GetEntriesFast()){
      //	PndDchTrackMatch* dchtrmatch = (PndDchTrackMatch*) fDchTrackMatchArray->At(id);
      //	if(dchtrmatch->GetRecTrackID()==itr){
      //	  mcTrid = dchtrmatch->GetMCTrackID();
      //	  break;
      //	}
      //	id++;
      //      }
      // ---------------------------------------
      // CHANGING TO:
      PndTrackCand* dchTrack = (PndTrackCand*)fDchTrackArray->At(itr);
      Int_t mcTrid = dchTrack->getMcTrackId();
      // ---------------------------------------
      if(mcTrid<0){
	if(fVerbose>0) std::cout<<"No MCTrack to compare the reco-track with!\n";
	continue;
      }
      
      PndMCTrack* mc=(PndMCTrack*)fMCTrackArray->At(mcTrid);
      if(mc==0){
	Error("PndDchPrepareKalmanTracks::Exec","MCTrack Id=0 not found!");
	continue;
      }
      TVector3 mcmom = mc->GetMomentum();
      std::cout<<"mcmom: "<<std::endl;
      mcmom.Print();
      if(0!=mcmom.Mag())
	if(fhP) fhP->Fill((mom0.Mag()-mcmom.Mag())/mcmom.Mag());
      if(0!=mcmom.X())
	if(fhPx) fhPx->Fill((mom0.X()-mcmom.X())/mcmom.X());
      if(0!=mcmom.Y())
	if(fhPy) fhPy->Fill((mom0.Y()-mcmom.Y())/mcmom.Y());
      if(0!=mcmom.Z())
	if(fhPz) fhPz->Fill((mom0.Z()-mcmom.Z())/mcmom.Z());
      if(fhChi2) fhChi2->Fill(trk->getChiSqu());
      if(fThetaH) fThetaH->Fill(mcmom.Theta()*TMath::RadToDeg(),
				(-mcmom.Theta()+mom0.Theta())*TMath::RadToDeg());
      if(fPhiH) fPhiH->Fill(mcmom.Phi()*TMath::RadToDeg(),
			    (-mcmom.Phi()+mom0.Phi())*TMath::RadToDeg());
      if(0!=rep) delete rep;
    }
    delete trk;
  }
  return;
}
  

Bool_t  PndDchKalmanQATask::WriteHistograms(){
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("DchKalmanQA");
  file->cd("DchKalmanQA");

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

void PndDchKalmanQATask::PlotHistograms(){
  gStyle->SetOptStat(111111);
  gStyle->SetOptFit(0101);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.15);
  gStyle->SetPalette(1,0);
  fCanvas = new TCanvas("DchQACanvas","Results from the DchKalmanQA",1200,900);
  fCanvas->Divide(4,2);
  
  Int_t i = 1;

  fCanvas->cd(i++);     
  gPad->SetGrid(1,0); 
  fhP->Draw();	      
  //  fhP->Fit("gaus");   

  fCanvas->cd(i++);     
  gPad->SetGrid(1,0); 
  fhPx->Draw();	      
  //fhPx->Fit("gaus");   

  fCanvas->cd(i++);     
  gPad->SetGrid(1,0); 
  fhPy->Draw();	      
  //fhPy->Fit("gaus");   

  fCanvas->cd(i++);     
  gPad->SetGrid(1,0); 
  fhPz->Draw();	      
  //fhPz->Fit("gaus");   

  fCanvas->cd(i++);
  gPad->SetGrid(1,1);
  fPhiH->Draw("colz");

  fCanvas->cd(i++);
  gPad->SetGrid(1,1);
  fThetaH->Draw("colz");

  fCanvas->cd(i++);
  gPad->SetGrid(1,0);
  fhChi2->Draw();
}

 
void PndDchKalmanQATask::Finish(){
  WriteHistograms();
}
  

ClassImp(PndDchKalmanQATask)
