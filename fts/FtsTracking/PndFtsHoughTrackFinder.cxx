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
					stepsPerThetaDegLineBehindDipole * 40, // TODO: Check values
					-200., // in cm // TODO: Check values
					200., // in cm
					fZParabolaLine, 0., 0, fTrackerTask);

	// Do straight line Hough transform on non-skewed hits from stations 1+2
	try {
		fHoughSpaceZxLineBehindDipole->FillHoughSpace();
    if (fTrackerTask->GetSaveDebugInfo()) {
      fHoughSpaceZxLineBehindDipole->GetXaxis()->SetTitle("#theta [rad]");
      fHoughSpaceZxLineBehindDipole->GetYaxis()->SetTitle("x_{LP} [cm]");
      fTrackerTask->WriteHistogram(fHoughSpaceZxLineBehindDipole);
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
	} else {
		std::cout
		<< "Error: Peak finder had a problem with Hough space for zx line behind dipole!!!\n";
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
					fZLineParabola, 0., 0, fTrackerTask);

	// Do straight line Hough transform on non-skewed hits from stations 1+2
	try {
		fHoughSpaceZxLineBeforeDipole->FillHoughSpace();
		if (fTrackerTask->GetSaveDebugInfo()) {
			fHoughSpaceZxLineBeforeDipole->GetXaxis()->SetTitle("#theta [rad]");
			fHoughSpaceZxLineBeforeDipole->GetYaxis()->SetTitle("x_{LP} [cm]");
			fTrackerTask->WriteHistogram(fHoughSpaceZxLineBeforeDipole);
		}
	} catch (std::runtime_error& e) {
		std::cerr
		<< "Hough Space could not be created! \n";
		std::cerr << "runtime_error: " << e.what() << '\n';
	}
	// find peaks for line Hough space and store in vector
	std::vector<PndFtsHoughTracklet> trackletsLineBeforeDipole;
	// Call peak finder and plot the solution if it was found!
	if (kTRUE == fHoughSpaceZxLineBeforeDipole->FindAllPeaks(
					fMinPeakHeightZxLineParabola, trackletsLineBeforeDipole)) {
		if (0 < fVerbose) {
			PrintFoundTracklets(trackletsLineBeforeDipole,
					fHoughSpaceZxLineBeforeDipole->GetName());
		}
	} else {
		std::cout
		<< "Error: Peak finder had a problem with Hough space for zx line before dipole!!!\n";
	}
	return trackletsLineBeforeDipole;
}

void PndFtsHoughTrackFinder::FindMatchingParabolaToLineBeforeDipoleZxAndAddLineBehindDipole(
		const std::vector<PndFtsHoughTracklet>& linesBeforeDipole,
		const std::vector<PndFtsHoughTracklet>& linesBehindDipole
) {
	// loop over all line tracklets which were found by line HT before dipole field and find a matching parabola
	// After that try to match a line after dipole to that track candidate.
	// If that is not possible, accept short track candidate if particle is likely to have left detector acceptance, otherwise discard.
	// LB4D = line before dipole

	// loop over lines before dipole
	for (UInt_t iLB4D = 0; iLB4D < linesBeforeDipole.size(); ++iLB4D) {
		const Double_t peakThetaRadLB4D = linesBeforeDipole[iLB4D].getThetaRadVal();
		const Double_t peakInterceptLB4D = linesBeforeDipole[iLB4D].getSecondVal();
		const Double_t peakThetaRadHwLB4D = linesBeforeDipole[iLB4D].getThetaRadHw();

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

		if (0 < fVerbose) std::cout << "event: " << fTrackerTask->GetEventNr()
      << "Line " << iLB4D
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
				fZLineParabola, peakInterceptLB4D, 0, fTrackerTask);

		// Do parabola Hough transform for current line before dipole (shifts FTS hits by hitshiftinx) for non-skewed hits in stations 3+4+5
		try {
			fHoughspaceZxParabola->FillHoughSpace();
			if (fTrackerTask->GetSaveDebugInfo()) {
				fHoughspaceZxParabola->GetXaxis()->SetTitle("#theta [rad]");
				fHoughspaceZxParabola->GetYaxis()->SetTitle("#frac{Q}{p_zx} [a.u.]");
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
			std::cout << "Error: Peak finder had a problem for parabola!!!\n";
		}




		// construct newHoughTrackCand from line+parabola+line (in zx)
		if (1 < fVerbose) {
			std::cout
			<< "Create track candidates from line in zx before dipole "
			<< iLB4D << " and all parabolas within dipole.\n"
			<< "And all lines behind dipole which match line+parabola (if exist)\n";
		}
		// loop over all parabolas which were found for one line before the dipole
		for (UInt_t iParabola = 0; iParabola < zxParabolaTracklets.size(); ++iParabola) {
			// Try to find a matching line behind the dipole

			PndFtsHoughTrackCand lineParabola(fTrackerTask);
			lineParabola.SetZxLineBeforeDipole(linesBeforeDipole[iLB4D]);
			lineParabola.SetZxParabola(zxParabolaTracklets[iParabola]);

			// loop over all lines behind dipole
			// add track cands for each matching line behind dipole (LBhD)
			Bool_t foundLineBehindDipole = kFALSE;
			for (UInt_t iLBhD = 0; iLBhD < linesBehindDipole.size(); ++iLBhD) {
				if (kTRUE == LineBehindDipoleMatchesToLinePlusParabola( lineParabola,  linesBehindDipole[iLBhD]) ){
					PndFtsHoughTrackCand lineParabolaLine(lineParabola); // make a copy
					lineParabolaLine.SetZxLineBehindDipole(linesBehindDipole[iLBhD]); // add line behind dipole
					fHoughTrackCandsZxPlaneOnly.push_back(lineParabolaLine); // add as track candidate
					foundLineBehindDipole = kTRUE;
				}
			}

			// iif no lines behind dipole matched, add track cand without any line behind dipole
			if (kFALSE == foundLineBehindDipole) fHoughTrackCandsZxPlaneOnly.push_back(lineParabola);
		}
	} // loop over all line tracklets which were found by line HT before dipole field
}

