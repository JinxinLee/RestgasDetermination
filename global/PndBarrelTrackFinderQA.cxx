// -------------------------------------------------------------------------
// -----                PndBarrelTrackFinderQA source file             -----
// -----                Created 08.12.2010 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndBarrelTrackFinderQA.h"

// Pnd includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairHit.h"
#include "FairTrackParP.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndDetectorList.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"
#include "TString.h"

// C++ includes
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
using std::cout;
using std::setw;
using std::endl;
using std::flush;
using std::map;

// -----   Default constructor   ------------------------------------------
PndBarrelTrackFinderQA::PndBarrelTrackFinderQA() : FairTask("QA task for Barrel Track Finder", 1) {
  fMCTrackArray  = NULL;
  fBarrelTrackArray = NULL;
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fMCPointArray[idet] = NULL;
  }
  fTNofEvents    = 0;
  
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndBarrelTrackFinderQA::PndBarrelTrackFinderQA(Int_t iVerbose) 
  : FairTask("QA task for Barrel Track Finder", iVerbose) {
  fMCTrackArray  = NULL;
  fBarrelTrackArray = NULL;
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fMCPointArray[idet] = NULL;
  }
  fTNofEvents    = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndBarrelTrackFinderQA::~PndBarrelTrackFinderQA() { 
}

