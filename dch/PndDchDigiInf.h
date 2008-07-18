// -------------------------------------------------------------------------
// -----                   PndDchDigiInf header file                   -----
// -----                 Created 30.06.2008  by A. Bubak                -----
// -------------------------------------------------------------------------

/** \class PndDchDigiInf
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 30.06.2008
 *  \brief cosik na testowanie
 **/

#ifndef PNDDCHDIGIINF_H
#define PNDDCHDIGIINF_H

#include "TObjArray.h"
#include "TString.h"
#include "TObjString.h"

#include <list>

class TMap;
class PndDchDigiPar;

class PndDchDigiInf: public TObject {

public:
	/** Public method Instance()
	 ** for access to a singleton object
	 **/
	static PndDchDigiInf* Instance();

	/** Public method Instance(...)
	 ** for access to a singleton object for the first time
	 **/
	static PndDchDigiInf* Instance(TString filename);

	/** Destructor **/
	virtual ~PndDchDigiInf();

	Int_t GetNumberOfPointsForDigi() const {
		return fNumberOfPoints;
	}
	Double_t GetTimeUnit2ns() const {
		return fTimeUnit2ns;
	}
	Double_t GetfDistanceUnit2cm() const {
		return fDistanceUnit2cm;
	}

	/** key must be lower case. For example, if have in
	 ** file AaaA=90, then you should call
	 ** GetVariable("aaaa").
	 ** If variable not found, will return -1111 **/
	Double_t GetVariable(const char* key);
	/** key must be lower case. For example, if have in
	 ** file AaaA=90, then you should call
	 ** GetVariableStrict("aaaa").
	 ** If variable not found, will generate Fatal **/
	Double_t GetVariableStrict(const char* key);

	void AddVariable(const char* key, const char* value);
	/** Writes Inf to par **/
	void FillDigiPar(PndDchDigiPar* par) const;

protected:
	/** Constructor of the singleton object **/
	/** Text file constructor **/
	PndDchDigiInf(const char* filename);

private:
	/** Init all other variables from fVariables **/
	void InitVariables();
	/** A map containing all variables. This variable should be saved in parameter file **/
	TMap* fVariables;

	Int_t fNumberOfPoints; ///< number of points used in calibration
	Double_t fTimeUnit2ns; ///< read from the file to recalculate time and distance in  proper units. I assume to have time in [ns]
	Double_t fDistanceUnit2cm; ///< reading the calibration distance to drift time file. I assume to have distance in [cm]

	/** 1 if evething Ok **/
	Int_t fSuccess;

	TString fFileName;
	static PndDchDigiInf* fgInfInstance; ///< pointer to the singleton object

	ClassDef(PndDchDigiInf, 1);
};


#endif