//void PndFtsHoughTrackFinder::CreatePndTrackCands() {
//
//}


void PndFtsHoughTrackFinder::FindTracks() {

	if (0<fVerbose) std::cout << "PndFtsHoughTrackFinder::FindTracks()\n";

	// reset
	fHoughTrackCandsZxPlaneOnly.clear();
	fHoughTrackCandsComplete.clear();


	//--------------------------
	// Do we have enough hits in the FTS?
	if( fMinPeakHeightZxParabola > fTrackerTask->GetNFtsHits() ) {
		if(0<fVerbose) fTrackerTask->fLogger->Info(MESSAGE_ORIGIN,"Skip the event, since we have too few hits in FTS");
		return;
	}




	// zx plane: Straight line Hough transform behind dipole
	std::cout << "Lines behind dipole:\n";
	std::vector<PndFtsHoughTracklet> linesBehindDipole = FindLineBehindDipoleZxTracklets();


	// zx plane: Straight line Hough transform before dipole
	std::cout << "Lines before dipole:\n";
	std::vector<PndFtsHoughTracklet> linesBeforeDipole = FindLineBeforeDipoleZxTracklets();


	// loop over all line tracklets which were found by line HT before dipole field and find a matching parabola
	std::cout << "Matching parabolas to lines before dipole:\n";
	FindMatchingParabolaToLineBeforeDipoleZxAndAddLineBehindDipole(linesBeforeDipole, linesBehindDipole);






	// loop over line+parabola+line track candidates in zx plane, pass track cand. to Hough space (one Hough transform needed for each line+parabola+line track cand.
	// Check all skewed hits whether they might belong to any of the track candidates
	// Calculate (x,z) coordinate hypotheses for skewed hits which might belong to track candidates and run line Hough transform on these hypotheses



	//----------------------------------
	// zy plane: Straight line Hough transform
	// loop over line+parabola+line from zx plane
	std::cout << "Lines in zy plane:\n";
	PndFtsHoughSpace* houghspaceZyLine = 0;
	for (UInt_t iLPL = 0; iLPL < fHoughTrackCandsZxPlaneOnly.size(); ++iLPL) {
		// determine where to look for line in zy plane
		static const Int_t stepsPerThetaDegZyLine = 4; // greater number means finer scanning in theta
		static const Int_t thetaDegLowZyLine = -18; // in degree
		static const Int_t thetaDegHighZyLine = 18; // in degree

		// create Hough space
		delete houghspaceZyLine;
		houghspaceZyLine = new PndFtsHoughSpace("lineZy",
				stepsPerThetaDegZyLine
				* (thetaDegHighZyLine
						- thetaDegLowZyLine),
						thetaDegLowZyLine / 180. * meinpi, // in rad
						thetaDegHighZyLine / 180. * meinpi, // in rad
						stepsPerThetaDegZyLine * 16, // TODO: Check values
						-80., // in cm // TODO: Check values
						80., // in cm
						fZLineParabola, 0., &(fHoughTrackCandsZxPlaneOnly[iLPL]), fTrackerTask);


		// Do line Hough transform for current line+parabola+line for skewed hits in all stations
		try {
			houghspaceZyLine->FillHoughSpace();
			if (fTrackerTask->GetSaveDebugInfo()) {
				houghspaceZyLine->GetXaxis()->SetTitle("#theta [rad]");
				houghspaceZyLine->GetYaxis()->SetTitle("y [cm]");
				fTrackerTask->WriteHistogram(houghspaceZyLine, iLPL);
			}
		} catch (std::runtime_error& e) {
			std::cerr
			<< "Hough Space for zy line before dipole could not be created! \n";
			std::cerr << "runtime_error: " << e.what() << '\n';
		}
		// find peaks for line Hough space and store in vector
		std::vector<PndFtsHoughTracklet> trackletsZyLine;
		// Call peak finder and plot the solution if it was found!
		if (kTRUE
				== houghspaceZyLine->FindAllPeaks(
						fMinPeakHeightZyLine, trackletsZyLine)) {
			if (0 < fVerbose) {
				PrintFoundTracklets(trackletsZyLine,
						houghspaceZyLine->GetName());
			}
		} else {
			std::cout
			<< "Error: Peak finder had a problem with Hough space for zy line!!!"
			<< '\n';
		}



		// construct newHoughTrackCand by adding zy line to line+parabola+line (in zx)
		if (1 < fVerbose) {
			std::cout
			<< "Create track candidates by adding line in zy to line+parabola+line in zx "
			<< iLPL <<"\n"
			<< "If no zy line can be found, delete track candidate.\n";
		}
		// loop over all zy lines which were found for line+parabola+line
		for (UInt_t iZyLine = 0; iZyLine < trackletsZyLine.size(); ++iZyLine) {
			PndFtsHoughTrackCand fullTrackCand(fHoughTrackCandsZxPlaneOnly[iLPL]); // make copy
			fullTrackCand.SetZyLine(trackletsZyLine[iZyLine]); // add zy line
			fHoughTrackCandsComplete.push_back(fullTrackCand); // add as track candidate
		}
	} // loop over all line+parabola+line from zx plane










	// HIER GEHT ES WEITER!



	// TODO: Convert momenta to physical units
	//	pzinvpeak = 1./pzinvpeak*0.00299792458;
	//	pzinvpeakWithBField = pzinvpeak/BMeanForParabola;


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
