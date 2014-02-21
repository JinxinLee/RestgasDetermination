// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
// Class for Hough space based on TH2S
//
//
// Created: 21.02.2014
// Modified: 21.02.2014
//
// *************************************************************************

#ifndef PndFtsHoughSpace_H
#define PndFtsHoughSpace_H

// Root Class Headers ----------------
#include "TH2.h"
class TClonesArray;



class PndFtsHoughSpace : public TH2S {
public:

	// Constructors/Destructors ---------
	PndFtsHoughSpace(
			TClonesArray *ftsHitArray=0,
			Double_t zRefPos=0.,
			Double_t interceptZx=0.
			);
	~PndFtsHoughSpace();

	// operators
	// PndFtsHoughSpace are the same if they contain the same hits, that means if the PndTrackCand are the same, therefore no need to implement that operator here

	// Accessors -----------------------
	void Print();




private:
	// Private Data Members ------------
	Int_t fVerbose;

	// FTS Hits
	TClonesArray *fFtsHitArray;



	// at which z value the values are calculated
	Double_t fZRefPos;

	// is used only for parabola in zx plane to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
	Double_t fInterceptZx;

public:
	ClassDef(PndFtsHoughSpace,1);
};

#endif
