/**
 @class PndFtsHoughSpace

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Class for Hough space based on TH2S (for the moment). Saves the hits which enter this Hough space and finds peaks.

TODO Save path through Hough spaces with global indices!!!
TODO: Separate peak finder from Hough space

 The angle (theta in rad) is always on x-axis, the value on the y-axis depends on the kind of Hough transform:
	HT type  | yValue
	--------:|:-------
	parabola | Q/p_{zx}
	line     | intercept (Achsenabschnitt) (in z-x- or z-y-plane)

 All FTS hits are scanned within the constructor by filterInputHits().
 The hits which are relevant for this Hough space are saved in fHitId and can be accessed with getHit(UInt_t index).

 Created: 21.02.2014
 */


#ifndef PndFtsHoughSpace_H
#define PndFtsHoughSpace_H

#include "PndFtsHoughTrackerTask.h"
#include "PndFtsHoughSpacePeak.h"

#include "TH2.h"
#include <cmath>
#include <vector>
#include "Rtypes.h"                     // for Double_t, Int_t, etc
#include "FairLogger.h" // for FairLogger, MESSAGE_ORIGIN

#include "PndTrackCandHit.h"
#include "TClonesArray.h"

class FairField;

#include "PndFtsHoughTracklet.h"
#include "PndFtsHoughTrackCand.h"

// For error throwing
#include "TString.h"
#include <stdexcept>



// for saving the path through the Hough space
typedef std::pair<Int_t, Int_t> ThetaYIdxPair; // helper -- just a pair of thetaRad index and the yVal index in the Hough space histogram
typedef std::vector< ThetaYIdxPair > IdxPath; // helper -- path for one hit
typedef std::map<Int_t, IdxPath > HitIdxPathMap; // that is the one I need -- map of hit indices to path for the corresponding hits
typedef std::pair<Int_t, IdxPath > HitIdxPathPair; // helper for inserting one pair of hit index and path into map



// for peaks
typedef std::vector< PndFtsHoughSpacePeak > PeakVec; // helper -- vector of peaks
typedef std::map<Int_t, PeakVec > HitIdxPeakVec; // that is the one I need -- map of hit indices to vector of peaks in which the hit could be in
typedef std::pair<Int_t, PeakVec > HitIdxPeakVecPair; // helper for inserting one pair of hit index and vector of peaks into map

// cout for the above types
std::ostream& operator <<(std::ostream& os, const ThetaYIdxPair& outPair);
std::ostream& operator <<(std::ostream& os, const IdxPath& outVector);
std::ostream& operator <<(std::ostream& os, const HitIdxPathMap& outMap);



class PndFtsHoughSpace : public TH2S {
public:

	// Constructors/Destructors ---------
	PndFtsHoughSpace(
			const char *name=0,

			Int_t nbinsx=0,
			Double_t xlow=0.,
			Double_t xup=0.,

			Int_t nbinsy=0,
			Double_t ylow=0.,
			Double_t yup=0.,

			Double_t zRefPos=0.,
			Double_t interceptZx=0.,

			PndFtsHoughTrackCand *associatedTrackCand=0,

			PndFtsHoughTrackerTask *trackerTask=0
	);
	~PndFtsHoughSpace();

	TH2S ExportTH2S();

	/**@brief Finds all peaks that satisfy the minimum height requirement minHeight.

	 TODO: This information is obsolete and should be adjusted
		peakTheta = theta for peak
		peakSecond = Q/pzx for peak (parabola HT)
		peakSecond = intercept for peak (line HT) (in z-x- or z-y-plane)

		peakThetaHw = half width of peak in theta
	 	 peakSecondHw = half width of peak in second value (see above for what it stands for)

	 actualHeight height of the peak in the histogram (in counts)
	 *
	 * @param minHeight
	 * @param tracklets should be empty at the beginning and will contain all values found for the peaks in the Hough space. They will contain the hitIds of all hits that contribute to the peaks.
	 * @return kTRUE if at least one peak was found, kFALSE otherwise (Hough Space is empty / has too few hits or tracklets were not empty)
	 */
	std::vector<PndFtsHoughTracklet> FindAllPeaks(const UInt_t minHeight);

