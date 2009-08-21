// -------------------------------------------------------------------------
// -----                   PndGemFindTracks source file                -----
// -----                  Created 19.03.2009  by R. Karabowicz         -----
// -----                 according to the PndDchFindTracks             -----
// -------------------------------------------------------------------------


// Pnd includes
#include "PndGemFindTracks.h"
#include "PndGemTrackFinder.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

// ROOT includes
#include "TClonesArray.h"

// c++ includes
using std::cout;
using std::endl;

// c++ declaration
class iostream;

// Pnd declaration
class FairBaseParSet;
class PndGemHit;
class PndGemDigi;
class PndTrackCand;

// -----   Default constructor   -------------------------------------------
PndGemFindTracks::PndGemFindTracks() {
  fFinder         	 = NULL;
  fGemHitOrDigiArray         = NULL;
  fTrackArray          = NULL;
  fNofTracks           = 0;
  fUseHitOrDigi        = "hit";
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndGemFindTracks::PndGemFindTracks(const char* name, 
				   const char* title, 
				   PndGemTrackFinder* finder) 
  : FairTask(name) {
  fFinder              = finder;
  fGemHitOrDigiArray         = NULL;
  fTrackArray          = NULL;
  fNofTracks           = 0;
  fUseHitOrDigi        = "hit";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGemFindTracks::~PndGemFindTracks() {
  fTrackArray->Delete();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndGemFindTracks::Init() {
  
  // Check for Track finder
  if( !fFinder ) {
    cout << "-W- PndGemFindTracks::Init: No track finder selected!" << endl;
    return kERROR;
  }
  
  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    cout << "-E- PndGemFindTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
  if("hit" == fUseHitOrDigi) {
    cout << "-I- PndGemFindTracks::Init: Works on hits!" << endl;
    
    // Get Gem hit Array
    fGemHitOrDigiArray  = (TClonesArray*) ioman->GetObject("GEMHit");
    if ( !fGemHitOrDigiArray ) {
      cout << "-W- PndGemFindTracks::Init: No PndGemHit array!" << endl;
      return kERROR;
    }
  }
  
  if("digi" == fUseHitOrDigi) {
    cout << "-I- PndGemFindTracks::Init: Works on digis!" << endl;
    
    // Get Gem digi Array
    fGemHitOrDigiArray  = (TClonesArray*) ioman->GetObject("GEMDigi");
    if ( !fGemHitOrDigiArray ) {
      cout << "-W- PndGemFindTracks::Init: No PndGemDigi array!" << endl;
      return kERROR;
    }
  }
  
  // Create and register GemTrack array
  fTrackArray = new TClonesArray("PndTrackCand",100);
  ioman->Register("GEMTrackCand", "Gem Track Candidates", fTrackArray, kTRUE);
  
  // Call the Init method of the track finder
  fFinder->Init();
  
  return kSUCCESS;  
}
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void PndGemFindTracks::SetParContainers() {
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("PndGeoPassivePar");
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndGemFindTracks::Exec(Option_t* opt) {
  fTrackArray->Delete();
  
  fNofTracks = fFinder->DoFind(fGemHitOrDigiArray, fTrackArray);
  
  
  //for (Int_t iTrack=0; iTrack<fTrackArray->GetEntriesFast(); iTrack++) {
  //PndGemTrack* track = (PndGemTrack*) fTrackArray->At(iTrack);
  //track->SortHits();
  //}
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void PndGemFindTracks::Finish() {
  fTrackArray->Clear();
}
// -------------------------------------------------------------------------




ClassImp(PndGemFindTracks)
