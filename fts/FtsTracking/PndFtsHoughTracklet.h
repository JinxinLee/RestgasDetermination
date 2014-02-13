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
//	PndFtsHoughTracklet(const PndFtsHoughTracklet& other);
	~PndFtsHoughTracklet();
//	PndFtsHoughTracklet& operator=(PndFtsHoughTracklet other) // http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
//	{
//	    swap(*this, other);
//	    return *this;
//	}
//
//	friend void swap(PndFtsHoughTracklet& first, PndFtsHoughTracklet& second) // nothrow
//	{
//		// enable ADL (not necessary in our case, but good practice)
//		using std::swap;
//
//		// by swapping the members of two classes,
//		// the two classes are effectively swapped
//		swap(first.fVerbose, second.fVerbose);
//		swap(first.fIsSet, second.fIsSet);
//		swap(first.fPeakHeightFromPeakFinder, second.fPeakHeightFromPeakFinder);
//		swap(first.fThetaVal, second.fThetaVal);
//		swap(first.fThetaHw, second.fThetaHw);
//		swap(first.fSecondVal, second.fSecondVal);
//		swap(first.fSecondHw, second.fSecondHw);
//	}

	// operators
	// PndFtsHoughTracklet are the same if they contain the same hits, that means if the PndTrackCand are the same, therefore no need to implement that operator here
//	bool operator= (const PndFtsHoughTracklet& rhs);

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
