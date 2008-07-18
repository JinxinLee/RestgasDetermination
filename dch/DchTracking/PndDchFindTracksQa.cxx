// -------------------------------------------------------------------------
// -----                  PndDchFindTracksQa source file               -----
// -------------------------------------------------------------------------


// Pnd includes
#include "PndDchFindTracksQa.h"
#include "PndDchTrack.h"
#include "PndDchTrackMatch.h"

#include "CbmMCTrack.h"
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

// Root includes
#include "TClonesArray.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TList.h"
#include "TVector3.h"

// C++ includes
#include <iostream>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndDchFindTracksQa::PndDchFindTracksQa()
: CbmTask("PndDchFindTracksQa") {
	fQuota        = 0.7;
	fVerbose      = 1;
	fMinPoints    = 18;
	fUseHitOrDigi = "hit";
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDchFindTracksQa::PndDchFindTracksQa(Double_t quota, Int_t minPoints, TString useHitOrDigi, Int_t verbose)
: CbmTask("PndDchFindTracksQa") {
	fQuota        = quota;
	fVerbose      = verbose;
	fMinPoints    = minPoints;
	fUseHitOrDigi = useHitOrDigi;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDchFindTracksQa::~PndDchFindTracksQa() { 
	fHistoList->Delete();
	delete fHistoList;
}
// -------------------------------------------------------------------------




// -----   Public method Init   --------------------------------------------
InitStatus PndDchFindTracksQa::Init() {

	// Get and check CbmRootManager
	CbmRootManager* ioman = CbmRootManager::Instance();
	if( !ioman ) {
		cout << "-E- "<< GetName() <<"::Init: "
		<< "RootManager not instantised!" << endl;
		return kFATAL;
	}

	// Get the pointer to the singleton CbmRunAna object
	CbmRunAna* ana = CbmRunAna::Instance();
	if(NULL == ana) {
		cout << "-E- "<< GetName() <<"::Init :"
		<<" no CbmRunAna object!" << endl;
		return kFATAL;
	}

	// Get MCTrack array
	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if( !fMCTracks ) {
		cout << "-E- "<< GetName() <<"::Init: No MCTrack array!" << endl;
		return kFATAL;
	}

	// Get DchTrack array
	fTracks = (TClonesArray*) ioman->GetObject("PndDchTrack");
	if( !fTracks ) {
		cout << "-E- "<< GetName() <<"::Init: No DchTrack array!" << endl;
		return kERROR;
	}

	// Get DchTrackMatch array
	fMatches = (TClonesArray*) ioman->GetObject("PndDchTrackMatch");
	if( !fMatches ) {
		cout << "-E- "<< GetName() <<"::Init: No DchTrackMatch array!"
		<< endl;
		return kERROR;
	}

	// Create histograms
	CreateHistos();
	Reset();

	// Output
	cout << " Matching quota   : " << fQuota     << endl;
	cout << " Number of points : " << fMinPoints << endl;

	return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndDchFindTracksQa::Exec(Option_t* opt) {

	cout << "-------------------------------------------------------" << endl;
	cout << "-I-      "<< GetName() <<": Exec                   -I-" << endl;
	cout << "-------------------------------------------------------" << endl;

	// Clear matching map and reset eventwise counters
	fMatchMap.clear();
	fQualiMap.clear();

	// Loop over DchTracks. Check matched MCtrack and fill maps.
	Int_t nGhosts = 0;
	Int_t nClones = 0;
	Int_t nRec = fTracks->GetEntriesFast();
	Int_t nMtc = fMatches->GetEntriesFast();

	if( nMtc != nRec ) {
		cout << "-E- "<< GetName() <<"::Exec: Number of DchMatches ("
		<< nMtc << ") does not equal number of DchTracks ("
		<< nRec << ")" << endl;
		Fatal("Exec", "Inequal number of DchTrack and DchTrackMatch");
	}

	for(Int_t iRec = 0; iRec < nRec; iRec++) {

		PndDchTrack* dchTrack = (PndDchTrack*) fTracks->At(iRec);
		if( !dchTrack ) {
			cout << "-E- "<< GetName() <<"::Exec: "
			<< "No DchTrack at index " << iRec << endl;
			Fatal("Exec", "No DchTrack in array");
		}

		Int_t nHords;
		if("digi" == fUseHitOrDigi) nHords = dchTrack->GetNofDchDigis();
		if("hit"  == fUseHitOrDigi) nHords = dchTrack->GetNofDchHits();
		if("chit" == fUseHitOrDigi) nHords = dchTrack->GetNofDchCylinderHits();

		PndDchTrackMatch* match = (PndDchTrackMatch*) fMatches->At(iRec);

		if( !match ) {
			cout << "-E- "<< GetName() <<"::Exec: "
			<< "No DchTrackMatch at index " << iRec << endl;
			Fatal("Exec", "No DchTrackMatch in array");
		}

		Int_t nTrue = match->GetNofTrueHorDs();

		Int_t iMC = match->GetMCTrackID();
		if(iMC == -1) { // no common point with MC
			if(fVerbose > 1)
				cout << "-I- "<< GetName() <<"::Exec: "
				<< "No MC match for DchTrack " << iRec << endl;
			fhNhGhosts->Fill(nHords);
			nGhosts++;
			continue;
		}

		// --- Check matching criterion (quota)---------------------------------
		Double_t quali = Double_t(nTrue) / Double_t(nHords);
		if(quali >= fQuota) {

			// No previous match for this MCTrack
			if(fMatchMap.find(iMC) == fMatchMap.end()) {
				fMatchMap[iMC] = iRec;
				fQualiMap[iMC] = quali;
			}
			// Previous match; take the better one
			else {
				if(fVerbose > 1)
					cout << "-I- "<< GetName() <<"::Exec: "
					<< "MCTrack " << iMC << " doubly matched."
					<< "Current match " << iRec
					<< ", previous match " << fMatchMap[iMC]
					                                    << endl;
				if(fQualiMap[iMC] < quali) {
					PndDchTrack* oldTrack = (PndDchTrack*) fTracks->At(fMatchMap[iMC]);

					if("digi" == fUseHitOrDigi) fhNhClones->Fill(Double_t(oldTrack->GetNofDchDigis()));
					if("hit"  == fUseHitOrDigi) fhNhClones->Fill(Double_t(oldTrack->GetNofDchHits()));
					if("chit" == fUseHitOrDigi) fhNhClones->Fill(Double_t(oldTrack->GetNofDchCylinderHits()));
					fMatchMap[iMC] = iRec;
					fQualiMap[iMC] = quali;
				}
				else fhNhClones->Fill(nHords);
				nClones++;
			}

		}

		// If not matched, it's a ghost
		else {
			if(fVerbose > 1)
				cout << "-I- "<< GetName() <<"::Exec: "
				<< "DchTrack " << iRec << " below matching criterion "
				<< "(" << quali << ")" << endl;
			fhNhGhosts->Fill(nHords);
			nGhosts++;
		}

	}   // Loop over DchTracks


	// Loop over MCTracks
	Int_t nAll     = 0;    // Number of ALL tracks
	Int_t nAcc     = 0;    // Number of ACCEPTED tracks
	Int_t nRecAll  = 0;    // Number of RECONSTRUCTES tracks
	Int_t nPrim    = 0;    // Number of PRIMARY tracks
	Int_t nRecPrim = 0;    // Number of RECONSTRUCTED (among primaries) tracks
	Int_t nRef     = 0;    // Number of REFERENCES tracks
	Int_t nRecRef  = 0;    // Number of RECONSTRUCTED (among references) tracks
	Int_t nSec     = 0;    // Number of SECONDARY tracks
	Int_t nRecSec  = 0;    // Number of RECONSTRUCTED (among secondaries) tracks

	if(fVerbose > 1) {
		cout << endl << "Definition:" << endl;
		cout <<"---------------------------------------------------------------------"<< endl; 
		cout <<"   ALL                  :  All mc tracks"<< endl;
		cout <<"   Primary tracks       :  tracks from vertex.Z < 1 and motherID equal to -1"<< endl;
		cout <<"   Secondary tracks     :  non-primary traks"<< endl;
		cout <<"   REFERENCES tracks    :  mc tracks momentum  > 1 && is Primary tracks"<< endl;
		cout <<"   ACCEPTED tracks      :  those which have number of points > fMinPoints"<< endl;
		cout <<"                             e.g.: 18"<< endl;
		cout <<"   RECONSTRUCTED tracks :  Accepted && number of points with"<< endl;
		cout << "                            the same track ID is > fQuota (e.g. 70%)"<< endl;
		cout <<"---------------------------------------------------------------------"<< endl << endl; 
	}

	TVector3 vertex(0,0,0);
	Int_t motherID = 0;
	Int_t nMC = fMCTracks->GetEntriesFast();
	for(Int_t iMC = 0; iMC < nMC; iMC++) {
		CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMC);

		if( !mcTrack ) {
			cout << "-E- "<< GetName() <<"::Exec: "
			<< "No MCTrack at index " << iMC << endl;
			Fatal("Exec", "No MCTrack in array");
		}

		// --- Check geometrical acceptance; continue only for accepted tracks
		nAll++;

		Int_t nPoints = mcTrack->GetDchPoints();
		if(nPoints < fMinPoints) continue;
		nAcc++;

		// --- Check origin of MCTrack ----------------------------
		vertex = mcTrack->GetStartVertex();
		motherID = mcTrack->GetMotherID(); 
		Bool_t isPrim = kFALSE;

		if(fVerbose > 2) {
			cout <<"MotherID: "         << motherID
			<<", PdgCode: "        << mcTrack->GetPdgCode()
			<<", vertex(x,y,z): (" << vertex.X() <<", "<< vertex.Y() <<", "<< vertex.Z() <<")"
			<<", startTime: "	    << mcTrack->GetStartTime() << " [ns]"
			<< endl;		
		}


		// --- Check if primary -----------------------------------
		if(motherID == -1 && fabs(vertex.Z()<1.)) {
			isPrim = kTRUE;
			nPrim++;
		}
		else nSec++;

		// --- Get momentum ---------------------------------------
		Double_t mom = mcTrack->GetMomentum().Mag();
		Bool_t isRef = kFALSE;
		// Count for references tracks
		if(mom > 1. && isPrim) {
			isRef = kTRUE;
			nRef++;
		}

		// --- Fill histograms for accepted tracks ----------------
		fhMomAccAll->Fill(mom);
		fhNpAccAll->Fill(Double_t(nPoints));
		if(isPrim) {
			fhMomAccPrim->Fill(mom);
			fhNpAccPrim->Fill(Double_t(nPoints));
		} else {
			fhMomAccSec->Fill(mom);
			fhNpAccSec->Fill(Double_t(nPoints));
			fhZAccSec->Fill(vertex.Z());
		}

		// --- Get matched DchTrack --------------------------------
		Int_t    iRec  = -1;
		Double_t quali =  0.;
		Bool_t   isRec = kFALSE;
		if(fMatchMap.find(iMC) != fMatchMap.end()) {
			iRec  = fMatchMap[iMC];
			isRec = kTRUE;
			PndDchTrack* dchTrack = (PndDchTrack*) fTracks->At(iRec);
			if( !dchTrack ) {
				cout << "-E- "<< GetName() <<"::Exec: "
				<< "No DchTrack for matched MCTrack " << iMC << endl;
				Fatal("Exec", "No DchTrack for matched MCTrack");
			}
			quali = fQualiMap[iMC];
			if(quali < fQuota) {
				cout << "-E- "<< GetName() <<"::Exec: "
				<< "Matched DchTrack " << iRec << " is below matching "
				<< "criterion ( " << quali << ")" << endl;
				Fatal("Exec", "Match below matching quota");
			}
			PndDchTrackMatch* match = (PndDchTrackMatch*) fMatches->At(iRec);
			if( !match ) {
				cout << "-E- "<< GetName() <<"::Exec: "
				<< "No DchTrackMatch for matched MCTrack " << iMC << endl;
				Fatal("Exec", "No DchTrackMatch for matched MCTrack");
			}
			Int_t nTrue  = match->GetNofTrueHorDs();
			Int_t nWrong = match->GetNofWrongHorDs();
			Int_t nFake  = match->GetNofFakeHorDs();
			Int_t nHords = 0;
			if("digi" == fUseHitOrDigi) nHords = dchTrack->GetNofDchDigis();
			if("hit"  == fUseHitOrDigi) nHords = dchTrack->GetNofDchHits();
			if("chit" == fUseHitOrDigi) nHords = dchTrack->GetNofDchCylinderHits();

			if(nTrue + nWrong + nFake != nHords) {
				cout << "True " << nTrue << " wrong " << nWrong << " Fake "
				<< nFake << " Hords " << nHords << endl;
				Fatal("Exec", "Wrong number of Hords");
			}

			if(fVerbose > 1) {
				cout << "MCTrack "     << iMC
				<< ", points "    << nPoints
				<< ", DchTrack "  << iRec
				<< ", Hords "      << nHords
				<< ", true Hords " << nTrue << endl;
			}

			// --- Fill histograms for reconstructed tracks ---------
			nRecAll++;
			fhMomRecAll->Fill(mom);
			fhNpRecAll->Fill(Double_t(nPoints));
			if(isPrim) {
				nRecPrim++;
				fhMomRecPrim->Fill(mom);
				fhNpRecPrim->Fill(Double_t(nPoints));
				if ( isRef ) nRecRef++;
			} else {
				nRecSec++;
				fhMomRecSec->Fill(mom);
				fhNpRecSec->Fill(Double_t(nPoints));
				fhZRecSec->Fill(vertex.Z());
			}
		}  // Match found in map?
	} // Loop over MCTracks


	// Calculate efficiencies
	Double_t effAll;
	if(nAcc != 0) effAll = Double_t(nRecAll)  / Double_t(nAcc);
	else effAll = 0;

	Double_t effPrim;
	if(nPrim != 0) effPrim = Double_t(nRecPrim)  / Double_t(nPrim);
	else effPrim = 0;

	Double_t effRef;
	if(nRef != 0) effRef = Double_t(nRecRef)  / Double_t(nRef);
	else effRef = 0;	

	Double_t effSec;
	if(nSec != 0) effSec = Double_t(nRecSec)  / Double_t(nSec);
	else effSec = 0;

	Double_t effGhosts;
	if(nAcc != 0) effGhosts = Double_t(nGhosts)  / Double_t(nAcc);
	else effGhosts = 0;

	Double_t effClones;
	if(nAcc != 0) effClones = Double_t(nClones)  / Double_t(nAcc);
	else effClones = 0;

	// Event summary
	if(fVerbose > 0) {

		cout << endl << endl;
		cout << "-------------------------------------------------------" << endl;
		cout << "-I-      "<< GetName() <<": Event summary          -I-" << endl;
		cout << "-------------------------------------------------------" << endl;
		cout << endl;
		cout << "MCTracks   : " << nAll << ", accepted: " << nAcc
		<< ", reconstructed: " << nRecAll << endl;
		cout << "All        : accepted: " << nAcc
		<< ", reconstructed: "        << nRecAll
		<< ", efficiency "     		  << effAll*100. << "%" << endl;

		cout << "Vertex     : accepted: " << nPrim
		<< ", reconstructed: "       << nRecPrim
		<< ", efficiency "           << effPrim*100. << "%" << endl;

		cout << "Reference  : accepted: " << nRef
		<< ", reconstructed: "       << nRecRef
		<< ", efficiency "           << effRef*100. << "%" << endl;

		cout << "Non-vertex : accepted: " << nSec
		<< ", reconstructed: "       << nRecSec
		<< ", efficiency "           << effSec*100. << "%" << endl;

		cout << "Ghosts : " << nGhosts
		<< ", ghosts/accepted MC tracks: " << effGhosts*100. << "%" << endl;

		cout << "Clones : " << nClones
		<< ", clones/accepted MC tracks: " << effClones*100. << "%" << endl;

		cout << "DchTracks " << nRec
		<< ", ghosts " << nGhosts
		<< ", clones " << nClones << endl;
		cout << "-----------------------------------------------------------" << endl << endl;
	}else {
		cout << "All: "        << effAll*100.
		<< " %, Primary: "     << effPrim*100.
		<< " %, References: "  << effRef*100. << " %" << endl;
	}

	// Increase counters
	fNAccAll  += nAcc;
	fNAccPrim += nPrim;
	fNAccRef  += nRef;
	fNAccSec  += nSec;
	fNRecAll  += nRecAll;
	fNRecPrim += nRecPrim;
	fNRecRef  += nRecRef;
	fNRecSec  += nRecSec;
	fNGhosts  += nGhosts;
	fNClones  += nClones;
	fNofEvents++;
}
// -------------------------------------------------------------------------



// -----   Private method Finish   -----------------------------------------
void PndDchFindTracksQa::Finish() {

	// Divide histograms for efficiency calculation
	DivideHistos(fhMomRecAll,  fhMomAccAll,  fhMomEffAll);
	DivideHistos(fhMomRecPrim, fhMomAccPrim, fhMomEffPrim);
	DivideHistos(fhMomRecSec,  fhMomAccSec,  fhMomEffSec);
	DivideHistos(fhNpRecAll,   fhNpAccAll,   fhNpEffAll);
	DivideHistos(fhNpRecPrim,  fhNpAccPrim,  fhNpEffPrim);
	DivideHistos(fhNpRecSec,   fhNpAccSec,   fhNpEffSec);
	DivideHistos(fhZRecSec,    fhZAccSec,    fhZEffSec);

	// Normalise histos for clones and ghosts to one event
	if(fNofEvents) {
		fhNhClones->Scale(1./Double_t(fNofEvents));
		fhNhGhosts->Scale(1./Double_t(fNofEvents));
	}

	// --- Calculate integrated efficiencies and rates -------------------

	Double_t effAll;
	if(fNAccAll != 0) effAll = Double_t(fNRecAll)  / Double_t(fNAccAll);
	else effAll = 0;

	Double_t effPrim;
	if(fNAccPrim != 0) effPrim = Double_t(fNRecPrim)  / Double_t(fNAccPrim);
	else effPrim = 0;

	Double_t effRef;
	if(fNAccRef != 0) effRef = Double_t(fNRecRef)  / Double_t(fNAccRef);
	else effRef = 0;	

	Double_t effSec;
	if(fNAccSec != 0) effSec = Double_t(fNRecSec)  / Double_t(fNAccSec);
	else effSec = 0;

	Double_t effGhosts;
	if(fNAccAll != 0) effGhosts = Double_t(fNGhosts)  / Double_t(fNAccAll);
	else effGhosts = 0;

	Double_t effClones;
	if(fNAccAll != 0) effClones = Double_t(fNClones)  / Double_t(fNAccAll);
	else effClones = 0;

	Double_t rateGhosts = Double_t(fNGhosts) / Double_t(fNofEvents);
	Double_t rateClones = Double_t(fNClones) / Double_t(fNofEvents);



	// --- Run summary to screen --------------------------------
	cout << endl << endl;
	cout << "=======================================================" << endl;
	cout << "-I-  "<< GetName() <<"::Finish Run summary         -I-" << endl;
	cout << "=======================================================" << endl;
	cout << "Events analyzed : " << fNofEvents << endl;
	cout << "-------------------------" << endl;
	cout << "Efficiency all tracks       : " << effAll*100 << " % (" 
	<< fNRecAll << "/" << fNAccAll <<")" << endl;
	cout << "Efficiency vertex tracks    : " << effPrim*100 << " % (" 
	<< fNRecPrim << "/" << fNAccPrim <<")" << endl;
	cout << "Efficiency reference tracks : " << effRef*100 << " % (" 
	<< fNRecRef << "/" << fNAccRef <<")" << endl;
	cout << "Efficiency secondary tracks : " << effSec*100 << " % (" 
	<< fNRecSec << "/" << fNAccSec <<")" << endl;
	cout << "Ghost rate " << rateGhosts << "/event, "
	<< "ghosts/acc. MC tracks :" << effGhosts*100 << "% ("
	<< fNGhosts << "/" << fNAccAll << ")" << endl;
	cout << "Clone rate " << rateClones << "/event, "
	<< "clones/acc. MC tracks :" << effClones*100 << "% ("
	<< fNClones << "/" << fNAccAll << ")" << endl;
	cout << "=======================================================" << endl;

	TDirectory *current = gDirectory;
	TDirectory *hdir = current->mkdir("DchFinderQa");
	hdir->cd();
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
	current->cd();

}
// -------------------------------------------------------------------------



// -----   Private method CreateHistos   -----------------------------------
void PndDchFindTracksQa::CreateHistos() {

	// Histogram list
	fHistoList = new TList();

	// Momentum distributions
	Double_t minMom   =  0.;
	Double_t maxMom   = 10.;
	Int_t    nBinsMom = 50;

	fhMomAccAll  = new TH1F("hMomAccAll", "Momentum: All accepted tracks",
			nBinsMom, minMom, maxMom);
	fhMomRecAll  = new TH1F("hMomRecAll", "Momentum: All reconstructed tracks",
			nBinsMom, minMom, maxMom);
	fhMomEffAll  = new TH1F("hMomEffAll", "Efficiency all tracks",
			nBinsMom, minMom, maxMom);

	fhMomAccPrim = new TH1F("hMomAccPrim", "Momentum: Accepted vertex tracks",
			nBinsMom, minMom, maxMom);
	fhMomRecPrim = new TH1F("hMomRecPrim", "Momentum: Reconstructed vertex tracks",
			nBinsMom, minMom, maxMom);
	fhMomEffPrim = new TH1F("hMomEffPrim", "Efficiency vertex tracks",
			nBinsMom, minMom, maxMom);

	fhMomAccSec  = new TH1F("hMomAccSec", "Momentum: Accepted non-vertex tracks",
			nBinsMom, minMom, maxMom);
	fhMomRecSec  = new TH1F("hMomRecSec", "Momentum: Reconstructed non-vertex tracks",
			nBinsMom, minMom, maxMom);
	fhMomEffSec  = new TH1F("hMomEffSec", "Efficiency non-vertex tracks",
			nBinsMom, minMom, maxMom);

	fHistoList->Add(fhMomAccAll);
	fHistoList->Add(fhMomRecAll);
	fHistoList->Add(fhMomEffAll);
	fHistoList->Add(fhMomAccPrim);
	fHistoList->Add(fhMomRecPrim);
	fHistoList->Add(fhMomEffPrim);
	fHistoList->Add(fhMomAccSec);
	fHistoList->Add(fhMomRecSec);
	fHistoList->Add(fhMomEffSec);


	// Number-of-points distributions
	Double_t minNp   = -0.5;
	Double_t maxNp   = 100.5;
	Int_t    nBinsNp = 101;

	fhNpAccAll  = new TH1F("hNpAccAll", "No of hords: All accepted tracks",
			nBinsNp, minNp, maxNp);
	fhNpRecAll  = new TH1F("hNpRecAll", "No of hords: All reconstructed tracks",
			nBinsNp, minNp, maxNp);
	fhNpEffAll  = new TH1F("hNpEffAll", "No of hords: Efficiency all tracks",
			nBinsNp, minNp, maxNp);

	fhNpAccPrim = new TH1F("hNpAccPrim", "No of hords: Accepted vertex tracks",
			nBinsNp, minNp, maxNp);
	fhNpRecPrim = new TH1F("hNpRecPrim", "No of hords: Reconstructed vertex tracks",
			nBinsNp, minNp, maxNp);
	fhNpEffPrim = new TH1F("hNpEffPrim", "No of hords: Efficiency vertex tracks",
			nBinsNp, minNp, maxNp);

	fhNpAccSec  = new TH1F("hNpAccSec", "No of hords: Accepted non-vertex tracks",
			nBinsNp, minNp, maxNp);
	fhNpRecSec  = new TH1F("hNpRecSec", "No of hords: Reconstructed non-vertex tracks",
			nBinsNp, minNp, maxNp);
	fhNpEffSec  = new TH1F("hNpEffSec", "No of hords: efficiency non-vertex tracks",
			nBinsNp, minNp, maxNp);

	fHistoList->Add(fhNpAccAll);
	fHistoList->Add(fhNpRecAll);
	fHistoList->Add(fhNpEffAll);
	fHistoList->Add(fhNpAccPrim);
	fHistoList->Add(fhNpRecPrim);
	fHistoList->Add(fhNpEffPrim);
	fHistoList->Add(fhNpAccSec);
	fHistoList->Add(fhNpRecSec);
	fHistoList->Add(fhNpEffSec);

	// z(vertex) distributions
	Double_t minZ    =  0.;
	Double_t maxZ    = 50.;
	Int_t    nBinsZ  = 50;

	fhZAccSec = new TH1F("hZAccSec",  "Z(vertex) distr.: Accepted non-vertex tracks",
			nBinsZ, minZ, maxZ);
	fhZRecSec = new TH1F("hZRecSecl", "Z(vertex) distr.: Reconstructed non-vertex tracks",
			nBinsZ, minZ, maxZ);
	fhZEffSec = new TH1F("hZEffRec",  "Z(vertex) distr.: Efficiency non-vertex tracks",
			nBinsZ, minZ, maxZ);
	fHistoList->Add(fhZAccSec);
	fHistoList->Add(fhZRecSec);
	fHistoList->Add(fhZEffSec);

	// Number-of-hit distributions
	fhNhClones  = new TH1F("hNhClones", "Number of hords for clones",
			nBinsNp, minNp, maxNp);
	fhNhGhosts  = new TH1F("hNhGhosts", "Number of hords for ghosts",
			nBinsNp, minNp, maxNp);
	fHistoList->Add(fhNhClones);
	fHistoList->Add(fhNhGhosts);

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void PndDchFindTracksQa::Reset() {

	TIter next(fHistoList);
	while(TH1* histo = ((TH1*)next())) histo->Reset();

	fNAccAll = fNAccPrim = fNAccRef = fNAccSec = 0;
	fNRecAll = fNRecPrim = fNRecRef = fNRecSec = 0;
	fNGhosts = fNClones = fNofEvents = 0;

}
// -------------------------------------------------------------------------



// -----   Private method DivideHistos   -----------------------------------
void PndDchFindTracksQa::DivideHistos(TH1* histo1, TH1* histo2, TH1* histo3) {

	if( !histo1 || !histo2 || !histo3 ) {
		cout << "-E- "<< GetName() <<"::DivideHistos: "
		<< "NULL histogram pointer" << endl;
		Fatal("DivideHistos", "Null histo pointer");
	}

	Int_t nBins = histo1->GetNbinsX();
	if(histo2->GetNbinsX() != nBins || histo3->GetNbinsX() != nBins) {
		cout << "-E- "<< GetName() <<"::DivideHistos: "
		<< "Different bin numbers in histos" << endl;
		cout << histo1->GetName() << " " << histo1->GetNbinsX() << endl;
		cout << histo2->GetName() << " " << histo2->GetNbinsX() << endl;
		cout << histo3->GetName() << " " << histo3->GetNbinsX() << endl;
		return;
	}

	Double_t c1, c2, c3, ce;
	for(Int_t iBin = 0; iBin < nBins; iBin++) {
		c1 = histo1->GetBinContent(iBin);
		c2 = histo2->GetBinContent(iBin);
		if(c2) {
			c3 = c1 / c2;
			ce = TMath::Sqrt( c3 * ( 1. - c3 ) / c2 );
		} else {
			c3 = 0.;
			ce = 0.;
		}
		histo3->SetBinContent(iBin, c3);
		histo3->SetBinError(iBin, ce);
	}

}
// -------------------------------------------------------------------------



ClassImp(PndDchFindTracksQa)
