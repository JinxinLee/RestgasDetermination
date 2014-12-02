#include "PndFtsHoughTrackFinder.h"



ClassImp(PndFtsHoughTrackFinder);

const Double_t PndFtsHoughTrackFinder::meinpi = 3.14159265359;
const Double_t PndFtsHoughTrackFinder::fZLineParabola = 368.;
const Double_t PndFtsHoughTrackFinder::fZParabolaLine = 605.;

PndFtsHoughTrackFinder::PndFtsHoughTrackFinder(PndFtsHoughTrackerTask *trackerTask) :
															fTrackerTask(trackerTask),

															// min peak heights
															fMinPeakHeightZxLineParabola(6),
															fMinPeakHeightZxParabola(8),
															fMinPeakHeightZxParabolaLine(6),
															fMinPeakHeightZyLine(4),

															// general
															fSaveDebugInfo(kFALSE),
															fVerbose(0),

															// Hough spaces
															fHoughSpaceZxLineBeforeDipole(0),
															fHoughspaceZxParabola(0),
															fHoughSpaceZxLineBehindDipole(0),
															fHoughspaceZyLine(0)
{
	if (0==fTrackerTask){
		std::cout << "PndFtsHoughTrackFinder FATAL ERROR Tracker task not set.\n";
	} else {
		fVerbose = fTrackerTask->GetVerbose();
		if(3<fVerbose) std::cout << "PndFtsHoughTrackFinder called with tracker ptr " << fTrackerTask << '\n';
		fSaveDebugInfo = fTrackerTask->GetSaveDebugInfo();
	}
}


PndFtsHoughTrackFinder::~PndFtsHoughTrackFinder()
{
	if(3<fVerbose) fTrackerTask->fLogger->Info(MESSAGE_ORIGIN,"Destructor of PndFtsHoughTrackFinder");
}


std::vector<PndFtsHoughTracklet> PndFtsHoughTrackFinder::FindLineBehindDipoleZxTracklets() {
	// zx plane: Straight line Hough transform behind dipole
	static const Int_t stepsPerThetaDegLineBehindDipole = 2; // greater number means finer scanning in theta
	static const Int_t thetaDegLowLineBehindDipole = -89; // in degree
	static const Int_t thetaDegHighLineBehindDipole = 89; // in degree
	delete fHoughSpaceZxLineBehindDipole;
	fHoughSpaceZxLineBehindDipole = new PndFtsHoughSpace("lineBehindDipole",
			stepsPerThetaDegLineBehindDipole
			* (thetaDegHighLineBehindDipole
					- thetaDegLowLineBehindDipole),
					thetaDegLowLineBehindDipole / 180. * meinpi, // in rad
					thetaDegHighLineBehindDipole / 180. * meinpi, // in rad
					stepsPerThetaDegLineBehindDipole * 20, // TODO: Check values
					-200., // in cm // TODO: Check values
					200., // in cm
					fZParabolaLine, 0., fTrackerTask);
	fHoughSpaceZxLineBehindDipole->GetXaxis()->SetTitle("#theta [rad]");
	fHoughSpaceZxLineBehindDipole->GetYaxis()->SetTitle("x_{LP} [cm]");
	// Do straight line Hough transform on non-skewed hits from stations 1+2
	try {
		fHoughSpaceZxLineBehindDipole->FillHoughSpace();
		if (0 < fVerbose) {
			std::cout
			<< "Hough Space for zx line behind dipole was created successfully!\n"
			<< "We have " << fHoughSpaceZxLineBehindDipole->GetNHits()
			<< " hits in the line Hough space.\n";
		}
	} catch (std::runtime_error& e) {
		std::cerr
		<< "Hough Space for zx line behind dipole could not be created! \n";
		std::cerr << "runtime_error: " << e.what() << '\n';
	}
	// find peaks for line hough space and store in vector
	std::vector<PndFtsHoughTracklet> trackletsLineBehindDipole;
	// Call peak finder and plot the solution if it was found!
	if (kTRUE
			== fHoughSpaceZxLineBehindDipole->FindAllPeaks(
					fMinPeakHeightZxParabolaLine, trackletsLineBehindDipole)) {
		if (0 < fVerbose) {
			PrintFoundTracklets(trackletsLineBehindDipole,
					fHoughSpaceZxLineBehindDipole->GetName());
		}
		if (fTrackerTask->GetSaveDebugInfo()) {
			fTrackerTask->WriteHistogram(fHoughSpaceZxLineBehindDipole);
		}
	} else {
		std::cout
		<< "Error: Peak finder had a problem with hough space for zx line behind dipole!!!"
		<< '\n';
	}
	return trackletsLineBehindDipole;
}

