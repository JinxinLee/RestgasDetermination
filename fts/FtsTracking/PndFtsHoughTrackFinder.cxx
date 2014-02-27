#include "PndFtsHoughTrackFinder.h"

#include <iostream>

#include "TMath.h"
#include <math.h>
#include <algorithm>

#include <set>
#include <vector>
#include <map>

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




using namespace std;

ClassImp(PndFtsHoughTrackFinder)
;





PndFtsHoughTrackFinder::PndFtsHoughTrackFinder(Int_t branchId, TClonesArray* hits, FairField* field) :
														fFtsHitArray(hits),
														fFtsBranchId(branchId),
														fField(field),

														// Hough spaces
														fHoughspaceZxLineParabola(0),
														fHoughspaceZxParabola(0),
														fHoughspaceZxParabolaLine(0),
														fHoughspaceZyLine(0),

														// min peak heights
														fMinPeakHeightZxLineParabola(4),
														fMinPeakHeightZxParabola(6),
														fMinPeakHeightZxParabolaLine(4),
														fMinPeakHeightZyLine(4),

														// general
														fSaveDebugInfo(kFALSE),
														fVerbose(0)
{
	if (0==fFtsHitArray){
		std::cout << "PndFtsHoughTrackFinder FATAL ERROR Hit array not set.\n";
	}
}