// -----   Init  -----------------------------------------------------------
InitStatus PndBarrelTrackFinderQA::Init() {
  
  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    cout << "-E- "<< GetName() <<"::Init: "
	 << "RootManager not instantised!" << endl;
    return kERROR;
  }
  
  // Get the pointer to the singleton FairRunAna object
  FairRunAna* ana = FairRunAna::Instance();
  if(NULL == ana) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no FairRunAna object!" << endl;
    return kERROR;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  if(NULL == rtdb) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no runtime database!" << endl;
    return kERROR;
  }
  
  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if( !fMCTrackArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCTrack array!"
	 << endl;
    return kERROR;
  }

  fBarrelTrackArray = (TClonesArray*) ioman->GetObject("BarrelTrack");
  if ( !fBarrelTrackArray ) {
    cout << "-E- "<< GetName() <<"::Init: No BarrelTrack array!"
	 << endl;
    return kERROR;
  }
  
  fDetName[0] = "MVD";
  fDetName[1] = "STT";
  fDetName[2] = "TPC";
  fDetName[3] = "GEM";
  TString trArrayName[4] = {"MVDPoint",
			    "STTPoint",
			    "TPCPoint",
			    "GEMPoint"};
  
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    if ( fIncludeDet[idet] == kTRUE ) {
      fMCPointArray[idet] = (TClonesArray*) ioman->GetObject(trArrayName[idet].Data());    // Get Mvd track Array
      
      if ( !fMCPointArray[idet] ) {
	cout << "-E- " << GetName() << "::Init: No " << trArrayName[idet].Data() << " array!" << endl;
	return kERROR;
      }
      std::cout << "-I- " << GetName() << ": " << trArrayName[idet].Data() << " array found" << std::endl;
    }

  }
  
  std::cout << "-I- " << GetName() << ": Initialization successfull" << std::endl;
  std::cout << "-I- " << GetName() << ": Tracks created in " << flush;
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    if ( fIncludeDet[idet] == kFALSE ) continue;
    cout << fDetName[idet].Data() << ", ";
  }
  cout << "\b\b. " << endl;
  cout << "================================================================================" << endl;

  CreateHistos();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndBarrelTrackFinderQA::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndBarrelTrackFinderQA::SetParContainers() {
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndBarrelTrackFinderQA::Exec(Option_t* opt) {
  if ( fVerbose > 0 ) 
    cout << "=============== EVENT " << fTNofEvents << " =================" << endl;

  fTNofEvents++;

  Int_t nofMCTracks = fMCTrackArray->GetEntriesFast();
  Int_t nofBarrelTracks = fBarrelTrackArray->GetEntriesFast();
    
  std::vector<Int_t> nofPoints(nofMCTracks,0);
  FairMCPoint* mcPoint;
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    if ( fIncludeDet[idet] == kTRUE ) {
      Int_t npoint = fMCPointArray[idet]->GetEntriesFast();
      for ( Int_t ipoint = 0 ; ipoint < npoint ; ipoint++ ) {
	mcPoint = (FairMCPoint*)fMCPointArray[idet]->At(ipoint);
	nofPoints[mcPoint->GetTrackID()] += 1;
      }
    }
  }

  for ( Int_t itr = 0 ; itr < nofBarrelTracks ; itr++ ) {
    PndTrack* bTrack = (PndTrack*)fBarrelTrackArray->At(itr);
    FairTrackParP parFirst = bTrack->GetParamFirst();
    FairTrackParP  parLast = bTrack->GetParamLast();
    Double_t reMag = parFirst.GetMomentum().Mag();    
    Double_t rePhi = parFirst.GetMomentum().Phi()  *TMath::RadToDeg();
    Double_t reThe = parFirst.GetMomentum().Theta()*TMath::RadToDeg();
    fhRecoMomMag->Fill(reMag);
    fhRecoMomPhi->Fill(rePhi);
    fhRecoMomThe->Fill(reThe);
  }

  for ( Int_t itm = 0 ; itm < nofMCTracks ; itm++ ) {
    if ( nofPoints[itm] < 2 ) continue;
    PndMCTrack* track = (PndMCTrack*)fMCTrackArray->At(itm);
    //    cout << "M " << track->GetMomentum().Mag() << " " << track->GetMomentum().Phi() << " " << track->GetMomentum().Theta() << endl;
    Double_t mcMag = track->GetMomentum().Mag();    
    Double_t mcPhi = track->GetMomentum().Phi()  *TMath::RadToDeg();
    Double_t mcThe = track->GetMomentum().Theta()*TMath::RadToDeg();
    fhMomMagSimHist->Fill(mcMag);
    fhMomPhiSimHist->Fill(mcPhi);
    fhMomTheSimHist->Fill(mcThe);

    Int_t matchingRecoTrack = -1;
    for ( Int_t itr = 0 ; itr < nofBarrelTracks ; itr++ ) {
      PndTrack* bTrack = (PndTrack*)fBarrelTrackArray->At(itr);
      FairTrackParP parFirst = bTrack->GetParamFirst();
      FairTrackParP  parLast = bTrack->GetParamLast();
      Double_t reMag = parFirst.GetMomentum().Mag();    
      Double_t rePhi = parFirst.GetMomentum().Phi()  *TMath::RadToDeg();
      Double_t reThe = parFirst.GetMomentum().Theta()*TMath::RadToDeg();
      //      cout << "F " << parFirst.GetMomentum().Mag() << " " << parFirst.GetMomentum().Phi() << " " << parFirst.GetMomentum().Theta() << endl;
      
      fhMomMagCompAll2D->Fill(mcMag,reMag);
      fhMomPhiCompAll2D->Fill(mcPhi,rePhi);
      fhMomTheCompAll2D->Fill(mcThe,reThe);
      fhMomMagCompAll1D->Fill(mcMag-reMag);
      fhMomPhiCompAll1D->Fill(mcPhi-rePhi);
      fhMomTheCompAll1D->Fill(mcThe-reThe);

      if ( TMath::Abs(mcMag-reMag) < 0.1*mcMag &&
	   TMath::Abs(mcPhi-rePhi) < 1.5 ) { // && 
	//	   TMath::Abs(mcThe-reThe) < 1.5 ) {
	if ( matchingRecoTrack != -1 ) {
	  // it's a clone!
	  continue;
	} 
	fhMomMagRecHist->Fill(mcMag);
	fhMomPhiRecHist->Fill(mcPhi);
	fhMomTheRecHist->Fill(mcThe);
	fhMomResCompAll1D->Fill((mcMag-reMag)/mcMag*100);

	matchingRecoTrack = itr;
	
	fhNofMvdPixsVsThetaHist->Fill(mcThe,parLast.GetPosition().X());
	fhNofMvdStrsVsThetaHist->Fill(mcThe,parLast.GetPosition().Y());
	fhNofSttHitsVsThetaHist->Fill(mcThe,parLast.GetPosition().Z());
	fhNofTpcHitsVsThetaHist->Fill(mcThe,parLast.GetMomentum().X());
	fhNofGemHitsVsThetaHist->Fill(mcThe,parLast.GetMomentum().Y());
      }
    }
  }

}
// ------------------------------------------------------------

