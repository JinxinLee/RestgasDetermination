#include "PndFtsHoughTrackFinder.h"

#include <iostream>
#include <math.h>
#include <set>

// FTS
#include "PndGeoFtsPar.h"
#include "PndFtsMapCreator.h"
#include "PndFtsHit.h"
#include "FairHit.h"

// magnetic field
#include "FairField.h"
#include "TVector3.h"

// (Hough) tracking
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "PndFtsHoughTrackCand.h"

// histogramming / plotting
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

// peak finder
#include "TSpectrum2.h"

// root IO
#include "TClonesArray.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"



#include "PndFtsHit.h"
#include "TString.h"
#include "FairHit.h"


#include "PndFtsTube.h"
#include "FairEventHeader.h"

#include "TMath.h"

#include <iostream>
#include <math.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

ClassImp(PndFtsHoughTrackFinder)
;

TString PndFtsHoughTrackFinder::peakfinderOption="allPeaks>minHeight"; // see FindAllPeaks for available options


PndFtsHoughTrackFinder::PndFtsHoughTrackFinder()
{
	if(3<fVerbose) Info("PndFtsHoughTrackFinder","Default Constructor of PndFtsHoughTrackFinder");
	Initialization_ClassVariables();
}


PndFtsHoughTrackFinder::~PndFtsHoughTrackFinder()
{
	if(3<fVerbose) Info(MESSAGE_ORIGIN,"Destructor of PndFtsHoughTrackFinder");
}

void PndFtsHoughTrackFinder::Initialization_ClassVariables()
{
	// in case of multiple constructors this method is useful
	// Hough stuff
	fHoughspaceZxLineParabola=0;
	fHoughspaceZxParabola=0;
	fHoughspaceZxParabolaLine=0;
	fHoughspaceZyLine=0;


	// general
	fSaveDebugInfo=kFALSE;
	fVerbose = 0;


	// arrays
	//	fFtsParameters=0;
	//	fFtsTubeArray=0;
	fFtsHitArray=0;
	fField=0;
	//	fFtsBranchID=0;
	//	fTracksArrayName="FTSTrkHough";
	//	SetTrackOutput();
	//	fTrackCands = new TClonesArray("PndTrackCand");
	//	fTracks = new TClonesArray("PndTrack");
}


void PndFtsHoughTrackFinder::WriteHistograms(){
	// TODO: Change this to save all Hough spaces (for each peak found in first line I get a parabola Hough space)
	// BUG: This does not work
	//	TFile* file = FairRootManager::Instance()->GetOutFile();
	//	if (0==file)
	//	{
	//		std::cout << "Cannot get outfile.\n";
	//	}
	//	else
	//	{
	//		file->cd();
	//		file->mkdir("PndFtsHoughTrackFinder");
	//		file->cd("PndFtsHoughTrackFinder");
	//		std::cout << "Worked.\n";
	//		//		if (0!=fHoughspaceZxLineParabola)
	//		//		{
	//		//			fHoughspaceZxLineParabola->Write();
	//		//		}
	//		//		if (0!=fHoughspaceZxParabola)
	//		//		{
	//		//			fHoughspaceZxParabola->Write();
	//		//		}if (0!=fHoughspaceZxParabolaLine)
	//		//		{
	//		//			fHoughspaceZxParabolaLine->Write();
	//		//		}if (0!=fHoughspaceZyLine)
	//		//		{
	//		//			fHoughspaceZyLine->Write();
	//		//		}
	//		file->Close();
	//		delete file;
	//	}
}


void PndFtsHoughTrackFinder::SetHits(TClonesArray* hits, Int_t branchId) {
	fFtsHitArray = hits;
	fFtsBranchID = branchId;
}

//void PndFtsHoughTrackFinder::CreatePndTrackCands() {
//
//}


