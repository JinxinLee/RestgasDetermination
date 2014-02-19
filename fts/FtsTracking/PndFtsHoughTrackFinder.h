// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
//  This class is modeled after the
// mvd/MvdTracking/PndRiemannTrackFinder
// sttmvdtracking/PndMvdSttGemRiemannTrackFinder
// classes
//
//
// This is a class version of the HoughTest.C macro tracking test implementation minus all the plotting stuff
// Take a look at the notes of the macro version
//
//
// Recent Changes
// Moved code from macro to class
// Major code cleanup and deletion of test code / unneeded code
// use PndFtsHoughTrackCand to store information about track candidates and Hough transforms
//
//
// TODO
// Fill PndTrackCands for output // I need help with this...
// Find all peaks with a minimum height (6 for 1+2, 8 for 3+4+5)
// Add straight line for stations 5+6 (6 hits at least)
// Add skewed hits
// Adaptive Hough
// Add drift circles
//
//
// Created: 18.06.2013
// Modified: 14.02.2014
//
// *************************************************************************






#ifndef PndFtsHoughTrackFinder_H
#define PndFtsHoughTrackFinder_H

#include "TClonesArray.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

#include <vector>
#include <map>
#include <fstream>


#include <cmath>
#include <vector>


#include "PndFtsHoughTracklet.h"
#include "PndFtsHoughTrackCand.h"

class FairField;
class TClonesArray;
//class PndGeoFtsPar;
//class TH1F;
class TH2S;
//class TGraph;

class FairHit;




class PndFtsHoughTrackFinder
{
public:
	PndFtsHoughTrackFinder();
	virtual ~PndFtsHoughTrackFinder();

	void FindTracks();										///< Main function to start the track finding

	// Input
	//	void SetHits(std::vector<FairHit*> hits){fHits = hits;};				///< Replaces the existing array of hits with a new one
	//	void AddHits(std::vector<FairHit*> hits, Int_t branchId);				///< Appends the new array of hits to the existing one
	void SetHits(TClonesArray* hits, Int_t branchId);						///< Replaces the existing array of hits with a new one
	//	void AddHit(FairHit* hit){fHits.push_back(hit);};						///< Adds one new hit to the array of hits

	void SetField(FairField* field){fField=field;};						///<

	void SetVerbose (int verbose){fVerbose = verbose;};
	void SetSaveDebugInfo(Bool_t saveDebugInfo){ fSaveDebugInfo = saveDebugInfo;};

	// Output
	int NTracks(){return fTrackCand.size();};								///< Returns the number of found tracks
	PndFtsHoughTrackCand GetTrack(int i){return fTracks[i];};					///< Returns the track with the index i
	PndTrack GetPndTrack(int i){return fTracks[i].getPndTrack();};
	PndTrackCand GetTrackCand(int i) {return fTrackCand[i];};


	// Parameters
	//	void SetMinPeakHeightZxLineParabola(UInt_t val){ fMinPeakHeightZxLineParabola = val; };
	//	void SetMinPeakHeightZxParabola(UInt_t val){ fMinPeakHeightZxParabola = val; };
	//	void SetMinPeakHeightZxParabolaLine(UInt_t val){ fMinPeakHeightZxParabolaLine = val; };
	//	void SetMinPeakHeightZyLine(UInt_t val){ fMinPeakHeightZyLine= val; };

	//  write out histograms for debugging
	void WriteHistograms();


private:
	//! FTS Branch ID
	Int_t   fFtsBranchID;
	/** Input array of PndFtsHit **/
	TClonesArray *fFtsHitArray;
	// TODO double
	//	std::vector<FairHit*> fHits;	// vector with all hits of the current event
	std::vector<PndFtsHoughTrackCand> fTracks;									///< Resulting Hough Track Cands
	std::vector<PndTrackCand> fTrackCand; // resulting tracks

	///< Minimum required heights for peaks in Hough spaces
	UInt_t    fMinPeakHeightZxLineParabola;					// zx line before dipole field
	UInt_t    fMinPeakHeightZxParabola;					// zx parabola within dipole field
	UInt_t    fMinPeakHeightZxParabolaLine;					// zx line after dipole field
	UInt_t    fMinPeakHeightZyLine;					// zy line


	Int_t fVerbose;




























	static const Double_t meinpi = 3.14159265;








	// helper method (handy for multiple constructors)
	void Initialization_ClassVariables();


	// for B field access
	FairField* fField;


	// Debug
	////////
	Bool_t fSaveDebugInfo;
	static const Int_t ResolutionX = 800, ResolutionY = 600; // for plotting



