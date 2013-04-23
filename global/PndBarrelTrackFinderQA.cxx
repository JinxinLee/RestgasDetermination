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
  for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fMCPointArray[idet] = NULL;
    fDetName[idet] = "";
  }

  fTNofEvents = 0;
  fHistoList = NULL;

  fhMomMagCompAll2D = NULL;
  fhMomPhiCompAll2D = NULL;
  fhMomTheCompAll2D = NULL;
  fhMomMagCompAll1D = NULL;
  fhMomResCompAll1D = NULL;
  fhMomPhiCompAll1D = NULL;
  fhMomTheCompAll1D = NULL;
  
  fhMomMagSimAllHist = NULL;
  fhMomMagRecAllHist = NULL;
  fhMomMagEffAllHist = NULL;
  fhMomPhiSimAllHist = NULL;
  fhMomPhiRecAllHist = NULL;
  fhMomPhiEffAllHist = NULL;
  fhMomTheSimAllHist = NULL;
  fhMomTheRecAllHist = NULL;
  fhMomTheEffAllHist = NULL;
  
  fhMomMagSimHist = NULL;
  fhMomMagRecHist = NULL;
  fhMomMagEffHist = NULL;
  fhMomPhiSimHist = NULL;
  fhMomPhiRecHist = NULL;
  fhMomPhiEffHist = NULL;
  fhMomTheSimHist = NULL;
  fhMomTheRecHist = NULL;
  fhMomTheEffHist = NULL;
  
  fhMomMagAllRecoHist = NULL;
  fhMomPhiAllRecoHist = NULL;
  fhMomTheAllRecoHist = NULL;
  
  fhMomMagCloneHist = NULL;
  fhMomPhiCloneHist = NULL;
  fhMomTheCloneHist = NULL;
  fhMomMagGhostHist = NULL;
  fhMomPhiGhostHist = NULL;
  fhMomTheGhostHist = NULL;
  
  fhRecoMomMag = NULL;
  fhRecoMomPhi = NULL;
  fhRecoMomThe = NULL;
  
  fhMomResVsMomMag = NULL;
  fhMomResVsMomPhi = NULL;
  fhMomResVsMomThe = NULL;
  fhPhiResVsMomMag = NULL;
  fhPhiResVsMomPhi = NULL;
  fhPhiResVsMomThe = NULL;
  fhTheResVsMomMag = NULL;
  fhTheResVsMomPhi = NULL;
  fhTheResVsMomThe = NULL;
  
  for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
    fhNofDetPntsVsMagFoundHist[idet] = NULL;
    fhNofDetPntsVsPhiFoundHist[idet] = NULL;
    fhNofDetPntsVsTheFoundHist[idet] = NULL;
    fhNofDetPntsVsMagNotFdHist[idet] = NULL;
    fhNofDetPntsVsPhiNotFdHist[idet] = NULL;
    fhNofDetPntsVsTheNotFdHist[idet] = NULL;
  }

  fhNofMvdPixsVsThetaHist = NULL;
  fhNofMvdStrsVsThetaHist = NULL;
  fhNofSttHitsVsThetaHist = NULL;
  fhNofGemHitsVsThetaHist = NULL;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndBarrelTrackFinderQA::PndBarrelTrackFinderQA(Int_t iVerbose) 
  : FairTask("QA task for Barrel Track Finder", iVerbose) {
  fMCTrackArray  = NULL;
  fBarrelTrackArray = NULL;
  for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fMCPointArray[idet] = NULL;
    fDetName[idet] = "";
  }

  fTNofEvents = 0;
  fHistoList = NULL;

  fhMomMagCompAll2D = NULL;
  fhMomPhiCompAll2D = NULL;
  fhMomTheCompAll2D = NULL;
  fhMomMagCompAll1D = NULL;
  fhMomResCompAll1D = NULL;
  fhMomPhiCompAll1D = NULL;
  fhMomTheCompAll1D = NULL;
  
  fhMomMagSimAllHist = NULL;
  fhMomMagRecAllHist = NULL;
  fhMomMagEffAllHist = NULL;
  fhMomPhiSimAllHist = NULL;
  fhMomPhiRecAllHist = NULL;
  fhMomPhiEffAllHist = NULL;
  fhMomTheSimAllHist = NULL;
  fhMomTheRecAllHist = NULL;
  fhMomTheEffAllHist = NULL;
  
  fhMomMagSimHist = NULL;
  fhMomMagRecHist = NULL;
  fhMomMagEffHist = NULL;
  fhMomPhiSimHist = NULL;
  fhMomPhiRecHist = NULL;
  fhMomPhiEffHist = NULL;
  fhMomTheSimHist = NULL;
  fhMomTheRecHist = NULL;
  fhMomTheEffHist = NULL;
  
  fhMomMagAllRecoHist = NULL;
  fhMomPhiAllRecoHist = NULL;
  fhMomTheAllRecoHist = NULL;
  
  fhMomMagCloneHist = NULL;
  fhMomPhiCloneHist = NULL;
  fhMomTheCloneHist = NULL;
  fhMomMagGhostHist = NULL;
  fhMomPhiGhostHist = NULL;
  fhMomTheGhostHist = NULL;
  
  fhRecoMomMag = NULL;
  fhRecoMomPhi = NULL;
  fhRecoMomThe = NULL;
  
  fhMomResVsMomMag = NULL;
  fhMomResVsMomPhi = NULL;
  fhMomResVsMomThe = NULL;
  fhPhiResVsMomMag = NULL;
  fhPhiResVsMomPhi = NULL;
  fhPhiResVsMomThe = NULL;
  fhTheResVsMomMag = NULL;
  fhTheResVsMomPhi = NULL;
  fhTheResVsMomThe = NULL;
  
  for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
    fhNofDetPntsVsMagFoundHist[idet] = NULL;
    fhNofDetPntsVsPhiFoundHist[idet] = NULL;
    fhNofDetPntsVsTheFoundHist[idet] = NULL;
    fhNofDetPntsVsMagNotFdHist[idet] = NULL;
    fhNofDetPntsVsPhiNotFdHist[idet] = NULL;
    fhNofDetPntsVsTheNotFdHist[idet] = NULL;
  }

  fhNofMvdPixsVsThetaHist = NULL;
  fhNofMvdStrsVsThetaHist = NULL;
  fhNofSttHitsVsThetaHist = NULL;
  fhNofGemHitsVsThetaHist = NULL;
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
  
  fDetName[0] = "Mvd";
  fDetName[1] = "Stt";
  fDetName[2] = "Gem";
  TString trArrayName[3] = {"MVDPoint",
			    "STTPoint",
			    "GEMPoint"};
  
  for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
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
  for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
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
  Int_t nofDetTrPoints[1000][3];
  if ( nofMCTracks > 1000 ) 
    cout << " will have problem, more than 1000 mc tracks (" << nofMCTracks << ")" << endl;
  //  std::vector<std::vector<Int_t>> nofDetTrPoints;
  for ( Int_t ipt = 0 ; ipt < 1000 ; ipt++ )
    for ( Int_t idet = 0 ; idet < 3 ; idet++ ) 
      nofDetTrPoints[ipt][idet] = 0;

  std::vector<Bool_t> ghostTrack(nofBarrelTracks,kTRUE);
  std::vector<Bool_t> multiTrack(nofMCTracks,kFALSE);  

  FairMCPoint* mcPoint;
  for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
    if ( fIncludeDet[idet] == kTRUE ) {
      Int_t npoint = fMCPointArray[idet]->GetEntriesFast();
      for ( Int_t ipoint = 0 ; ipoint < npoint ; ipoint++ ) {
	mcPoint = (FairMCPoint*)fMCPointArray[idet]->At(ipoint);
	nofPoints[mcPoint->GetTrackID()] += 1;
	nofDetTrPoints[mcPoint->GetTrackID()][idet] += 1;
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

  if ( fVerbose > 2 ) {
    for ( Int_t itr = 0 ; itr < nofBarrelTracks ; itr++ ) {
      PndTrack* bTrack = (PndTrack*)fBarrelTrackArray->At(itr);
      FairTrackParP parFirst = bTrack->GetParamFirst();
      FairTrackParP  parLast = bTrack->GetParamLast();
      Double_t reMag = parFirst.GetMomentum().Mag();    
      Double_t rePhi = parFirst.GetMomentum().Phi()  *TMath::RadToDeg();
      Double_t reThe = parFirst.GetMomentum().Theta()*TMath::RadToDeg();
      cout << "F " << reMag << " " << rePhi << " " << reThe << endl;
    }
  }

  for ( Int_t itm = 0 ; itm < nofMCTracks ; itm++ ) {
    if ( nofPoints[itm] < 3 ) continue;
    PndMCTrack* track = (PndMCTrack*)fMCTrackArray->At(itm);

    Bool_t isPrim = kFALSE;
    Bool_t magQQQ = kFALSE;
    Bool_t phiQQQ = kFALSE;
    Bool_t theQQQ = kFALSE;

    if ( track->GetStartVertex().Mag() < 1. ) isPrim = kTRUE;
    if ( track->GetMomentum().Mag() > 1. ) magQQQ = kTRUE;
    if ( track->GetMomentum().Theta()*TMath::RadToDeg() > 3. ) theQQQ = kTRUE;
    if ( TMath::Abs(TMath::Abs(track->GetMomentum().Phi()*TMath::RadToDeg())-90.) > 10. ) phiQQQ = kTRUE;

    //    cout << "M " << track->GetMomentum().Mag() << " " << track->GetMomentum().Phi() << " " << track->GetMomentum().Theta() << endl;
    Double_t mcMag = track->GetMomentum().Mag();    
    Double_t mcPhi = track->GetMomentum().Phi()  *TMath::RadToDeg();
    Double_t mcThe = track->GetMomentum().Theta()*TMath::RadToDeg();
    if ( fVerbose )
      cout << "                      M " << mcMag << " " << mcPhi << " " << mcThe << " ( " << nofPoints[itm] << " hits ) "<< endl;
    fhMomMagSimAllHist->Fill(mcMag);
    fhMomPhiSimAllHist->Fill(mcPhi);
    fhMomTheSimAllHist->Fill(mcThe);
    if ( isPrim && phiQQQ && theQQQ )    fhMomMagSimHist->Fill(mcMag);
    if ( isPrim && magQQQ && theQQQ )    fhMomPhiSimHist->Fill(mcPhi);
    if ( isPrim && phiQQQ && magQQQ )    fhMomTheSimHist->Fill(mcThe);

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
      fhMomResCompAll1D->Fill((mcMag-reMag)/mcMag*100);
      fhMomResVsMomMag->Fill(mcMag,(mcMag-reMag)/mcMag*100);
      fhMomResVsMomPhi->Fill(mcPhi,(mcMag-reMag)/mcMag*100);
      fhMomResVsMomThe->Fill(mcThe,(mcMag-reMag)/mcMag*100);
      fhPhiResVsMomMag->Fill(mcMag,mcPhi-rePhi);
      fhPhiResVsMomPhi->Fill(mcPhi,mcPhi-rePhi);
      fhPhiResVsMomThe->Fill(mcThe,mcPhi-rePhi);
      fhTheResVsMomMag->Fill(mcMag,mcThe-reThe);
      fhTheResVsMomPhi->Fill(mcPhi,mcThe-reThe);
      fhTheResVsMomThe->Fill(mcThe,mcThe-reThe);

      /*      cout << "comparing " << mcMag << " " << mcPhi << " " << mcThe << " with " << reMag << " " << rePhi << " " << reThe << flush;
      if (           (mcMag-reMag) <  0.1*mcMag &&
		     (mcMag-reMag) > -0.3*mcMag &&
	   TMath::Abs(mcPhi-rePhi) < 5. &&
	   TMath::Abs(mcThe-reThe) < 5. ) {
	cout << " THE SAME" << flush;
      }
      else
	cout << " DIFFERENT" << flush;
	cout << endl;*/

      if (           (mcMag-reMag) <  0.3*mcMag &&
		     (mcMag-reMag) > -0.3*mcMag &&
	   TMath::Abs(mcPhi-rePhi) <  5. &&
	   TMath::Abs(mcThe-reThe) <  5. ) {
	ghostTrack[itr] = kFALSE;

	if ( fVerbose ) {
	  cout << " +++++++ Track found!!! " << reMag << " " << rePhi << " " << reThe << flush;
	}

	if ( matchingRecoTrack != -1 ) {
	  // it's a clone!
	  multiTrack[itm] = kTRUE;
	  if ( fVerbose ) 
	    cout << "   CLONE!!!" << endl;
	  continue;
	} 
	if ( fVerbose ) 
	  cout << endl;
 
	fhMomMagRecAllHist->Fill(mcMag);
	fhMomPhiRecAllHist->Fill(mcPhi);
	fhMomTheRecAllHist->Fill(mcThe);
	if ( isPrim && phiQQQ && theQQQ )    fhMomMagRecHist->Fill(mcMag);
	if ( isPrim && magQQQ && theQQQ )    fhMomPhiRecHist->Fill(mcPhi);
	if ( isPrim && phiQQQ && magQQQ )    fhMomTheRecHist->Fill(mcThe);

	matchingRecoTrack = itr;
	
	fhNofMvdPixsVsThetaHist->Fill(mcThe,parLast.GetPosition().X());
	fhNofMvdStrsVsThetaHist->Fill(mcThe,parLast.GetPosition().Y());
	fhNofSttHitsVsThetaHist->Fill(mcThe,parLast.GetPosition().Z());
	fhNofGemHitsVsThetaHist->Fill(mcThe,parLast.GetMomentum().Y());
      }
    }

    if ( matchingRecoTrack == -1 ) {
      for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
	fhNofDetPntsVsMagNotFdHist[idet]->Fill(mcMag,nofDetTrPoints[itm][idet]);
	fhNofDetPntsVsPhiNotFdHist[idet]->Fill(mcPhi,nofDetTrPoints[itm][idet]);
	fhNofDetPntsVsTheNotFdHist[idet]->Fill(mcThe,nofDetTrPoints[itm][idet]);
      }
    }
    else {
      for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
	fhNofDetPntsVsMagFoundHist[idet]->Fill(mcMag,nofDetTrPoints[itm][idet]);
	fhNofDetPntsVsPhiFoundHist[idet]->Fill(mcPhi,nofDetTrPoints[itm][idet]);
	fhNofDetPntsVsTheFoundHist[idet]->Fill(mcThe,nofDetTrPoints[itm][idet]);
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
    fhMomMagAllRecoHist->Fill(reMag);
    fhMomPhiAllRecoHist->Fill(rePhi);
    fhMomTheAllRecoHist->Fill(reThe);
    if ( ghostTrack[itr] == kFALSE ) continue;
    fhMomMagGhostHist->Fill(reMag);
    fhMomPhiGhostHist->Fill(rePhi);
    fhMomTheGhostHist->Fill(reThe);	
    if ( fVerbose ) 
      cout << "---------- GHOST TRACK: " << reMag << " " << rePhi << " " << reThe << endl;

  }
  
  for ( Int_t itm = 0 ; itm < nofMCTracks ; itm++ ) {
    if ( multiTrack[itm] == kFALSE ) continue;
    PndMCTrack* track = (PndMCTrack*)fMCTrackArray->At(itm);
    //    cout << "M " << track->GetMomentum().Mag() << " " << track->GetMomentum().Phi() << " " << track->GetMomentum().Theta() << endl;
    Double_t mcMag = track->GetMomentum().Mag();    
    Double_t mcPhi = track->GetMomentum().Phi()  *TMath::RadToDeg();
    Double_t mcThe = track->GetMomentum().Theta()*TMath::RadToDeg();
    fhMomMagCloneHist->Fill(mcMag);
    fhMomPhiCloneHist->Fill(mcPhi);
    fhMomTheCloneHist->Fill(mcThe);
  }
  if ( fVerbose > 0 ) 
    cout << "==========================================" << endl;
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
  fhMomPhiCompAll1D = new TH1F("fhMomPhiCompAll1D","Momentum phi angle comparison;#phi_{MC}-#phi_{reco} [deg]",200,-10.,10.);
  fhMomTheCompAll1D = new TH1F("fhMomTheCompAll1D","Momentum theta angle comparison;#theta_{MC}-#theta_{reco} [deg]",200,-10.,10.);

  fHistoList->Add(fhMomMagCompAll2D);
  fHistoList->Add(fhMomPhiCompAll2D);
  fHistoList->Add(fhMomTheCompAll2D);
  fHistoList->Add(fhMomMagCompAll1D);
  fHistoList->Add(fhMomPhiCompAll1D);
  fHistoList->Add(fhMomTheCompAll1D);

  fhMomResCompAll1D = new TH1F("fhMomResCompAll1D","Momentum resolution comparison;(p_{MC}-p_{reco})/p_{MC} [%]",200,-50.,50.);
  fHistoList->Add(fhMomResCompAll1D);
  fhMomResVsMomMag  = new TH2F("fhMomResVsMomMag", "Momentum resolution vs momentum;p_{MC} [GeV/c];(p_{MC}-p_{reco})/p_{MC} [%]",      80,   0., 20.,200,-50.,50.);
  fhMomResVsMomPhi  = new TH2F("fhMomResVsMomPhi", "Momentum resolution vs phi angle;#phi_{MC} [deg];(p_{MC}-p_{reco})/p_{MC} [%]"    ,90,-180.,180.,200,-50.,50.);
  fhMomResVsMomThe  = new TH2F("fhMomResVsMomThe", "Momentum resolution vs theta angle;#theta_{MC} [deg];(p_{MC}-p_{reco})/p_{MC} [%]",90,   0.,180.,200,-50.,50.);
  fHistoList->Add(fhMomResVsMomMag);
  fHistoList->Add(fhMomResVsMomPhi);
  fHistoList->Add(fhMomResVsMomThe);

  fhPhiResVsMomMag  = new TH2F ("fhPhiResVsMomMag","Phi resolution vs momentum;p_{MC} [GeV/c];#phi_{MC}-#phi_{reco} [deg]",        80,   0., 20.,200,-10.,10.);
  fhPhiResVsMomPhi  = new TH2F ("fhPhiResVsMomPhi","Phi resolution vs phi angle;#phi_{MC} [deg];#phi_{MC}-#phi_{reco} [deg]",      90,-180.,180.,200,-10.,10.);
  fhPhiResVsMomThe  = new TH2F ("fhPhiResVsMomThe","Phi resolution vs theta angle;#theta_{MC} [deg];#phi_{MC}-#phi_{reco} [deg]",  90,   0.,180.,200,-10.,10.);
  fhTheResVsMomMag  = new TH2F ("fhTheResVsMomMag","Theta resolution vs momentum;p_{MC} [GeV/c];#theta_{MC}-#theta_{reco} [deg]",        80,   0., 20.,200,-10.,10.);
  fhTheResVsMomPhi  = new TH2F ("fhTheResVsMomPhi","Theta resolution vs phi angle;#phi_{MC} [deg];#theta_{MC}-#theta_{reco} [deg]",      90,-180.,180.,200,-10.,10.);
  fhTheResVsMomThe  = new TH2F ("fhTheResVsMomThe","Theta resolution vs theta angle;#theta_{MC} [deg];#theta_{MC}-#theta_{reco} [deg]",  90,   0.,180.,200,-10.,10.);
  fHistoList->Add(fhPhiResVsMomMag);
  fHistoList->Add(fhPhiResVsMomPhi);
  fHistoList->Add(fhPhiResVsMomThe);
  fHistoList->Add(fhTheResVsMomMag);
  fHistoList->Add(fhTheResVsMomPhi);
  fHistoList->Add(fhTheResVsMomThe);

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

  fhMomMagSimAllHist = new TH1F("fhMomMagSimAllHist","Number of all simulated tracks vs MC momentum magnitude;p [GeV/c];a.u.",200,0.,20.);
  fhMomMagRecAllHist = new TH1F("fhMomMagRecAllHist","Number of all reconstructed tracks vs MC momentum magnitude;p [GeV/c];a.u.",200,0.,20.);
  fhMomMagEffAllHist = new TH1F("fhMomMagEffAllHist","Efficiency all vs MC momentum magnitude;p [GeV/c];efficiency [%]",200,0.,20.);
  fhMomPhiSimAllHist = new TH1F("fhMomPhiSimAllHist","Number of all simulated tracks vs MC momentum phi angle;#phi [deg];a.u.",360,-180.,180.);
  fhMomPhiRecAllHist = new TH1F("fhMomPhiRecAllHist","Number of all reconstructed tracks vs MC momentum phi angle;#phi [deg];a.u.",360,-180.,180.);
  fhMomPhiEffAllHist = new TH1F("fhMomPhiEffAllHist","Efficiency all vs MC momentum phi angle;#phi [deg];efficiency [%]",360,-180.,180.);
  fhMomTheSimAllHist = new TH1F("fhMomTheSimAllHist","Number of all simulated tracks vs MC momentum theta angle;#theta [deg];a.u.",180,0.,180.);
  fhMomTheRecAllHist = new TH1F("fhMomTheRecAllHist","Number of all reconstructed tracks vs MC momentum theta angle;#theta [deg];a.u.",180,0.,180.);
  fhMomTheEffAllHist = new TH1F("fhMomTheEffAllHist","Efficiency all vs MC momentum theta angle;#theta [deg];efficiency [%]",180,0.,180.);

  fHistoList->Add(fhMomMagSimAllHist);
  fHistoList->Add(fhMomMagRecAllHist);
  fHistoList->Add(fhMomMagEffAllHist);
  fHistoList->Add(fhMomPhiSimAllHist);
  fHistoList->Add(fhMomPhiRecAllHist);
  fHistoList->Add(fhMomPhiEffAllHist);
  fHistoList->Add(fhMomTheSimAllHist);
  fHistoList->Add(fhMomTheRecAllHist);
  fHistoList->Add(fhMomTheEffAllHist);

  fhMomMagAllRecoHist = new TH1F("fhMomMagAllRecoHist","Number of all reco tracks vs momentum;p_{MC} [GeV/c];a.u."        ,200,   0., 20.);
  fhMomPhiAllRecoHist = new TH1F("fhMomPhiAllRecoHist","Number of all reco tracks vs phi angle;#phi_{MC} [deg];a.u."      ,360,-180.,180.);
  fhMomTheAllRecoHist = new TH1F("fhMomTheAllRecoHist","Number of all reco tracks vs theta angle;#theta_{MC} [deg];a.u."  ,180,   0.,180.);
  fhMomMagCloneHist   = new TH1F("fhMomMagCloneHist"  ,"Number of clone tracks vs momentum;p_{MC} [GeV/c];a.u."           ,200,   0., 20.);
  fhMomPhiCloneHist   = new TH1F("fhMomPhiCloneHist"  ,"Number of clone tracks vs phi angle;#phi_{MC} [deg];a.u."         ,360,-180.,180.);
  fhMomTheCloneHist   = new TH1F("fhMomTheCloneHist"  ,"Number of clone tracks vs theta angle;#theta_{MC} [deg];a.u."     ,180,   0.,180.);
  fhMomMagGhostHist   = new TH1F("fhMomMagGhostHist"  ,"Number of ghost tracks vs momentum;p_{reco} [GeV/c];a.u."         ,200,   0., 20.);
  fhMomPhiGhostHist   = new TH1F("fhMomPhiGhostHist"  ,"Number of ghost tracks vs phi angle;#phi_{reco} [deg];a.u."       ,360,-180.,180.);
  fhMomTheGhostHist   = new TH1F("fhMomTheGhostHist"  ,"Number of ghost tracks vs theta angle;#theta_{reco} [deg];a.u."   ,180,   0.,180.);

  fHistoList->Add(fhMomMagAllRecoHist);
  fHistoList->Add(fhMomPhiAllRecoHist);
  fHistoList->Add(fhMomTheAllRecoHist);
  fHistoList->Add(fhMomMagCloneHist);
  fHistoList->Add(fhMomPhiCloneHist);
  fHistoList->Add(fhMomTheCloneHist);
  fHistoList->Add(fhMomMagGhostHist);
  fHistoList->Add(fhMomPhiGhostHist);
  fHistoList->Add(fhMomTheGhostHist);

  fhRecoMomMag = new TH1F("fhRecoMomMag","Reconstructed tracks momentum magnitude distribution",200,0.,20.);
  fhRecoMomPhi = new TH1F("fhRecoMomPhi","Reconstructed tracks momentum phi angle distribution",360,-180.,180.);
  fhRecoMomThe = new TH1F("fhRecoMomThe","Reconstructed tracks momentum theta angle distribution",180,0.,180.);
  fHistoList->Add(fhRecoMomMag);
  fHistoList->Add(fhRecoMomPhi);
  fHistoList->Add(fhRecoMomThe);

  fhNofMvdPixsVsThetaHist = new TH2F("fhNofMvdPixsVsThetaHist","Number of Mvd Pixel Hits vs theta;theta [deg];nof hits",180,0.,180.,41,-0.5,40.5);
  fhNofMvdStrsVsThetaHist = new TH2F("fhNofMvdStrsVsThetaHist","Number of Mvd Strip Hits vs theta;theta [deg];nof hits",180,0.,180.,41,-0.5,40.5);
  fhNofSttHitsVsThetaHist = new TH2F("fhNofSttHitsVsThetaHist","Number of Stt Hits vs theta;theta [deg];nof hits",180,0.,180.,41,-0.5,40.5);
  fhNofGemHitsVsThetaHist = new TH2F("fhNofGemHitsVsThetaHist","Number of Gem Hits vs theta;theta [deg];nof hits",180,0.,180.,41,-0.5,40.5);

  fHistoList->Add(fhNofMvdPixsVsThetaHist);
  fHistoList->Add(fhNofMvdStrsVsThetaHist);
  fHistoList->Add(fhNofSttHitsVsThetaHist);
  fHistoList->Add(fhNofGemHitsVsThetaHist);

  for ( Int_t idet = 0 ; idet < 3 ; idet++ ) {
    fhNofDetPntsVsMagFoundHist[idet] = new TH2F(Form("fhNof%sPntsVsMagFoundHist",fDetName[idet].Data()),
						Form("Number of %s points vs magnitude, for found tracks",fDetName[idet].Data()),
						200,0.,20.,41,-0.5,40.5);
    fhNofDetPntsVsPhiFoundHist[idet] = new TH2F(Form("fhNof%sPntsVsPhiFoundHist",fDetName[idet].Data()),
						Form("Number of %s points vs phi angle, for found tracks",fDetName[idet].Data()),
						360,-180.,180.,41,-0.5,40.5);
    fhNofDetPntsVsTheFoundHist[idet] = new TH2F(Form("fhNof%sPntsVsTheFoundHist",fDetName[idet].Data()),
						Form("Number of %s points vs theta angle, for found tracks",fDetName[idet].Data()),
						180,0.,180.,41,-0.5,40.5);
    fhNofDetPntsVsMagNotFdHist[idet] = new TH2F(Form("fhNof%sPntsVsMagNotFdHist",fDetName[idet].Data()),
						Form("Number of %s points vs magnitude, for not found tracks",fDetName[idet].Data()),
						200,0.,20.,41,-0.5,40.5);
    fhNofDetPntsVsPhiNotFdHist[idet] = new TH2F(Form("fhNof%sPntsVsPhiNotFdHist",fDetName[idet].Data()),
						Form("Number of %s points vs phi angle, for not found tracks",fDetName[idet].Data()),
						360,-180.,180.,41,-0.5,40.5);
    fhNofDetPntsVsTheNotFdHist[idet] = new TH2F(Form("fhNof%sPntsVsTheNotFdHist",fDetName[idet].Data()),
						Form("Number of %s points vs theta angle, for not found tracks",fDetName[idet].Data()),
						180,0.,180.,41,-0.5,40.5);

    fHistoList->Add(fhNofDetPntsVsMagFoundHist[idet]);
    fHistoList->Add(fhNofDetPntsVsPhiFoundHist[idet]);
    fHistoList->Add(fhNofDetPntsVsTheFoundHist[idet]);
    fHistoList->Add(fhNofDetPntsVsMagNotFdHist[idet]);
    fHistoList->Add(fhNofDetPntsVsPhiNotFdHist[idet]);
    fHistoList->Add(fhNofDetPntsVsTheNotFdHist[idet]);
  }

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

  fhMomMagEffAllHist->Divide(fhMomMagRecAllHist,fhMomMagSimAllHist);
  fhMomPhiEffAllHist->Divide(fhMomPhiRecAllHist,fhMomPhiSimAllHist);
  fhMomTheEffAllHist->Divide(fhMomTheRecAllHist,fhMomTheSimAllHist);
  fhMomMagEffAllHist->Scale(100.);
  fhMomPhiEffAllHist->Scale(100.);
  fhMomTheEffAllHist->Scale(100.);

  TFile* temp = gFile;
  cout << "tempFile = \"" << temp->GetName() << "\"" << endl;

  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    cout << "-E- "<< GetName() <<"::Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }


  gFile = ioman->GetOutFile();

  cout << "gDirectory = \"" << gDirectory->GetName() << "\"" << endl;
  cout << "gFile = \"" << gFile->GetName() << "\"" << endl;

  gDirectory = (TDirectory*)gFile;

  gDirectory->mkdir("BarrelTrackFinderQA");
  gDirectory->cd("BarrelTrackFinderQA");

  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  gDirectory->cd("..");

  gFile = temp;

}
// ------------------------------------------------------------
 

ClassImp(PndBarrelTrackFinderQA)