void PndFtsHoughTrackFinder::FindTracks() {

	if (0<fVerbose) {
		cout << "PndFtsHoughTrackFinder::FindTracks()" << endl;
	}

	// reset
	fHoughTrackCands.clear();
	fHoughTrackCandsNew.clear();








	// Do we have hits in the FTS?
	if(4 > fFtsHitArray->GetEntriesFast()) {
		if(0<fVerbose) Info("Exec","Skip the event, since we have too few hits in FTS");
		return;
	}







	// get back how many hits are in the Hough space
	UInt_t nHitsForHoughSpaceLine=0;


	delete fHoughspaceZxLineParabola;
	fHoughspaceZxLineParabola = new TH2F("houghspaceFirstLine", "houghspaceFirstLine", invthetastepForLine*(thetalimithighForLineHoughSpacePlot-thetalimitlowForLineHoughSpacePlot), thetalimitlowForLineHoughSpacePlot, thetalimithighForLineHoughSpacePlot, invthetastepForLine*40, -50., 50.);
	fHoughspaceZxLineParabola->GetXaxis()->SetTitle("#theta [^{0}]");
	fHoughspaceZxLineParabola->GetYaxis()->SetTitle("x_{LP} [cm]");



	// Do straight line hough transform on non-skewed hits from stations 1+2
	if (kTRUE == MakeHoughSpace("lineBeforeDipole", onlyUseHitsFromzForLine, onlyUseHitsUpTozForLine, kTRUE, thetalimitlowForLine, thetalimithighForLine, thetastepForLine, 0., nHitsForHoughSpaceLine, fHoughspaceZxLineParabola))
	{
		if (0<fVerbose) {
			std::cout << "Hough Space for Line was created successfully!" << std::endl
					<< "We have " << nHitsForHoughSpaceLine << " hits in the line Hough space.\n";

		}
	}
	else
	{
		std::cout << "Hough Space for Line could not be created! " << std::endl;
	}



	// find peaks for line hough space
	const Int_t minHeightZxLineParabola = 6;

	std::vector<PndFtsHoughTracklet> zxLineParabolaTracklets;


	// Call peak finder and plot the solution if it was found!
	//	Bool_t PndFtsHoughTrackFinder::FindAllPeaks(
	//			TString option,
	//			TH2F *houghspace,
	//			const UInt_t minHeight,
	//			std::vector<PndFtsHoughTrackCand> &tracklets
	//	)
	if (kTRUE == FindAllPeaks(peakfinderOption, fHoughspaceZxLineParabola, minHeightZxLineParabola, zxLineParabolaTracklets))
	{
		if (0<fVerbose) {
			std::cout << zxLineParabolaTracklets.size() << " peaks found for line " << std::endl; // TODO Add missing output
			if (10<fVerbose)
			{
				for (UInt_t i=0; i< zxLineParabolaTracklets.size(); ++i)
				{
					zxLineParabolaTracklets[i].Print();
				}
			}
		}
	}
	else
	{
		std::cout << "Error: Peak finder had a problem with hough space for line!!!" << std::endl;
	}




	// loop over all track candidates which were found by line HT before dipole field

	for (UInt_t iTrackCandLine=0; iTrackCandLine < zxLineParabolaTracklets.size(); ++iTrackCandLine)
	{
		// determine where to look for parabola
		const Int_t invthetastepForParabola = 10;
		const Double_t thetastepForParabola = 1.0/invthetastepForParabola; // Je kleiner, umso feiner wird in theta-Richtung gescannt
		const Double_t peakThetaZxLineParabola = zxLineParabolaTracklets[iTrackCandLine].getThetaVal();
		const Int_t thetalimitlowForParabolaHoughSpacePlot = floor(peakThetaZxLineParabola-0.3)-0;
		const Int_t thetalimithighForParabolaHoughSpacePlot = ceil(peakThetaZxLineParabola+0.3)+0;
		const Double_t thetalimitlowForParabola = thetalimitlowForParabolaHoughSpacePlot+thetastepForParabola/2.0; // Search in theta from this value (in degree)
		const Double_t thetalimithighForParabola = thetalimithighForParabolaHoughSpacePlot+thetastepForParabola/2.0; // Search in theta up to this value (in degree)

		delete fHoughspaceZxParabola;
		fHoughspaceZxParabola= new TH2F("houghspaceParabola", "houghspaceParabola", invthetastepForParabola*(thetalimithighForParabolaHoughSpacePlot-thetalimitlowForParabolaHoughSpacePlot), thetalimitlowForParabolaHoughSpacePlot, thetalimithighForParabolaHoughSpacePlot, invthetastepForParabola*300, -0.015, 0.015); // 300 is good as factor
		fHoughspaceZxParabola->GetXaxis()->SetTitle("#theta [^{0}]");
		fHoughspaceZxParabola->GetYaxis()->SetTitle("#frac{Q}{p_zx} [a.u.]");

		// get back how many hits are in the Hough space
		UInt_t nHitsForHoughSpaceParabola=0;
		Double_t peakInterceptZxLineParabola = zxLineParabolaTracklets[iTrackCandLine].getSecondVal();


		// Do parabola hough transform (shifts FTS hits by hitshiftinx) for non-skewed hits in stations 3+4+5
		if (kTRUE == MakeHoughSpace("parabola", onlyusehitsfromzForParabola, onlyusehitsuptozForParabola, kTRUE, thetalimitlowForParabola, thetalimithighForParabola, thetastepForParabola, peakInterceptZxLineParabola, nHitsForHoughSpaceParabola, fHoughspaceZxParabola))
		{
			if (0<fVerbose) {
				std::cout << "Hough Space for Parabola was created successfully!" << std::endl
						<< "We have " << nHitsForHoughSpaceParabola << " hits in the line Hough space.\n";
			}
		}
		else
		{
			std::cout << "Hough Space for parabola could not be created! " << std::endl;
		}



		// Get momenta from arbitrary units to meaningful ones
		//		pzinvpeak = 1./pzinvpeak*0.00299792458;
		//	pzinvpeakWithBField = pzinvpeak/BMeanForParabola;


		// find peaks for line hough space
		const Int_t minHeightZxParabola = 8;
		std::vector<PndFtsHoughTracklet> zxParabolaTracklets;

		// Call peak finder and plot the solution if it was found!

		if (kTRUE == FindAllPeaks(peakfinderOption, fHoughspaceZxParabola, minHeightZxParabola, zxParabolaTracklets))
		{
			if (0<fVerbose)
			{
				std::cout << zxParabolaTracklets.size() << " peaks found for parabola \n"; // TODO Add missing output
				if (10<fVerbose)
				{
					for (UInt_t i=0; i< zxParabolaTracklets.size(); ++i)
					{
						zxParabolaTracklets[i].Print();
					}
				}
			}

		}
		else
		{
			std::cout << "Error: Peak finder had a problem for parabola!!!" << std::endl;
		}

		if(1<fVerbose) {
			std::cout << "Create track candidates from line in zx before dipole "<< iTrackCandLine << " and all parabolas within dipole." << std::endl;
		}
		for (UInt_t iTrackCandParabola=0; iTrackCandParabola < zxParabolaTracklets.size(); ++iTrackCandParabola)
		{
			PndFtsHoughTrackCand newHoughTrackCand;
			newHoughTrackCand.SetZxFirstLine(zxLineParabolaTracklets[iTrackCandLine]);
			newHoughTrackCand.SetZxParabola(zxParabolaTracklets[iTrackCandParabola]);
			fHoughTrackCands.push_back(newHoughTrackCand);
		}

	} // loop over all track candidates which were found by line HT before dipole field




	std::cout << "At this point I have track candidates from line in zx before dipole and parabola within. Add line after dipole field in zx" << std::endl;
	// TODO




	//	if (0<fSaveDebugInfo){
	//		WriteHistograms();
	//	}


	if(0<fVerbose) {

		// TODO Add missing output




		if (kTRUE == correctpz)
		{
			std::cout << "Watch out! p_z value in plot is already corrected!!!"  << std::endl;
		}
	}


}