// -----   Private method CreateHistos   --------------------------------------------
void PndBarrelTrackFinderQA::CreateHistos() {
  fHistoList = new TList();
  // number of mc tracks, reco tracks, efficiency as function of MOMENTUM
  fhMomMagCompAll2D = new TH2F("fhMomMagCompAll2D","Momentum magnitude comparison;p_{MC} [GeV/c];p_{reco} [GeV/c]",200,0.,20.,200,0.,20.);
  fhMomPhiCompAll2D = new TH2F("fhMomPhiCompAll2D","Momentum phi angle comparison;#phi_{MC} [deg];#phi_{reco} [deg]",180,-180.,180.,180,-180.,180.);
  fhMomTheCompAll2D = new TH2F("fhMomTheCompAll2D","Momentum theta angle comparison;#theta_{MC} [deg];#theta_{reco} [deg]",180,0.,180.,180,0.,180.);
  fhMomMagCompAll1D = new TH1F("fhMomMagCompAll1D","Momentum magnitude comparison;p_{MC}-p_{reco} [GeV/c]",200,-10.,10.);
  fhMomResCompAll1D = new TH1F("fhMomResCompAll1D","Momentum resolution comparison;(p_{MC}-p_{reco})/p_{MC} [%]",200,-50.,50.);
  fhMomPhiCompAll1D = new TH1F("fhMomPhiCompAll1D","Momentum phi angle comparison;#phi_{MC}-#phi_{reco} [deg]",200,-10.,10.);
  fhMomTheCompAll1D = new TH1F("fhMomTheCompAll1D","Momentum theta angle comparison;#theta_{MC}-#theta_{reco} [deg]",200,-10.,10.);

  fHistoList->Add(fhMomMagCompAll2D);
  fHistoList->Add(fhMomPhiCompAll2D);
  fHistoList->Add(fhMomTheCompAll2D);
  fHistoList->Add(fhMomMagCompAll1D);
  fHistoList->Add(fhMomResCompAll1D);
  fHistoList->Add(fhMomPhiCompAll1D);
  fHistoList->Add(fhMomTheCompAll1D);

  fhMomMagSimHist = new TH1F("fhMomMagSimHist","Number of simulated tracks vs MC momentum magnitude;p [GeV/c];a.u.",200,0.,20.);
  fhMomMagRecHist = new TH1F("fhMomMagRecHist","Number of reconstructed tracks vs MC momentum magnitude;p [GeV/c];a.u.",200,0.,20.);
  fhMomMagEffHist = new TH1F("fhMomMagEffHist","Efficiency vs MC momentum magnitude;p [GeV/c];efficiency [%]",200,0.,20.);
  fhMomPhiSimHist = new TH1F("fhMomPhiSimHist","Number of simulated tracks vs MC momentum phi angle;#phi [deg];a.u.",360,-180.,180.);
  fhMomPhiRecHist = new TH1F("fhMomPhiRecHist","Number of reconstructed tracks vs MC momentum phi angle;#phi [deg];a.u.",360,-180.,180.);
  fhMomPhiEffHist = new TH1F("fhMomPhiEffHist","Efficiency vs MC momentum phi angle;#phi [deg];efficiency [%]",360,-180.,180.);
  fhMomTheSimHist = new TH1F("fhMomTheSimHist","Number of simulated tracks vs MC momentum theta angle;#theta [deg];a.u.",180,0.,180.);
  fhMomTheRecHist = new TH1F("fhMomTheRecHist","Number of reconstructed tracks vs MC momentum theta angle;#theta [deg];a.u.",180,0.,180.);
  fhMomTheEffHist = new TH1F("fhMomTheEffHist","Efficiency vs MC momentum theta angle;#theta [deg];efficiency [%]",180,0.,180.);

  fHistoList->Add(fhMomMagSimHist);
  fHistoList->Add(fhMomMagRecHist);
  fHistoList->Add(fhMomMagEffHist);
  fHistoList->Add(fhMomPhiSimHist);
  fHistoList->Add(fhMomPhiRecHist);
  fHistoList->Add(fhMomPhiEffHist);
  fHistoList->Add(fhMomTheSimHist);
  fHistoList->Add(fhMomTheRecHist);
  fHistoList->Add(fhMomTheEffHist);

  fhRecoMomMag = new TH1F("fhRecoMomMag","Reconstructed tracks momentum magnitude distribution",200,0.,20.);
  fhRecoMomPhi = new TH1F("fhRecoMomPhi","Reconstructed tracks momentum phi angle distribution",360,-180.,180.);
  fhRecoMomThe = new TH1F("fhRecoMomThe","Reconstructed tracks momentum theta angle distribution",180,0.,180.);
  fHistoList->Add(fhRecoMomMag);
  fHistoList->Add(fhRecoMomPhi);
  fHistoList->Add(fhRecoMomThe);

  fhNofMvdPixsVsThetaHist = new TH2F("fhNofMvdPixsVsThetaHist","Number of Mvd Pixel Hits vs theta;theta [deg];nof hits",180,0.,180.,41,-0.5,40.5);
  fhNofMvdStrsVsThetaHist = new TH2F("fhNofMvdStrsVsThetaHist","Number of Mvd Strip Hits vs theta;theta [deg];nof hits",180,0.,180.,41,-0.5,40.5);
  fhNofSttHitsVsThetaHist = new TH2F("fhNofSttHitsVsThetaHist","Number of Stt Hits vs theta;theta [deg];nof hits",180,0.,180.,41,-0.5,40.5);
  fhNofTpcHitsVsThetaHist = new TH2F("fhNofTpcHitsVsThetaHist","Number of Tpc Hits vs theta;theta [deg];nof hits",180,0.,180.,41,-0.5,40.5);
  fhNofGemHitsVsThetaHist = new TH2F("fhNofGemHitsVsThetaHist","Number of Gem Hits vs theta;theta [deg];nof hits",180,0.,180.,41,-0.5,40.5);

  fHistoList->Add(fhNofMvdPixsVsThetaHist);
  fHistoList->Add(fhNofMvdStrsVsThetaHist);
  fHistoList->Add(fhNofSttHitsVsThetaHist);
  fHistoList->Add(fhNofTpcHitsVsThetaHist);
  fHistoList->Add(fhNofGemHitsVsThetaHist);

}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndBarrelTrackFinderQA::Finish() {
  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << "--------------------------------------------------------------------------------" << endl; 

  fhMomMagEffHist->Divide(fhMomMagRecHist,fhMomMagSimHist);
  fhMomPhiEffHist->Divide(fhMomPhiRecHist,fhMomPhiSimHist);
  fhMomTheEffHist->Divide(fhMomTheRecHist,fhMomTheSimHist);
  fhMomMagEffHist->Scale(100.);
  fhMomPhiEffHist->Scale(100.);
  fhMomTheEffHist->Scale(100.);

  gDirectory->mkdir("BarrelTrackFinderQA");
  gDirectory->cd("BarrelTrackFinderQA");
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  gDirectory->cd("..");

}
// ------------------------------------------------------------
 

ClassImp(PndBarrelTrackFinderQA)
