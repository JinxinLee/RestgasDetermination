// -------------------------------------------------------------------------
// -----                  PndDchMatchTracks source file                -----
// -------------------------------------------------------------------------

// Pnd includes
#include "PndDchMatchTracks.h"
#include "PndDchHit.h"
#include "PndDchCylinderHit.h"
#include "PndDchDigi.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"

// Root includes
#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "TClonesArray.h"

// C++ includes
#include <iostream>
#include <map>
using std::endl;
using std::cout;
using std::map;


// -----   Default constructor   -------------------------------------------
PndDchMatchTracks::PndDchMatchTracks() 
: FairTask("PndDchMatchTracks") {
	fTracks       = NULL;
	fPoints       = NULL;
	fHits         = NULL;
	fCylinderHits = NULL;
	fDigis        = NULL;
	fHorDs        = NULL;
	fVerbose = 1;
	fUseHitOrDigi = "hit";
}
// -------------------------------------------------------------------------



// -----   Constructor with verbosity level   ------------------------------
PndDchMatchTracks::PndDchMatchTracks(Int_t verbose) 
: FairTask("PndDchMatchTracks") {
	fTracks       = NULL;
	fPoints       = NULL;
	fHits         = NULL;
	fCylinderHits = NULL;
	fDigis        = NULL;
	fHorDs        = NULL;
	fVerbose = verbose;
	fUseHitOrDigi = "hit";
}
// -------------------------------------------------------------------------



// -----   Constructor with name, title, digi/hits usage and verbosity  ---------------------
PndDchMatchTracks::PndDchMatchTracks(const char* name, const char* title,
		TString hitOrDigi, Int_t verbose) 
