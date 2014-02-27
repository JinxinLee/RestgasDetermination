// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
// Class for Hough transform based FTS tracking
// loosely modeled according to PndTools/riemannfit/PndRiemannTrack.h
//
// Created: 24.01.2014
// Modified: 19.02.2014
//
// *************************************************************************

#ifndef PNDFTSHOUGHTRACKCAND_H
#define PNDFTSHOUGHTRACKCAND_H

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
	PndFtsHoughTrackCand(Int_t ftsBranchId=0, TClonesArray *ftsHitArray=0);
	~PndFtsHoughTrackCand();

	// operators
	// PndFtsHoughTrackCand are the same if they contain the same hits, that means if the PndTrackCand are the same, therefore no need to implement that operator here

	// Accessors -----------------------
	void Print();
	// isComplete() is kTRUE iif data from all Hough transforms have been entered
	Bool_t isComplete() const { return (fZxLineParabola.isSet() && fZxParabola.isSet() && fZxParabolaLine.isSet() && fZyLine.isSet()); };
	const PndFtsHit *getHit(UInt_t index); // gets the FairHit corresponding to index
	PndTrack getPndTrack(); // convert *this to a PndTrack
	FairTrackParP getTrackParPForHit(UInt_t i); // get the track parameters (needed for conversion to PndTrack) for hit with index i
	Double_t getQoverPzx() const{ return fZxParabola.getSecondVal(); };
	Int_t getCharge() const; // gets charge of track candidate // TODO only charge sign is implemented
	TVector3 getPforHit(UInt_t index); // gets the momentum calculated at some hit
	TVector3 getPositionForHit(UInt_t index); // gets the position calculated at some hit

	// Modifiers -----------------------
	// add results from Hough transforms
	void SetZxFirstLine(const PndFtsHoughTracklet zxLineParabola);
	void SetZxParabola(const PndFtsHoughTracklet zxParabola);
	void SetZxSecondLine(const PndFtsHoughTracklet zxParabolaLine);
	void SetZyLine(const PndFtsHoughTracklet zyLine);


private:
	void addUniqueTrackletHits(const PndFtsHoughTracklet inTracklet);


	// TODO: Add the equations for parabola
	inline Double_t getXOrYLabForLine(const Double_t &zLabSys, PndFtsHoughTracklet *lineTracklet)
	{
		// calculate x in lab sys for a given z position in lab sys for which the line assumption holds
		// theta in radian in zx plane given at z = fZLineParabola
		const Double_t thetaRad = lineTracklet->getThetaVal();
		const Double_t intercept = lineTracklet->getSecondVal();
		const Double_t zRefLabSys = lineTracklet->getZRefLabSys();

		Double_t xOrYLabSys = tan(thetaRad)*(zLabSys-zRefLabSys)+intercept;

		return xOrYLabSys;
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


public:
	ClassDef(PndFtsHoughTrackCand,1);
};

#endif
