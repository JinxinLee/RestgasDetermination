// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
// Class for Hough transform based FTS tracking
// loosely modeled according to PndTools/riemannfit/PndRiemannTrack.h
//
// Created: 24.01.2014
// Modified: 03.03.2014
//
// *************************************************************************

#ifndef PNDFTSHOUGHTRACKCAND_H
#define PNDFTSHOUGHTRACKCAND_H

class PndFtsHoughTrackerTask;

// Root Class Headers ----------------
#include "PndTrackCand.h"
#include "PndFtsHoughTracklet.h"
#include "Rtypes.h"                     // for Double_t, Int_t, etc

#include <cmath>

class PndFtsHit;
class PndTrack;
class FairTrackParP;
class TVector3;
class TClonesArray;


class PndFtsHoughTrackCand : public PndTrackCand {
public:

	// Constructors/Destructors ---------
	PndFtsHoughTrackCand(
//			Int_t ftsBranchId=0,
//			TClonesArray *ftsHitArray=0,
			PndFtsHoughTrackerTask *trackerTask=0);
	~PndFtsHoughTrackCand();

	// operators
	// PndFtsHoughTrackCand are the same if they contain the same hits, that means if the PndTrackCand are the same, therefore no need to implement that operator here

	// Accessors -----------------------
	void Print();
	// isComplete() is kTRUE iif data from all Hough transforms have been entered
	Bool_t isComplete() const { return (fZxLineParabola.isSet() && fZxParabola.isSet() && fZxParabolaLine.isSet() && fZyLine.isSet()); };
	const PndFtsHit *getHit(UInt_t index); // gets the FairHit corresponding to index
	PndTrackCand getPndTrackCand(); // convert *this to a PndTrack, cannot be const
	PndTrack getPndTrack(); // convert *this to a PndTrack, cannot be const // calculates first and last parameter, but uses an empty PndTrackCand which has to be set lateron using SetTrackCandRef
	FairTrackParP getTrackParPForHit(const UInt_t i); // get the track parameters (needed for conversion to PndTrack) for hit with index i, cannot be const
	Int_t getCharge() const; // gets charge of track candidate // TODO only charge sign is implemented
	TVector3 getP(const Double_t zLabSys) const; // gets the momentum calculated at some hit
	TVector3 getPos(const Double_t zLabSys) const; // gets the position calculated at some hit

	Double_t getZLineParabola() { return fZLineParabola; };
	Double_t getZParabolaLine() { return fZParabolaLine; };



	// Modifiers -----------------------
	// add results from Hough transforms
	void SetZxFirstLine(const PndFtsHoughTracklet zxLineParabola);
	void SetZxParabola(const PndFtsHoughTracklet zxParabola);
	void SetZxSecondLine(const PndFtsHoughTracklet zxParabolaLine);
	void SetZyLine(const PndFtsHoughTracklet zyLine);


private:
	// for PandaRoot input/output
	PndFtsHoughTrackerTask *fTrackerTask;

	void addUniqueTrackletHits(const PndFtsHoughTracklet inTracklet);

	Double_t getQoverPzx() const{ return fZxParabola.getSecondVal(); };

	inline Double_t getPYLab() const {
		// theta in radian in zy plane given at z where first zx line meets the parabola
		const Double_t zRefLabSys = fZxParabola.getZRefLabSys();
		const Double_t thetaZyRad = fZyLine.getThetaRadVal();
		std::pair<Double_t, Double_t> pZPXLabSys = getPZPXLabLine(zRefLabSys, &fZxLineParabola);
		const Double_t pZLab = pZPXLabSys.first;
		Double_t pYLabSys = tan(thetaZyRad)*pZLab;
		return pYLabSys;
	}

	inline std::pair<Double_t, Double_t> getPZPXLabLine(const Double_t &zLabSys, const PndFtsHoughTracklet * const lineTracklet) const {
		// theta in radian in zx plane given at z = zRefLabSys
		const Double_t thetaRad = lineTracklet->getThetaRadVal();
		const Double_t qDivPzx = fZxParabola.getSecondVal(); // Q/pzx
		const Double_t pZx = getCharge() / qDivPzx;

		const Double_t pZLabSys = pZx*cos(thetaRad);
		const Double_t pXLabSys = pZx*sin(thetaRad);
		std::pair<Double_t, Double_t> pZPXLabSys(pZLabSys, pXLabSys);

		return pZPXLabSys;
	}