	/**@brief Fills the Hough space using the equation which corresponds to the name of the Hough space.

	 If something goes wrong the function throws a runtime_error (probably Hough space name is set incorrectly).

	 !!! WARNING The theta values (in rad) are NOT the same as in the interaction point.
	 They are always calculated relative to a shifted coordinate system and only 2-dimensional (either in z-x- or z-y-plane) !!!

	 The angle (theta in rad) to the z-axis in the z-x- or z-y-plane at a z reference position will be scanned
	 from theta corresponding to lowest bin to theta corresponding to highest bin of x-axis.

	 TODO For each hit the "path" through the 2d histogram is saved for peak finding.

	 y component of B field will be read from field maps if fKeepBConstant is kFALSE.

	 @param[in] Running index, only needed for Debugging output of Hough spaces
	 */
	void FillHoughSpace(const Int_t index = -1);

	// operators
	// PndFtsHoughSpace are the same if they contain the same hits, that means if the PndTrackCand are the same, therefore no need to implement that operator here

	// Accessors -----------------------
	inline void Print() const;

	void setVerbose(Int_t verbose) { fVerbose = verbose; };

	Double_t getInterceptZx() const { return fInterceptZx; };

	Double_t getZRefPos() const { return fZRefPos; };

	inline UInt_t GetNHits() const { return fHitId.size(); };

	//------
	//DEBUG
	//-----
	/** @brief For writing out Hough spaces as histograms (for debugging purposes).
	 * @param[in] houghSpace To be written as histogram.
	 * @param[in] index Use this parameter in case the same kind of Hough space needs to be written out multiple times per event.
	 */
	void WriteHistoOfHoughSpace(Int_t index=-1) const;


private:
	// for PandaRoot input/output
	PndFtsHoughTrackerTask *fTrackerTask;

	/** @brief For error reporting */
	void throwError(const TString s) const{ throw std::runtime_error(s.Data()); };

	Bool_t setParametersForHsOption(); // set parameters according to the kind of Hough transform I want to do
	void filterInputHits(); // copies input hits (based on z coordinate and skewed/non-skewed) from fFtsHitArray (all FTS hits) to fHitId (only the hits that qualify for the specific Hough transform)
	inline void AddHitToHS(UInt_t hitId, Double_t rho);
	inline void AddHitToHS(FairLink link, Double_t rho);
	inline const PndFtsHit *const getHitFromHS(UInt_t index) const; // gets the FTS hit corresponding to index
	inline const Int_t getHitIdFromHS(UInt_t index) const { return fHitId.at(index).GetHitId(); }; // gets the FTS hit Id corresponding to index
	inline const TVector3 CalculateHitPosFromIntersectionsWithZxTrackModel(const PndFtsHit *const myHit) const;
	inline const TVector3 GetRawOrCalculatedHitPos(const PndFtsHit* const myHit) const;


	/**For each hit index the "path through the Hough space" [order of index pairs in which the (thetaRad, yVal) pairs are filled during the thetaRad scan] is saved
	 * This is useful for peak finding.
	 * Map index: hit index, use as argument of getHit.
	 * The map gives the vector of (index(thetaRad), index(yVal)) pairs which were filled during the theta scan for the hit.
	 */
	HitIdxPathMap fHitThetaYIdxPath;

	// Private Data Members ------------
	Int_t fVerbose;

	/// @ brief FTS Hits
	Int_t   fFtsBranchId;
	std::vector<PndTrackCandHit> fHitId;  ///< @brief hits relevant for this Hough space
	///< first index is detId, second index is hit Id

	PndFtsHoughTrackCand *fAssociatedTrackCand; ///< @brief For skewed hits a track candidate is needed


	// only hits with a z value (in the laboratory system, zreal) between onlyusehitsfromz and onlyusehitsuptoz will be used for building the houghspace
	Double_t fOnlyUseHitsFromZ;
	Double_t fOnlyUseHitsUpToZ;
	Bool_t fUseNonSkewedStraws; // if kTRUE, then hits from non-skewed straws are used for Hough transform
	Bool_t fUseSkewedStraws; // if kTRUE, then hits from skewed straws are used for Hough transform

	Bool_t fKeepBConstant; // set only to kFALSE for testing
	// If kTRUE the y-component of the B-field is not used in the parabola hough transform
	// if kFALSE the parabola's shape will be adjusted based on the magnetic field maps

	// at which z value the values are calculated
	Double_t fZRefPos; // is used to redefine an origin for the coordinate system (so that the angle definition gives meaningful theta values)
	//			Double_t interceptZx; // cannot be constant, because might need to be reset if set incorrectly (has to be 0 for line HT)
	// is used to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
	// (zreal=zOffset, xreal=interceptZx) = (zshifted = 0, xshifted = 0)
	// zshifted = zreal - zRefPos
	// xshifted = xreal - interceptZx
	// zreal = zshifted + zRefPos
	// xreal = xshifted + interceptZx
	// For the z-x-plane parabola, a shift in x (hitshiftinx) needs to be set (which should be the result of the straight line hough transform before the dipole field)
	// For the straight line (stations before dipole field) hitshiftinx HAS TO BE ZERO

