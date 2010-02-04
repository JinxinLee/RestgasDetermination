// -------------------------------------------------------------------------
// -----                PndGlobalIdealTrackMerger source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGlobalIdealTrackMerger.h"

// Pnd includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairHit.h"
#include "PndTrackCandHit.h"
#include "FairRootManager.h"
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
using std::endl;
using std::flush;
using std::map;

// -----   Default constructor   ------------------------------------------
PndGlobalIdealTrackMerger::PndGlobalIdealTrackMerger() : FairTask("Global Ideal Track Merger", 1) {
  fMCTrackArray  = NULL;
  //  fMCTrackSeen   = NULL;
  for ( Int_t idet = 0 ; idet < 5 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fTrackArray[idet] = NULL;
  }
  fGlobalTrackArray          = NULL;
  fNofEvents    = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGlobalIdealTrackMerger::PndGlobalIdealTrackMerger(Int_t iVerbose) 
  : FairTask("GEM Track Finder QA", iVerbose) {
  fMCTrackArray  = NULL;
  //  fMCTrackSeen   = NULL;
  for ( Int_t idet = 0 ; idet < 5 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fTrackArray[idet] = NULL;
  }
  fGlobalTrackArray          = NULL;
  fNofEvents    = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGlobalIdealTrackMerger::~PndGlobalIdealTrackMerger() { 
  fGlobalTrackArray->Delete();
}

// -----   Init  -----------------------------------------------------------
InitStatus PndGlobalIdealTrackMerger::Init() {
  
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
  
  TString trArrayName[5] = {"MVDIdealTrackCand",
			    "STTTrackCand",
			    "TPCTrackCand",
			    "GEMTrackCand",
			    "DCHTrackCand"};

  for ( Int_t idet = 0 ; idet < 5 ; idet++ ) {
    if ( fIncludeDet[idet] == kTRUE ) {
      fTrackArray[idet] = (TClonesArray*) ioman->GetObject(trArrayName[idet].Data());    // Get Mvd track Array
      
      if ( !fTrackArray[idet] ) {
	cout << "-E- " << GetName() << "::Init: No " << trArrayName[idet].Data() << " array!" << endl;
	return kERROR;
      }
      std::cout << "-I- " << GetName() << ": " << trArrayName[idet].Data() << " array found" << std::endl;
    }

  }
  
  // Create and register GemTrack array
  fGlobalTrackArray = new TClonesArray("PndTrackCand",100);
  ioman->Register("GlobalTrackCand", "Global Track Candidates", fGlobalTrackArray, kTRUE);

  std::cout << "-I- "<< GetName() <<": Intialization successfull" << std::endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGlobalIdealTrackMerger::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGlobalIdealTrackMerger::SetParContainers() {
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndGlobalIdealTrackMerger::Exec(Option_t* opt) {
   
  //  cout << "=============== EVENT " << fNofEvents << " =================" << endl;
  fGlobalTrackArray->Delete();

  fNofEvents++;

  Int_t nofMCTracks = fMCTrackArray->GetEntriesFast();

  TString detName[5] = {"MVD","STT","TPC","GEM","DCH"};

  std::vector<Int_t> trackSeen (5,-1);

  fMCDetTracks.resize(nofMCTracks);
  fMCTrackSeen.resize(nofMCTracks);
  fMCDetTracks.assign(nofMCTracks,trackSeen);
  fMCTrackSeen.assign(nofMCTracks,kFALSE);

  PndTrackCand* trCand;

  Int_t nofTracks;
  Int_t mcTrId;

  for ( Int_t idet = 0 ; idet < 5 ; idet++ ) {
    if ( fIncludeDet[idet] == kTRUE ) {
      nofTracks = fTrackArray[idet]->GetEntriesFast();
      for ( Int_t itr = 0 ; itr < nofTracks ; itr++ ) {
	trCand = (PndTrackCand*)fTrackArray[idet]->At(itr);
	mcTrId = trCand->getMcTrackId();
	if ( mcTrId < 0 || mcTrId >= nofMCTracks ) 
	  cout << detName[idet].Data() << " TRACK " << itr << " HAS NO MC TRACK ID (" << mcTrId << ")" << endl;
	else if ( fMCDetTracks[mcTrId][idet] != -1 ) 
	  cout << detName[idet].Data() << " TRACKS " << itr << " AND " 
	       << fMCDetTracks[mcTrId][idet] << " HAVE THE SAME MC ID = " << mcTrId << endl;
	else {
	  fMCDetTracks[mcTrId][idet] = itr;
	  fMCTrackSeen[mcTrId]       = kTRUE;
	  //	  cout << detName[idet].Data() << " TRACK " << itr << " HAS MC ID = " << mcTrId << endl;
	}
      }
    }
  }
  
  Int_t nofCreatedTracks = 0;
  PndTrackCand* globalTrackCand;
  PndTrackCandHit candHit;
  FairHit* detHit;
  for ( Int_t itr = 0 ; itr < nofMCTracks ; itr++ ) {
    if ( fMCTrackSeen[itr] == kFALSE ) continue;
    
    new((*fGlobalTrackArray)[nofCreatedTracks]) PndTrackCand();
    
    globalTrackCand = (PndTrackCand*) fGlobalTrackArray->At(nofCreatedTracks);
        cout << "TRACK " << itr << " has " << flush;

    for ( Int_t idet = 0 ; idet < 5 ; idet++ ) {
      if ( fMCDetTracks[itr][idet] == -1 ) continue;
      trCand = (PndTrackCand*)fTrackArray[idet]->At(fMCDetTracks[itr][idet]);
      if ( trCand->getMcTrackId() != itr ) {
	//	cout << "trackCand " << fMCDetTracks[itr][idet] << " has MC id " << trCand->getMcTrackId() << " while expected " << itr << endl;
	continue;
      }
      // take seed position and momentum from the first track in {MVD,STT,TPC,GEM,DCH}, should be closest to vertex
      if ( globalTrackCand->GetNHits() == 0 ) {
	TVector3 posSeed = trCand->getPosSeed();
	TVector3 dirSeed = trCand->getDirSeed();
	Double_t QoverPs = trCand->getQoverPseed();
	globalTrackCand->setTrackSeed(posSeed,dirSeed,QoverPs);
	globalTrackCand->setMcTrackId(itr);
      }
      Int_t nLocH = trCand->GetNHits();
            cout << nLocH << "(" << detName[idet].Data() << ")  + " << flush;
      for ( Int_t ih = 0 ; ih < nLocH ; ih++ ) {
	candHit = trCand->GetSortedHit(ih);
	globalTrackCand->AddHit(candHit.GetDetId(),candHit.GetHitId(),candHit.GetRho());
      }
    }
    
    globalTrackCand->Sort();
    
    cout << " = " << globalTrackCand->GetNHits() << " hits" << endl;

    nofCreatedTracks++;
  }
    
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGlobalIdealTrackMerger::Finish() {
  fGlobalTrackArray->Clear();

  std::cout << " -I- " << GetName() << " FINISHED AFTER " << fNofEvents << " EVENTS!" << endl;
}
// ------------------------------------------------------------
 

ClassImp(PndGlobalIdealTrackMerger)
