// -------------------------------------------------------------------------
// -----                PndDchTrackFinderIdealDigi source file              -----
// -----                       Created 8/03/08                         -----
// -------------------------------------------------------------------------

#include "PndDchTrackFinderIdealDigi.h"

// Pnd includes
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmBaseParSet.h"
#include "PndDchPoint.h"
#include "PndDchDigi.h"
#include "CbmRootManager.h"

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
PndDchTrackFinderIdealDigi::PndDchTrackFinderIdealDigi() {
	fMCTrackArray = NULL;
	fMCPointArray = NULL;
	fNofEvents    = 0;
}

// -----   Destructor   ----------------------------------------------------
PndDchTrackFinderIdealDigi::~PndDchTrackFinderIdealDigi() { }

void PndDchTrackFinderIdealDigi::Init() {

	// Get and check CbmRootManager
	CbmRootManager* ioman = CbmRootManager::Instance();
	if( !ioman ) {
		cout << "-E- "<< GetName() <<"::Init: "
		<< "RootManager not instantised!" << endl;
		return;
	}

	// Get the pointer to the singleton CbmRunAna object
	CbmRunAna* ana = CbmRunAna::Instance();
	if(NULL == ana) {
		cout << "-E- "<< GetName() <<"::Init :"
		<<" no CbmRunAna object!" << endl;
		return;
	}
	
	// Get the pointer to run-time data base
	CbmRuntimeDb* rtdb = ana->GetRuntimeDb();
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
	TFile *infile = ioman->GetInFile();
	TGeoManager *geoMan = (TGeoManager*) infile->Get("CBMGeom");
	fDchStructure = PndDchStructure::Instance(geoMan);

	std::cout << "-I- "<< GetName() <<": Intialization successfull" << std::endl;
}

// -----   Public method DoFind   ------------------------------------------
Int_t PndDchTrackFinderIdealDigi::DoFind(TClonesArray* digiArray,
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
		cout << "-E- "<< GetName() <<"::DoFind: "
		<< "MCTrack array missing! " << endl;
		return -1;
	}

	if( !fMCPointArray ) {
		cout << "-E- "<< GetName() <<"::DoFind: "
		<< "MCPoint array missing! " << endl;
		return -1;
	}

	if( !digiArray ) {
		cout << "-E- "<< GetName() <<"::DoFind: "
		<< "Digi arrays missing! "<< endl;
		return -1;
	}

	// Initialise control counters
	Int_t nNoMCTrack   = 0;
	Int_t nNoTrack     = 0;
	Int_t nNoDchPoint  = 0;
	Int_t nNoDchDigi   = 0;
	Int_t nDchDigis6CH = 0;

	// Create pointers to DchDigi and DchPoint
	PndDchDigi*  dchDigi  = NULL;
	CbmMCPoint*  mcPoint  = NULL;
	CbmMCTrack*  mcTrack  = NULL;
	PndDchTrack* dchTrack = NULL;

	// Declare variables outside the loop
	Int_t ptIndex = 0;       // MC point index
	Int_t mcTrackIndex = 0;  // MC track index
	Int_t trackIndex = 0;    // Dch track index
	

	// Create STL map from MCTrack index to number of digis
	std::map<Int_t, Int_t> digiMap;
	std::map<Int_t, Int_t>::iterator itDigiMap;

	// Size of fMCTrackArray
	cout <<"# MC Tracks: "<< fMCTrackArray->GetEntriesFast() << endl;
	// Size of fMCTrackArray
	cout <<"# MC Points: "<< fMCPointArray->GetEntriesFast() << endl;

	// Number of Dch digis
	Int_t nDchDigis = digiArray->GetEntriesFast();
	if(fVerbose > 2) cout <<"# DchDigis: "<< nDchDigis << endl;

	for(Int_t iDigi = 0; iDigi < nDchDigis; iDigi++){
		// Get the pointer to Dch 
		dchDigi = (PndDchDigi*) digiArray->At(iDigi);
		if(dchDigi->GetChamber() > 2) {
			if(NULL == dchDigi ) continue;
			nDchDigis6CH++;
			
			// Get point index
			ptIndex = dchDigi->GetRefIndex();

			// Get pointer to MC point
			mcPoint = (CbmMCPoint*) fMCPointArray->At(ptIndex);
			if(NULL == mcPoint) continue;

			// Get MC track index
			mcTrackIndex = mcPoint->GetTrackID();

			// Increment the number of digis
			digiMap[mcTrackIndex] += 1;
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
		mcTrack = (CbmMCTrack*) fMCTrackArray->At(iMCTrack);
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
			cout << iMCTrack << ": #DchPoints in MCTrack: "<< mcTrack->GetDchPoints() << endl;
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
	
	// Loop over digis. Get corresponding MCPoint and MCTrack index
	for(Int_t iDigi = 0; iDigi < nDchDigis; iDigi++) {
		dchDigi = (PndDchDigi*) digiArray->At(iDigi);
		if( !dchDigi ) {
			nNoDchDigi++;
			continue;
		}

		ptIndex = dchDigi->GetRefIndex();

		if(ptIndex < 0) continue;           // fake or background digi
		mcPoint = (CbmMCPoint*) fMCPointArray->At(ptIndex);

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
			<< "No DchTrack pointer. " << iDigi << " " << ptIndex
			<< " " << mcTrackIndex << " " << trackIndex << endl;
			nNoTrack++;
			continue;
		}

		if(dchDigi->GetChamber() > 2) {// Only Dch chamber in forward spectrometer
			dchTrack->AddDigi(dchDigi, iDigi);
		}

		if(fVerbose > 3) {
			cout << "DCH digi " << iDigi << " from DCH point "
			<< ptIndex << " (" << mcTrackIndex << ") "
			<< "added to DCH track " << trackIndex << endl;
		}
	}

	// Sorting digis
	for(Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
		dchTrack = (PndDchTrack*) trackArray->At(iTrack);
		dchTrack->SortDigis();
	}
	
	if(fVerbose) {
		cout << endl;
		cout << "-------------------------------------------------------" << endl;
		cout << "-I-    "<< GetName() <<": Event summary    -I-" << endl;
		cout << "-------------------------------------------------------" << endl;
		cout << "Total Dch digis: " << nDchDigis <<"   In 6CH: "<< nDchDigis6CH << endl;
		cout << "MC tracks total: " << nMCTracks << ", accepted: "
		<< nMCacc << ", reconstructable: " << nTracks << endl;
		if(nNoDchDigi)  cout << "DchDigis not found  : " << nNoDchDigi  << endl;
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


ClassImp(PndDchTrackFinderIdealDigi)