PndFtsHoughTrackFinder::~PndFtsHoughTrackFinder()
{
	if(3<fVerbose) Info(MESSAGE_ORIGIN,"Destructor of PndFtsHoughTrackFinder");
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





	// Do we have enough hits in the FTS?
	if(fMinPeakHeightZxParabola > fFtsHitArray->GetEntriesFast()) {
		if(0<fVerbose) Info("Exec","Skip the event, since we have too few hits in FTS");
		return;
	}








	// Straight line Hough transform
	static const Int_t invthetastepForLine = 8; // greater number means finer scanning in theta
	static const Int_t thetaLowFirstLine = -20;
	static const Int_t thetaHighFirstLine = 20;
	delete fHoughspaceZxLineParabola;
	fHoughspaceZxLineParabola = new PndFtsHoughSpace(
			"lineBeforeDipole",
			invthetastepForLine*(thetaHighFirstLine-thetaLowFirstLine),
			thetaLowFirstLine,
			thetaHighFirstLine,

			invthetastepForLine*40,
			-50.,
			50.,

			zLineParabola,
			0.,

			fFtsBranchId,
			fFtsHitArray,

			fField
	);
	fHoughspaceZxLineParabola->GetXaxis()->SetTitle("#theta [^{0}]");
	fHoughspaceZxLineParabola->GetYaxis()->SetTitle("x_{LP} [cm]");



	// Do straight line hough transform on non-skewed hits from stations 1+2
	if ( kTRUE ==
			fHoughspaceZxLineParabola->MakeHoughSpace() )
	{
		if (0<fVerbose) {
			std::cout << "Hough Space for zx line before dipole was created successfully!" << std::endl
					<< "We have " << fHoughspaceZxLineParabola->GetNHits() << " hits in the line Hough space.\n";

		}
	}
	else
	{
		std::cout << "Hough Space for zx line before dipole could not be created! " << std::endl;
	}



	// find peaks for line hough space and store in vector
	const Int_t minHeightZxLineParabola = 6;
	std::vector<PndFtsHoughTracklet> zxLineParabolaTracklets;


	// Call peak finder and plot the solution if it was found!
	if (kTRUE == fHoughspaceZxLineParabola->FindAllPeaks(minHeightZxLineParabola, zxLineParabolaTracklets))
	{
		if (0<fVerbose) {
			std::cout << zxLineParabolaTracklets.size() << " peaks found for zx line before dipole. \n";
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
		std::cout << "Error: Peak finder had a problem with hough space for zx line before dipole!!!" << std::endl;
	}




	// loop over all line tracklets which were found by line HT before dipole field

	for (UInt_t iTrackletLine=0; iTrackletLine < zxLineParabolaTracklets.size(); ++iTrackletLine)
	{
		const Double_t peakThetaZxLineParabola = zxLineParabolaTracklets[iTrackletLine].getThetaVal();
		const Double_t peakInterceptZxLineParabola = zxLineParabolaTracklets[iTrackletLine].getSecondVal();

		// determine where to look for parabola
		const Int_t invthetastepForParabola = 10; // greater number means finer scanning in theta
		const Int_t thetaLowParabola = floor(peakThetaZxLineParabola-0.3)-0; // TODO: Optimize parameters
		const Int_t thetaHighParabola = ceil(peakThetaZxLineParabola+0.3)+0;

		delete fHoughspaceZxParabola;
		fHoughspaceZxParabola= new PndFtsHoughSpace(
				"parabola",

				invthetastepForParabola*(thetaHighParabola-thetaLowParabola),
				thetaLowParabola,
				thetaHighParabola,

				invthetastepForParabola*300, // 300 is good as factor
				-0.015,
				0.015,

				zLineParabola,
				peakInterceptZxLineParabola,

				fFtsBranchId,
				fFtsHitArray,

				fField
		);
		fHoughspaceZxParabola->GetXaxis()->SetTitle("#theta [^{0}]");
		fHoughspaceZxParabola->GetYaxis()->SetTitle("#frac{Q}{p_zx} [a.u.]");



		// Do parabola hough transform (shifts FTS hits by hitshiftinx) for non-skewed hits in stations 3+4+5
		if ( kTRUE == fHoughspaceZxParabola->MakeHoughSpace() )
		{
			if (0<fVerbose) {
				std::cout << "Line " << iTrackletLine << ": Hough Space for zx parabola was created successfully!" << std::endl
						<< "We have " << fHoughspaceZxParabola->GetNHits() << " hits in the line Hough space.\n";
			}
		}
		else
		{
			std::cout << "Hough Space for zx parabola could not be created! " << std::endl;
		}


		// TODO: Rewrite the following conversion
		// Get momenta from arbitrary units to meaningful ones
		//		pzinvpeak = 1./pzinvpeak*0.00299792458;
		//	pzinvpeakWithBField = pzinvpeak/BMeanForParabola;


		// find peaks for zx parabola hough space
		const Int_t minHeightZxParabola = 8;
		std::vector<PndFtsHoughTracklet> zxParabolaTracklets;


		if (kTRUE == fHoughspaceZxParabola->FindAllPeaks(minHeightZxParabola, zxParabolaTracklets))
		{
			if (0<fVerbose)
			{
				std::cout << zxParabolaTracklets.size() << " peaks found for parabola \n";
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
			std::cout << "Create track candidates from line in zx before dipole "<< iTrackletLine << " and all parabolas within dipole." << std::endl;
		}
		for (UInt_t iTrackletParabola=0; iTrackletParabola < zxParabolaTracklets.size(); ++iTrackletParabola)
		{
			PndFtsHoughTrackCand newHoughTrackCand(fFtsBranchId, fFtsHitArray);
			newHoughTrackCand.SetZxFirstLine(zxLineParabolaTracklets[iTrackletLine], zLineParabola);
			newHoughTrackCand.SetZxParabola(zxParabolaTracklets[iTrackletParabola], zLineParabola);
			fHoughTrackCands.push_back(newHoughTrackCand);
		}

	} // loop over all line tracklets which were found by line HT before dipole field




	std::cout << "At this point I have track candidates from line in zx before dipole and parabola within. Now add line after dipole field in zx" << std::endl;
	// TODO















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


	delete fHoughspaceZxLineParabola;
	delete fHoughspaceZxParabola;
	delete fHoughspaceZxParabolaLine;
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


	// compare all tracklets with each other, keep track which tracklets should be "deleted"
	// delete all tracklets that share more than maxAcceptableSharedHits hits
	// keep only the heighest peak
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
				}
			}

		}
	} // end for loop: compare all tracklets with each other

	// if we have no tracklets to "delete", we don't need to do anything
	if (0==indicesToDelete.size()){
		if (0<fVerbose) {  std::cout << "FilterTrackletsBasedOnSharedHits: No tracklets are marked for deletion." << std::endl; }
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


