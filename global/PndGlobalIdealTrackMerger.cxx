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
#include "PndTrack.h"
#include "PndTrackCand.h"
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
using std::setw;
using std::endl;
using std::flush;
using std::map;

// -----   Default constructor   ------------------------------------------
PndGlobalIdealTrackMerger::PndGlobalIdealTrackMerger() : FairTask("Global Ideal Track Merger", 1) {
  fMCTrackArray  = NULL;
  //  fMCTrackSeen   = NULL;
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fTrackArray[idet] = NULL;
  }
  fGlobalTrackArray          = NULL;
  fGlobalTrackCandArray      = NULL;
  fTNofEvents    = 0;
  fTNofTracks    = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGlobalIdealTrackMerger::PndGlobalIdealTrackMerger(Int_t iVerbose) 
  : FairTask("Global Ideal Track Merger", iVerbose) {
  fMCTrackArray  = NULL;
  //  fMCTrackSeen   = NULL;
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fTrackArray[idet] = NULL;
  }
  fGlobalTrackArray          = NULL;
  fGlobalTrackCandArray      = NULL;
  fTNofEvents    = 0;
  fTNofTracks    = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGlobalIdealTrackMerger::~PndGlobalIdealTrackMerger() { 
  fGlobalTrackArray    ->Delete();
  fGlobalTrackCandArray->Delete();
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
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if( !fMCTrackArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCTrack array!"
	 << endl;
    return kERROR;
  }
  
  fDetName[0] = "MVD";
  fDetName[1] = "STT";
  fDetName[2] = "GEM";
  fDetName[3] = "FTS";
  TString trArrayName[4] = {"MVDIdealTrackCand",
			    "STTFoundTrack",
			    "GEMTrack",
			    "FTSTrackCand"};

  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
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
  fGlobalTrackArray = new TClonesArray("PndTrack",100);
  ioman->Register("GlobalTrack", "Global Track", fGlobalTrackArray, kTRUE);

  fGlobalTrackCandArray = new TClonesArray("PndTrackCand",100);
  ioman->Register("GlobalTrackCand", "Global TrackCand", fGlobalTrackCandArray, kTRUE);

  std::cout << "-I- " << GetName() << ": Initialization successfull" << std::endl;
  std::cout << "-I- " << GetName() << ": Merging tracks from " << flush;
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    if ( fIncludeDet[idet] == kFALSE ) continue;
    cout << fDetName[idet].Data() << ", ";
  }
  cout << "\b\b. " << endl;
  cout << "================================================================================" << endl;

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
  if ( fVerbose > 0 ) 
    cout << "=============== EVENT " << fTNofEvents << " =================" << endl;
  fGlobalTrackArray    ->Delete();
  fGlobalTrackCandArray->Delete();

  fTNofEvents++;

  Int_t nofMCTracks = fMCTrackArray->GetEntriesFast();

  std::vector<Int_t> trackSeen (4,-1);

  fMCDetTracks.resize(nofMCTracks);
  fMCTrackSeen.resize(nofMCTracks);
  fMCDetTracks.assign(nofMCTracks,trackSeen);
  fMCTrackSeen.assign(nofMCTracks,kFALSE);

  PndTrack*     localTrack;
  PndTrackCand* localTrackCand;

  Int_t nofTracks;
  Int_t mcTrId;

  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    if ( fIncludeDet[idet] == kTRUE ) {
      nofTracks = fTrackArray[idet]->GetEntriesFast();
      if ( fVerbose > 1 ) 
	cout << "THERE ARE " << nofTracks << " " << fDetName[idet].Data() << " TRACKS IN ARRAY \"" << fTrackArray[idet]->GetName() << "\"" << endl;

      TString arrayName = fTrackArray[idet]->GetName();
      for ( Int_t itr = 0 ; itr < nofTracks ; itr++ ) {
      
	if ( arrayName.Contains("TrackCand") ) { // array of PndTrackCand
	  localTrackCand = (PndTrackCand*)fTrackArray[idet]->At(itr);
	  //	  mcTrId = localTrackCand->getMcTrackId();
	}
	else { // array of PndTrack
	  localTrack = (PndTrack*)fTrackArray[idet]->At(itr);
	  mcTrId = localTrack->GetRefIndex();
	}
	
	if ( mcTrId < 0 || mcTrId >= nofMCTracks ) 
	  cout << fDetName[idet].Data() << " TRACK " << itr << " HAS NO MC TRACK ID (" << mcTrId << ")" << endl;
	else if ( fMCDetTracks[mcTrId][idet] != -1 ) 
	  cout << fDetName[idet].Data() << " TRACKS " << itr << " AND " 
	       << fMCDetTracks[mcTrId][idet] << " HAVE THE SAME MC ID = " << mcTrId << endl;
	else {
	  fMCDetTracks[mcTrId][idet] = itr;
	  fMCTrackSeen[mcTrId]       = kTRUE;
	  //	  cout << fDetName[idet].Data() << " TRACK " << itr << " HAS MC ID = " << mcTrId << endl;
	}
      }
    }
  }
  
  Int_t nofCreatedTracks = 0;
  PndTrack*     globalTrack;
  PndTrackCand* globalTrackCand;
  PndTrackCandHit candHit;
  //FairHit* detHit; //[R.K. 01/2017] unused variable?
  FairTrackParP firstPar;
  FairTrackParP  lastPar;
  for ( Int_t itr = 0 ; itr < nofMCTracks ; itr++ ) {
    if ( fMCTrackSeen[itr] == kFALSE ) continue;
    
    globalTrackCand = new PndTrackCand();
    
    if ( fVerbose > 1 ) 
      cout << "TRACK " << itr << " has " << flush;

    for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
      if ( fMCDetTracks[itr][idet] == -1 ) continue;
      TString arrayName = fTrackArray[idet]->GetName();

      Int_t trackMcId;
      if ( arrayName.Contains("TrackCand") ) { // array of PndTrackCand
	localTrackCand = (PndTrackCand*)fTrackArray[idet]->At(fMCDetTracks[itr][idet]);
	trackMcId = localTrackCand->getMcTrackId();
 	TVector3 posSeed;// = localTrackCand->getPosSeed();
 	TVector3 dirSeed;// = localTrackCand->getDirSeed();
 	Double_t QoverPs;// = localTrackCand->getQoverPseed();
	//	cout << "track " << fDetName[idet].Data() << " Q/P = " << TMath::Abs(QoverPs) << endl;
	Int_t charge = (QoverPs>0?1:-1);
	dirSeed  = dirSeed.Unit();
	if ( TMath::Abs(QoverPs) > 10e-12)
	  dirSeed *= 1./TMath::Abs(QoverPs);
	if ( globalTrackCand->GetNHits() == 0 )
	  firstPar = FairTrackParP(posSeed,dirSeed,
				   TVector3(0.5, 0.5, 0.5),
				   0.1*dirSeed,
				   charge,
				   posSeed,
				   TVector3(1.,0.,0.),
				   TVector3(0.,1.,0.));

	lastPar = FairTrackParP(posSeed,dirSeed,
				TVector3(0.5, 0.5, 0.5),
				0.1*dirSeed,
				charge,
				posSeed,
				TVector3(1.,0.,0.),
				TVector3(0.,1.,0.));
      }
      else {
	localTrack     = (PndTrack*    )fTrackArray[idet]->At(fMCDetTracks[itr][idet]);
	localTrackCand = localTrack->GetTrackCandPtr();
	trackMcId = localTrack->GetRefIndex();
	if ( globalTrackCand->GetNHits() == 0 )
	  firstPar = localTrack->GetParamFirst();
	//	cout << "track " << fDetName[idet].Data() << " mom " << localTrack->GetParamFirst().GetMomentum().Mag() << endl;
	lastPar = localTrack->GetParamLast();
      }

      if ( trackMcId != itr ) {
	continue;
      }

      Int_t nLocH = localTrackCand->GetNHits();
      if ( fVerbose > 1 ) 
	cout << nLocH << "(" << fDetName[idet].Data() << ")  + " << flush;
      for ( Int_t ih = 0 ; ih < nLocH ; ih++ ) {
	candHit = localTrackCand->GetSortedHit(ih);
	Int_t candHitDetId = candHit.GetDetId();
// 	cout << "hit det id " << candHitDetId << " from " << fDetName[idet].Data() << endl;
// 	if ( candHitDetId == 10 ) // THIS IF WOULD TO BE USE STTHELIXHITS INSTEAD OF STTHITS,
// 	  candHitDetId = 11;      // DID NOT THINK YET HOW TO SOLVE IT NICER
	globalTrackCand->AddHit(candHitDetId,candHit.GetHitId(),candHit.GetRho());
      }
    }
    if ( fVerbose > 1 ) 
      cout << "\b\bhits" << flush;    
    
    globalTrackCand->Sort();
    
    if ( fVerbose > 1 ) 
      cout << " = " << globalTrackCand->GetNHits() << " hits" << endl;

    if ( fVerbose > 1 ) {
      cout << "Track hits: " << flush;
      for ( Int_t itemp = 0 ; itemp < globalTrackCand->GetNHits() ; itemp++ )
        cout << globalTrackCand->GetSortedHit(itemp).GetDetId() << " " << flush;
      cout << endl;
    }

    globalTrackCand->setMcTrackId(globalTrackCand->GetNHits()); // TO REMOVE, only temporarily for debugging

    if ( globalTrackCand->GetNHits() < 3 ) continue;

    new((*fGlobalTrackArray)[nofCreatedTracks]) PndTrack(firstPar,lastPar,*globalTrackCand);

    PndTrackCand* trackCand = new((*fGlobalTrackCandArray)[nofCreatedTracks]) PndTrackCand();
    for ( Int_t ihit = 0 ; ihit < globalTrackCand->GetNHits() ; ihit++ ) {
      candHit = globalTrackCand->GetSortedHit(ihit);  
      trackCand->AddHit(candHit.GetHitId(),candHit.GetDetId(),candHit.GetRho());
      //      trackCand->setMcTrackId(globalTrackCand->getMcTrackId());
    }

    globalTrack = (PndTrack*)fGlobalTrackArray->At(nofCreatedTracks);

    globalTrack->SetRefIndex(itr);

    nofCreatedTracks++;
  }

  fTNofTracks += nofCreatedTracks;
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGlobalIdealTrackMerger::Finish() {
  fGlobalTrackArray    ->Clear();
  fGlobalTrackCandArray->Clear();

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Tracks:     " << setw(10) << fTNofTracks << "    ( " << (Double_t)fTNofTracks/((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "--------------------------------------------------------------------------------" << endl; 
}
// ------------------------------------------------------------
 

ClassImp(PndGlobalIdealTrackMerger)
