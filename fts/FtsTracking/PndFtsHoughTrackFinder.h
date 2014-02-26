// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
//  This class is loosely modeled after the
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
// Find all peaks with a minimum height
// Moved all Hough space related code to PndFtsHoughSpace -> Major code simplification, better maintainability
//
// TODO
// Fill PndTrackCands for output
// Add straight line for stations 5+6
// Add skewed hits
// Adaptive Hough
// Add drift circles
//
//
// Created: 18.06.2013
// Modified: 26.02.2014
//
// *************************************************************************






#ifndef PndFtsHoughTrackFinder_H
#define PndFtsHoughTrackFinder_H

#include "TClonesArray.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

#include <cmath>
#include <vector>
#include <map>
#include <fstream>


#include "PndFtsHoughSpace.h"
#include "PndFtsHoughTracklet.h"
#include "PndFtsHoughTrackCand.h"

class FairField;
class TClonesArray;
//class PndGeoFtsPar;
//class TGraph;

class FairHit;




class PndFtsHoughTrackFinder
{
public:
	PndFtsHoughTrackFinder(TClonesArray* hits, Int_t branchId, FairField* field); ///< Sets the array of all FTS hits and the branchId (super important!)
	virtual ~PndFtsHoughTrackFinder();

	void FindTracks();										///< Main function to start the track finding


	void SetVerbose (int verbose){fVerbose = verbose;};
	void SetSaveDebugInfo(Bool_t saveDebugInfo){ fSaveDebugInfo = saveDebugInfo;};

	// Output
	int NTracks(){return fTrackCand.size();};								///< Returns the number of found tracks
	PndFtsHoughTrackCand GetTrack(int i){return fHoughTrackCands[i];};					///< Returns the track with the index i
	PndTrack GetPndTrack(int i){return fHoughTrackCands[i].getPndTrack();};
	PndTrackCand GetTrackCand(int i) {return fTrackCand[i];};


	// Parameters
	//	void SetMinPeakHeightZxLineParabola(UInt_t val){ fMinPeakHeightZxLineParabola = val; };
	//	void SetMinPeakHeightZxParabola(UInt_t val){ fMinPeakHeightZxParabola = val; };
	//	void SetMinPeakHeightZxParabolaLine(UInt_t val){ fMinPeakHeightZxParabolaLine = val; };
	//	void SetMinPeakHeightZyLine(UInt_t val){ fMinPeakHeightZyLine= val; };

	//  write out histograms for debugging
	void WriteHistograms();


private:
	PndFtsHoughTrackFinder(); // do not use!

	Int_t   fFtsBranchId; // needed for saving and accessing hits
	TClonesArray *fFtsHitArray; // Input array of all FTS hits




	///< Minimum required heights for peaks in Hough spaces
	UInt_t    fMinPeakHeightZxLineParabola;					// zx line before dipole field
	UInt_t    fMinPeakHeightZxParabola;					// zx parabola within dipole field
	UInt_t    fMinPeakHeightZxParabolaLine;					// zx line after dipole field
	UInt_t    fMinPeakHeightZyLine;					// zy line


	Int_t fVerbose;
	Bool_t fSaveDebugInfo;




	// for B field access
	FairField* fField;





	// for Hough
	///////////////////
	PndFtsHoughSpace* fHoughspaceZxLineParabola;
	PndFtsHoughSpace* fHoughspaceZxParabola;
	PndFtsHoughSpace* fHoughspaceZxParabolaLine;
	PndFtsHoughSpace* fHoughspaceZyLine;
	std::vector<PndFtsHoughTrackCand> fHoughTrackCandsNew;									///< Temporary Hough Track Cands are used internally to store track cands
	std::vector<PndFtsHoughTrackCand> fHoughTrackCands;									///< Hough Track Cands are used internally to store track cands
	std::vector<PndTrackCand> fTrackCand; // resulting track candidates, also used for returning PndTracks




	// sets where the midpoint of the parabola is supposed to be
	static const Double_t zLineParabola = 368.; // the value should coincide with the start of the dipole field // 368. was ok
	static const Double_t zParabolaLine = 650.; // the value should coincide with the end of the dipole field // TODO determine this value








	Bool_t FilterTrackletsBasedOnSharedHits(
			UInt_t maxAcceptableSharedHits,
			std::vector<PndFtsHoughTracklet> &tracklets
	);
	// takes the heighest peak (according to peak finder)
	// of all peaks that share > maxSameHits


	ClassDef(PndFtsHoughTrackFinder,1);
};

#endif /*PndFtsHoughTrackFinder_H*/