std::vector<PndFtsHoughTracklet> PndFtsHoughTrackFinder::FindLineBeforeDipoleZxTracklets() {
	//----------------------------------
	// zx plane: Straight line Hough transform before dipole
	static const Int_t stepsPerThetaDegLineBeforeDipole = 4; // greater number means finer scanning in theta
	static const Int_t thetaDegLowLineBeforeDipole = -18; // in degree
	static const Int_t thetaDegHighLineBeforeDipole = 18; // in degree
	delete fHoughSpaceZxLineBeforeDipole;
	fHoughSpaceZxLineBeforeDipole = new PndFtsHoughSpace("lineBeforeDipole",
			stepsPerThetaDegLineBeforeDipole
			* (thetaDegHighLineBeforeDipole
					- thetaDegLowLineBeforeDipole),
					thetaDegLowLineBeforeDipole / 180. * meinpi, // in rad
					thetaDegHighLineBeforeDipole / 180. * meinpi, // in rad
					stepsPerThetaDegLineBeforeDipole * 16, // TODO: Check values
					-80., // in cm // TODO: Check values
					80., // in cm
					fZLineParabola, 0., fTrackerTask);
	fHoughSpaceZxLineBeforeDipole->GetXaxis()->SetTitle("#theta [rad]");
	fHoughSpaceZxLineBeforeDipole->GetYaxis()->SetTitle("x_{LP} [cm]");
	// Do straight line Hough transform on non-skewed hits from stations 1+2
	try {
		fHoughSpaceZxLineBeforeDipole->FillHoughSpace();
		if (0 < fVerbose) {
			std::cout
			<< "Hough Space for zx line before dipole was created successfully!"
			<< '\n' << "We have "
			<< fHoughSpaceZxLineBeforeDipole->GetNHits()
			<< " hits in the line Hough space.\n";
		}
		if (fTrackerTask->GetSaveDebugInfo()) {
			fTrackerTask->WriteHistogram(fHoughSpaceZxLineBeforeDipole);
		}
	} catch (std::runtime_error& e) {
		std::cerr
		<< "Hough Space for zx line before dipole could not be created! \n";
		std::cerr << "runtime_error: " << e.what() << '\n';
	}
	// find peaks for line Hough space and store in vector
	std::vector<PndFtsHoughTracklet> trackletsLineBeforeDipole;
	// Call peak finder and plot the solution if it was found!
	if (kTRUE
			== fHoughSpaceZxLineBeforeDipole->FindAllPeaks(
					fMinPeakHeightZxLineParabola, trackletsLineBeforeDipole)) {
		if (0 < fVerbose) {
			PrintFoundTracklets(trackletsLineBeforeDipole,
					fHoughSpaceZxLineBeforeDipole->GetName());
			fTrackerTask->WriteHistogram(fHoughSpaceZxLineBeforeDipole);
		}
	} else {
		std::cout
		<< "Error: Peak finder had a problem with Hough space for zx line before dipole!!!"
		<< '\n';
	}
	return trackletsLineBeforeDipole;
}