: FairTask(name) {
	fTracks  = NULL;
	fPoints  = NULL;
	fHits    = NULL;
	fDigis   = NULL;
	fHorDs   = NULL;
	fVerbose = verbose;
	fUseHitOrDigi = hitOrDigi;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDchMatchTracks::~PndDchMatchTracks() { }
// -------------------------------------------------------------------------




// -----   Public method Init   --------------------------------------------
InitStatus PndDchMatchTracks::Init() {

	// Get FairRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if( !ioman ) {
		cout << "-E- PndDchMatchTracks::Init: "
		<< "RootManager not instantised!" << endl;
		return kFATAL;
	}

	if("hit" == fUseHitOrDigi) {
		cout << "-I- "<< GetName() <<"::Init: Works on hits!" << endl;
		// Get PndDchHit Array
		fHorDs  = (TClonesArray*) ioman->GetObject("PndDchHit");
		if( !fHorDs ) {
			cout << "-W- "<< GetName() <<"::Init: No PndDchHit array!"	<< endl;
			return kERROR;
		}
	}

	if("digi" == fUseHitOrDigi) {
		cout << "-I- "<< GetName() <<"::Init: Works on digis!" << endl;

		// Get PndDchDigi Array
		fHorDs  = (TClonesArray*) ioman->GetObject("PndDchDigi");
		if( !fHorDs ) {
			cout << "-E- "<< GetName() <<"::Init: No PndDchDigi array!" << endl;
			return kERROR;
		}
	}  

	if("chit" == fUseHitOrDigi) {
		cout << "-I- "<< GetName() <<"::Init: Works on cylinder hits!" << endl;

		// Get PndDchCylinderHit Array
		fHorDs  = (TClonesArray*) ioman->GetObject("PndDchCylinderHit");
		fDigis  = (TClonesArray*) ioman->GetObject("PndDchDigi");
		if( !fHorDs ) {
			cout << "-E- "<< GetName() <<"::Init: No PndDchCylinderHit array!" << endl;
			return kERROR;
		}
		if( !fDigis ) {
			cout << "-E- "<< GetName() <<"::Init: No PndDchDigi array!" << endl;
			return kERROR;
		}
	}  
	
	// Get PndTrackCand Array
	fTracks = (TClonesArray*) ioman->GetObject("DCHTrackCand");
	if( !fTracks ) {
		cout << "-E- "<< GetName() <<"::Init: No DCHTrackCand array!" << endl;
		return kERROR;
	}

	// Get PndDchPoint array
	fPoints = (TClonesArray*) ioman->GetObject("PndDchPoint");
	if( !fPoints ) {
		cout << "-E- "<< GetName() <<"::Init: No DchPoint array!" << endl;
		return kERROR;
	}

	return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndDchMatchTracks::Exec(Option_t* opt) {

  cout << "-------------------------------------------------------" << endl;
  cout << "-I-      "<< GetName() <<"::Exec                      -I-" << endl;
  cout << "-------------------------------------------------------" << endl;
  
  // Create some pointers and variables
  PndTrackCand*      track  = NULL;
  PndDchHit*         hit    = NULL;
  PndDchCylinderHit* cylHit = NULL;
  PndDchDigi*        digi   = NULL;
  FairMCPoint*       point  = NULL;
  Int_t nHits       = 0;
  Int_t nDigis      = 0;
  Int_t nHorDs      = 0;
  Int_t nMCTracks   = 0;
  Int_t iPoint      = 0;
  Int_t iMCTrack    = 0;
  Int_t nAll        = 0;
  Int_t nTrue       = 0;
  Int_t nWrong      = 0;
  Int_t nFake       = 0;
  Int_t nHorDSum    = 0;
  Int_t nTrueSum    = 0;
  Int_t nWrongSum   = 0;
  Int_t nFakeSum    = 0;
  Int_t nMCTrackSum = 0;
  map<Int_t, Int_t>::iterator it;
  
  // Loop over PndTrackCands
  Int_t nTracks = fTracks->GetEntriesFast();
	
  if(fVerbose > 1) {
    cout <<"# of Tracks: "<< nTracks << endl
	 <<"----------------------------"<< endl;
  }
  
  
  for(Int_t iTrack = 0; iTrack < nTracks; iTrack++) { //loop over PndTrackCand
    track = (PndTrackCand*) fTracks->At(iTrack);
    if ( !track ) {
      cout << "-W- "<< GetName() <<"::Exec: Empty DchTrackCand at " 
	   << iTrack << endl;
      continue;
    }
    
    nHorDs = track->GetNHits();
    
    //cout <<"***************"<<  nHorDs<< endl;
    
    nAll = nTrue = nWrong = nFake = nMCTracks = 0;
    
    iPoint = -1;
    // Loop over DchHorDs of track
    for(Int_t iHorD = 0; iHorD < nHorDs; iHorD++) {
      PndTrackCandHit trkHit = track->GetSortedHit(iHorD);
      if("hit" == fUseHitOrDigi) {
	hit = (PndDchHit*) fHorDs->At(trkHit.GetHitId());
	
	if( !hit ) {
	  cout << "-E- "<< GetName() <<"::Exec: "
	       << "No DchHit " << iHorD << " for track " << iTrack << endl;
	  continue;
	}
	iPoint = hit->GetRefIndex();
      }
      if("digi" == fUseHitOrDigi) {
	digi = (PndDchDigi*) fHorDs->At(trkHit.GetHitId());
	
	if( !digi ) {
	  cout << "-E- "<< GetName() <<"::Exec: "
	       << "No Dchdigi " << iHorD << " for track " << iTrack << endl;
	  continue;
	}
	iPoint = digi->GetRefIndex();
      }
      if("chit" == fUseHitOrDigi) {
	cylHit = (PndDchCylinderHit*) fHorDs->At(trkHit.GetHitId());
	
	if( !cylHit ) {
	  cout << "-E- "<< GetName() <<"::Exec: "
	       << "No DchCylHit " << iHorD << " for track " << iTrack << endl;
	  continue;
	}
	digi = (PndDchDigi*) fDigis->At(cylHit->GetDigiIndex());
	iPoint = digi->GetRefIndex();
      }
      
      if(iPoint < 0) { //Fake or background hit
	nFake++;
	continue;
      }
      
      point = (FairMCPoint*) fPoints->At(iPoint);
      if( !point ) {
	cout << "-E- "<< GetName() <<"::Exec: "
	     << "Empty MCPoint " << iPoint << " from DchHorD " << iHorD
	     << " (track " << iTrack << ")" << endl;
	continue;
      }
      
      iMCTrack = point->GetTrackID();
      if(fVerbose > 2) {
	cout <<"NoTrack: "<< iTrack <<" HorD: ";
	cout << track->GetNHits();
	cout <<" ------- DchPoint: "<< iPoint <<" MCTrack: "<< iMCTrack << endl;
      }
      fMatchMap[iMCTrack]++;
    }
    
    // Search for best matching MCTrack
    iMCTrack = -1;
    for(it = fMatchMap.begin(); it != fMatchMap.end(); it++) {
      if (fVerbose > 2) cout << it->second << " common points with MCtrack " << it->first << endl;
      nMCTracks++;
      nAll += it->second;
      if(it->second > nTrue) {
	iMCTrack = it->first;
	nTrue    = it->second;	
      }
    }
    
    nWrong = nAll - nTrue;
    
    if(fVerbose > 1) {
      cout <<"DchTrack no:"<< iTrack
	   <<", #HorDs: " << nHorDs
	   <<", MCTrack: "<< iMCTrack
	   <<", true: "   << nTrue
	   <<", wrong: "  << nWrong
	   <<", fake: "   << nFake
	   <<", #MCTracks: "<< nMCTracks << endl;
    }
    
    track->setMcTrackId(iMCTrack); 
    
    // Some statistics
    nHorDSum    += nHorDs;
    nTrueSum    += nTrue;
    nWrongSum   += nWrong;
    nFakeSum    += nFake;
    nMCTrackSum += nMCTracks;
    
  } // Track loop
  
  // Event statistics
  Double_t qTrue = 0.;
  if(nHorDSum){
    qTrue  = Double_t(nTrueSum)  / Double_t(nHorDSum) * 100.;
    if(fVerbose) {
      Double_t qWrong = Double_t(nWrongSum) / Double_t(nHorDSum) * 100.;
      Double_t qFake  = Double_t(nFakeSum)  / Double_t(nHorDSum) * 100.;
      Double_t qMC    = Double_t(nMCTrackSum) / Double_t(nTracks);
      
      cout << endl;
      cout << "-------------------------------------------------------" << endl;
      cout << "-I-      "<< GetName() <<": Dch track matching       -I-" << endl;
      cout << "-------------------------------------------------------" << endl;
      cout << "Reconstructed DchTracks : " << nTracks << endl;;
      cout << "True  hord assignments  : " << qTrue  << " %" << endl;
      cout << "Wrong hord assignments  : " << qWrong << " %" << endl;
      cout << "Fake  hord assignments  : " << qFake  << " %" << endl;
      cout << "MCTracks per DchTrack   : " << qMC << endl;
      cout << "--------------------------------------------------------" 
	   << endl;
    }else {
      cout << "Reconstructed: "     << nTracks
	   << ", True hit assignments: " << qTrue << " %" << endl;
    }	
  }else {
    cout	 << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "-I-      "<< GetName() <<": Dch track matching       -I-" << endl;
    cout << "-------------------------------------------------------" << endl;
    cout <<"### Empty Track. No hits/digis inside"<< endl;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void PndDchMatchTracks::Finish() {

	//	cout << "-------------------------------------------------------" << endl;
	//	cout << "-I-      "<< GetName() <<": Finish                   -I-" << endl;
	//	cout << "-------------------------------------------------------" << endl;

}
// -------------------------------------------------------------------------



ClassImp(PndDchMatchTracks)
