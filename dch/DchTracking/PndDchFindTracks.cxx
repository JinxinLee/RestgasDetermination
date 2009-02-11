// -------------------------------------------------------------------------
// -----                   PndDchFindTracks source file                -----
// -----                  Created 27/01/08  by A. Bubak               -----
// -----                 according to the CbmStsFindTracks             -----
// -------------------------------------------------------------------------


// Pnd includes
#include "PndDchFindTracks.h"
#include "PndDchTrackFinder.h"

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
class PndDchHit;
class PndDchCylinderHit;
class PndDchDigi;
class PndDchTrack;

// -----   Default constructor   -------------------------------------------
PndDchFindTracks::PndDchFindTracks() {
	fFinder         	 = NULL;
	fDchHitArray         = NULL;
	fDchCylinderHitArray = NULL;
	fDchDigiArray        = NULL;
	fTrackArray          = NULL;
	fNofTracks           = 0;
	fUseHitOrDigi        = "hit";
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndDchFindTracks::PndDchFindTracks(const char* name, 
		const char* title, 
		PndDchTrackFinder* finder) 
: FairTask(name) {
	fFinder              = finder;
	fDchHitArray         = NULL;
	fDchCylinderHitArray = NULL;
	fDchDigiArray        = NULL;
	fTrackArray          = NULL;
	fNofTracks           = 0;
	fUseHitOrDigi        = "hit";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDchFindTracks::~PndDchFindTracks() {
	fTrackArray->Delete();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndDchFindTracks::Init() {

	// Check for Track finder
	if( !fFinder ) {
		cout << "-W- PndDchFindTracks::Init: No track finder selected!" << endl;
		return kERROR;
	}

	// Get and check FairRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if( !ioman ) {
		cout << "-E- PndDchFindTracks::Init: "
		<< "RootManager not instantised!" << endl;
		return kFATAL;
	}

	if("hit" == fUseHitOrDigi) {
		cout << "-I- PndDchFindTracks::Init: Works on hits!" << endl;

		// Get Dch hit Array
		fDchHitArray  = (TClonesArray*) ioman->GetObject("PndDchHit");
		if ( !fDchHitArray ) {
			cout << "-W- PndDchFindTracks::Init: No PndDchHit array!" << endl;
			return kERROR;
		}
		fDchHitOrDigiArray = fDchHitArray;
	}

	if("chit" == fUseHitOrDigi) {
		cout << "-I- PndDchFindTracks::Init: Works on cylinder hits!" << endl;

		// Get Dch cylinder hit Array
		fDchCylinderHitArray  = (TClonesArray*) ioman->GetObject("PndDchCylinderHit");
		if ( !fDchCylinderHitArray ) {
			cout << "-W- PndDchFindTracks::Init: No PndDchCylinderHit array!" << endl;
			return kERROR;
		}
		fDchHitOrDigiArray = fDchCylinderHitArray;
	}
	
	if("digi" == fUseHitOrDigi) {
		cout << "-I- PndDchFindTracks::Init: Works on digis!" << endl;

		// Get Dch digi Array
		fDchDigiArray  = (TClonesArray*) ioman->GetObject("PndDchDigi");
		if ( !fDchDigiArray ) {
			cout << "-W- PndDchFindTracks::Init: No PndDchDigi array!" << endl;
			return kERROR;
		}
		fDchHitOrDigiArray = fDchDigiArray;
	}

	// Create and register DchTrack array
	fTrackArray = new TClonesArray("PndDchTrack",100);
	ioman->Register("PndDchTrack", "Dch", fTrackArray, kTRUE);

	// Call the Init method of the track finder
	fFinder->Init();

	return kSUCCESS;  
}
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void PndDchFindTracks::SetParContainers() {
	FairRunAna* ana = FairRunAna::Instance();
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	rtdb->getContainer("FairBaseParSet");
	rtdb->getContainer("PndGeoPassivePar");
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndDchFindTracks::Exec(Option_t* opt) {
	fTrackArray->Delete();
	//cout <<"PndDchFindTracks::Exec  fDchHitArray # "<< fDchHitArray->GetEntriesFast()  << endl;
	//cout <<"PndDchFindTracks::Exec  fMCTrackArray # "<< fMCTrackArray->GetEntriesFast()  << endl;

	fNofTracks = fFinder->DoFind(fDchHitOrDigiArray, fTrackArray);


	//for (Int_t iTrack=0; iTrack<fTrackArray->GetEntriesFast(); iTrack++) {
		//PndDchTrack* track = (PndDchTrack*) fTrackArray->At(iTrack);
		//track->SortHits();
	//}
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void PndDchFindTracks::Finish() {
	fTrackArray->Clear();
}
// -------------------------------------------------------------------------




ClassImp(PndDchFindTracks)
