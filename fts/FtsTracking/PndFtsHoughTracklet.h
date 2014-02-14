// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
// Class for Hough transform based FTS tracking
// This saves the result of one Hough transform
// The tracklet can represent a line or a parabola (actually an arbitrary result of a 2d Hough transform)
//
// Created: 11.02.2014
// Modified: 11.02.2014
//
// *************************************************************************

#ifndef PNDFTSHOUGHTRACKLET_H
#define PNDFTSHOUGHTRACKLET_H

// Root Class Headers ----------------
#include "PndTrackCand.h"

class PndFtsHit;
class PndTrack;




class PndFtsHoughTracklet : public PndTrackCand {
public:

	// Constructors/Destructors ---------
	PndFtsHoughTracklet();
	~PndFtsHoughTracklet();


	// Accessors -----------------------
	void Print();
	// isSet() is kTRUE iif data from the 2d Hough transforms have been entered
	Bool_t isSet() const { return fIsSet; };
	const PndFtsHit *getHit(UInt_t index); // gets the FairHit corresponding to index
	Double_t getPeakHeightFromPeakFinder() const { return fPeakHeightFromPeakFinder; };
	Double_t getThetaVal() const { return fThetaVal; };
	Double_t getThetaHw() const { return fThetaHw; };
	Double_t getSecondVal() const { return fSecondVal; };
	Double_t getSecondHw() const { return fSecondHw; };

	UInt_t getNSharedHits(PndFtsHoughTracklet& rhs);


	// Modifiers -----------------------
	// add results from Hough transforms
	void SetHoughTransformResults(const Double_t thetaVal, const Double_t secondVal, const Double_t peakHeight, const Double_t thetaHw, const Double_t secondHw);





private:
	// Private Data Members ------------
	Int_t fVerbose;

	// FTS Hits
	static TClonesArray *fFtsHitArray;


	Bool_t fIsSet; // kTRUE if values have already been filled

	Double_t fPeakHeightFromPeakFinder;
	// height of peak in Hough transform
	// (as determined by peakFinder -- does not need to be == fHitId.size()
	// as the peak finder might add a weight to the hits depending on how close they have been to the actual peak)

	// saving peak position in a 2d Hough transform as well as the half width of the histogram
	Double_t fThetaVal; // theta angle
	Double_t fThetaHw; // error on angle

	Double_t fSecondVal; // second value is x/y intercept for line in zx / zy plane and Q/pzx for parabola in zx plane
	Double_t fSecondHw; // error on second value




public:
	ClassDef(PndFtsHoughTracklet,1);
};

#endif
