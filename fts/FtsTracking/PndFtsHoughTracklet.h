// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
// Class for Hough transform based FTS tracking
// This saves the result of one Hough transform
// The tracklet can represent a line or a parabola (actually an arbitrary result of a 2d Hough transform)
//
// Created: 11.02.2014
// Modified: 19.02.2014
//
// *************************************************************************

#ifndef PNDFTSHOUGHTRACKLET_H
#define PNDFTSHOUGHTRACKLET_H

// Root Class Headers ----------------
#include "PndTrackCand.h"

class PndFtsHit;
class PndTrack;
class TClonesArray;



class PndFtsHoughTracklet : public PndTrackCand {
public:

	// Constructors/Destructors ---------
	PndFtsHoughTracklet(TClonesArray *ftsHitArray=0);
	~PndFtsHoughTracklet();


	// Accessors -----------------------
	void Print();
	// isSet() is kTRUE iif data from the 2d Hough transforms have been entered
	Bool_t isSet() const { return fIsSet; };

	// Hough space peak info
	Double_t getPeakHeightFromPeakFinder() const { return fPeakHeightFromPeakFinder; };
	Double_t getThetaVal() const { return fThetaVal; }; // value for peak
	Double_t getThetaHw() const { return fThetaHw; }; // hw = half width = half length of Hough space bin
	Double_t getSecondVal() const { return fSecondVal; }; // value for peak
	Double_t getSecondHw() const { return fSecondHw; }; // hw = half width = half length of Hough space bin

	// hits
	const PndFtsHit *getHit(UInt_t index); // gets the hit corresponding to index, will sort the hitId vector if necessary
	UInt_t getNSharedHits(PndFtsHoughTracklet& rhs); // gives the number of hits that are contained both in rhs and *this


	// Modifiers -----------------------
	// add results from Hough transforms
	void SetHoughTransformResults(
			const Double_t thetaVal,
			const Double_t secondVal,
			const Double_t peakHeight,
			const Double_t thetaHw,
			const Double_t secondHw
			);





private:
	// Private Data Members ------------
	Int_t fVerbose;

	// FTS Hits
	TClonesArray *fFtsHitArray;


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


//	void addPeakHits(); // check which hits have contributed to the peak and add them to the hitId vector


public:
	ClassDef(PndFtsHoughTracklet,1);
};

#endif
