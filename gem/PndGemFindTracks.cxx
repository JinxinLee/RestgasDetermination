// -------------------------------------------------------------------------
// -----                   PndGemFindTracks source file                -----
// -----                  Created 19.03.2009  by R. Karabowicz         -----
// -----                 according to the PndDchFindTracks             -----
// -------------------------------------------------------------------------


// Pnd includes
#include "PndGemFindTracks.h"
#include "PndGemTrackFinder.h"
#include "PndGemDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

// ROOT includes
#include "TClonesArray.h"

#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"

#include <iomanip>

// c++ includes
using std::cout;
using std::endl;
using std::setw;

// c++ declaration
class iostream;

// Pnd declaration
class FairBaseParSet;
class PndGemHit;
class PndGemDigi;
class PndTrack;
class PndTrackCand;
//class PndTrackCandHit;

// -----   Default constructor   -------------------------------------------
PndGemFindTracks::PndGemFindTracks() {
  fDigiPar             = NULL;
  fFinder              = NULL;
  fGemHitOrDigiArray   = NULL;
  fTrackArray          = NULL;
  fTrackCandArray      = NULL;
  fNofTracks           = 0;
  fUseHitOrDigi        = "hit";

  fTTime      = 0.;
  fTNofTracks = 0;
  fTNofEvents = 0;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndGemFindTracks::PndGemFindTracks(const char* name, 
				   const char* title, 
				   PndGemTrackFinder* finder) 
  : FairTask(name) {
  fDigiPar             = NULL;
  fFinder              = finder;
  fGemHitOrDigiArray   = NULL;
  fTrackArray          = NULL;
  fTrackCandArray      = NULL;
  fNofTracks           = 0;
  fUseHitOrDigi        = "hit";

  fTTime      = 0.;
  fTNofTracks = 0;
  fTNofEvents = 0;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGemFindTracks::~PndGemFindTracks() {
  fTrackArray    ->Delete();
  fTrackCandArray->Delete();
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
  
  // Create and register PndTrack array
  fTrackArray = new TClonesArray("PndTrack",100);
  ioman->Register("GEMTrack", "Gem Tracks", fTrackArray, kTRUE);

  // Create and register PndTrackCand array
  fTrackCandArray = new TClonesArray("PndTrackCand",100);
  ioman->Register("GEMTrackCand", "Gem Track Cands", fTrackCandArray, kTRUE);
  
  // Call the Init method of the track finder
  fFinder->Init();

  cout << "-I- " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations." << endl;
  cout << "-I- " << fName.Data() << "::Init(). Initialization succesfull." << endl;
  
  return kSUCCESS;  
}
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void PndGemFindTracks::SetParContainers() {
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("PndGeoPassivePar");

  fDigiPar = (PndGemDigiPar*) rtdb->getContainer("PndGemDetectors");
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndGemFindTracks::Exec(Option_t*) {

  fTimer.Start();

  fTrackArray->Delete();
  fTrackCandArray->Delete();
  
  fNofTracks = fFinder->DoFind(fGemHitOrDigiArray, fTrackArray, fTrackCandArray);
  

  fTimer.Stop();
  fTTime      += fTimer.RealTime();
  fTNofTracks += fNofTracks;
  fTNofEvents += 1;
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void PndGemFindTracks::Finish() {
  fTrackArray->Delete();
  fTrackCandArray->Delete();

  cout << "-------------------- " << fName.Data() << " : Summary ---------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Tracks:     " << setw(10) << fTNofTracks << "    ( " << (Double_t)fTNofTracks/((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Time:       " << setw(10) << fTTime      << "    ( " << fTTime/((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                           ( " << fTTime/((Double_t)fTNofTracks) << " per track )" << endl;
  cout << "---------------------------------------------------------------------" << endl; 

  fFinder->Finish();
}
// -------------------------------------------------------------------------




ClassImp(PndGemFindTracks)
