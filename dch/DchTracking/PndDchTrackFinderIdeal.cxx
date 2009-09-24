// -------------------------------------------------------------------------
// -----                PndDchTrackFinderIdeal source file             -----
// -----                       Created 27/01/08                        -----
// -----              according to the CbmStsTrackFinderIdeal          -----
// -------------------------------------------------------------------------

#include "PndDchTrackFinderIdeal.h"

// Pnd includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "PndDchPoint.h"
#include "FairRootManager.h"
#include "PndDetectorList.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"

// C++ includes
#include <iostream>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndDchTrackFinderIdeal::PndDchTrackFinderIdeal() {
	fMCTrackArray = NULL;
	fMCPointArray = NULL;
	fNofEvents    = 0;
}

// -----   Destructor   ----------------------------------------------------
PndDchTrackFinderIdeal::~PndDchTrackFinderIdeal() { }

void PndDchTrackFinderIdeal::Init() {

	// Get and check FairRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if( !ioman ) {
		cout << "-E- "<< GetName() <<"::Init: "
		<< "RootManager not instantised!" << endl;
		return;
	}

	// Get the pointer to the singleton FairRunAna object
	FairRunAna* ana = FairRunAna::Instance();
	if(NULL == ana) {
		cout << "-E- "<< GetName() <<"::Init :"
		<<" no FairRunAna object!" << endl;
		return;
	}
	// Get the pointer to run-time data base
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	if(NULL == rtdb) {
		cout << "-E- "<< GetName() <<"::Init :"
		<<" no runtime database!" << endl;
		return;
	}

	// Get MCTrack array
	fMCTrackArray  = (TClonesArray*) ioman->ActivateBranch("MCTrack");
	if( !fMCTrackArray ) {
		cout << "-E- "<< GetName() <<"::Init: No MCTrack array!"
		<< endl;
		return;
	}

	// Get PndDchPoint (MCPoint) array
	fMCPointArray  = (TClonesArray*) ioman->GetObject("PndDchPoint");
	if( !fMCPointArray ) {
		cout << "-E- "<< GetName() <<"::Init: No MCPoint array!"
		<< endl;
		return;
	}
	cout <<"fMCPointArray #: "<< fMCPointArray->GetEntriesFast() << endl;

	// Geometry loading
	//TFile *infile = ioman->GetInFile();
//	TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
	fDchStructure = PndDchStructure::Instance(gGeoManager);

	std::cout << "-I- "<< GetName() <<": Intialization successfull" << std::endl;
}