	// for Hough
	///////////////////
	TH2S* fHoughspaceZxLineParabola;
	TH2S* fHoughspaceZxParabola;
	TH2S* fHoughspaceZxParabolaLine;
	TH2S* fHoughspaceZyLine;
	std::vector<PndFtsHoughTrackCand> fHoughTrackCandsNew;									///< Temporary Hough Track Cands are used internally to store track cands
	std::vector<PndFtsHoughTrackCand> fHoughTrackCands;									///< Hough Track Cands are used internally to store track cands




	// Which PeakFinder should be used?
	static TString peakfinderOption;




	// for HoughTransform
	/////////////////////
	// if kTRUE will correct the pz prediction according to values which should be obtained from a line fit mc truth momentum VS. reco momentum with high statistics
	static const Bool_t correctpz = kFALSE;









	// sets where the midpoint of the parabola is supposed to be
	static const Double_t zLineParabola = 368.; // the value should coincide with the start of the dipole field // 368. was ok
	static const Double_t zParabolaLine = 650.; // the value should coincide with the end of the dipole field // TODO determine this value








	Bool_t FilterTrackletsBasedOnSharedHits(
			UInt_t maxAcceptableSharedHits,
			std::vector<PndFtsHoughTracklet> &tracklets
	);
	// takes the heighest peak (according to peak finder)
	// of all peaks that share > maxSameHits


	Bool_t FindAllPeaks(
			TString option,
			TH2S *houghspace,
			const UInt_t minHeight,
			std::vector<PndFtsHoughTracklet> &tracklets
	);

	Bool_t MakeHoughSpace(
			const Double_t zRefPos, // is used to redefine an origin for the coordinate system (so that the angle definition gives meaningful theta values)
			Double_t interceptZx, // cannot be constant, because might need to be reset if set incorrectly (has to be 0 for line HT)
			// is used to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
			// (zreal=zOffset, xreal=interceptZx) = (zshifted = 0, xshifted = 0)
			// zshifted = zreal - zRefPos
			// xshifted = xreal - interceptZx
			// zreal = zshifted + zRefPos
			// xreal = xshifted + interceptZx
			// For the z-x-plane parabola, a shift in x (hitshiftinx) needs to be set (which should be the result of the straight line hough transform)
			// For the straight line (stations before dipole field) hitshiftinx HAS TO BE ZERO

			const Bool_t UseNonSkewedStraws, // if kTRUE, then hits from non-skewed straws are used for Hough transform
			const Bool_t UseSkewedStraws, // if kTRUE, then hits from skewed straws are used for Hough transform


			UInt_t &nHitsForHoughSpace,

			TH2S* houghspace // has always the angle (theta) on x-coordinate axis, the value on the y-axis depends on the kind of hough transform
			//	parabola HT: yValue = Q/pzx
			//	line HT: yValue = intercept (Achsenabschnitt) (in z-x- or z-y-plane)
	);






	Double_t equationParabola(Double_t thetaRad, Double_t hitZShifted, Double_t hitXShifted, Double_t By)
	{
		// for parabola equation
		const Double_t n = 1.;
		const Double_t e = 1.;
		const Double_t c = n * e / 2.;

		Double_t yVal = 1. / c / By 	* (-hitZShifted * sin(thetaRad) + hitXShifted * cos(thetaRad))/ pow((hitZShifted * cos(thetaRad) + hitXShifted * sin(thetaRad)), 2);

		// next line is with rotation as in paper (I believe it is incorrect)
		//					value = 1. / c / By 	* (hitZshifted * sin(realtheta) - hitXshifted * cos(realtheta))/ pow((hitZshifted * cos(realtheta) + hitXshifted * sin(realtheta)), 2);
		return yVal;
	};

	Double_t equationParabolaPz(Double_t thetaRad, Double_t hitZShifted, Double_t hitXShifted, Double_t By)
	{
		// for parabola equation
		const Double_t n = 1.;
		const Double_t e = 1.;
		const Double_t c = n * e / 2.;

		// Use shifted x and shifted z for parabola
		Double_t yVal = c*By*pow((hitZShifted * cos(thetaRad) + hitXShifted * sin(thetaRad)), 2)/(-hitZShifted * sin(thetaRad) + hitXShifted * cos(thetaRad));

		// next line is with rotation as in paper (I believe it is incorrect)
		//					value = c*By*pow((hitZshifted * cos(realtheta) + hitXshifted * sin(realtheta)), 2)/(hitZshifted * sin(realtheta) - hitXshifted * cos(realtheta));

		return yVal;
	};

	Double_t equationLineZx(Double_t thetaRad, Double_t hitZShifted, Double_t hitXLabSys)
	{
		// calculate b which is the distance of point on line at z = zOffset from z axis
		Double_t yVal = -tan(thetaRad)*hitZShifted+hitXLabSys;
		return yVal;
	}

	ClassDef(PndFtsHoughTrackFinder,1);
};

#endif /*PndFtsHoughTrackFinder_H*/