void PndFtsHoughTrackFinder::FindMatchingParabolaToLineBeforeDipoleZx(
		const std::vector<PndFtsHoughTracklet>& trackletsLineBeforeDipole,
		const std::vector<PndFtsHoughTracklet>& trackletsLineBehindDipole
) {
	// loop over all line tracklets which were found by line HT before dipole field and find a matching parabola
	// LB4D = line before dipole
	for (UInt_t iLB4D = 0; iLB4D < trackletsLineBeforeDipole.size(); ++iLB4D) {
		const Double_t peakThetaRadLB4D = trackletsLineBeforeDipole[iLB4D].getThetaRadVal();
		const Double_t peakInterceptLB4D = trackletsLineBeforeDipole[iLB4D].getSecondVal();
		const Double_t peakThetaRadHwLB4D = trackletsLineBeforeDipole[iLB4D].getThetaRadHw();

		// determine where to look for parabola
		const Int_t stepsPerThetaDegParabola = 10; // greater number means finer scanning in theta
		// TODO: Rework this
		const Double_t thetaRadLowParabola = peakThetaRadLB4D
				- peakThetaRadHwLB4D; // in rad
		const Double_t thetaRadHighParabola = peakThetaRadLB4D
				+ peakThetaRadHwLB4D; // in rad
		const Double_t thetaDegLowParabola = thetaRadLowParabola / meinpi * 180.; // in deg
		const Double_t thetaDegHighParabola = thetaRadHighParabola / meinpi * 180.; // in deg
		if (thetaRadHighParabola == thetaRadLowParabola) std::cout << "ERROR: low and high are the same for parabola!\n";

		UInt_t thetaBins = ceil(
				stepsPerThetaDegParabola
				* (thetaDegHighParabola - thetaDegLowParabola));

		std::cout << "event: " << fTrackerTask->GetEventNr()
						<< "\nthetaDegLowParabola=" << thetaDegLowParabola
						<< " thetaDegHighParabola=" << thetaDegHighParabola
						<< " thetaBins=" << thetaBins
						<< "  peakThetaDegLineBeforeDipole="
						<< peakThetaRadLB4D / meinpi * 180.
						<< " peakThetaDegHwLineBeforeDipole="
						<< peakThetaRadHwLB4D / meinpi * 180. << '\n';

		delete fHoughspaceZxParabola;
		fHoughspaceZxParabola = new PndFtsHoughSpace("parabola", thetaBins,
				thetaRadLowParabola, // in rad
				thetaRadHighParabola, // in rad
				stepsPerThetaDegParabola * 300, // 300 is good as factor
				-0.015, // a.u.
				0.015, // a.u.
				fZLineParabola, peakInterceptLB4D, fTrackerTask);
		fHoughspaceZxParabola->GetXaxis()->SetTitle("#theta [rad]");
		fHoughspaceZxParabola->GetYaxis()->SetTitle("#frac{Q}{p_zx} [a.u.]");

		// Do parabola Hough transform for current line before dipole (shifts FTS hits by hitshiftinx) for non-skewed hits in stations 3+4+5
		try {
			fHoughspaceZxParabola->FillHoughSpace();
			if (0 < fVerbose) {
				std::cout << "Line " << iLB4D
						<< ": Hough Space for zx parabola was created successfully!\n"
						<< "We have " << fHoughspaceZxParabola->GetNHits()
						<< " hits in the line Hough space.\n";
			}
			if (fTrackerTask->GetSaveDebugInfo()) {
				fTrackerTask->WriteHistogram(fHoughspaceZxParabola,iLB4D);
			}
		} catch (std::runtime_error& e) {
			std::cerr << "Hough Space for zx parabola could not be created! \n";
			std::cerr << "runtime_error: " << e.what() << '\n';
		}

		// find peaks for zx parabola Hough space (for current line before dipole)
		std::vector<PndFtsHoughTracklet> zxParabolaTracklets;
		if (kTRUE
				== fHoughspaceZxParabola->FindAllPeaks(fMinPeakHeightZxParabola,
						zxParabolaTracklets)) {
			if (0 < fVerbose) {
				PrintFoundTracklets(zxParabolaTracklets,
						fHoughspaceZxParabola->GetName());
			}
		} else {
			std::cout << "Error: Peak finder had a problem for parabola!!!"
					<< '\n';
		}

		if (1 < fVerbose) {
			std::cout
			<< "Create track candidates from line in zx before dipole "
			<< iLB4D << " and all parabolas within dipole.\n"
			<< "And add all lines which match line+parabola\n";
		}



		//----------------------------
		//At this point I have track candidates from line in zx before dipole matched to parabolas within the dipole: fHoughTrackCands
		// I also have line in zx after dipole field: trackletsLineBehindDipole
		// Now match them based on angle theta to z axis at z = fZParabolaLine
		// MatchLineAfterDipoleZx

		// HIER GEHT ES WEITER!!!






		// construct newHoughTrackCand from line+parabola+line (in zx)
		for (UInt_t iParabola = 0; iParabola < zxParabolaTracklets.size(); ++iParabola) {
			PndFtsHoughTrackCand newHoughTrackCand(fTrackerTask);
			newHoughTrackCand.SetZxLineBeforeDipole(trackletsLineBeforeDipole[iLB4D]);
			newHoughTrackCand.SetZxParabola(zxParabolaTracklets[iParabola]);
			fHoughTrackCands.push_back(newHoughTrackCand);
		}
	} // loop over all line tracklets which were found by line HT before dipole field
}

//void PndFtsHoughTrackFinder::CreatePndTrackCands() {
//
//}


