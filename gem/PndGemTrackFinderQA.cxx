// -------------------------------------------------------------------------
// -----                PndGemTrackFinderQA source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGemTrackFinderQA.h"

// FairRoot includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"
// Pnd includes
#include "PndGemMCPoint.h"
#include "PndGemDigiPar.h"
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
using std::map;

// -----   Default constructor   ------------------------------------------
PndGemTrackFinderQA::PndGemTrackFinderQA() : FairTask("GEM Track Finder QA", 1) {
  fMCTrackArray  = NULL;
  fMCPointArray  = NULL;
  fGemHitArray   = NULL;
  fGemTrackArray = NULL;
  fNofEvents    = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemTrackFinderQA::PndGemTrackFinderQA(Int_t iVerbose) 
  : FairTask("GEM Track Finder QA", iVerbose) {
  fMCTrackArray  = NULL;
  fMCPointArray  = NULL;
  fGemHitArray   = NULL;
  fGemTrackArray = NULL;
  fNofEvents    = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemTrackFinderQA::~PndGemTrackFinderQA() { }

// -----   Init  -----------------------------------------------------------
InitStatus PndGemTrackFinderQA::Init() {
  
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
  fMCTrackArray  = (TClonesArray*) ioman->ActivateBranch("MCTrack");
  if( !fMCTrackArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCTrack array!"
	 << endl;
    return kERROR;
  }
  
  // Get PndGemPoint (MCPoint) array
  fMCPointArray  = (TClonesArray*) ioman->GetObject("GEMPoint");
  if( !fMCPointArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCPoint array!"
	 << endl;
    return kERROR;
  }

  // Get Gem hit Array
  fGemHitArray  = (TClonesArray*) ioman->GetObject("GEMHit");
  if ( !fGemHitArray ) {
    cout << "-E- " << GetName() << "::Init: No PndGemHit array!" << endl;
    return kERROR;
  }

  // Get Gem track Array
  fGemTrackArray  = (TClonesArray*) ioman->GetObject("GEMTrack");
  if ( !fGemTrackArray ) {
    cout << "-E- " << GetName() << "::Init: No PndGemTrack array!" << endl;
    return kERROR;
  }
  

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar*)(rtdb->getContainer("PndGemDetectors"));
  cout << "THERE ARE " << fDigiPar->GetNStations() << " GEM STATIONS" << endl;

  std::cout << "-I- "<< GetName() <<": Intialization successfull" << std::endl;

  CreateHistos();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemTrackFinderQA::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemTrackFinderQA::SetParContainers() {
  
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar*)(db->getContainer("PndGemDetectors"));

  fNofEvents = 0;
  fTargetPos.SetXYZ(0.,0.,0.);
  fNeededStationsToRecoTrack = 3;
  fMinQuota = 0.6;

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
void PndGemTrackFinderQA::Exec(Option_t* opt) {
  
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

  Int_t nofMCTracks = fMCTrackArray->GetEntriesFast();
  Int_t nofRecoTracks = fGemTrackArray->GetEntriesFast();

  fhNofMCTracksPerEvent  ->Fill(fNofEvents-1,nofMCTracks);
  fhNofRecoTracksPerEvent->Fill(fNofEvents-1,nofRecoTracks);

  PndMCTrack* mcTrack;
  PndTrack* gemTrack;
  PndGemHit* gemHit;

  for ( Int_t imct = 0 ; imct < nofMCTracks ; imct++ ) {

    mcTrack = (PndMCTrack*) fMCTrackArray->At(imct);

    if ( mcTrack->GetNPoints(kGEM) == 0 ) continue;

    TVector3 mcVertex = mcTrack->GetStartVertex();
    Bool_t isPrim = kFALSE;
    TVector3 mcMomVec = mcTrack->GetMomentum();
    Double_t mcMomMag = mcMomVec.Mag();
    Double_t mcMomThe = mcMomVec.Theta()*TMath::RadToDeg();
    Int_t    mcPoints = fMCTrackNofGemPoints[imct];

    if ( (mcVertex-fTargetPos).Mag() < 1. ) 
      isPrim = kTRUE;

    fhMCAllVsP->Fill(mcMomMag);
    fhMCAllVsT->Fill(mcMomThe);
    fhMCAllVsN->Fill(mcPoints);

    // check reconstrubility, continue only for reconstruable tracks
    nofMCAll ++;
    if ( fMCTrackNofCrossedGemStations[imct] < fNeededStationsToRecoTrack ) continue;
    nofMCAcc ++;

    fhMCAccVsP->Fill(mcMomMag);
    fhMCAccVsT->Fill(mcMomThe);
    fhMCAccVsN->Fill(mcPoints);

    if ( isPrim ) {
      nofMCPrim ++;
      fhMCPrimVsP->Fill(mcMomMag);
      fhMCPrimVsT->Fill(mcMomThe);
      fhMCPrimVsN->Fill(mcPoints);
    }
    else {
      nofMCSec ++;
      fhMCSecVsP->Fill(mcMomMag);
      fhMCSecVsT->Fill(mcMomThe);
      fhMCSecVsN->Fill(mcPoints);
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
      fhMCRefVsN->Fill(mcPoints);
    }

    for ( Int_t irtr = 0 ; irtr < nofRecoTracks ; irtr++ ) {
      if ( fRecoTrackMCMatch[irtr] != imct ) continue;
      gemTrack = (PndTrack*) fGemTrackArray->At(irtr);
      
      nofRecoAcc ++;

      fhRecoAccVsP->Fill(mcMomMag);
      fhRecoAccVsT->Fill(mcMomThe);
      fhRecoAccVsN->Fill(mcPoints);
      fhMomResAccVsP->Fill(mcMomMag,100.*(mcMomMag-1./TMath::Abs(gemTrack->GetTrackCand().getQoverPseed()))/mcMomMag);
      fhMomResAccVsT->Fill(mcMomThe,100.*(mcMomMag-1./TMath::Abs(gemTrack->GetTrackCand().getQoverPseed()))/mcMomMag);

      if ( isRefP ) {
	fhRecoRefVsT->Fill(mcMomThe);
	fhMomResRefVsT->Fill(mcMomThe,100.*(mcMomMag-1./TMath::Abs(gemTrack->GetTrackCand().getQoverPseed()))/mcMomMag);
      }
      if ( isRefT ) {
	fhRecoRefVsP->Fill(mcMomMag);
	fhMomResRefVsP->Fill(mcMomMag,100.*(mcMomMag-1./TMath::Abs(gemTrack->GetTrackCand().getQoverPseed()))/mcMomMag);
      }
      if ( isRefP && isRefT ) {
	nofRecoRef ++;
	fhRecoRefVsN->Fill(mcPoints);
      }
      if ( isPrim ) {
	nofRecoPrim ++;
	fhRecoPrimVsP->Fill(mcMomMag);
	fhRecoPrimVsT->Fill(mcMomThe);
	fhRecoPrimVsN->Fill(mcPoints);
	fhMomResPrimVsP->Fill(mcMomMag,100.*(mcMomMag-1./TMath::Abs(gemTrack->GetTrackCand().getQoverPseed()))/mcMomMag);
	fhMomResPrimVsT->Fill(mcMomThe,100.*(mcMomMag-1./TMath::Abs(gemTrack->GetTrackCand().getQoverPseed()))/mcMomMag);
      }
      else {
	nofRecoSec ++;
	fhRecoSecVsP->Fill(mcMomMag);
	fhRecoSecVsT->Fill(mcMomThe);
	fhRecoSecVsN->Fill(mcPoints);
	fhMomResSecVsP->Fill(mcMomMag,100.*(mcMomMag-1./TMath::Abs(gemTrack->GetTrackCand().getQoverPseed()))/mcMomMag);
	fhMomResSecVsT->Fill(mcMomThe,100.*(mcMomMag-1./TMath::Abs(gemTrack->GetTrackCand().getQoverPseed()))/mcMomMag);
      }
      break; //do not include clones, is good for efficiency plots but bad(?) for momentum resolution
    }
  }

  for ( Int_t irtr = 0 ; irtr < nofRecoTracks ; irtr++ ) {
    gemTrack = (PndTrack*) fGemTrackArray->At(irtr);

    fhNofHitsPerTrack->Fill(gemTrack->GetTrackCand().GetNHits());

    if ( fRecoTrackMCMatch[irtr] == -1 ) {
      nofRecoGhosts ++;
      fhNofHitsPerGhost->Fill(gemTrack->GetTrackCand().GetNHits());
      continue;
    }
    fhNofHitsPerRecoTrack->Fill(gemTrack->GetTrackCand().GetNHits());

    Int_t nofCorrHits = 0;
    Int_t nofOthTHits = 0;
    Int_t nofNoTrHits = 0;
    for ( Int_t ihit = 0 ; ihit < gemTrack->GetTrackCand().GetNHits() ; ihit++ ) { 
      PndTrackCandHit tch = gemTrack->GetTrackCand().GetSortedHit(ihit);
      gemHit = (PndGemHit*) fGemHitArray->At(tch.GetHitId());
      if ( gemHit->GetRefIndex() == -1 ) { nofNoTrHits++; continue; }
      if ( gemHit->GetRefIndex() != fRecoTrackMCMatch[irtr] ) { nofCorrHits++; continue; }
      nofCorrHits ++;
    }

    fhNofCorrHitsPerRecoTrack->Fill(nofCorrHits);
    fhNofOthTHitsPerRecoTrack->Fill(nofOthTHits);
    fhNofNoTrHitsPerRecoTrack->Fill(nofNoTrHits);

    for ( Int_t irtr2 = 0 ; irtr2 < nofRecoTracks ; irtr2++ ) {
      if ( irtr == irtr2 ) continue;
      if ( fRecoTrackMCMatch[irtr2] == fRecoTrackMCMatch[irtr] ) {
	fhNofHitsPerClone->Fill(gemTrack->GetTrackCand().GetNHits());
	nofRecoClones ++;
	break;
      }
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
    cout << "---- PndGemTrackFinderQA : Event " << fNofEvents << " summary -----" << endl;
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
void PndGemTrackFinderQA::PrepareMCTracks() {
  
  fMCTrackNofCrossedGemStations.clear();
  fMCTrackNofCrossedGemStations.resize(fMCTrackArray->GetEntriesFast(),0);
  fMCTrackNofGemPoints.clear();
  fMCTrackNofGemPoints.resize(fMCTrackArray->GetEntriesFast(),0);

  const Int_t nofGemPoints = fMCPointArray->GetEntriesFast();

  const Int_t nofMCTracks    = fMCTrackArray->GetEntriesFast();
  const Int_t nofGemStations = fDigiPar->GetNStations();
  Int_t nofPointsPerStation[nofMCTracks][nofGemStations];
  for ( Int_t imct = 0 ; imct < nofMCTracks ; imct++ )
    for ( Int_t is = 0 ; is < nofGemStations ; is++ )
      nofPointsPerStation[imct][is] = 0;

  PndGemMCPoint* mcPoint;
  for ( Int_t imcp = 0 ; imcp < nofGemPoints ; imcp++ ) {
    mcPoint = (PndGemMCPoint*)fMCPointArray->At(imcp);
    TString nodeName = mcPoint->GetDetName();

    Int_t lastDiskStringPos = (string(nodeName.Data())).rfind("Disk");
    nodeName.Remove(0,lastDiskStringPos+4);

    nofPointsPerStation[mcPoint->GetTrackID()][nodeName.Atoi()-1] += 1;

    fMCTrackNofGemPoints[mcPoint->GetTrackID()] += 1;
  }

  Int_t nofCGM = 0;
  for ( Int_t imct = 0 ; imct < nofMCTracks ; imct++ ) {
    //    fMCTrackNofCrossedGemStations[imct] = nofCGM;
    nofCGM = 0;
    for ( Int_t is = 0 ; is < nofGemStations ; is++ )
      if ( nofPointsPerStation[imct][is] > 0 ) 
	nofCGM++;
    fMCTrackNofCrossedGemStations[imct] = nofCGM;
  }
}
// ------------------------------------------------------------

// -----   Private method PrepareMCTrack   --------------------------------------------
void PndGemTrackFinderQA::MatchRecoTracks() {
  
  Int_t nofRecoTracks = fGemTrackArray->GetEntriesFast();

  fRecoTrackMCMatch.clear();
  fRecoTrackMCMatch.resize(nofRecoTracks,-1);

  PndGemHit* gemHit;
  FairMCPoint* mcPoint;
  PndTrack* gemTrack;

  const Int_t nsh = 2*fDigiPar->GetNStations();

  for ( Int_t irtr = 0 ; irtr < nofRecoTracks ; irtr++ ) {
    gemTrack = (PndTrack*) fGemTrackArray->At(irtr);
    vector<Int_t> nofTrMCId(500,0);
 
    for ( Int_t ihit = 0 ; ihit < gemTrack->GetTrackCand().GetNHits() ; ihit++ ) { 
      PndTrackCandHit tch = gemTrack->GetTrackCand().GetSortedHit(ihit);
      gemHit = (PndGemHit*) fGemHitArray->At(tch.GetHitId());
      if ( gemHit->GetRefIndex() == -1 ) { continue; }
      mcPoint = (FairMCPoint*) fMCPointArray->At(gemHit->GetRefIndex());
      nofTrMCId[mcPoint->GetTrackID()] += 1;  
    }
    Int_t bestMCId = -1;
    Int_t largestNofMCId = 0;
    for ( Int_t itm = 0 ; itm < 500 ; itm++ ) {
      if ( nofTrMCId[itm] == largestNofMCId ) { bestMCId = -1; }
      if ( nofTrMCId[itm]  > largestNofMCId ) { bestMCId = itm; largestNofMCId = nofTrMCId[itm]; }
    }
    if ( bestMCId == -1 ) { continue; }
    if ( largestNofMCId < fMinQuota*gemTrack->GetTrackCand().GetNHits() ) continue;

    fRecoTrackMCMatch[irtr] = bestMCId;
  }

}
// ------------------------------------------------------------

// -----   Private method CreateHistos   --------------------------------------------
void PndGemTrackFinderQA::CreateHistos() {
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
  Double_t maxThe = 40.;
  Int_t   theBins = 40;
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

  // number of mc tracks, reco tracks, efficiency as function of NUMBER OF POINTS
  Double_t minPnt =  -0.5;
  Double_t maxPnt = 40.5;
  Int_t   pntBins = 41;
  fhMCAllVsN  = new TH1F("hMCAllVsN" ,"all mc tracks;# points;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhMCAccVsN  = new TH1F("hMCAccVsN" ,"reconstruable mc tracks;# points;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhMCPrimVsN = new TH1F("hMCPrimVsN","primary mc tracks;# points;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhMCSecVsN  = new TH1F("hMCSecVsN" ,"secondary mc tracks;# points;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhRecoAccVsN  = new TH1F("hRecoAccVsN" ,"reconstructed tracks;# points;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhRecoPrimVsN = new TH1F("hRecoPrimVsN","reco primary tracks;# points;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhRecoSecVsN  = new TH1F("hRecoSecVsN" ,"reco secondary tracks;# points;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhEffAccVsN  = new TH1F("hEffAccVsN" ,"eff all tracks;# points;efficiency [%]",pntBins,minPnt,maxPnt);
  fhEffPrimVsN = new TH1F("hEffPrimVsN","eff primary tracks;# points;efficiency [%]",pntBins,minPnt,maxPnt);
  fhEffSecVsN  = new TH1F("hEffSecVsN" ,"eff secondary tracks;# points;efficiency [%]",pntBins,minPnt,maxPnt);
  fhMCRefVsN  = new TH1F("hMCRefVsN" ,"reference mc tracks;# points;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhRecoRefVsN  = new TH1F("hRecoRefVsN" ,"reco reference tracks;# points;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhEffRefVsN  = new TH1F("hEffRefVsN" ,"eff reference tracks;# points;efficiency [%]",pntBins,minPnt,maxPnt);

  fHistoList->Add(fhMCAllVsN);
  fHistoList->Add(fhMCAccVsN);
  fHistoList->Add(fhMCPrimVsN);
  fHistoList->Add(fhMCSecVsN);
  fHistoList->Add(fhRecoAccVsN);
  fHistoList->Add(fhRecoPrimVsN);
  fHistoList->Add(fhRecoSecVsN);
  fHistoList->Add(fhEffAccVsN);
  fHistoList->Add(fhEffPrimVsN);
  fHistoList->Add(fhEffSecVsN);
  fHistoList->Add(fhMCRefVsN);
  fHistoList->Add(fhRecoRefVsN);
  fHistoList->Add(fhEffRefVsN);

  // momentum resolution vs MOMENTUM
  fhMomResAccVsP  = new TH2F("hMomResAccVsP" ,"momentum resolution for all tracks;p [GeV/c];#delta{p}/p [%]",momBins,minMom,maxMom,400,-20.,20.);
  fhMomResPrimVsP = new TH2F("hMomResPrimVsP","momentum resolution for primary tracks;p [GeV/c];#delta{p}/p [%]",momBins,minMom,maxMom,400,-20.,20.);
  fhMomResSecVsP  = new TH2F("hMomResSecVsP" ,"momentum resolution for secondary tracks;p [GeV/c];#delta{p}/p [%]",momBins,minMom,maxMom,400,-20.,20.);
  fhMomResRefVsP  = new TH2F("hMomResRefVsP" ,"momentum resolution for reference tracks;p [GeV/c];#delta{p}/p [%]",momBins,minMom,maxMom,400,-20.,20.);
  fHistoList->Add(fhMomResAccVsP);
  fHistoList->Add(fhMomResPrimVsP);
  fHistoList->Add(fhMomResSecVsP);
  fHistoList->Add(fhMomResRefVsP);
  fhMomResAccVsT  = new TH2F("hMomResAccVsT" ,"momentum resolution for all tracks;theta [deg];#delta{p}/p [%]",theBins,minThe,maxThe,400,-20.,20.);
  fhMomResPrimVsT = new TH2F("hMomResPrimVsT","momentum resolution for primary tracks;theta [deg];#delta{p}/p [%]",theBins,minThe,maxThe,400,-20.,20.);
  fhMomResSecVsT  = new TH2F("hMomResSecVsT" ,"momentum resolution for secondary tracks;theta [deg];#delta{p}/p [%]",theBins,minThe,maxThe,400,-20.,20.);
  fhMomResRefVsT  = new TH2F("hMomResRefVsT" ,"momentum resolution for reference tracks;theta [deg];#delta{p}/p [%]",theBins,minThe,maxThe,400,-20.,20.);
  fHistoList->Add(fhMomResAccVsT);
  fHistoList->Add(fhMomResPrimVsT);
  fHistoList->Add(fhMomResSecVsT);
  fHistoList->Add(fhMomResRefVsT);


  fhNofHitsPerTrack = new TH1F("hNofHitsPerTrack","nof hits per track;# hits;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhNofHitsPerRecoTrack = new TH1F("hNofHitsPerRecoTrack","nof hits per reco track;# hits;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhNofHitsPerGhost = new TH1F("hNofHitsPerGhost","nof hits per ghost track;# hits;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhNofHitsPerClone = new TH1F("hNofHitsPerClone","nof hits per clone track;# hits;yield [a.u.]",pntBins,minPnt,maxPnt);
  fHistoList->Add(fhNofHitsPerTrack);
  fHistoList->Add(fhNofHitsPerRecoTrack);
  fHistoList->Add(fhNofHitsPerGhost);
  fHistoList->Add(fhNofHitsPerClone);

  fhNofCorrHitsPerRecoTrack = new TH1F("hNofCorrHitsPerRecoTrack","nof hits with correct MCId per track;# hits;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhNofOthTHitsPerRecoTrack = new TH1F("hNofOthTHitsPerRecoTrack","nof hits with different MCId per track;# hits;yield [a.u.]",pntBins,minPnt,maxPnt);
  fhNofNoTrHitsPerRecoTrack = new TH1F("hNofNoTrHitsPerRecoTrack","nof hits without any MCId per track;# hits;yield [a.u.]",pntBins,minPnt,maxPnt);
  fHistoList->Add(fhNofCorrHitsPerRecoTrack);
  fHistoList->Add(fhNofOthTHitsPerRecoTrack);
  fHistoList->Add(fhNofNoTrHitsPerRecoTrack);


  fhNofMCTracksPerEvent   = new TH1F("hNofMCTracksPerEvent","nof MC tracks per event;event no;nof mc tracks",100001,-0.5,100000.5);
  fhNofRecoTracksPerEvent = new TH1F("hNofRecoTracksPerEvent","nof reco tracks per event;event no;nof mc tracks",100001,-0.5,100000.5);
  fHistoList->Add(fhNofMCTracksPerEvent);
  fHistoList->Add(fhNofRecoTracksPerEvent);
}
// ------------------------------------------------------------

// -----   Private method DivideHistos  --------------------------------------------
void PndGemTrackFinderQA::DivideHistos(TH1* hist1, TH1* hist2, TH1* hist3) {
  hist1->Sumw2();
  hist2->Sumw2();
  hist3->Divide(hist1,hist2,1,1,"B");
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGemTrackFinderQA::Finish() {
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

  cout << "-------------------- PndGemTrackFinderQA : Summary ------------------" << endl;
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
  DivideHistos(fhRecoAccVsN, fhMCAccVsN, fhEffAccVsN);
  DivideHistos(fhRecoPrimVsN,fhMCPrimVsN,fhEffPrimVsN);
  DivideHistos(fhRecoSecVsN, fhMCSecVsN, fhEffSecVsN);
  DivideHistos(fhRecoRefVsN, fhMCRefVsN, fhEffRefVsN);
  fhEffAccVsN ->Scale(100.);
  fhEffPrimVsN->Scale(100.);
  fhEffSecVsN ->Scale(100.);
  fhEffRefVsN ->Scale(100.);

  gDirectory->mkdir("GemTrackFinderQA");
  gDirectory->cd("GemTrackFinderQA");
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  gDirectory->cd("..");
}
// ------------------------------------------------------------
 

ClassImp(PndGemTrackFinderQA)
