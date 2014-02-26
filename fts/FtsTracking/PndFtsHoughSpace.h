// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
// Class for Hough space based on TH2S (for the moment)
//
//
// Created: 21.02.2014
// Modified: 26.02.2014
//
// *************************************************************************

#ifndef PndFtsHoughSpace_H
#define PndFtsHoughSpace_H


#include "TH2.h"
#include <cmath>
#include <vector>

#include "PndTrackCandHit.h"
#include "TClonesArray.h"

class FairField;

#include "PndFtsHoughTracklet.h"
#include "PndFtsHoughTrackCand.h"



class PndFtsHoughSpace : public TH2S {
public:

	// Constructors/Destructors ---------
	PndFtsHoughSpace(
			const char *name,

			Int_t nbinsx,
			Double_t xlow,
			Double_t xup,

			Int_t nbinsy,
			Double_t ylow,
			Double_t yup,

			TClonesArray *ftsHitArray,
			Int_t ftsBranchId,

			Double_t zRefPos,
			Double_t interceptZx,

			FairField *field
	);
	~PndFtsHoughSpace();

	Bool_t FindAllPeaks(
			const UInt_t minHeight,
			std::vector<PndFtsHoughTracklet> &tracklets
	);

	Bool_t MakeHoughSpace(
//			const Double_t zRefPos, // is used to redefine an origin for the coordinate system (so that the angle definition gives meaningful theta values)
//			Double_t interceptZx, // cannot be constant, because might need to be reset if set incorrectly (has to be 0 for line HT)
			// is used to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
			// (zreal=zOffset, xreal=interceptZx) = (zshifted = 0, xshifted = 0)
			// zshifted = zreal - zRefPos
			// xshifted = xreal - interceptZx
			// zreal = zshifted + zRefPos
			// xreal = xshifted + interceptZx
			// For the z-x-plane parabola, a shift in x (hitshiftinx) needs to be set (which should be the result of the straight line hough transform)
			// For the straight line (stations before dipole field) hitshiftinx HAS TO BE ZERO

//			const Bool_t UseNonSkewedStraws, // if kTRUE, then hits from non-skewed straws are used for Hough transform
//			const Bool_t UseSkewedStraws, // if kTRUE, then hits from skewed straws are used for Hough transform


//			UInt_t &nHitsForHoughSpace

			//			TH2S* houghspace // has always the angle (theta) on x-coordinate axis, the value on the y-axis depends on the kind of hough transform
			//	parabola HT: yValue = Q/pzx
			//	line HT: yValue = intercept (Achsenabschnitt) (in z-x- or z-y-plane)
	);

	// operators
	// PndFtsHoughSpace are the same if they contain the same hits, that means if the PndTrackCand are the same, therefore no need to implement that operator here

	// Accessors -----------------------
	inline void Print() const;

	void setVerbose(Int_t verbose) {
		fVerbose = verbose;
	}

	Double_t getInterceptZx() const {
		return fInterceptZx;
	}

	Double_t getZRefPos() const {
		return fZRefPos;
	}

	inline UInt_t GetNHits() const {return fHitId.size();}
	inline const PndFtsHit *getHit(UInt_t index) const; // gets the FTS hit corresponding to index



private:
	PndFtsHoughSpace(); // do not use

	Bool_t setParametersForHsOption(); // set parameters according to the kind of Hough transform I want to do
	Bool_t filterInputHits(); // copies input hits (based on z coordinate and skewed/non-skewed) from fFtsHitArray (all FTS hits) to fHitId (only the hits that qualify for the specific Hough transform)
	inline void AddHit(UInt_t hitId, Double_t rho);
	inline void AddHit(FairLink link, Double_t rho);


	// Private Data Members ------------
	Int_t fVerbose;

	// FTS Hits
	Int_t   fFtsBranchId;
	TClonesArray *fFtsHitArray; // all FTS hits
	std::vector<PndTrackCandHit> fHitId;  // hits relevant for this Hough space
	///< first index is detId, second index is hit Id


	// only hits with a z value (in the laboratory system, zreal) between onlyusehitsfromz and onlyusehitsuptoz will be used for building the houghspace
	Double_t fOnlyUseHitsFromZ;
	Double_t fOnlyUseHitsUpToZ;
	Bool_t fUseNonSkewedStraws; // if kTRUE, then hits from non-skewed straws are used for Hough transform
	Bool_t fUseSkewedStraws; // if kTRUE, then hits from skewed straws are used for Hough transform
	Bool_t fKeepBConstant;
	// If kTRUE the y-component of the B-field is not used in the parabola hough transform
	// if kFALSE the parabola's shape will be adjusted based on the magnetic field maps
	// set only to kFALSE for testing

	// at which z value the values are calculated
	Double_t fZRefPos;

	// is used only for parabola in zx plane to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
	Double_t fInterceptZx;


	// for B field access
	FairField* fField;



	static const Double_t meinpi = 3.14159265;

	// Which PeakFinder should be used?
	static TString peakfinderOption;

	// for HoughTransform
	/////////////////////
	// if kTRUE will correct the pz prediction according to values which should be obtained from a line fit mc truth momentum VS. reco momentum with high statistics
	static const Bool_t correctpz = kFALSE;






	inline Bool_t FillHoles(
			Int_t lastBinX,
			Int_t lastBinY,
			Int_t currentBinY
	);




	inline Double_t equationParabola(Double_t thetaRad, Double_t hitZShifted, Double_t hitXShifted, Double_t By)
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

	inline Double_t equationParabolaPz(Double_t thetaRad, Double_t hitZShifted, Double_t hitXShifted, Double_t By)
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

	inline Double_t equationLineZx(Double_t thetaRad, Double_t hitZShifted, Double_t hitXLabSys)
	{
		// calculate b which is the distance of point on line at z = zOffset from z axis
		Double_t yVal = -tan(thetaRad)*hitZShifted+hitXLabSys;
		return yVal;
	}


public:
	ClassDef(PndFtsHoughSpace,1);

};


// inline functions
void PndFtsHoughSpace::Print() const {
	std::cout << "=========== PndFtsHoughSpace::Print() ==========" << std::endl;
	std::cout << "fZRefPos = " << fZRefPos << '\n';
	std::cout << "fInterceptZx = " << fInterceptZx << 'n';
	TH2S::Print();
}


const PndFtsHit* PndFtsHoughSpace::getHit(UInt_t index) const {
	if (index < GetNHits()){
		Int_t hitIndex = fHitId.at(index).GetHitId();
		const PndFtsHit *myHit = (PndFtsHit*) fFtsHitArray->At(hitIndex);
		return myHit;
	} else {
		return 0;
	}
}


#endif