	// is used only for parabola in zx plane to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
	Double_t fInterceptZx;




	// for B field access
	FairField* fField;



	// Which PeakFinder should be used?
	static TString peakfinderOption;

	// for HoughTransform
	/////////////////////
	// if kTRUE will correct the pzx prediction according to values which should be obtained from a line fit mc truth momentum VS. reco momentum with high statistics
	static const Bool_t fCorrectPzx = kFALSE;





	/**@brief Makes sure there are no holes in the Hough space by filling them with a line.

	 Fills holes between (lastBinX, lastBinY) and (currentBinX, currentBinY) = (lastBinX+1, currentBinY) with a line.

	 Holes can only appear in the yDirection and cannot appear in theta direction:
	   We go from one bin to the next.
	   From lower theta values to the next higher value.
	 * @param lastBinX
	 * @param lastBinY
	 * @param currentBinY
	 * @param ptrThetaYIdxPathVec Pointer to the vector which contains the path through the Hough space. If any holes are filled, the interpolated values will be written into this vector.
	 * @return
	 */
	inline Bool_t FillHoles(
			const Int_t lastBinX,
			const Int_t lastBinY,
			const Int_t currentBinY,
			IdxPath * ptrThetaYIdxPathVec
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

	inline Double_t equationLineZxOrZy(Double_t thetaRad, Double_t hitZShifted, Double_t hitXLabSys)
	{ // TODO: Check if that also works in zy plane
		// calculate b which is the distance of point on line at z = zOffset from z axis
		Double_t yVal = -tan(thetaRad)*hitZShifted+hitXLabSys;
		return yVal;
	}


	//------
	//DEBUG
	//-----
	TH2S MakeEmptyHistoOfSameDimensions() const;
	void WriteHistoOfAllPeaks(const PeakVec& mergedPeaksForAllHits) const;

public:
	ClassDef(PndFtsHoughSpace,1);

};






// inline functions
void PndFtsHoughSpace::Print() const {
	std::cout << "=========== PndFtsHoughSpace::Print() ==========" << '\n';
	std::cout << "fZRefPos = " << fZRefPos << '\n';
	std::cout << "fInterceptZx = " << fInterceptZx << 'n';
	TH2S::Print();
}


const PndFtsHit *const PndFtsHoughSpace::getHitFromHS(UInt_t index) const {
	if (index < GetNHits()){
		Int_t hitIndex = fHitId.at(index).GetHitId();
		const PndFtsHit *const myHit = (PndFtsHit*) fTrackerTask->GetFtsHit(hitIndex);
		return myHit;
	}
	throwError("index too high");
}


const TVector3 PndFtsHoughSpace::GetRawOrCalculatedHitPos(
		const PndFtsHit* const myHit
) const {
	// get hit position
	TVector3 hitPos;
	if (kFALSE == myHit->GetSkewed()) {
		myHit->Position(hitPos);
	} else {
		hitPos = CalculateHitPosFromIntersectionsWithZxTrackModel(myHit);
	}
	return hitPos;
}

const TVector3 PndFtsHoughSpace::CalculateHitPosFromIntersectionsWithZxTrackModel(const PndFtsHit* const myHit) const {
	if (0==fAssociatedTrackCand) throwError("No track cand associated with Hough space. Cannot calculate possible hit pos. from track model.");

	const PndFtsTube *ftsTube = fTrackerTask->GetFtsTube(myHit);
	const TVector3 wireDirection = ftsTube->GetWireDirection();
	const TVector3 wireCenter = ftsTube->GetPosition();

	const Double_t hitZLabSys = wireCenter.Z();

	// calculate xTM according to track model at hitZLabSys
	const Double_t xTM = fAssociatedTrackCand->getXLabSys(hitZLabSys);
	// calculate param which is needed for xStraw = xTM
	// xTM = wireCenter.X() + param*wireDirection.X()
	const Double_t param = ( xTM - wireCenter.X() ) / wireDirection.X();
	// calculate corresponding yStraw
	const Double_t yStraw = wireCenter.Y() + param*wireDirection.Y();

	TVector3 crossingPosition(xTM,yStraw,hitZLabSys);
	return crossingPosition;
}


#endif
