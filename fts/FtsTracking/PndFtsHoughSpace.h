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


#include "TH2.h"
#include <vector>
#include "PndTrackCandHit.h"
class TClonesArray;



class PndFtsHoughSpace : public TH2S {
public:

	// Constructors/Destructors ---------
	PndFtsHoughSpace();

	PndFtsHoughSpace(
			const char *name,

			Int_t nbinsx,
			Double_t xlow,
			Double_t xup,

			Int_t nbinsy,
			Double_t ylow,
			Double_t yup,

			TClonesArray *ftsHitArray,
			Double_t zRefPos,
			Double_t interceptZx=0.
	);
	~PndFtsHoughSpace();

	// operators
	// PndFtsHoughSpace are the same if they contain the same hits, that means if the PndTrackCand are the same, therefore no need to implement that operator here

	// Accessors -----------------------
	void Print();


	// getters / setters
	void setFtsHitArray(const TClonesArray*& ftsHitArray) {
		fFtsHitArray = ftsHitArray;
	}

	Double_t getInterceptZx() const {
		return fInterceptZx;
	}

	void setInterceptZx(Double_t interceptZx) {
		fInterceptZx = interceptZx;
	}

	void setVerbose(Int_t verbose) {
		fVerbose = verbose;
	}

	Double_t getZRefPos() const {
		return fZRefPos;
	}

	void setZRefPos(Double_t zRefPos) {
		fZRefPos = zRefPos;
	}



private:
	// Private Data Members ------------
	Int_t fVerbose;

	// FTS Hits
	TClonesArray *fFtsHitArray; // all FTS hits
	std::vector<PndTrackCandHit> fHitId;  // hits relevant for this Hough space
	///< first index is detId, second index is hit Id
	void AddHit(UInt_t detId, UInt_t hitId, Double_t rho);
	void AddHit(TString branchName, UInt_t hitId, Double_t rho);
	void AddHit(FairLink link, Double_t rho);
	UInt_t GetNHits() const {return fHitId.size();}


	// at which z value the values are calculated
	Double_t fZRefPos;

	// is used only for parabola in zx plane to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
	Double_t fInterceptZx;



public:
	ClassDef(PndFtsHoughSpace,1);

};

#endif