void PndFtsHoughTrackFinder::FindTracks() {

	if (0<fVerbose) {
		std::cout << "PndFtsHoughTrackFinder::FindTracks()\n";
	}

	// reset
	fHoughTrackCands.clear();
	fHoughTrackCandsNew.clear();


	//--------------------------
	// Do we have enough hits in the FTS?
	if( fMinPeakHeightZxParabola > fTrackerTask->GetNFtsHits() ) {
		if(0<fVerbose) fTrackerTask->fLogger->Info(MESSAGE_ORIGIN,"Skip the event, since we have too few hits in FTS");
		return;
	}




	// zx plane: Straight line Hough transform behind dipole
	std::vector<PndFtsHoughTracklet> trackletsLineBehindDipole = FindLineBehindDipoleZxTracklets();


	// zx plane: Straight line Hough transform before dipole
	std::vector<PndFtsHoughTracklet> trackletsLineBeforeDipole = FindLineBeforeDipoleZxTracklets();


	// loop over all line tracklets which were found by line HT before dipole field and find a matching parabola
	FindMatchingParabolaToLineBeforeDipoleZx(trackletsLineBeforeDipole, trackletsLineBehindDipole);






	// TODO: Rewrite the following conversion
	// Get momenta from arbitrary units to meaningful ones
	//		pzinvpeak = 1./pzinvpeak*0.00299792458;
	//	pzinvpeakWithBField = pzinvpeak/BMeanForParabola;
















	//	if (0<fSaveDebugInfo){
	//		WriteHistograms();
	//	}


	if(0<fVerbose) {

		// TODO Add missing output




		//		if (kTRUE == correctpz)
		//		{
		//			std::cout << "Watch out! p_z value in plot is already corrected!!!"  << std::endl;
		//		}
	}


	delete fHoughSpaceZxLineBeforeDipole;
	delete fHoughspaceZxParabola;
	delete fHoughSpaceZxLineBehindDipole;
	delete fHoughspaceZyLine;

}









Bool_t PndFtsHoughTrackFinder::FilterTrackletsBasedOnSharedHits(
		UInt_t maxAcceptableSharedHits,
		std::vector<PndFtsHoughTracklet> &tracklets
)
{
	// to store indices that I want to "delete"
	std::set<UInt_t> indicesToDelete;
	std::set<UInt_t>::iterator findIndex;

	// for all tracklets that share more than maxAcceptableSharedHits hits keep only the heighest peak
	// compare all tracklets with each other, keep track of which tracklets should be "deleted"
	for (UInt_t iTrackletLeft = 0; iTrackletLeft < tracklets.size(); ++iTrackletLeft)
	{
		for (UInt_t iTrackletRight = iTrackletLeft+1; iTrackletRight < tracklets.size(); ++iTrackletRight)
		{
			// check if two tracks share more than maxAcceptableSharedHits hits
			PndFtsHoughTracklet trackletLeft = tracklets[iTrackletLeft];
			PndFtsHoughTracklet trackletRight = tracklets[iTrackletRight];
			const UInt_t nSharedHits = trackletLeft.getNSharedHits(trackletRight);
			if (nSharedHits>maxAcceptableSharedHits){
				// mark the peak with the lower "height" for deletion
				const Double_t heightLeft = trackletLeft.getPeakHeightFromPeakFinder();
				const Double_t heightRight = trackletRight.getPeakHeightFromPeakFinder();
				if (heightLeft>heightRight){
					indicesToDelete.insert(iTrackletRight);
				} else if (heightLeft<heightRight){
					indicesToDelete.insert(iTrackletLeft);
				} else if (heightLeft==heightRight){
					std::cout << "WARNING: Found two peaks of the same height that share " <<  nSharedHits << " hits.\n";
					std::cout << "The max. number of shared hits was set to " <<  maxAcceptableSharedHits << " hits.\n";
					std::cout << "Both peaks / tracklets will be kept. \n";
				}
			}

		}
	} // end for loop: compare all tracklets with each other

	// if we have no tracklets to "delete", we don't need to do anything
	if (0==indicesToDelete.size()){
		if (0<fVerbose) {  std::cout << "FilterTrackletsBasedOnSharedHits: No tracklets are marked for deletion.\n"; }
		return kTRUE;
	}

	// create new tracklets vector and copy all entries from input vector to it which are not marked for deletion
	std::vector<PndFtsHoughTracklet> filteredTracklets;
	for (UInt_t iTracklet = 0; iTracklet < tracklets.size(); ++iTracklet)
	{
		// check if the index is marked for "deletion"
		findIndex = indicesToDelete.find(iTracklet);
		if (findIndex == indicesToDelete.end()){
			// index was not found -> entry is not marked for deletion -> I should copy it
			filteredTracklets.push_back(tracklets[iTracklet]);
		}
	}
	// replace input tracklets with filtered ones
	tracklets = filteredTracklets;
	return kTRUE;
}


