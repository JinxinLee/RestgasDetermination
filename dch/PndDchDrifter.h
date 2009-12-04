// -------------------------------------------------------------------------
// -----                PndDchDrifter header file                      -----
// -----                Created 03/08/07  by P. Hawranek               -----
// -------------------------------------------------------------------------

/** \class PndDchDrifter
 *  \author P.Hawranek
 *  \date 03/08/07
 *  \brief Class for conversion drift time to distance
 *
 *     The class serves for recalculation of measured drift time
 *     to distance to the sense wire and vice versa.
 *     It is used for digitisation and spatial reconstruction of tracks.
 **/

#ifndef PNDDCHDRIFTER_H
#define PNDDCHDRIFTER_H

// ROOT includes
#include "TString.h"
#include "TObject.h"
// c++ includes
#include <vector>

class PndDchDigiPar;
class PndDchDigiInf;

class PndDchDrifter: public TObject {
public:
	/** Public method Instance()
	 ** for access to a singleton object
	 **/
	static PndDchDrifter* Instance();

	/** Public method Instance(...)
	 ** for access to a singleton object for the first time
	 **/
	static PndDchDrifter* Instance(TString filename, Double_t cellsize);

	/** Destructor **/
	~PndDchDrifter() {
		;
	}

	/** Public method CalculateDriftTime(...)
	 calculates drift time from distance
	 **/
	Bool_t CalculateDriftTime(Double_t &time, Double_t distance) const;

	/** Public method CalculateDistance(...)
	 calculates distance from drift time
	 **/
	Bool_t CalculateDistance(Double_t time, Double_t &distance) const;

	/** Parameter container init **/
	void SetParContainers();

	/** Initialize **/
	//void Initialize();  // EAD_EDIT

	PndDchDigiInf* GetDchInf() const {
		return fInf;
	}

	void Print(const Option_t* option = "") const;

protected:
	/** Constructor of the singleton object **/
	PndDchDrifter(TString fileName, Double_t cellSize);

private:
	Double_t fCellSize; ///< cell size in cm
	Int_t fNumberOfPoints; ///< number of points used in calibration
	Double_t fTimeUnit2ns; ///< aaaa
	Double_t fDistanceUnit2cm; ///< aaa
	std::vector<Double_t> fDriftTime; ///< driftTime array in calibration
	std::vector<Double_t> fDistance; ///< array of distances from the wire in calibration
	static PndDchDrifter* fgDrifterInstance; ///< pointer to the singleton object

	PndDchDigiPar *fPar;
	PndDchDigiInf *fInf; ///< pointer to dch digi information


	ClassDef(PndDchDrifter,1)
};

#endif