Bool_t PndFtsHoughTrackFinder::FilterFoundTracklets(
		UInt_t maxAcceptableSharedHits,
		std::vector<PndFtsHoughTracklet> &tracklets
)
{
	// to store indices that I want to "delete"
	std::set<UInt_t> indicesToDelete;
	std::set<UInt_t>::iterator findIndex;


	// compare all tracklets with each other
	for (UInt_t iTrackletLeft = 0; iTrackletLeft < tracklets.size(); ++iTrackletLeft)
	{
		for (UInt_t iTrackletRight = iTrackletLeft+1; iTrackletRight < tracklets.size(); ++iTrackletRight)
		{
			// check if two tracks share too many hits
			const UInt_t numberOfSharedHits = tracklets[iTrackletLeft].getNumberOfSharedHits(tracklets[iTrackletRight]);
			if (numberOfSharedHits>maxAcceptableSharedHits){
				// mark the peak with the lower "height" for deletion
				const Double_t heightLeft = tracklets[iTrackletLeft].getPeakHeightFromPeakFinder();
				const Double_t heightRight = tracklets[iTrackletRight].getPeakHeightFromPeakFinder();
				if (heightLeft>heightRight){
					indicesToDelete.insert(iTrackletRight);
				} else if (heightLeft<heightRight){
					indicesToDelete.insert(iTrackletLeft);
				} else if (heightLeft==heightRight){
					std::cout << "WARNING: Found two peaks of the same height that share " <<  numberOfSharedHits << " hits.\n";
				}
			}

		}
	} // end for loop: compare all tracklets with each other
	// create new vector and copy all entries from input vector to it which are not marked for deletion
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





Bool_t PndFtsHoughTrackFinder::FindAllPeaks(
		TString option,
		TH2F *houghspace,
		const UInt_t minHeight,
		std::vector<PndFtsHoughTracklet> &tracklets
)
{
	// finds all peaks that satisfy the minimum height requirement minHeight
	// returns kTRUE if at least one peak was found
	// kFALSE if not
	// (probably option was not set correctly or houghspace is empty / has too few hits)

	// overwrites the following vectors with the values found for the peak in the histogram houghspace
	//	peakTheta = theta for peak
	//	peakSecond = Q/pzx for peak (parabola HT)
	//	peakSecond = intercept for peak (line HT) (in z-x- or z-y-plane)

	// peakThetaHw = half width of peak in theta
	// peakSecondHw = half width of peak in second value (see above for what it stands for)

	// actualHeight returns the height of the peak in the histogram (in counts)


	// clear all the output vectors (just to be sure, they should be empty anyway at the beginning)
	tracklets.clear();


	// check if hough space has at least one entry
	if (1>houghspace->GetEntries())
	{
		std::cout << "Hough Space is empty. No peak can be found. Return empty vectors." << std::endl;
		return kFALSE;
	}

	TAxis *fXaxis = houghspace->GetXaxis();
	TAxis *fYaxis = houghspace->GetYaxis();
	TAxis *fZaxis = houghspace->GetZaxis();



	if (("allPeaks>minHeight" == option) || ("allPeaksSearchWindow>minHeight" == option))
	{
		// peak finder based on weighted means (only for 2D histograms implemented)


		// the following is an adaptation of the TH1::GetMaximumBin() code



		// define vicinityLength for search window which determines an effective height of the bin
		Int_t vicinityLength = 0; // set for not using a search window
		// search window has size (2*vicinityLength+1)^2
		if ("allPeaksSearchWindow>minHeight" == option)
		{
			vicinityLength = 2; // set for using a search window
		}


		// move search window over histogram and select highest value
		Int_t locmax, locmay, locmaz; // location of maximum (x,y,z)

		//   -*-*-*-*-*Return location of bin with maximum value in the range*-*
		//             ======================================================
		Int_t binNumber, binx, biny, binz=0;
		// get values for first and last bins on each axis and take into account that we might want a search window (make sure search window stays within histogram)
		Int_t xfirst  = fXaxis->GetFirst()+vicinityLength;
		Int_t xlast   = fXaxis->GetLast()-vicinityLength;
		Int_t yfirst  = fYaxis->GetFirst()+vicinityLength;
		Int_t ylast   = fYaxis->GetLast()-vicinityLength;
		//		   Int_t zfirst  = fZaxis->GetFirst()+vicinityLength;
		//		   Int_t zlast   = fZaxis->GetLast()-vicinityLength;

		// find all peaks that have a height >= minHeight
		Double_t currentHeight;
		locmax = locmay = locmaz = 0;
		//		   for (binz=zfirst;binz<=zlast;binz++) {
		for (biny=yfirst;biny<=ylast;biny++) {
			for (binx=xfirst;binx<=xlast;binx++) {
				currentHeight = 0.;
				// iterate over vicinity
				for (Int_t xVicinityBin = -vicinityLength; xVicinityBin <= vicinityLength; ++xVicinityBin)
				{
					for (Int_t yVicinityBin = -vicinityLength; yVicinityBin <= vicinityLength; ++yVicinityBin)
					{
						binNumber = houghspace->GetBin(binx+xVicinityBin,biny+yVicinityBin,binz);
						currentHeight += 1./(1.+max(abs(xVicinityBin),abs(yVicinityBin)))*houghspace->GetBinContent(binNumber); // linear, unendlich norm
						//							currentHeight += 1./(1.+abs(xVicinity) +abs(yVicinity))*houghspace->GetBinContent(binNumber); // linear
						// currentHeight += 1./(1.+pow(max(xVicinity,yVicinity),2))*houghspace->GetBinContent(binNumber); // quadratic, unendlich norm
					}
				}
				if (currentHeight >= minHeight) {
					locmax  = binx;
					locmay  = biny;
					//		               locmaz  = binz;
					// get values corresponding to the peak
					Double_t peakThetaVal = fXaxis->GetBinCenter(locmax);
					Double_t peakSecondVal = fYaxis->GetBinCenter(locmay);

					Int_t binmaxglobal = houghspace->Fill(peakThetaVal, peakSecondVal, 0); // returns binnumber without modifying the histogram
					Double_t peakThetaHwVal = fXaxis->GetBinWidth(peakThetaVal);
					Double_t peakSecondHwVal = fYaxis->GetBinWidth(peakSecondVal);

					// create tracklet and push it back to output
					PndFtsHoughTracklet currentTracklet;
					currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currentHeight, peakThetaHwVal, peakSecondHwVal);
					tracklets.push_back(currentTracklet);
				}
			}
		}
		//		   }

		return kTRUE;
	}
	else if ("tspectrum2" == option)
	{
		// TODO: This peakfinder should be rechecked!
		Int_t maxpeaks = 20;
		// Finding the peaks (as in example macro)
		TSpectrum2 s(maxpeaks,3); // second argument: higher = peaks can be closer together (1 enforces 3 sigma seperation between peaks)
		s.SetAverageWindow(3); // standard is 3 (for Markov smoothing)
		s.SetDeconIterations(100); // standard is 3, more is better
		// 2nd parameter: sigma of searched peaks = 2 standard
		// 4th parameter: threshold: (default=0.05)  peaks with amplitude less than threshold*highest_peak are discarded.  0<threshold<1
		//		Int_t nfound = s.Search(houghspace, 2,"nobackground,nomarkov",0.5); // works ok for line and for parabola, kind of...
		Int_t nfound = s.Search(houghspace, 2,"",0.5);
		Float_t *xpeaks = s.GetPositionX();
		Float_t *ypeaks = s.GetPositionY();
		s.Print();

		// for output
		for (UInt_t iPeak = 0; iPeak < nfound; ++iPeak){
			Double_t peakThetaVal = xpeaks[iPeak];
			Double_t peakSecondVal = ypeaks[iPeak];

			Int_t binmaxglobal = houghspace->Fill(peakThetaVal, peakSecondVal, 0); // returns binnumber without modifying the histogram
			Double_t peakThetaHwVal = fXaxis->GetBinWidth(peakThetaVal);
			Double_t peakSecondHwVal = fYaxis->GetBinWidth(peakSecondVal);


			Double_t currentHeight = houghspace->GetBinContent(binmaxglobal);

			// create tracklet and push it back to output
			PndFtsHoughTracklet currentTracklet;
			currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currentHeight, peakThetaHwVal, peakSecondHwVal);
			tracklets.push_back(currentTracklet);
		}
		//		binmaxglobal = houghspace->Fill(peakTheta, peakSecond, 0); // returns binnumber without modifying the histogram
		return kTRUE;
	}
	else
	{
		std::cout << "PeakFinder error: Option " << option << " is not implemented!" << std::endl;
		return kFALSE;
	}
}



