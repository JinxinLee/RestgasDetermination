// -------------------------------------------------------------------------
// -----                PndGlobalTrackMergerQA source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGlobalTrackMergerQA.h"

// FairRoot includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"
// Pnd includes
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndDetectorList.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"

// C++ includes
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::setw;
using std::map;

// -----   Default constructor   ------------------------------------------
PndGlobalTrackMergerQA::PndGlobalTrackMergerQA() : FairTask("Global Track Merger QA", 1) {
  fMCTrackArray  = NULL;
  fRecoTrackArray = NULL;
  fNofEvents    = 0;
  fRecoTrackBranchName = "GlobalTrack";
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGlobalTrackMergerQA::PndGlobalTrackMergerQA(Int_t iVerbose) 
  : FairTask("Global Track Merger QA", iVerbose) {
  fMCTrackArray  = NULL;
  fRecoTrackArray = NULL;
  fNofEvents    = 0;
  fRecoTrackBranchName = "GlobalTrack";
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGlobalTrackMergerQA::~PndGlobalTrackMergerQA() { }

// -----   Init  -----------------------------------------------------------
InitStatus PndGlobalTrackMergerQA::Init() {
  
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

  // Get Global track Array
  fRecoTrackArray  = (TClonesArray*) ioman->GetObject(fRecoTrackBranchName.Data());
  if ( !fRecoTrackArray ) {
    cout << "-E- " << GetName() << "::Init: No \"" << fRecoTrackBranchName.Data() << "\" PndTrack array!" << endl;
    return kERROR;
  }
  

  std::cout << "-I- "<< GetName() <<": Initialization successfull" << std::endl;

  CreateHistos();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGlobalTrackMergerQA::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGlobalTrackMergerQA::SetParContainers() {
  
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  fNofEvents = 0;
  fTargetPos.SetXYZ(0.,0.,0.);

  fNofMCAll = 0;
  fNofMCAcc = 0;
  fNofMCPrim = 0;
  fNofMCSec = 0;
  fNofMCRef = 0; // primary, mom.mag > 0.5GeV/c

  fNofRecoAcc = 0;
  fNofRecoPrim = 0;
  fNofRecoSec = 0;
  fNofRecoRef = 0; // primary, mom.mag > 0.5GeV/c

  fNofRecoGhosts = 0;
  fNofRecoClones = 0;

}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndGlobalTrackMergerQA::Exec(Option_t*) {
  
  fNofEvents++;

  Int_t nofMCAll = 0;
  Int_t nofMCAcc = 0;
  Int_t nofMCPrim = 0;
  Int_t nofMCSec = 0;
  Int_t nofMCRef = 0; // primary, mom.mag > 0.5GeV/c

  Int_t nofRecoAcc = 0;
  Int_t nofRecoPrim = 0;
  Int_t nofRecoSec = 0;
  Int_t nofRecoRef = 0; // primary, mom.mag > 0.5GeV/c
 
  Int_t nofRecoClones = 0;
  Int_t nofRecoGhosts = 0;

  PrepareMCTracks();
  MatchRecoTracks();

  Int_t nofMCTracks   = fMCTrackArray  ->GetEntriesFast();
  Int_t nofRecoTracks = fRecoTrackArray->GetEntriesFast();

  fhNofMCTracksPerEvent  ->Fill(fNofEvents-1,nofMCTracks);
  fhNofRecoTracksPerEvent->Fill(fNofEvents-1,nofRecoTracks);

  PndMCTrack* mcTrack;
  PndTrack*   recoTrack;

  for ( Int_t imct = 0 ; imct < nofMCTracks ; imct++ ) {

    mcTrack = (PndMCTrack*) fMCTrackArray->At(imct);

    if ( mcTrack->GetNPoints(kMVD) +
	 mcTrack->GetNPoints(kSTT) +
	 mcTrack->GetNPoints(kGEM) < 3 )
      continue;

    TVector3 mcVertex = mcTrack->GetStartVertex();
    Bool_t isPrim = kFALSE;
    TVector3 mcMomVec = mcTrack->GetMomentum();
    Double_t mcMomMag = mcMomVec.Mag();
    Double_t mcMomThe = mcMomVec.Theta()*TMath::RadToDeg();
    Double_t mcMomPhi = mcMomVec.Phi()*TMath::RadToDeg();

    if ( (mcVertex-fTargetPos).Mag() < 1. ) 
      isPrim = kTRUE;

    fhMCAllVsP->Fill(mcMomMag);
    fhMCAllVsT->Fill(mcMomThe);
    fhMCAllVsA->Fill(mcMomPhi);

    // check reconstrubility, continue only for reconstruable tracks
    nofMCAll ++;

    nofMCAcc ++;

    fhMCAccVsP->Fill(mcMomMag);
    fhMCAccVsT->Fill(mcMomThe);
    fhMCAccVsA->Fill(mcMomPhi);

    if ( isPrim ) {
      nofMCPrim ++;
      fhMCPrimVsP->Fill(mcMomMag);
      fhMCPrimVsT->Fill(mcMomThe);
      fhMCPrimVsA->Fill(mcMomPhi);
    }
    else {
      nofMCSec ++;
      fhMCSecVsP->Fill(mcMomMag);
      fhMCSecVsT->Fill(mcMomThe);
      fhMCSecVsA->Fill(mcMomPhi);
    }

    Bool_t isRefP = kFALSE;
    Bool_t isRefT = kFALSE;
    if ( isPrim && mcMomMag > 0.5 ) {
      isRefP = kTRUE;
      fhMCRefVsT->Fill(mcMomThe);
    }
    if ( isPrim && mcMomThe > 5. && mcMomThe < 25. ) {
      isRefT = kTRUE;
      fhMCRefVsP->Fill(mcMomMag);
    }
    if ( isRefP && isRefT ) {
      nofMCRef ++;
    }

    for ( Int_t irtr = 0 ; irtr < nofRecoTracks ; irtr++ ) {
      if ( fRecoTrackMCMatch[irtr] != imct ) continue;
      recoTrack = (PndTrack*) fRecoTrackArray->At(irtr);

      TVector3 recoTrackMom = recoTrack->GetParamFirst().GetMomentum();
      Double_t recoMomMag = recoTrackMom.Mag();

      nofRecoAcc ++;

//       cout << "mc mom = " << mcMomMag << ", reco mom = " << recoMomMag << " -> " << 100.*(mcMomMag-recoMomMag)/mcMomMag 
// 	   << " FROM ( " << recoTrack->GetParamFirst().GetX()
// 	   << " , "      << recoTrack->GetParamFirst().GetY()
// 	   << " , "      << recoTrack->GetParamFirst().GetZ()
// 	   << " ) "      << endl;
      fhRecoAccVsP->Fill(mcMomMag);
      fhRecoAccVsT->Fill(mcMomThe);
      fhRecoAccVsA->Fill(mcMomPhi);
      fhMomResAccVsP->Fill(mcMomMag,100.*(mcMomMag-recoMomMag)/mcMomMag);
      fhMomResAccVsT->Fill(mcMomThe,100.*(mcMomMag-recoMomMag)/mcMomMag);
      fhMomResAccVsA->Fill(mcMomPhi,100.*(mcMomMag-recoMomMag)/mcMomMag);

      if ( isRefP ) {
	fhRecoRefVsT->Fill(mcMomThe);
	fhMomResRefVsT->Fill(mcMomThe,100.*(mcMomMag-recoMomMag)/mcMomMag);
      }
      if ( isRefT ) {
	fhRecoRefVsP->Fill(mcMomMag);
	fhMomResRefVsP->Fill(mcMomMag,100.*(mcMomMag-recoMomMag)/mcMomMag);
      }
      if ( isRefP && isRefT ) {
	nofRecoRef ++;
      }
      if ( isPrim ) {
	nofRecoPrim ++;
	fhRecoPrimVsP->Fill(mcMomMag);	
	fhRecoPrimVsT->Fill(mcMomThe);
	fhRecoPrimVsA->Fill(mcMomPhi);
	fhMomResPrimVsP->Fill(mcMomMag,100.*(mcMomMag-recoMomMag)/mcMomMag);
	fhMomResPrimVsT->Fill(mcMomThe,100.*(mcMomMag-recoMomMag)/mcMomMag);
	fhMomResPrimVsA->Fill(mcMomPhi,100.*(mcMomMag-recoMomMag)/mcMomMag);

	fhRecoPrimP->Fill(recoTrackMom.Mag());	
	fhRecoPrimT->Fill(recoTrackMom.Theta()*TMath::RadToDeg());	
	fhRecoPrimA->Fill(recoTrackMom.Phi()*TMath::RadToDeg());	
      }
      else {
	nofRecoSec ++;
	fhRecoSecVsP->Fill(mcMomMag);
	fhRecoSecVsT->Fill(mcMomThe);
	fhRecoSecVsA->Fill(mcMomPhi);
	fhMomResSecVsP->Fill(mcMomMag,100.*(mcMomMag-recoMomMag)/mcMomMag);
	fhMomResSecVsT->Fill(mcMomThe,100.*(mcMomMag-recoMomMag)/mcMomMag);
	fhMomResSecVsA->Fill(mcMomPhi,100.*(mcMomMag-recoMomMag)/mcMomMag);

	fhRecoSecP->Fill(recoTrackMom.Mag());	
	fhRecoSecT->Fill(recoTrackMom.Theta()*TMath::RadToDeg());	
	fhRecoSecA->Fill(recoTrackMom.Phi()*TMath::RadToDeg());	
      }
      break; //do not include clones, the break is good for efficiency plots but bad(?) for momentum resolution
    }
  }

  fNofMCAll += nofMCAll;
  fNofMCAcc += nofMCAcc;
  fNofMCPrim += nofMCPrim;
  fNofMCSec += nofMCSec;
  fNofMCRef += nofMCRef;

  fNofRecoAcc += nofRecoAcc;
  fNofRecoPrim += nofRecoPrim;
  fNofRecoSec += nofRecoSec;
  fNofRecoRef += nofRecoRef;

  fNofRecoGhosts += nofRecoGhosts;
  fNofRecoClones += nofRecoClones;

  if ( fVerbose ) {
    Double_t effAcc  = 1.;
    Double_t effPrim = 1.;
    Double_t effSec  = 1.;
    Double_t effRef  = 1.;
    if ( nofMCAcc  ) effAcc  = 100.*(Double_t)nofRecoAcc /((Double_t)nofMCAcc);
    if ( nofMCPrim ) effPrim = 100.*(Double_t)nofRecoPrim/((Double_t)nofMCPrim);
    if ( nofMCSec  ) effSec  = 100.*(Double_t)nofRecoSec /((Double_t)nofMCSec);
    if ( nofMCRef  ) effRef  = 100.*(Double_t)nofRecoRef /((Double_t)nofMCRef);
    cout << "---- PndGlobalTrackMergerQA : Event " << fNofEvents << " summary -----" << endl;
    cout << "MC Tracks: " << nofMCTracks << endl;
    cout << " reconstruable: " << nofMCAcc  << " reconstructed " << nofRecoAcc  << " >>>> " << effAcc  << endl;
    cout << " primaries    : " << nofMCPrim << " reconstructed " << nofRecoPrim << " >>>> " << effPrim << endl;
    cout << " secondaries  : " << nofMCSec  << " reconstructed " << nofRecoSec  << " >>>> " << effSec  << endl;
    cout << " reference    : " << nofMCRef  << " reconstructed " << nofRecoRef  << " >>>> " << effRef  << endl;
    cout << " ghosts       : " << nofRecoGhosts << " clones         " << nofRecoClones << endl;
    cout << "---------------------------------------------------------------" << endl; 
  }
  
}
// ------------------------------------------------------------

// -----   Private method PrepareMCTrack   --------------------------------------------
void PndGlobalTrackMergerQA::PrepareMCTracks() {
  
  //const Int_t nofMCTracks    = fMCTrackArray->GetEntriesFast(); //[R.K. 01/2017] unused variable?

}
// ------------------------------------------------------------

// -----   Private method PrepareMCTrack   --------------------------------------------
void PndGlobalTrackMergerQA::MatchRecoTracks() {
  
  Int_t nofRecoTracks = fRecoTrackArray->GetEntriesFast();

  fRecoTrackMCMatch.clear();
  fRecoTrackMCMatch.resize(nofRecoTracks,-1);

  PndTrack* recoTrack;

  for ( Int_t irtr = 0 ; irtr < nofRecoTracks ; irtr++ ) {
    recoTrack = (PndTrack*) fRecoTrackArray->At(irtr);

    Int_t bestMCId = recoTrack->GetRefIndex();

    if ( bestMCId == -1 ) { continue; }

    fRecoTrackMCMatch[irtr] = bestMCId;

    TVector3 recoTrackMom = recoTrack->GetParamFirst().GetMomentum();

    fhRecoAllP->Fill(recoTrackMom.Mag());
    fhRecoAllT->Fill(recoTrackMom.Theta()*TMath::RadToDeg());
    fhRecoAllA->Fill(recoTrackMom.Phi()*TMath::RadToDeg());
  }
  
}
// ------------------------------------------------------------

// -----   Private method CreateHistos   --------------------------------------------
void PndGlobalTrackMergerQA::CreateHistos() {
  fHistoList = new TList();
  // number of mc tracks, reco tracks, efficiency as function of MOMENTUM
  Double_t minMom =  0.;
  Double_t maxMom = 10.;
  Int_t   momBins = 40;
  fhMCAllVsP  = new TH1F("hMCAllVsP" ,"all mc tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhMCAccVsP  = new TH1F("hMCAccVsP" ,"reconstruable mc tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhMCPrimVsP = new TH1F("hMCPrimVsP","primary mc tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhMCSecVsP  = new TH1F("hMCSecVsP" ,"secondary mc tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhRecoAccVsP  = new TH1F("hRecoAccVsP" ,"reconstructed tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhRecoPrimVsP = new TH1F("hRecoPrimVsP","reco primary tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhRecoSecVsP  = new TH1F("hRecoSecVsP" ,"reco secondary tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhEffAccVsP  = new TH1F("hEffAccVsP" ,"eff all tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhEffPrimVsP = new TH1F("hEffPrimVsP","eff primary tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhEffSecVsP  = new TH1F("hEffSecVsP" ,"eff secondary tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhMCRefVsP  = new TH1F("hMCRefVsP" ,"reference mc tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhRecoRefVsP  = new TH1F("hRecoRefVsP" ,"reco reference tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);
  fhEffRefVsP  = new TH1F("hEffRefVsP" ,"eff reference tracks;p [GeV/c];yield [a.u.]",momBins,minMom,maxMom);

  fHistoList->Add(fhMCAllVsP);
  fHistoList->Add(fhMCAccVsP);
  fHistoList->Add(fhMCPrimVsP);
  fHistoList->Add(fhMCSecVsP);
  fHistoList->Add(fhRecoAccVsP);
  fHistoList->Add(fhRecoPrimVsP);
  fHistoList->Add(fhRecoSecVsP);
  fHistoList->Add(fhEffAccVsP);
  fHistoList->Add(fhEffPrimVsP);
  fHistoList->Add(fhEffSecVsP);
  fHistoList->Add(fhMCRefVsP);
  fHistoList->Add(fhRecoRefVsP);
  fHistoList->Add(fhEffRefVsP);

  // number of mc tracks, reco tracks, efficiency as function of THETA
  Double_t minThe =  0.;
  Double_t maxThe = 150.;
  Int_t   theBins = 150;
  fhMCAllVsT  = new TH1F("hMCAllVsT" ,"all mc tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhMCAccVsT  = new TH1F("hMCAccVsT" ,"reconstruable mc tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhMCPrimVsT = new TH1F("hMCPrimVsT","primary mc tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhMCSecVsT  = new TH1F("hMCSecVsT" ,"secondary mc tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhRecoAccVsT  = new TH1F("hRecoAccVsT" ,"reconstructed tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhRecoPrimVsT = new TH1F("hRecoPrimVsT","reco primary tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhRecoSecVsT  = new TH1F("hRecoSecVsT" ,"reco secondary tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhEffAccVsT  = new TH1F("hEffAccVsT" ,"eff all tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhEffPrimVsT = new TH1F("hEffPrimVsT","eff primary tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhEffSecVsT  = new TH1F("hEffSecVsT" ,"eff secondary tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhMCRefVsT  = new TH1F("hMCRefVsT" ,"reference mc tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhRecoRefVsT  = new TH1F("hRecoRefVsT" ,"reco reference tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);
  fhEffRefVsT  = new TH1F("hEffRefVsT" ,"eff reference tracks;theta [deg];yield [a.u.]",theBins,minThe,maxThe);

  fHistoList->Add(fhMCAllVsT);
  fHistoList->Add(fhMCAccVsT);
  fHistoList->Add(fhMCPrimVsT);
  fHistoList->Add(fhMCSecVsT);
  fHistoList->Add(fhRecoAccVsT);
  fHistoList->Add(fhRecoPrimVsT);
  fHistoList->Add(fhRecoSecVsT);
  fHistoList->Add(fhEffAccVsT);
  fHistoList->Add(fhEffPrimVsT);
  fHistoList->Add(fhEffSecVsT);
  fHistoList->Add(fhMCRefVsT);
  fHistoList->Add(fhRecoRefVsT);
  fHistoList->Add(fhEffRefVsT);

  // number of mc tracks, reco tracks, efficiency as function of PHI
  Double_t minPhi = -180.;
  Double_t maxPhi =  180.;
  Int_t   phiBins = 72;
  fhMCAllVsA  = new TH1F("hMCAllVsA" ,"all mc tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhMCAccVsA  = new TH1F("hMCAccVsA" ,"reconstruable mc tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhMCPrimVsA = new TH1F("hMCPrimVsA","primary mc tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhMCSecVsA  = new TH1F("hMCSecVsA" ,"secondary mc tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhRecoAccVsA  = new TH1F("hRecoAccVsA" ,"reconstructed tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhRecoPrimVsA = new TH1F("hRecoPrimVsA","reco primary tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhRecoSecVsA  = new TH1F("hRecoSecVsA" ,"reco secondary tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhEffAccVsA  = new TH1F("hEffAccVsA" ,"eff all tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhEffPrimVsA = new TH1F("hEffPrimVsA","eff primary tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhEffSecVsA  = new TH1F("hEffSecVsA" ,"eff secondary tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhMCRefVsA  = new TH1F("hMCRefVsA" ,"reference mc tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhRecoRefVsA  = new TH1F("hRecoRefVsA" ,"reco reference tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);
  fhEffRefVsA  = new TH1F("hEffRefVsA" ,"eff reference tracks;phi [deg];yield [a.u.]",phiBins,minPhi,maxPhi);

  fHistoList->Add(fhMCAllVsA);
  fHistoList->Add(fhMCAccVsA);
  fHistoList->Add(fhMCPrimVsA);
  fHistoList->Add(fhMCSecVsA);
  fHistoList->Add(fhRecoAccVsA);
  fHistoList->Add(fhRecoPrimVsA);
  fHistoList->Add(fhRecoSecVsA);
  fHistoList->Add(fhEffAccVsA);
  fHistoList->Add(fhEffPrimVsA);
  fHistoList->Add(fhEffSecVsA);
  fHistoList->Add(fhMCRefVsA);
  fHistoList->Add(fhRecoRefVsA);
  fHistoList->Add(fhEffRefVsA);

  // momentum resolution vs MOMENTUM
  fhMomResAccVsP  = new TH2F("hMomResAccVsP" ,"momentum resolution for all tracks;p [GeV/c];#delta{p}/p [%]",momBins,minMom,maxMom,400,-20.,20.);
  fhMomResPrimVsP = new TH2F("hMomResPrimVsP","momentum resolution for primary tracks;p [GeV/c];#delta{p}/p [%]",momBins,minMom,maxMom,400,-20.,20.);
  fhMomResSecVsP  = new TH2F("hMomResSecVsP" ,"momentum resolution for secondary tracks;p [GeV/c];#delta{p}/p [%]",momBins,minMom,maxMom,400,-20.,20.);
  fhMomResRefVsP  = new TH2F("hMomResRefVsP" ,"momentum resolution for reference tracks;p [GeV/c];#delta{p}/p [%]",momBins,minMom,maxMom,400,-20.,20.);
  fHistoList->Add(fhMomResAccVsP);
  fHistoList->Add(fhMomResPrimVsP);
  fHistoList->Add(fhMomResSecVsP);
  fHistoList->Add(fhMomResRefVsP);
  // momentum resolution vs THETA
  fhMomResAccVsT  = new TH2F("hMomResAccVsT" ,"momentum resolution for all tracks;theta [deg];#delta{p}/p [%]",theBins,minThe,maxThe,400,-20.,20.);
  fhMomResPrimVsT = new TH2F("hMomResPrimVsT","momentum resolution for primary tracks;theta [deg];#delta{p}/p [%]",theBins,minThe,maxThe,400,-20.,20.);
  fhMomResSecVsT  = new TH2F("hMomResSecVsT" ,"momentum resolution for secondary tracks;theta [deg];#delta{p}/p [%]",theBins,minThe,maxThe,400,-20.,20.);
  fhMomResRefVsT  = new TH2F("hMomResRefVsT" ,"momentum resolution for reference tracks;theta [deg];#delta{p}/p [%]",theBins,minThe,maxThe,400,-20.,20.);
  fHistoList->Add(fhMomResAccVsT);
  fHistoList->Add(fhMomResPrimVsT);
  fHistoList->Add(fhMomResSecVsT);
  fHistoList->Add(fhMomResRefVsT);
  // momentum resolution vs PHI
  fhMomResAccVsA  = new TH2F("hMomResAccVsA" ,"momentum resolution for all tracks;phi [deg];#delta{p}/p [%]",phiBins,minPhi,maxPhi,400,-20.,20.);
  fhMomResPrimVsA = new TH2F("hMomResPrimVsA","momentum resolution for primary tracks;phi [deg];#delta{p}/p [%]",phiBins,minPhi,maxPhi,400,-20.,20.);
  fhMomResSecVsA  = new TH2F("hMomResSecVsA" ,"momentum resolution for secondary tracks;phi [deg];#delta{p}/p [%]",phiBins,minPhi,maxPhi,400,-20.,20.);
  fhMomResRefVsA  = new TH2F("hMomResRefVsA" ,"momentum resolution for reference tracks;phi [deg];#delta{p}/p [%]",phiBins,minPhi,maxPhi,400,-20.,20.);
  fHistoList->Add(fhMomResAccVsA);
  fHistoList->Add(fhMomResPrimVsA);
  fHistoList->Add(fhMomResSecVsA);
  fHistoList->Add(fhMomResRefVsA);

  fhRecoAllP  = new TH1F("hRecoAllP" ,"reconstructed track momentum for all",2000,0.,20.);
  fhRecoPrimP = new TH1F("hRecoPrimP","reconstructed track momentum for primaries",2000,0.,20.);
  fhRecoSecP  = new TH1F("hRecoSecP" ,"reconstructed track momentum for secondaries",2000,0.,20.);
  fhRecoAllT  = new TH1F("hRecoAllT" ,"reconstructed track theta for all",400,0.,40.);
  fhRecoPrimT = new TH1F("hRecoPrimT","reconstructed track theta for primaries",400,0.,40.);
  fhRecoSecT  = new TH1F("hRecoSecT" ,"reconstructed track theta for secondaries",400,0.,40.);
  fhRecoAllA  = new TH1F("hRecoAllA" ,"reconstructed track phi for all",360,-180.,180.);
  fhRecoPrimA = new TH1F("hRecoPrimA","reconstructed track phi for primaries",360,-180.,180.);
  fhRecoSecA  = new TH1F("hRecoSecA" ,"reconstructed track phi for secondaries",360,-180.,180.);
  fHistoList->Add(fhRecoAllP);
  fHistoList->Add(fhRecoPrimP);
  fHistoList->Add(fhRecoSecP);
  fHistoList->Add(fhRecoAllT);
  fHistoList->Add(fhRecoPrimT);
  fHistoList->Add(fhRecoSecT);
  fHistoList->Add(fhRecoAllA);
  fHistoList->Add(fhRecoPrimA);
  fHistoList->Add(fhRecoSecA);


  fhNofMCTracksPerEvent   = new TH1F("hNofMCTracksPerEvent","nof MC tracks per event;event no;nof mc tracks",100001,-0.5,100000.5);
  fhNofRecoTracksPerEvent = new TH1F("hNofRecoTracksPerEvent","nof reco tracks per event;event no;nof mc tracks",100001,-0.5,100000.5);
  fHistoList->Add(fhNofMCTracksPerEvent);
  fHistoList->Add(fhNofRecoTracksPerEvent);
}
// ------------------------------------------------------------

// -----   Private method DivideHistos  --------------------------------------------
void PndGlobalTrackMergerQA::DivideHistos(TH1* hist1, TH1* hist2, TH1* hist3) {
  hist1->Sumw2();
  hist2->Sumw2();
  hist3->Divide(hist1,hist2,1,1,"B");
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGlobalTrackMergerQA::Finish() {
  Double_t effAcc  = 0.;
  Double_t effPrim = 0.;
  Double_t effSec  = 0.;
  Double_t effRef  = 0.;
  if ( fNofMCAcc  ) effAcc  = 100.*(Double_t)fNofRecoAcc /((Double_t)fNofMCAcc);
  if ( fNofMCPrim ) effPrim = 100.*(Double_t)fNofRecoPrim/((Double_t)fNofMCPrim);
  if ( fNofMCSec  ) effSec  = 100.*(Double_t)fNofRecoSec /((Double_t)fNofMCSec);
  if ( fNofMCRef  ) effRef  = 100.*(Double_t)fNofRecoRef /((Double_t)fNofMCRef);

  Double_t ghPerEv = 0.;
  Double_t ghPerTr = 0.;
  Double_t clPerEv = 0.;
  Double_t clPerTr = 0.;
  if ( fNofEvents ) ghPerEv = (Double_t)fNofRecoGhosts/((Double_t)fNofEvents);
  if ( fNofMCAll  ) ghPerTr = (Double_t)fNofRecoGhosts/((Double_t)fNofMCAll);
  if ( fNofEvents ) clPerEv = (Double_t)fNofRecoClones/((Double_t)fNofEvents);
  if ( fNofMCAll  ) clPerTr = (Double_t)fNofRecoClones/((Double_t)fNofMCAll);

  cout << "-------------------- PndGlobalTrackMergerQA : Summary ------------------" << endl;
  cout << " Events:        " << setw(10) << fNofEvents << endl;
  cout << " MC Tracks:     " << setw(10) << fNofMCAll << endl;
  cout << " reconstruable: " << setw(10) << fNofMCAcc  << " reconstructed: " << setw(10) << fNofRecoAcc  << " >>>> " << effAcc  << "%" << endl;
  cout << " primaries    : " << setw(10) << fNofMCPrim << " reconstructed: " << setw(10) << fNofRecoPrim << " >>>> " << effPrim << "%" << endl;
  cout << " reference    : " << setw(10) << fNofMCRef  << " reconstructed: " << setw(10) << fNofRecoRef  << " >>>> " << effRef  << "%" << endl;
  cout << " secondaries  : " << setw(10) << fNofMCSec  << " reconstructed: " << setw(10) << fNofRecoSec  << " >>>> " << effSec  << "%" << endl;
  cout << " ghosts       : " << setw(10) << fNofRecoGhosts << "   >>> " << setw(10) << ghPerEv << " per event   >>> " << setw(10) << ghPerTr << " per MC Track" << endl;
  cout << " clones       : " << setw(10) << fNofRecoClones << "   >>> " << setw(10) << clPerEv << " per event   >>> " << setw(10) << clPerTr << " per MC Track" << endl;
  cout << "---------------------------------------------------------------------" << endl; 


  DivideHistos(fhRecoAccVsP, fhMCAccVsP, fhEffAccVsP);
  DivideHistos(fhRecoPrimVsP,fhMCPrimVsP,fhEffPrimVsP);
  DivideHistos(fhRecoSecVsP, fhMCSecVsP, fhEffSecVsP);
  DivideHistos(fhRecoRefVsP, fhMCRefVsP, fhEffRefVsP);
  fhEffAccVsP ->Scale(100.);
  fhEffPrimVsP->Scale(100.);
  fhEffSecVsP ->Scale(100.);
  fhEffRefVsP ->Scale(100.);
  DivideHistos(fhRecoAccVsT, fhMCAccVsT, fhEffAccVsT);
  DivideHistos(fhRecoPrimVsT,fhMCPrimVsT,fhEffPrimVsT);
  DivideHistos(fhRecoSecVsT, fhMCSecVsT, fhEffSecVsT);
  DivideHistos(fhRecoRefVsT, fhMCRefVsT, fhEffRefVsT);
  fhEffAccVsT ->Scale(100.);
  fhEffPrimVsT->Scale(100.);
  fhEffSecVsT ->Scale(100.);
  fhEffRefVsT ->Scale(100.);
  DivideHistos(fhRecoAccVsA, fhMCAccVsA, fhEffAccVsA);
  DivideHistos(fhRecoPrimVsA,fhMCPrimVsA,fhEffPrimVsA);
  DivideHistos(fhRecoSecVsA, fhMCSecVsA, fhEffSecVsA);
  DivideHistos(fhRecoRefVsA, fhMCRefVsA, fhEffRefVsA);
  fhEffAccVsA ->Scale(100.);
  fhEffPrimVsA->Scale(100.);
  fhEffSecVsA ->Scale(100.);
  fhEffRefVsA ->Scale(100.);

  TFile* temp = gFile;
  FairRootManager* ioman = FairRootManager::Instance();
  gFile = ioman->GetOutFile();
  gDirectory = (TDirectory*)gFile;

  gDirectory->mkdir("GlobalTrackMergerQA");
  gDirectory->cd("GlobalTrackMergerQA");
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  gDirectory->cd("..");

  gFile = temp;
}
// ------------------------------------------------------------
 

ClassImp(PndGlobalTrackMergerQA)
