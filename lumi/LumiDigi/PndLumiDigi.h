/*
 * PndLumiDigi.h
 *
 *    Author: tsito
 *
 */


#ifndef PNDLUMIDIGI_H_
#define PNDLUMIDIGI_H_

#include "PndDetectorList.h"

#include "PndLumiStrip.h"

#include <cmath>
#include "TMath.h"
#include "TVector3.h"
#include "FairHit.h"

using std::cout;
using std::endl;

class PndLumiDigi : public FairHit
{
public:

	/** Default constructor*/
	PndLumiDigi();


	/** Constructor */
	PndLumiDigi(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index,
			PndLumiStrip strip, TString detname, SensorSide side, Int_t m, Bool_t isactive);

	/** Destructor */
	virtual ~PndLumiDigi();

	/** Accessing variables methods apart from ones inherited from FairHit*/
	PndLumiStrip GetStrip() const {return fStrip;}
	TString GetDetName() const {return fDetName;}
	SensorSide GetSide() const {return fSide;}
	Int_t GetMulti() const {return fMulti;}

	Bool_t IsActive() const{return fIsActive;}

	void Print(const Option_t* opt) const;

private:

	TString fDetName;		// name of the sensor hit
	SensorSide fSide;  		// side hit
	PndLumiStrip fStrip;	// Info of the strips fired
	Int_t fMulti;

	Bool_t fIsActive;

	ClassDef(PndLumiDigi,1);
};
#endif /*PNDLUMIDIGI_H_*/