// TODO actually I don't need the the parameters thetalimitlow,thetalimithigh, thetastep <- I can get the info from the Houghspace directly
Bool_t PndFtsHoughTrackFinder::MakeHoughSpace(
		TString option,
		const Double_t onlyusehitsfromz,
		const Double_t onlyusehitsuptoz,
		const Bool_t onlyUseHitsFromNonSkewedStraws,
		const Double_t thetalimitlow,
		const Double_t thetalimithigh,
		const Double_t thetastep,
		Double_t interceptZxOrZy, // cannot be constant, because might need to be reset if set incorrectly (has to be 0 for line HT)
		UInt_t &nHitsForHoughSpace,
		TH2F* houghspace)
{
	// !!! WARNING The theta values are NOT the same as in the interaction point. They are always calculated relative to a shifted coordinate system and only 2-dimensional !!!

	// houghspace has always the angle (theta) on x-coordinate axis, the value on the y-axis depends on the kind of hough transform
	//	parabola HT: yValue = Q/pzx
	//	line HT: yValue = intercept (Achsenabschnitt) (in z-x- or z-y-plane)

	// produces a houghspace histogram using the equation corresponding to string option
	// only hits with a z value (in the laboratory system, zreal) between onlyusehitsfromz and onlyusehitsuptoz will be used for building the houghspace
	// if onlyUseHitsFromNonSkewedStraws is kTRUE, then only hits from non-skewed straws are used for Hough transform

	// The angle to the z-axis in the z-x- or z-y-plane (theta) will be scanned from thetalimitlow to thetalimithigh with stepsize thetastep

	// y component of B field will be read from field maps if keepBConstant is kFALSE

	// For the z-x-plane parabola, a shift in x (hitshiftinx) needs to be set (which should be the result of the straight line hough transform)
	// For the straight line (stations before dipole field) hitshiftinx HAS TO BE ZERO

	// zOffset is used to redefine an origin for the coordinate system (so that the angle definition gives meaningful theta values)


	// If everything goes well, the function returns kTRUE and the histogram houghspace contains the hough space
	// This function returns kFALSE if option is set incorrectly


	// interceptZxOrZy is used to shift the true X values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
	// (zreal=zOffset, xreal=interceptZxOrZy) = (zshifted = 0, xshifted = 0)
	// zshifted = zreal - zOffset
	// xshifted = xreal - interceptZxOrZy
	// zreal = zshifted + zOffset
	// xreal = xshifted + interceptZxOrZy


	// for B field access
	Double_t By = 0.;
	Double_t po[3], BB[3];

	// c is a factor for the parabola
	// for parabola equation
	const Double_t n = 1.;
	const Double_t e = 1.;
	const Double_t c = n * e / 2.;

	// for storing the value to be calculated in Hough transform (yValue = offset for line, yValue = Q/pzx for parabola)
	Double_t yValue = 0.;

	// store bin numbers for last and current entry (for making sure that there are no holes in the histogram)
	Int_t globalBin = 0;
	Int_t currentBinX = 0, currentBinY = 0, currentBinZ = 0;
	Int_t lastBinX = 0, lastBinY = 0;

	Bool_t firstEntry = kTRUE; // is used to indicate when holes in histogram have to be filled, set this to kTRUE for the first entry FOR EACH HIT


	// make sure hits are not shifted for line hough transform
	if ("lineBeforeDipole" == option)
	{
		if (0!=interceptZxOrZy) {
			std::cout << "MakeHoughSpace: " << "interceptZxOrZy was set to " << interceptZxOrZy << " That is not correct for line HT! of stations before dipole field!\n";
		}
		if (0<fVerbose) {  std::cout << "Will set interceptZxOrZy to 0" << std::endl; }
		interceptZxOrZy = 0.;
	}

	// The hits have to be stored in TCA fFtsHitArray
	// This produces the hough space for a parabola or a line (with constant B field or with B field read from field maps)
	for (int iHit = 0; iHit < fFtsHitArray->GetEntriesFast(); iHit++)
	{
		firstEntry = kTRUE;
		PndFtsHit* myHit = (PndFtsHit*) fFtsHitArray->At(iHit);
		// Skip hit if it comes from skewed straw and if I wish not to use it
		if (kTRUE == onlyUseHitsFromNonSkewedStraws)
		{
			if (0 != myHit->GetSkewed())
			{
				if (1<fVerbose) {std::cout << "Skipping hit with index " << iHit << " , because it comes from a skewed straw! LayerID = " << myHit->GetLayerID() << std::endl;}
				continue;
			}
		}

		// get hit
		TVector3 hitVector;
		myHit->Position(hitVector);

		Double_t hitXLabSys = hitVector.X();
		Double_t hitYLabSys = hitVector.Y();
		Double_t hitZLabSys = hitVector.Z();
		Double_t hitXShifted = hitXLabSys - interceptZxOrZy; // shifts all x positions of hits so that they go through x=0 at z=zOffset (for parabola)
		Double_t hitZShifted = hitZLabSys - zOffset; // z coordinate in local coordinate system (for parabola and for line)

		// calculate Hough transform for hit iHit
		// for each hit a scan in theta is done
		for (Double_t theta = thetalimitlow; theta < thetalimithigh; theta += thetastep)
		{
			// only hits with (non-shifted) z component between onlyusehitsfromz and onlyusehitsuptoz will be used in the hough transform
			if (hitZLabSys >= onlyusehitsfromz && hitZLabSys <= onlyusehitsuptoz)
			{
				if (thetalimitlow == theta) // only print the message once per hit
				{
					++nHitsForHoughSpace; // count hits for making of Hough space (only once per hit)
					if (1<fVerbose) {cout << "Doing " << option << " hough transform for hit (hitZreal, hitXreal) = (" << hitZLabSys << ", " << hitXLabSys << ") cm";}
				}
				if (kTRUE == keepBConstant)
				{
					// do not take B field into account
					By = 1.;
					if (thetalimitlow == theta)
					{
						if (1<fVerbose) {cout << " ignoring B field maps";}
					}
				}
				else
				{
					// Use B field information
					po[0] = hitXLabSys; // Use magnetic field at real (not shifted) x position
					po[1] = hitYLabSys;
					po[2] = hitZLabSys;
					fField->GetFieldValue(po, BB); //return value in KG (G3)
					By = BB[1] / 10.; // By is y-component of magnetic field in Tesla
					if (thetalimitlow == theta)
					{
						if (1<fVerbose) { cout << "  By = " << By << " T "; }
					}
				}

				if (thetalimitlow == theta)
				{
					if (1<fVerbose) { cout << std::endl; }
				}

				Double_t thetaRad = theta / 360. * 2. * meinpi;

				if ("parabola" == option)
				{
					// Use shifted x and shifted z for parabola

					yValue = 1. / c / By 	* (-hitZShifted * sin(thetaRad) + hitXShifted * cos(thetaRad))/ pow((hitZShifted * cos(thetaRad) + hitXShifted * sin(thetaRad)), 2);

					// next line is with rotation as in paper (I believe it is incorrect)
					//					value = 1. / c / By 	* (hitZshifted * sin(realtheta) - hitXshifted * cos(realtheta))/ pow((hitZshifted * cos(realtheta) + hitXshifted * sin(realtheta)), 2);
					if (9<fVerbose)	{ cout << "Q/pzx = " << yValue; }
				}
				else if ("parabolapz" == option)
				{
					// Use shifted x and shifted z for parabola
					yValue = c*By*pow((hitZShifted * cos(thetaRad) + hitXShifted * sin(thetaRad)), 2)/(-hitZShifted * sin(thetaRad) + hitXShifted * cos(thetaRad));

					// next line is with rotation as in paper (I believe it is incorrect)
					//					value = c*By*pow((hitZshifted * cos(realtheta) + hitXshifted * sin(realtheta)), 2)/(hitZshifted * sin(realtheta) - hitXshifted * cos(realtheta));
					if (9<fVerbose)	{ cout << "pz/Q = " << yValue; }
				}
				else if ("lineBeforeDipole" == option)
				{
					// Use real x and shifted z for line
					// calculate b which is the distance of point on line at z = zOffset from z axis
					yValue = -tan(thetaRad)*hitZShifted+hitXLabSys;
					if (9<fVerbose) { cout << "b = " << yValue; }
				}
				else
				{
					std::cout << "Error in MakeHoughSpace! option " << option << " is not implemented!" << std::endl;
					return kFALSE;
				}

				if (9<fVerbose)	{ std::cout << " for (theta, hitXreal) = (" << theta << ", " << hitXLabSys << ")" << std::endl; }



				// make sure we have a Hough space
				if (0==houghspace){
					Info("MakeHoughSpace","houghspace is not set.");
					return kFALSE;
				}
				globalBin = houghspace->Fill(theta,yValue);
				if (5<fVerbose) { cout << "OK! Hough point was filled into histogram. globalbin = " << globalBin << " for option" << option <<std::endl; }
				// Find binx and biny for histogram from global bin number
				houghspace->GetBinXYZ(globalBin, currentBinX, currentBinY, currentBinZ);




				// Check if the Fill was actually into a real bin or in over-/underflow
				if (globalBin>=0)
				{
					if (5<fVerbose) { cout << "OK! Hough point was NOT written to over- or underflow of histogram. Setting firstEntry to kFALSE now. "<< option <<std::endl; }

					// only do this if the current theta is not the first for the hit
					if (kFALSE == firstEntry)
					{
						if (5<fVerbose)
						{
							cout << "This is not the first point of the hit in the histogram. I will fix all holes which might be between this entry and the last one in the histogram"<<std::endl;
						}

						// Make sure there are no holes in the histogram
						for (Int_t iCorrect = 1; iCorrect < abs(currentBinY-lastBinY); ++iCorrect)
						{


							Int_t xCorrect = round(float(iCorrect*(currentBinX-lastBinX))/float(abs(currentBinY-lastBinY))); // gives -1, 0 or 1
							Int_t yCorrect = 0;
							if (currentBinY > lastBinY)
							{
								yCorrect = iCorrect;
							}
							else
							{
								yCorrect = -iCorrect;
							}


							houghspace->AddBinContent(houghspace->GetBin(lastBinX+xCorrect,lastBinY+yCorrect));

							if (8<fVerbose)
							{
								cout << "I am filling hole number " << iCorrect << std::endl;
								cout << "globalbin = " << globalBin << std::endl;
								cout << "(lastbinx, lastbiny) = (" << lastBinX << ", " << lastBinY << ")" << std::endl;
								cout << "(binx,     biny)     = (" << currentBinX << ", " << currentBinY << ")" << std::endl;
								cout << "xCorrect = " << xCorrect << "  yCorrect = " << yCorrect << std::endl;
								cout << "(lastbinx+xCorrect, lastbiny+yCorrect) = (" << lastBinX+xCorrect << ", " << lastBinY+yCorrect << ")" << std::endl;
							}
						}// for iCorrect


					} // if not first entry to be written into histogram
					else
					{
						if (5<fVerbose) { cout << "This is the first point of the hit in the histogram. I will not try to fix any holes in the " << option << " histogram"<<std::endl;}
					}


					firstEntry = kFALSE;


				}
				else
				{
					if (9<fVerbose)	{ cout << "Watch out! Point was written to over- or underflow of histogram. firsttheta is set to kTRUE. "<< option <<std::endl; }
					firstEntry = kTRUE; // otherwise, algorithm connects first point which does not go into over-/underflow with (0,0)
				}

				if (9<fVerbose)	{ cout << "biny = " << currentBinY << "  lastbiny = " << lastBinY << std::endl; }
				lastBinX = currentBinX;
				lastBinY = currentBinY;

			} // if hitZLabSys is in correct range
		} // for theta
	} // for iHit
	return kTRUE;
}