	inline std::pair<Double_t, Double_t> getPZPXLabParabola(const Double_t &zLabSys) const {
		// theta in radian in zx plane given at z = zRefLabSys
		const Double_t thetaRadVor = fZxLineParabola.getThetaRadVal();
		const Double_t zRefLabSysVor = fZxLineParabola.getZRefLabSys();

		const Double_t thetaRadNach = fZxParabolaLine.getThetaRadVal();
		const Double_t zRefLabSysNach = fZxParabolaLine.getZRefLabSys();

		// assume that theta is rotated linearly along z which is (probably) true for a circle, but not for a parabola // TODO: Check if this is a good assumption
		const Double_t zDistTotal = zRefLabSysNach - zRefLabSysVor;
		const Double_t zDistTraveled = zLabSys - zRefLabSysVor;

		const Double_t currentThetaRad = thetaRadVor + zDistTraveled / zDistTotal * (thetaRadNach - thetaRadVor);

		const Double_t qDivPzx = fZxParabola.getSecondVal(); // Q/pzx
		const Double_t pZx = getCharge() / qDivPzx;

		const Double_t pZLabSys = pZx*cos(currentThetaRad);
		const Double_t pXLabSys = pZx*sin(currentThetaRad);
		std::pair<Double_t, Double_t> pZPXLabSys(pZLabSys, pXLabSys);

		return pZPXLabSys;
	}


	inline Double_t getXOrYLabForLine(const Double_t &zLabSys, const PndFtsHoughTracklet * const lineTracklet) const {
		// calculate x or y in lab sys for a given z position in lab sys for which the line assumption holds
		// theta in radian in zx plane given at z = zRefLabSys
		const Double_t thetaRad = lineTracklet->getThetaRadVal();
		const Double_t intercept = lineTracklet->getSecondVal();
		const Double_t zRefLabSys = lineTracklet->getZRefLabSys();
		const Double_t zshifted = zLabSys-zRefLabSys;

		Double_t xOrYLabSys = tan(thetaRad)*zshifted+intercept;

		return xOrYLabSys;
	}

	// TODO: Check this!
	inline Double_t getXLabForParabola(const Double_t &zLabSys) const {
		// does not take option of varying B field into account
		// calculate x in lab sys for a given z position in lab sys for which the parabola assumption holds
		// theta in radian in zx plane given at z = zRefLabSys
		const Double_t thetaRad = fZxParabola.getThetaRadVal();
		const Double_t qDivPzx = fZxParabola.getSecondVal(); // Q/pzx
		const Double_t zRefLabSys = fZxParabola.getZRefLabSys();
		const Double_t zshifted = zLabSys-zRefLabSys;

		const Double_t interceptLine = fZxLineParabola.getSecondVal();

		const Double_t By = 1.;

		if (0==thetaRad){
			Double_t xParabolaSys = qDivPzx /2. * By * zshifted;
			// go from local parabola system to the lab system
			Double_t xLabSys = xParabolaSys + interceptLine;
			return xLabSys;
		}


		const Double_t tantheta = tan(thetaRad);
		const Double_t pzstuff = 1. / qDivPzx / By / sin(thetaRad);
		const Double_t ztantheta = zshifted / tantheta;
		const Double_t a = -ztantheta + pzstuff / tantheta;

		const Double_t wurzel = sqrt(a * a - 2. * pzstuff * zshifted - ztantheta * ztantheta);
		const Double_t x1ParabolaSys = a - wurzel;
		const Double_t x2ParabolaSys = a + wurzel;

		// go from local parabola system to the lab system
		Double_t x1LabSys = x1ParabolaSys + interceptLine;
		Double_t x2LabSys = x2ParabolaSys + interceptLine;

		// there are two analytical solutions, take the one closer to 0
		if (fabs(x1LabSys)<fabs(x2LabSys)) {
			return x1LabSys;
		} else {
			return x2LabSys;
		}
	}



	// Private Data Members ------------
	Int_t fVerbose;

	// FTS Hits
	Int_t   fFtsBranchId;
	TClonesArray *fFtsHitArray;

	// zx plane
	// straight line Hough transform in zx plane (stations before dipole field)
	PndFtsHoughTracklet fZxLineParabola;
	// theta in zx plane
	// vs
	// x intercept

	// parabola Hough transform in zx plane (stations inside dipole field)
	PndFtsHoughTracklet fZxParabola;
	// theta in zx plane
	// vs
	// charge divided by momentum projected into zx plane

	// straight line Hough transform in zx plane (stations after dipole field)
	PndFtsHoughTracklet fZxParabolaLine;
	// theta in zx plane
	// vs
	// x intercept

	// zy plane
	// straight line Hough transform in zy plane (all stations)
	PndFtsHoughTracklet fZyLine;
	// theta in zy plane
	// vs
	// y intercept


	// at which z value the transition in the bending zx plane is done from a line (before dipole field) to a parabola (within dipole field)
	Double_t fZLineParabola;
	// at which z value the transition in the bending zx plane is done from a parabola (within dipole field) to a line (before dipole field)
	Double_t fZParabolaLine;


	ClassDef(PndFtsHoughTrackCand,1);
};

#endif