// -----   Public method DoFind   ------------------------------------------
Int_t PndDchTrackFinderIdeal::DoFind(TClonesArray* hitArray,
		TClonesArray* trackArray) {

	// Count events
	fNofEvents++;
	cout << endl << endl<< endl << endl;
	cout << "=======================================================" << endl;
	cout << "-I-        Event No: " <<  fNofEvents << endl;
	cout << "=======================================================" << endl;


	cout <<"-I- "<< GetName() <<"::DoFind "<< endl;
	cout << "-------------------------------------------------------" << endl;
	cout << "     ### Start DoFind" << endl;
	cout << "-------------------------------------------------------" << endl;

	// Check pointers
	if( !fMCTrackArray ) {
		cout << "-E- PndDchTrackFinderIdeal::DoFind: "
		<< "MCTrack array missing! " << endl;
		return -1;
	}

	if( !fMCPointArray ) {
		cout << "-E- "<< GetName() <<"::DoFind: "
		<< "MCPoint array missing! " << endl;
		return -1;
	}

	if( !hitArray ) {
		cout << "-E- "<< GetName() <<"::DoFind: "
		<< "Hit arrays missing! "<< endl;
		return -1;
	}

	// Initialise control counters
	Int_t nNoMCTrack   = 0;
	Int_t nNoTrack     = 0;
	Int_t nNoDchPoint  = 0;
	Int_t nNoDchHit    = 0;
	Int_t nDchHits6CH  = 0;

	// Create pointers to DchHit and DchPoint
	PndDchHit*   dchHit   = NULL;
	FairMCPoint*  mcPoint  = NULL;
	PndMCTrack*  mcTrack  = NULL;
	PndDchTrack* dchTrack = NULL;

	// Declare variables outside the loop
	Int_t ptIndex = 0;       // MC point index
	Int_t mcTrackIndex = 0;  // MC track index
	Int_t trackIndex = 0;    // Dch track index
	Int_t relDetID = 3000;   // 

	// Create STL map from MCTrack index to number of hits
	std::map<Int_t, Int_t> hitMap;
	std::map<Int_t, Int_t>::iterator itHitMap;

	// Size of fMCTrackArray
	cout <<"# MC Tracks: "<< fMCTrackArray->GetEntriesFast() << endl;
	// Size of fMCTrackArray
	cout <<"# MC Points: "<< fMCPointArray->GetEntriesFast() << endl;

	// Number of Dch hits
	Int_t nDchHits = hitArray->GetEntriesFast();
	if(fVerbose > 2) cout <<"# DchHits: "<< nDchHits << endl;

	for(Int_t iHit = 0; iHit < nDchHits; iHit++){
		// Get the pointer to Dch hit
		dchHit = (PndDchHit*) hitArray->At(iHit);
		if(dchHit->GetDetectorID() > relDetID) {
			if(NULL == dchHit ) continue;
			nDchHits6CH++;

			// Get point index
			ptIndex = dchHit->GetRefIndex();

			// Get pointer to MC point
			mcPoint = (FairMCPoint*) fMCPointArray->At(ptIndex);
			if(NULL == mcPoint) continue;

			// Get MC track index
			mcTrackIndex = mcPoint->GetTrackID();

			// Increment the number of hits
			hitMap[mcTrackIndex] += 1;
		}
	}


	// Create STL map from MCTrack index to DchTrack index
	map<Int_t, Int_t> trackMap;

	// Loop over reconstructable MCTracks and create corresponding DchTrack
	Int_t nMCacc = 0;  // Accepted MC tracks
	Int_t nTracks = 0; // Reconstructable MC tracks
	Int_t nMCTracks = fMCTrackArray->GetEntriesFast();

	if(fVerbose > 2) {
		cout <<"# of MC Tracks (nMCTracks): " << nMCTracks << endl;
	}

	for(Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++) {
		mcTrack = (PndMCTrack*) fMCTrackArray->At(iMCTrack);
		if( !mcTrack ) continue;


		// Either Primary or All MC tracks are considered
		if(fPrimary) {
			if(mcTrack->GetMotherID() != -1 && fabs((mcTrack->GetStartVertex()).Z())>1.) continue;

			if(fVerbose > 2) {
				cout <<"MC track #: "<< iMCTrack
				<< "\t MotherID: "<< mcTrack->GetMotherID()
				<< "\t StartVertex Z: "<< mcTrack->GetStartVertex().Z() << endl;
			}
		}
		//----------------------------------------------------
		nMCacc++;

		if(fVerbose > 2) {
			cout << iMCTrack << ": #DchPoints in MCTrack: "<< mcTrack->GetNPoints(kDCH) << endl;
		}

		new((*trackArray)[nTracks]) PndDchTrack();

		dchTrack = (PndDchTrack*) trackArray->At(nTracks);
		TLorentzVector tlVec = mcTrack->Get4Momentum();
		Double_t mom = tlVec.Mag();
		if(mom != 0) {
			dchTrack->GetParamLast()->SetQp(1./mom);
		} else {
			dchTrack->GetParamLast()->SetQp(0);
		}
		trackMap[iMCTrack] = nTracks;

		nTracks++;
	}
	
	// Loop over hits. Get corresponding MCPoint and MCTrack index
	for(Int_t iHit = 0; iHit < nDchHits; iHit++) {
		dchHit = (PndDchHit*) hitArray->At(iHit);
		if( !dchHit ) {
			nNoDchHit++;
			continue;
		}

		ptIndex = dchHit->GetRefIndex();
		if(ptIndex < 0) continue;           // fake or background hit
		mcPoint = (FairMCPoint*) fMCPointArray->At(ptIndex);

		if( !mcPoint ) {
			nNoDchPoint++;
			continue;
		}

		mcTrackIndex = mcPoint->GetTrackID();

		if(mcTrackIndex < 0 || mcTrackIndex > nMCTracks) {
			cout << "-E- "<< GetName() <<"::DoFind: "
			<< "MCTrack index out of range. " << mcTrackIndex << " "
			<< nMCTracks << endl;
			nNoMCTrack++;
			continue;
		}

		if(trackMap.find(mcTrackIndex) == trackMap.end()) continue;
		trackIndex = trackMap[mcTrackIndex];
		dchTrack = (PndDchTrack*) trackArray->At(trackIndex);

		if( !dchTrack ) {
			cout << "-E- "<< GetName() <<"::DoFind: "
			<< "No DchTrack pointer. " << iHit << " " << ptIndex
			<< " " << mcTrackIndex << " " << trackIndex << endl;
			nNoTrack++;
			continue;
		}

		if(dchHit->GetDetectorID() > relDetID) {// Only Dch chamber in forward spectrometer
			dchTrack->AddHit(dchHit, iHit);
		}
		
		if(fVerbose > 3) {
			cout << "DCH hit " << iHit << " from DCH point "
			<< ptIndex << " (" << mcTrackIndex << ") "
			<< "added to DCH track " << trackIndex << endl;
		}
	}

	// Sorting hits
	for(Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
		dchTrack = (PndDchTrack*) trackArray->At(iTrack);
		dchTrack->SortHits();
	}

	if(fVerbose) {
		cout << endl;
		cout << "-------------------------------------------------------" << endl;
		cout << "-I-      "<< GetName() <<": Event summary      -I-" << endl;
		cout << "-------------------------------------------------------" << endl;
		cout << "Total Dch hits:  " << nDchHits <<"   In 6CH: "<< nDchHits6CH << endl;
		cout << "MC tracks total: " << nMCTracks << ", accepted: "
		<< nMCacc << ", reconstructable: " << nTracks << endl;
		if(nNoDchHit)   cout << "DchHits not found   : " << nNoDchHit   << endl;
		if(nNoDchPoint) cout << "DchPoints not found : " << nNoDchPoint << endl;
		if(nNoMCTrack)  cout << "MCTracks not found  : " << nNoMCTrack  << endl;
		if(nNoTrack)    cout << "DchTracks not found : " << nNoTrack    << endl;
		cout << "------------------------------------------------------" << endl;
		cout << endl;
	} else {
		cout << "All: "        << nMCTracks
		<< ", Accepted: "      << nMCacc
		<< ", Reconstructed: " << nTracks << endl;
	}

	return nTracks;
}


ClassImp(PndDchTrackFinderIdeal)
